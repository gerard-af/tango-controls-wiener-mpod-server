/*

    Copyright (c) 2018 - 2019 Analog Flavor EURL

    Contact us for support:
        http://www.analogflavor.com

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of Analog Flavor EURL nor 
      the names of its contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------

*/

#include <WienerMpodLvHvSnmp.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

//#define WIENER_MPOD_DEBUG_FLAG 1
//#define WIENER_MPOD_DEBUG_SNMP_FLAG 1
//#define WIENER_MPOD_DEBUG_ON_OFF_FLAG 1
//#define WIENER_MPOD_DEBUG_CRATE_STRUCTURE 1
//#define WIENER_MPOD_DEBUG_NEXT_OID_FLAG 1
//#define WIENER_MPOD_DEBUG_PRINT_ERROR_STRINGS 1
//#define WIENER_MPOD_DEBUG_THREAD 1
//#define WIENER_MPOD_DEBUG_EXECUTION_QUEUE 1
//#define WIENER_MPOD_DEBUG_STATUS_VARIABLE_DETECTION 1
//#define WIENER_MPOD_DEBUG_READ_DATA_FAST 1
//#define WIENER_MPOD_DEBUG_DATA_BUFFER 1
//#define WIENER_MPOD_DEBUG_TIMEOUT 1


WienerMpodLvHvSnmp::WienerMpodLvHvSnmp()
{
    initAtContruction();
    crateConnect("", 161, "", 1000, 1000, 1000);
}

WienerMpodLvHvSnmp::WienerMpodLvHvSnmp(const std::string &ip_address, int port_index, const std::string &community_name)
{
    initAtContruction();
    crateConnect(ip_address, port_index, community_name, 1000, 1000, 1000);
}

void WienerMpodLvHvSnmp::initAtContruction() {
    m_pollingIntervalMsFast = 0;
    m_pollingIntervalMsIntermediate = 0;
    m_pollingIntervalMsSlow = 0;
    m_flagThreadIsRunning = false;
    m_flagJoinThread = false;
    m_flagPollingInThreadIsActive = false;
    m_session = 0;
    m_crate = 0;
    m_timeoutIntervalMs = -1; /// snmp default
    setConnectionStatus(mpod_snmp_connection_status_undefined);
    m_flagCheckOnOff = false;
    m_cArray = 0;
    m_cArrayAllocatedSize = 0;

    m_measuredDataTimeBuffer = 0;
    m_measuredDataBuffer.clear();
    m_measuredDataBufferInitialSize = 1024;
    m_measuredDataBufferAllocatedSize = 0;
    m_measuredDataBufferSize = 0;
    m_measuredDataStartPosition = 0;
    m_measuredDataLastReadNumberOfValues = 0;

    m_measuredDataTmpAllocateSize = 0;
    m_measuredDataTmpValues = 0;
    m_measuredDataTmpTimeValues = 0;

    m_snmpTimeoutCounter = 0;
    m_snmpTimeoutCounterMax = 10;
}

WienerMpodLvHvSnmp::~WienerMpodLvHvSnmp()
{
#ifdef WIENER_MPOD_DEBUG_THREAD
    std::cout << "destructor" << std::endl;
#endif
    bool flag_join_thread = false;
    {
        AfThreadsMutexLocker lock(m_dataAccessMutex);
        if (m_flagThreadIsRunning) {
            m_flagJoinThread = true;
            flag_join_thread = true;
        }
    }
    if (flag_join_thread) {
#ifdef WIENER_MPOD_DEBUG_THREAD
        std::cout << "joining thread ..." << std::endl;
#endif
        joinThread();
#ifdef WIENER_MPOD_DEBUG_THREAD
        std::cout << "... thread joined" << std::endl;
#endif
    }

    crateDisconnect();

    if (m_session) {
        delete m_session;
        m_session = 0;
    }

    if (m_cArray) {
        delete [] m_cArray;
        m_cArray = 0;
    }
    freeMeasuredDataBuffers();
    freeTmpDataBuffers();
}

std::string WienerMpodLvHvSnmp::lockAndGetErrorString() {
    AfThreadsMutexLocker lock(m_snmpAccessMutex);
    return m_errorStr;
}

void WienerMpodLvHvSnmp::lockAndSetErrorString(const std::string &s) {
    AfThreadsMutexLocker lock(m_snmpAccessMutex);
    setErrorString(s);
}

void WienerMpodLvHvSnmp::setErrorString(const std::string &s) {
#ifdef WIENER_MPOD_DEBUG_PRINT_ERROR_STRINGS
    std::cout << s << std::endl;
#endif
    m_errorStr = s;
}

const int *WienerMpodLvHvSnmp::toCArray(const std::vector<size_t> &v)
{
    if (v.size() + 1 > m_cArrayAllocatedSize) {
        if (m_cArray) {
            delete [] m_cArray;
        }
        m_cArrayAllocatedSize = v.size() + 10;
        m_cArray = new int[m_cArrayAllocatedSize];
    }
    for (size_t i = 0; i < v.size(); i++ ) {
        m_cArray[i] = v[i];
    }
    m_cArray[v.size()] = getIntError();
    return m_cArray;
}

void WienerMpodLvHvSnmp::crateDisconnect()
{
    snmp_close_sessions();
    m_crate = 0;
    m_flagCheckOnOff = false;
    setConnectionStatus(mpod_snmp_connection_status_undefined);
    m_flagMibFileIsLoaded = false;
}

bool WienerMpodLvHvSnmp::crateConnect(const std::string &ip_address, int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow) {
    return crateConnect(ip_address, 0, "", thread_polling_interval_ms_fast, thread_polling_interval_ms_intermediate, thread_polling_interval_ms_slow);
}

bool WienerMpodLvHvSnmp::crateConnect(const std::string &ip_address, const std::string &community_name, int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow) {
    return crateConnect(ip_address, 0, community_name, thread_polling_interval_ms_fast, thread_polling_interval_ms_intermediate, thread_polling_interval_ms_slow);
}

void WienerMpodLvHvSnmp::setPollingInterval(int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow) {
    bool flag_join_thread = false;
#ifdef WIENER_MPOD_DEBUG_THREAD
    std::cout << "setting polling interval to " << thread_polling_interval_ms << std::endl;
#endif
    if (thread_polling_interval_ms_fast> 0
            && thread_polling_interval_ms_intermediate > 0
            && thread_polling_interval_ms_slow > 0) {
        AfThreadsMutexLocker lock(m_dataAccessMutex);
        m_pollingIntervalMsFast = thread_polling_interval_ms_fast;
        m_pollingIntervalMsIntermediate = thread_polling_interval_ms_intermediate;
        m_pollingIntervalMsSlow = thread_polling_interval_ms_slow;
        m_flagJoinThread = false;
        m_flagPollingInThreadIsActive = true;
        if ( ! m_flagThreadIsRunning) {
            if ( ! createThread()) {
                m_flagPollingInThreadIsActive = false;
                m_flagJoinThread = true;
            }
        }
    }
    else {
        AfThreadsMutexLocker lock(m_dataAccessMutex);
        m_pollingIntervalMsFast = 0;
        m_pollingIntervalMsIntermediate = 0;
        m_pollingIntervalMsSlow = 0;
        if (m_flagThreadIsRunning) {
            m_flagJoinThread = true;
        }
        m_flagPollingInThreadIsActive = false;
    }
    if (flag_join_thread) {
#ifdef WIENER_MPOD_DEBUG_THREAD
        std::cout << "joining thread ..." << std::endl;
#endif
        joinThread();
#ifdef WIENER_MPOD_DEBUG_THREAD
        std::cout << "... thread joined" << std::endl;
#endif
    }
}

bool WienerMpodLvHvSnmp::crateConnect(const std::string &ip_address, int port_index, const std::string &community_name, int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow)
{
    std::string oid_str, simple_oid;
    DataContainer data;

    m_flagMibFileIsLoaded = false;

    lockAndSetConnectionStatus(mpod_snmp_connection_status_disconnected);
    snmpInit();
    if (m_crate != 0) {

        AfThreadsMutexLocker lock(m_snmpAccessMutex);

        snmp_close_sessions();
        m_crate = 0;
    }
    m_crate = snmpOpen(ip_address, port_index, community_name);
    m_flagCheckOnOff = false;

    setPollingInterval(thread_polling_interval_ms_fast, thread_polling_interval_ms_intermediate, thread_polling_interval_ms_slow);

    if (!m_crate) {
        lockAndSetConnectionStatus(mpod_snmp_connection_status_disconnected);
        lockAndSetErrorString("Unable to open SNMP connection to MPOD crate.");
        return false;
    }

    lockAndSetErrorString("Connected to MPOD crate over SNMP.");

    // this is required for recomputeCrateStructure()
    if (m_flagPollingInThreadIsActive) {
        oid_str = getSysMainSwitchOid();
        if (snmpGetValueFromSnmp(oid_str, data)) {
            if (checkIsCrateOid(oid_str, simple_oid)) {
                AfThreadsMutexLocker lock(m_dataAccessMutex);
                m_readCrateData[simple_oid] = data;
            }
        }
    }
    recomputeCrateStructure(true);
    return true;
}

double WienerMpodLvHvSnmp::channelGetConfigMaxCurrent(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.23."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetConfigMaxSenseVoltage(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.21."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetConfigMaxTerminalVoltage(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.22."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetCurrent(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.12."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetCurrentFallRate(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.26."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetCurrentRiseRate(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.25."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

int WienerMpodLvHvSnmp::channelGetIndexImpl(unsigned slot_index, unsigned channel_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.1."<<  snmp_channel_index;
    argout = snmpGetValueIntImpl(cmd.str());
    return argout;
}

int WienerMpodLvHvSnmp::channelGetIndex(unsigned slot_index, unsigned channel_index)
{
    int argout = channelGetIndexImpl(slot_index, channel_index);
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetMeasurementCurrent(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.7."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetMeasurementSenseVoltage(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.5."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetMeasurementTemperature(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.8."<<  snmp_channel_index;
    // the returned integer is converted to a double value here
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetMeasurementTerminalVoltage(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.6."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

int WienerMpodLvHvSnmp::channelGetStatus(unsigned slot_index, unsigned channel_index)
{
    int argout = channelGetStatusImpl(slot_index, channel_index);
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

int WienerMpodLvHvSnmp::channelGetStatusImpl(unsigned slot_index, unsigned channel_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.4."<<  snmp_channel_index;
    argout = snmpGetValueIntImpl(cmd.str());
    return argout;
}

std::string WienerMpodLvHvSnmp::channelGetStatusAsString(unsigned slot_index, unsigned channel_index)
{
    unsigned integer_status_flags;
    std::string argout;
    integer_status_flags = channelGetStatus(slot_index, channel_index);
    argout = convertChannelStatusToString(integer_status_flags, slot_index);
    return argout;
}

int WienerMpodLvHvSnmp::channelGetSupervisionBehavior(unsigned slot_index, unsigned channel_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.15."<<  snmp_channel_index;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetSupervisionMaxCurrent(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.19."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetSupervisionMaxPower(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.24."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetSupervisionMaxTerminalVoltage(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.18."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetSupervisionMinSenseVoltage(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.16."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

int WienerMpodLvHvSnmp::channelGetSwitch(unsigned slot_index, unsigned channel_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.9."<<  snmp_channel_index;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

int WienerMpodLvHvSnmp::channelGetTripTime(unsigned slot_index, unsigned channel_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.27."<<  snmp_channel_index;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

int WienerMpodLvHvSnmp::channelGetUserConfig(unsigned slot_index, unsigned channel_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.37."<<  snmp_channel_index;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetVoltage(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.10."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetVoltageFallRate(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.14."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::channelGetVoltageRiseRate(unsigned slot_index, unsigned channel_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.13."<<  snmp_channel_index;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetCurrent(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.12." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetCurrentFallRate(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.26." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}
bool WienerMpodLvHvSnmp::channelSetCurrentRiseRate(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.25." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetSupervisionBehavior(unsigned slot_index, unsigned channel_index, int value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.15." << snmp_channel_index;
    argout = snmpSetValueIntImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetSupervisionMaxCurrent(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.19." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetSupervisionMaxTerminalVoltage(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.18." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetSupervisionMinSenseVoltage(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.16." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetSwitch(unsigned slot_index, unsigned channel_index, int value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.9." << snmp_channel_index;
    argout = snmpSetValueIntImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetTripTime(unsigned slot_index, unsigned channel_index, int value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.27." << snmp_channel_index;
    argout = snmpSetValueIntImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetUserConfig(unsigned slot_index, unsigned channel_index, int value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.37." << snmp_channel_index;
    argout = snmpSetValueIntImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetVoltage(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.10." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetVoltageFallRate(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.14." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::channelSetVoltageRiseRate(unsigned slot_index, unsigned channel_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    unsigned snmp_channel_index = slot_index * 100 + channel_index + 1;
    cmd << "iso.3.6.1.4.1.19947.1.3.2.1.13." << snmp_channel_index;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive ) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

int WienerMpodLvHvSnmp::crateGetFanSpeed()
{
    int argout;
    std::string oid_str("iso.3.6.1.4.1.19947.1.7.6.0");
    argout = snmpGetValueInt(oid_str);
    return argout;
}

int WienerMpodLvHvSnmp::crateGetNumberOfChannels()
{
    int argout;
    std::string oid_str("iso.3.6.1.4.1.19947.1.3.1.0");
    argout = snmpGetValueInt(oid_str);
    recomputeCrateStructureIfNecessary((argout > 0), (argout < 0));
    return argout;
}

int WienerMpodLvHvSnmp::crateGetNumberOfGroups()
{
    int argout;
    std::string oid_str("iso.3.6.1.4.1.19947.1.3.3.0");
    argout = snmpGetValueInt(oid_str);
    return argout;
}

int WienerMpodLvHvSnmp::crateGetNumberOfModules()
{
    int argout;
    std::string oid_str("iso.3.6.1.4.1.19947.1.3.5.0");
    argout = snmpGetValueInt(oid_str);
    recomputeCrateStructureIfNecessary((argout > 0), (argout < 0));
    return argout;
}

std::vector<size_t> WienerMpodLvHvSnmp::crateGetOccupiedSlots()
{
    std::vector<size_t> occupied_slots;
    // crate might have been switched on or off
    recomputeCrateStructure();
    getOccupiedSlotsImpl(occupied_slots);
    return occupied_slots;
}

std::string WienerMpodLvHvSnmp::crateGetSerialNumber()
{
    std::string argout;
    // ~ WIENER-CRATE-MIB::psSerialNumber.0
    std::string oid_str("iso.3.6.1.4.1.19947.1.6.2.0");
    argout = snmpGetValueString((char*)(oid_str.c_str()));
    return argout;
}

int WienerMpodLvHvSnmp::crateGetSysMainSwitchImpl()
{
    int argout;
    std::string oid_str = getSysMainSwitchOid();
    argout = snmpGetValueInt(oid_str);
    return argout;
}

int WienerMpodLvHvSnmp::crateGetSysMainSwitch()
{
    int argout;
    argout = crateGetSysMainSwitchImpl();
    // recompute crate structure if result doesn't mach connection status
    recomputeCrateStructureIfNecessary((argout > 0), (argout < 0));
    return argout;
}

std::string WienerMpodLvHvSnmp::crateGetSysDescr()
{
    std::string argout;
    // ~ SNMPv2-MIB::sysDescr.0 = STRING: WIENER MPOD (4688040, MPOD 2.1.2212.0, MPODslave 1.09, MPOD-BL 1.50 )
    std::string oid_str("iso.3.6.1.2.1.1.1.0");
    argout = snmpGetValueString((char*)(oid_str.c_str()));
    return argout;
}

//--------------------------------------------------------
/**
 *    Command CrateGetSysStatus related method
 *    Description:
 *
 *    @returns
 */
//--------------------------------------------------------
int WienerMpodLvHvSnmp::crateGetSysStatus()
{
    int argout;
    std::string oid_str = getSysStatusOid();
    argout = snmpGetValueInt(oid_str);
    // recompute crate structure if result doesn't mach connection status
    recomputeCrateStructureIfNecessary((argout > 0 && checkBitIsSet(argout, 0)), (argout < 0));
    return argout;
}

std::string WienerMpodLvHvSnmp::crateGetSysStatusAsString()
{
    std::string argout;
    unsigned integer_status_flags;
    integer_status_flags = crateGetSysStatus();
    argout = convertCrateStatusToString(integer_status_flags);
    return argout;
}

bool WienerMpodLvHvSnmp::crateSetFanSpeed(int value)
{
    bool argout;
    std::string oid_str("iso.3.6.1.4.1.19947.1.7.6.0");
    argout = snmpSetValueIntImpl((char*)(oid_str.c_str()), value);
    return argout;
}

bool WienerMpodLvHvSnmp::crateSetSysHardwareReset(int value)
{
    bool argout;
    std::string oid_str("iso.3.6.1.4.1.19947.1.1.4.0");
    argout = snmpSetValueIntImpl((char*)(oid_str.c_str()), value);
    recomputeCrateStructure();
    return argout;
}

bool WienerMpodLvHvSnmp::crateSetSysMainSwitch(int value)
{
    bool argout;
    std::string oid_str = getSysMainSwitchOid();
    argout = snmpSetValueIntImpl((char*)(oid_str.c_str()), value);
    recomputeCrateStructure();
    return argout;
}

int WienerMpodLvHvSnmp::groupGetIndex(unsigned index)
{
    int argout;
    std::stringstream cmd;
    // groups are numbered form 0 ...  in function call
    //     and 0 ... in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.4.1.1." << index;
    argout = snmpGetValueIntImpl(cmd.str());
    return argout;
}

bool WienerMpodLvHvSnmp::groupSetSwitch(unsigned group_index, int value)
{
    bool argout;
    std::stringstream cmd;
    // groups are numbered form 0 ...  in function call
    //     and 0 ... in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.4.1.9." << group_index;
    argout = snmpSetValueIntImpl(cmd.str(), value);
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetAuxiliaryMeasurementVoltage0(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.3.1." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetAuxiliaryMeasurementVoltage1(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.3.2." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

std::string WienerMpodLvHvSnmp::moduleGetDescriptionImpl(unsigned slot_index)
{
    std::string argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueStringImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.2." << slot_index + 1;
    argout = snmpGetValueStringImpl(cmd.str().c_str());
    return argout;
}

std::string WienerMpodLvHvSnmp::moduleGetDescription(unsigned slot_index)
{
    std::string argout = moduleGetDescriptionImpl(slot_index);
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getStringError()));
    return argout;
}

int WienerMpodLvHvSnmp::moduleGetEventChannelStatus(unsigned slot_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.10." << slot_index + 1;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

int WienerMpodLvHvSnmp::moduleGetEventStatus(unsigned slot_index)
{
    int argout;

    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.9." << slot_index + 1;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

std::string WienerMpodLvHvSnmp::moduleGetEventStatusAsString(unsigned slot_index)
{
    std::string argout;
    unsigned integer_status_flags;
    integer_status_flags = moduleGetEventStatus(slot_index);
    argout = convertModuleEventStatusToString(integer_status_flags);
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetHardwareLimitCurrent(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.5." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetHardwareLimitVoltage(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.4." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

int WienerMpodLvHvSnmp::moduleGetIndex(unsigned slot_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.1." << slot_index + 1;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

bool WienerMpodLvHvSnmp::moduleGetIsHvModule(unsigned slot_index)
{
    bool argout;
    argout = checkSlotContainsHvModule(slot_index);
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetMeasurementTemperature0(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.12.1." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetMeasurementTemperature1(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.12.2." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetMeasurementTemperature2(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.12.3." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetMeasurementTemperature3(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.12.4." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

int WienerMpodLvHvSnmp::moduleGetNumberOfChannels(unsigned slot_index)
{
    int argout;
    argout = 0;
    if (slot_index < m_moduleNumberOfChannels.size()) {
        argout = m_moduleNumberOfChannels[slot_index];
    }
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetRampSpeedCurrent(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.7." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

double WienerMpodLvHvSnmp::moduleGetRampSpeedVoltage(unsigned slot_index)
{
    double argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.6." << slot_index + 1;
    argout = snmpGetValueDoubleImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getDoubleError()));
    return argout;
}

int WienerMpodLvHvSnmp::moduleGetStatus(unsigned slot_index)
{
    int argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.8." << slot_index + 1;
    argout = snmpGetValueIntImpl(cmd.str());
    recomputeCrateStructureIfModuleNotDetected(slot_index, (argout != getIntError()));
    return argout;
}

std::string WienerMpodLvHvSnmp::moduleGetStatusAsString(unsigned slot_index)
{
    std::string argout;
    unsigned integer_status_flags;
    integer_status_flags = moduleGetStatus(slot_index);
    argout = convertModuleStatusToString(integer_status_flags);
    return argout;
}

bool WienerMpodLvHvSnmp::moduleSetDoClear(unsigned slot_index, int value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpGetValueIntImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.11." << slot_index + 1;
    argout = snmpSetValueIntImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::moduleSetRampSpeedCurrent(unsigned slot_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpSetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.7." << slot_index + 1;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::moduleSetRampSpeedVoltage(unsigned slot_index, double value)
{
    bool argout;
    std::stringstream cmd;
    // modules are numbered form 0 ... 9 in function call
    //     and 1 ... 10 in snmpSetValueDoubleImpl()
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.6." << slot_index + 1;
    argout = snmpSetValueDoubleImpl(cmd.str(), value);
    if ( ! m_flagPollingInThreadIsActive) {
        recomputeCrateStructureIfModuleNotDetected(slot_index, argout);
    }
    return argout;
}

std::string WienerMpodLvHvSnmp::snmpGetDescription(const char *string_oid)
{

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    std::string argout;
    char buffer[4000];
    size_t buf_len = 4000;

    if (snmpGetNode(string_oid)) {
        if (snprint_description(buffer, buf_len, m_OIDVector, m_OIDVectorLength, 80)) {
            argout = std::string(buffer);
        }
    }
    return argout;
}

std::string WienerMpodLvHvSnmp::snmpGetNextOid(const std::string &string_oid) {
    DataContainer data;
    return snmpGetNextOid(string_oid, data);
}

bool WienerMpodLvHvSnmp::getDataFromSnmpVariable(struct variable_list *vars, DataContainer &data) const {
    uint64_t ui64, name_length;
    unsigned char ch;

    data.m_type = data_type_undefined;
    data.m_stringValue.clear();
    memcpy(data.m_OIDVector, vars->name, vars->name_length * sizeof(oid));
    data.m_OIDVectorLength = vars->name_length;

    if (vars) {
        switch(vars->type) {
        case ASN_OPAQUE_FLOAT:                          // 0x78
            data.m_doubleValue = *vars->val.floatVal;
            data.m_type = data_type_double;
            return true;
        case ASN_OPAQUE_DOUBLE:                         // 0x79
            data.m_doubleValue = *vars->val.doubleVal;
            data.m_type = data_type_double;
            return true;
        case ASN_INTEGER:                               // 0x02
            data.m_intValue = *vars->val.integer;
            data.m_type = data_type_int;
            return true;
        case ASN_BIT_STR:
        case ASN_OCTET_STR:
            if (checkOidDefinesStatusVariable(vars->name, vars->name_length)) {
#ifdef WIENER_MPOD_DEBUG_STATUS_VARIABLE_DETECTION
                std::cout << convertOidToString(vars->name, vars->name_length) << " is a status variable" << std::endl;
#endif
                if (vars->val_len < sizeof(unsigned long long)) {
                    ui64 = 0;
                    for (size_t i = 0; i < vars->val_len; i++) {
                        ch = vars->buf[i];
                        ch = inverseBits(ch);
                        ((char*)(&ui64))[i] = ch;
                    }
                    data.m_intValue = (int)ui64;
                    data.m_type = data_type_int;
                    return true;
                }
            }
            else {
#ifdef WIENER_MPOD_DEBUG_STATUS_VARIABLE_DETECTION
                std::cout << convertOidToString(vars->name, vars->name_length) << " ISN'T a status variable" << std::endl;
#endif
                if (vars->val.string && vars->val_len) {
                    data.m_stringValue = std::string((const char*)(vars->val.string), vars->val_len);
                    data.m_type = data_type_string;
                    return true;
                }
            }
            break;
         case ASN_OBJECT_ID:
             name_length = vars->val_len / sizeof(oid);
             data.m_stringValue = convertOidToString(vars->val.objid, name_length);
             data.m_type = data_type_string;
             return true;
        case SNMP_NOSUCHINSTANCE:
            break;
        default:
            break;
        }
    }
    return false;
}

std::string WienerMpodLvHvSnmp::snmpGetNextOid(const std::string &string_oid, DataContainer &data) {

    std::string argout;
    std::vector<DataContainer> data_vector;

    argout = snmpGetBulk(string_oid, 1, data_vector);
    if (data_vector.size()) {
        data = data_vector.front();
    }

    return argout;
}

std::string WienerMpodLvHvSnmp::snmpGetBulk(const std::string &string_oid, size_t nb, std::vector<DataContainer> &data) {

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    std::string argout;
    int r_status;
    netsnmp_pdu *pdu = 0;
    netsnmp_pdu *response = 0;
    variable_list *vars = 0;
    DataContainer single_data;

#ifdef WIENER_MPOD_DEBUG_NEXT_OID_FLAG
    std::cout << "<<< snmpGetBulk " << std::string(string_oid) << std::endl;
    std::cout << "    nb == " << nb << std::endl;
#endif

    if (nb > 20) {
        nb = 20;
    }

    data.clear();

    if (snmpGetNode(string_oid)) {

//        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        pdu = snmp_pdu_create(SNMP_MSG_GETBULK);
        pdu->max_repetitions = nb;
        pdu->non_repeaters = 0;

        snmp_add_null_var(pdu, m_OIDVector, m_OIDVectorLength);

        /*
        * Send the Request out.
        */
        r_status = snmp_synch_response(m_crate, pdu, &response);

        if (response) {
            if (r_status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
                vars = response->variables;
                while (vars) {
                    // make sure this is a new OID
                    getDataFromSnmpVariable(vars, single_data);
                    data.push_back(single_data);
                    vars = vars->next_variable;
                }
                if (data.size()) {
                    const DataContainer &last_data = data.back();
                    if (! compareOidToCurrentOidVector(last_data.m_OIDVector, last_data.m_OIDVectorLength)) {
                        argout = convertOidToString(last_data.m_OIDVector, last_data.m_OIDVectorLength);
                    }
                    snmpSucceededSetState();
                }
            }
            else {
                snmpFailedSetState(r_status, response->errstat, string_oid);
            }
            snmp_free_pdu(response);
        }
        else {
            snmpFailedSetState(r_status, SNMP_ERR_GENERR, string_oid);
#ifdef WIENER_MPOD_DEBUG_NEXT_OID_FLAG
            std::cout << "           snmp_synch_response() fails with code " << r_status << " ~ \"" << m_errorStr << "\" !" << std::endl;
#endif
        }
    }
#ifdef WIENER_MPOD_DEBUG_NEXT_OID_FLAG
    else {
        std::cout << "           snmpGetNode() fails !" << std::endl;
    }
#endif
    if (argout == string_oid) {
        argout.clear();
    }
#ifdef WIENER_MPOD_DEBUG_NEXT_OID_FLAG
    std::cout << ">>> snmpGetBulk returns \"" << argout << "\"" << std::endl;
#endif
    return argout;
}

std::string WienerMpodLvHvSnmp::snmpTranslateFromOid(const char *string_oid) {

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    std::string argout;
    char buffer[1000];
    size_t buf_len = 1000;

    if (snmpGetNode(string_oid)) {
        if (snprint_objid(buffer, buf_len, m_OIDVector, m_OIDVectorLength)) {
            argout = std::string(buffer);
        }
    }
    return argout;
}

std::string WienerMpodLvHvSnmp::snmpTranslateToOid(const char *string_oid) {

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    std::string argout;
    std::string res;

    m_OIDVectorLength = MAX_OID_LEN;
    if (read_objid(string_oid, m_OIDVector, &m_OIDVectorLength)) {
        argout = convertOidToString(m_OIDVector, m_OIDVectorLength);
    }
    else {
        setErrorString("Call to read_objid() failed.");
    }
    return argout;
}

void WienerMpodLvHvSnmp::snmpInit(void) {
    {
        AfThreadsMutexLocker lock(m_snmpAccessMutex);

#ifdef WIENER_MPOD_DEBUG_SNMP_FLAG
        snmp_set_do_debugging(1);
#endif
        init_snmp("af_snmp");
        init_mib();

        // read specific mibs (default at /usr/share/snmp/mibs)
        read_module("WIENER-CRATE-MIB");
    }

    // ~ sys main switch
    std::string oid_str = getSysMainSwitchOid();
    std::string var_name = snmpTranslateFromOid(oid_str);
    m_flagMibFileIsLoaded = false;
    if (var_name.find("MainSwitch") < var_name.size()) {
        m_flagMibFileIsLoaded = true;
    }
}

void WienerMpodLvHvSnmp::snmpFailedSetState(int r_status, long snmp_errstat, const std::string &string_oid) {
    const char *err_str = 0;
    char *err = 0;
    netsnmp_session *session = 0;

    if (r_status == STAT_SUCCESS){
        setErrorString("Error in packet.");
        switch (snmp_errstat) {
        case SNMP_ERR_NOACCESS:
            setErrorString("Error in packet with oid " + std::string(string_oid) + ", Reason: unsufficient access rights (the DS property snmpCommunityName = " + m_snmpCommunityName + " might have to be changed).");
            break;
        case SNMP_MSG_INTERNAL_OBJECT_LOOKUP:
            // The requested object wasn't found
            //     we need to check if the crate was switched off.
            m_flagCheckOnOff = true;
            setErrorString("Error in packet, Reason: object with oid " + std::string(string_oid) +  " not found.");
            break;
        default:
            err_str = snmp_errstring(snmp_errstat);
            if (err_str) {
                setErrorString("Error in packet with oid " + std::string(string_oid) + ", Reason: " + std::string(err_str));
            }
        }
    }
    else if (r_status == STAT_TIMEOUT) {
        m_snmpTimeoutCounter++;
#ifdef WIENER_MPOD_DEBUG_TIMEOUT
        std::cout << "timeout counter == " << m_snmpTimeoutCounter << std::endl;
#endif
        if ( m_snmpTimeoutCounter >= m_snmpTimeoutCounterMax ) {
#ifdef WIENER_MPOD_DEBUG_TIMEOUT
            std::cout << "timeout counter exceeded maximum => disconnected" << std::endl;
#endif
            setStatusDisconnected(mpod_snmp_connection_status_timeout);
        }
        setErrorString("SNMP protocol timeout.");
    }
    else{
        // Caller must free the string returned after use.
        if (m_crate) {
            session = snmp_sess_session(m_crate);
            if (session) {
                snmp_error(session, 0, 0, &err);
            }
        }
        if (err) {
            setErrorString("SNMP error with oid " + std::string(string_oid) + ": " + std::string(err) + ".");
            SNMP_FREE(err);
        }
        else {
            setErrorString("SNMP error with oid " + std::string(string_oid) + ".");
        }
        setStatusDisconnected(mpod_snmp_connection_status_error);
    }
}

void WienerMpodLvHvSnmp::setStatusDisconnected(mpod_snmp_connection_status_type new_status) {

    AfThreadsMutexLocker lock(m_dataAccessMutex);

    setConnectionStatus(new_status);
    m_moduleTypes.clear();
    m_moduleNumberOfChannels.clear();
    m_moduleFirstChannelIndex.clear();
    m_readCrateData.clear();
    m_readGenericData.clear();
    m_executionQueue.clear();
    freeMeasuredDataBuffers();
}

void WienerMpodLvHvSnmp::snmpSucceededSetState() {
    if (!checkConnectionStatusOk()) {
#ifdef WIENER_MPOD_DEBUG_FLAG
        std::cout << "WIENER_MPOD_DEBUG_FLAG: getConnectionStatus() == " << (size_t)getConnectionStatus() << std::endl;
#endif
        lockAndSetConnectionStatus(mpod_snmp_connection_status_connected);
#ifdef WIENER_MPOD_DEBUG_FLAG
        std::cout << "               ==> set to " << (size_t)getConnectionStatus() << std::endl;
#endif
    }
#ifdef WIENER_MPOD_DEBUG_TIMEOUT
    std::cout << "timeout counter reset." << std::endl;
#endif
    m_snmpTimeoutCounter = 0;
    clearErrorString();
}

void WienerMpodLvHvSnmp::recomputeCrateStructureIfModuleNotDetected(unsigned slot_index, bool flag_command_succeeded) {

    // the slot index refers to a module that has not yet considered in
    //     m_moduleTypes ==> recomputeCrateStructure

    bool flag_check = false;
    if (flag_command_succeeded) {
        if (slot_index < m_moduleTypes.size()) {
            if (m_moduleTypes[slot_index] == module_type_undefined) {
#ifdef WIENER_MPOD_DEBUG_ON_OFF_FLAG
                std::cout << "command succeeded and slot_index empty --> checking crate status" << std::endl;
#endif
                flag_check = true;
            }
        }
        else {
#ifdef WIENER_MPOD_DEBUG_ON_OFF_FLAG
            std::cout << "command succeeded and slot_index >= m_moduleTypes.size() --> checking crate status" << std::endl;
#endif
            flag_check = true;
        }
    }
    else {
        if (slot_index < m_moduleTypes.size()) {
            if (m_moduleTypes[slot_index] != module_type_undefined) {
#ifdef WIENER_MPOD_DEBUG_ON_OFF_FLAG
                std::cout << "command failed and slot_index not empty --> checking crate status" << std::endl;
#endif
                flag_check = true;
            }
        }
        if (m_flagCheckOnOff && lockAndCheckConnectionStatusSwitchedOn()) {
#ifdef WIENER_MPOD_DEBUG_ON_OFF_FLAG
            std::cout << "command failed and OID wasn't found for slot_index --> checking crate status" << std::endl;
#endif
            flag_check = true;
        }
    }

    if (flag_check) {
        recomputeCrateStructure(true);
    }
}

size_t WienerMpodLvHvSnmp::getNumberOfDetecteModules() {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    return m_moduleTypes.size();
}

void WienerMpodLvHvSnmp::recomputeCrateStructureIfNecessary(bool flag_crate_presumed_on, bool flag_crate_presumed_disconnected) {

    // we recompute the crate structure if the connection status doesn't match flag_crate_presumed_on

    mpod_snmp_connection_status_type stat = lockAndGetCrateConnectionStatus();
    bool flag_recompute = false;
    if (flag_crate_presumed_disconnected) {
        switch (stat) {
        case mpod_snmp_connection_status_disconnected:
        case mpod_snmp_connection_status_timeout:
            flag_recompute = false;
            break;
        default:
            flag_recompute = true;
            break;
        }
    }
    else if (flag_crate_presumed_on) {
        switch (stat) {
        case mpod_snmp_connection_status_on:
            flag_recompute = false;
            break;
        default:
            flag_recompute = true;
            break;
        }
    }
    else {
        switch (stat) {
        case mpod_snmp_connection_status_off:
            flag_recompute = false;
            break;
        default:
            flag_recompute = true;
            break;
        }
    }
    if (flag_recompute) {
        recomputeCrateStructure(true);
    }
}

void WienerMpodLvHvSnmp::recomputeCrateStructure(bool flag_force) {

    size_t slot_index, channel_index;
    int snmp_channel_index;
    std::string result;
    long int_result;
    bool flag_is_hv_module;
    size_t nb_of_channels;
    mpod_snmp_connection_status_type old_status = mpod_snmp_connection_status_undefined;
    mpod_snmp_connection_status_type new_status = mpod_snmp_connection_status_undefined;
    long sys_switch;
    size_t measured_data_buffer_allocated_size;

    sys_switch = crateGetSysMainSwitchImpl();
    old_status = getConnectionStatus();
    if (old_status == mpod_snmp_connection_status_on_structure_changed
            || old_status == mpod_snmp_connection_status_off_structure_changed) {
        flag_force = true;
    }
    if (m_moduleTypes.size() == 0) {
        old_status = mpod_snmp_connection_status_off;
    }

    {
        AfThreadsMutexLocker lock(m_dataAccessMutex);
        if (sys_switch == 1) {
            setConnectionStatus(mpod_snmp_connection_status_on);
        }
        else if (sys_switch == 0) {
            setConnectionStatus(mpod_snmp_connection_status_off);
        }
        /* this is set by snmp requests only
        else if (sys_switch == -1) {
            setConnectionStatus(mpod_snmp_connection_status_disconnected);
        }
        */
        new_status = getConnectionStatus();
    }

    if ( flag_force || old_status != new_status ) {
#ifdef WIENER_MPOD_DEBUG_CRATE_STRUCTURE
        std::cout << "re-computing crate structure" << std::endl;
#endif
        {
            AfThreadsMutexLocker lock(m_dataAccessMutex);

            m_moduleTypes.clear();
            m_moduleNumberOfChannels.clear();
            m_moduleFirstChannelIndex.clear();
            m_flagCheckOnOff = false;
            m_totalNumberOfChannels = 0;
            measured_data_buffer_allocated_size = m_measuredDataBufferAllocatedSize;
            if (measured_data_buffer_allocated_size == 0) {
                measured_data_buffer_allocated_size = m_measuredDataBufferInitialSize;
            }
        }

        if (sys_switch == 1) {
            for (slot_index = 0; slot_index < 20; slot_index++) {
                result = moduleGetDescriptionImpl(slot_index);
                if (! result.empty()) {
                    // slot is occupied => find out if it is a HV slot
                    flag_is_hv_module = checkDescribesHvModule(result);
                    nb_of_channels = 0;
                    for (channel_index = 0; channel_index < 100; channel_index++) {
                        snmp_channel_index = slot_index * 100 + channel_index + 1;
                        int_result = channelGetStatusImpl(slot_index, channel_index);
                        if (int_result < 0) {
                            nb_of_channels = channel_index;
                            break;
                        }
                    }
                    {
                        AfThreadsMutexLocker lock(m_dataAccessMutex);
                        m_moduleTypes.resize(slot_index + 1, module_type_undefined);
                        m_moduleTypes[slot_index] = module_type_lv;
                        m_moduleNumberOfChannels.resize(slot_index + 1, 0);
                        m_moduleNumberOfChannels[slot_index] = nb_of_channels;
                        m_moduleFirstChannelIndex.resize(slot_index + 1, m_totalNumberOfChannels);
                        m_totalNumberOfChannels += nb_of_channels;
                        if (flag_is_hv_module) {
                            m_moduleTypes[slot_index] = module_type_hv;
#ifdef WIENER_MPOD_DEBUG_FLAG
                            std::cout << "WIENER_MPOD_DEBUG_FLAG: slot " << slot_index << " contains a HV module." << std::endl;
#endif
                        }
                    }
#ifdef WIENER_MPOD_DEBUG_FLAG
                    else {
                        std::cout << "WIENER_MPOD_DEBUG_FLAG: slot " << slot_index << " contains a LV module." << std::endl;
                    }
#endif
#ifdef WIENER_MPOD_DEBUG_FLAG
                    std::cout << "WIENER_MPOD_DEBUG_FLAG: slot " << slot_index << " has " << nb_of_channels << " channels." << std::endl;
#endif
                }
#ifdef WIENER_MPOD_DEBUG_FLAG
                else {
                    std::cout << "WIENER_MPOD_DEBUG_FLAG: slot " << slot_index << " is inoccupied." << std::endl;
                }
#endif
            }
        }

        resizeMeasuredDataBuffer(measured_data_buffer_allocated_size);
    }
}

bool WienerMpodLvHvSnmp::snmpGetNode(const std::string &string_oid) {

    // mutex must have been locked above

    m_OIDVectorLength = MAX_OID_LEN;
    if ( ! string_oid.empty()) {
        if (get_node(string_oid.c_str(), m_OIDVector,&m_OIDVectorLength)) {
            return true;
        }
        else {
            // setStatusDisconnected();
            setErrorString("The snmp function get_node() failed.");
        }
    }
    return false;
}

bool WienerMpodLvHvSnmp::snmpGetImpl(const std::string &string_oid, snmp_pdu **response) {

    // mutex must have been locked above

    int r_status;
    int count_vars = 0;
    netsnmp_pdu *pdu = 0; // remark : must not be freed !
    variable_list *vars = 0;
    bool flag_success = false;

    *response = 0;
    m_flagCheckOnOff = false;
    if (!m_crate) {
        return false;
    }
    if (! snmpGetNode(string_oid)) {
        return false;
    }

    pdu = snmp_pdu_create(SNMP_MSG_GET);
    snmp_add_null_var(pdu, m_OIDVector, m_OIDVectorLength);

    /*
    * Send the Request out.
    */
    // r_status = snmp_synch_response(m_crate, pdu, response);
    r_status = snmp_synch_response(m_crate, pdu, response);

    if (*response) {
        if (r_status == STAT_SUCCESS && (*response)->errstat == SNMP_ERR_NOERROR) {
            for (vars = (*response)->variables; vars; vars = vars->next_variable) {
                count_vars++;
            }
            if (count_vars == 1) {
                flag_success = true;
                snmpSucceededSetState();
            }
            else {
                if (count_vars == 0) {
                    setErrorString("Empty answer");
                }
                else {
                    setErrorString("More than 1 answer at a time");
                }
            }
        }
        else {
            snmpFailedSetState(r_status, (*response)->errstat, string_oid);
        }
    }
    else {
        snmpFailedSetState(r_status, SNMP_ERR_GENERR, string_oid);
    }

    return flag_success;
}

bool WienerMpodLvHvSnmp::compareOidToCurrentOidVector(const oid *name, size_t name_length) const {

    // returns true if vectors are identical

    if (name_length != m_OIDVectorLength) {
        return false;
    }
    for (size_t i = 0; i < name_length; i++) {
        if (name[i] != m_OIDVector[i]) {
            return false;
        }
    }
    return true;
}

std::string WienerMpodLvHvSnmp::convertOidToString(const oid *name, size_t name_length, size_t offset) const {
    std::stringstream tmp;
    if (offset < name_length) {
        if ( offset == 0) {
            tmp << "iso";
            for (size_t i = 1; i < name_length; i++) {
                tmp << "." << name[i];
            }
            return tmp.str();
        }
        tmp << name[offset];
        for (size_t i = offset + 1; i < name_length; i++) {
            tmp << "." << name[i];
        }
        return tmp.str();
    }
    return tmp.str();
}

void WienerMpodLvHvSnmp::snmpSetTimeoutInterval(int interval_in_ms) {
    AfThreadsMutexLocker lock(m_snmpAccessMutex);
    m_timeoutIntervalMs = interval_in_ms;
}

struct snmp_session *WienerMpodLvHvSnmp::snmpOpen(const char* ip_address, int port_index, const char *community_name) {

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    snmp_session *sessp = 0;
    char *end_ptr;
    long l;
    std::stringstream tmp;
    if (ip_address && community_name) {

        size_t pos;
        std::string sub_str_1, sub_str_0;
        std::string ip = std::string(ip_address);
        std::string community = std::string(community_name);
        std::string cmp = "tcpip";

        pos = ip.find(':');
        // check if we have a labview-style declaration : "TCPIP0::10.144.10.61::10161::guru"
        if (pos + 1 < ip.size() && ip[pos + 1] == ':') {
            sub_str_0 = ip.substr(0, pos);
            std::transform(sub_str_0.begin(), sub_str_0.end(), sub_str_0.begin(), ::tolower);
            if (sub_str_0.substr(0, cmp.size()) == cmp) {
                ip = ip.substr(pos + 2);
                pos = ip.find(':');
            }
        }
        if (pos < ip.size()) {
            sub_str_0 = ip.substr(pos + 1);
            if (sub_str_0.size() && sub_str_0[0] == ':') {
                sub_str_0 = ip.substr(pos + 2);
            }
            ip = ip.substr(0, pos);
            pos = sub_str_0.find(':');
            if (pos < sub_str_0.size()) {
                sub_str_1 = sub_str_0.substr(pos + 1);
                if (sub_str_1.size() && sub_str_1[0] == ':') {
                    sub_str_1 = sub_str_0.substr(pos + 2);
                }
                sub_str_0 = sub_str_0.substr(0, pos);
            }
        }
        if (! sub_str_1.empty()) {
            community = sub_str_1;
        }
        if (! sub_str_0.empty()) {
            l = strtol(sub_str_0.c_str(), &end_ptr, 10);
            if (end_ptr == sub_str_0.c_str() + sub_str_0.size()) {
                port_index = l;
            }
            else if (sub_str_1.empty()) {
                community = sub_str_0;
            }
        }
        if (! ip.empty()) {
            if (community.empty()) {
                community = "guru";
            }
            if (port_index == 0) {
                port_index = 161;
            }

            tmp << ip << ":" << port_index;
            m_ipAddr = tmp.str();
            m_snmpCommunityName = community;
            m_portIndex = port_index;


            if (m_session == 0) {
                m_session = new netsnmp_session;
            }
            snmp_sess_init(m_session);                   /* set up defaults */
            m_session->version = SNMP_VERSION_2c;
            m_session->peername = (char*)(m_ipAddr.c_str());
            m_session->community = (u_char*)(m_snmpCommunityName.c_str());
            m_session->community_len = m_snmpCommunityName.size();
            if (m_timeoutIntervalMs >= 0) {
                // otherwise the net-snmp library default is used
                m_session->timeout = m_timeoutIntervalMs * 1000;   // timeout (us)
            }
            m_session->retries = 0;        // retries

            sessp = snmp_open(m_session);
        }
    }

    return sessp;
}

unsigned char WienerMpodLvHvSnmp::inverseBits(unsigned char ch) const {
    unsigned char res = 0;

    for (size_t i = 0; i < 8; ++i) {
        res = res << 1;
        res = res | (ch &0x01);
        ch = ch >> 1;
    }
    return res;
}

bool WienerMpodLvHvSnmp::checkIsCrateOid(const std::string &oid_str, std::string &simple_oid) const {
    if (oid_str.substr(0, 20) == "iso.3.6.1.4.1.19947.") {
        simple_oid = oid_str.substr(20);
        return true;
    }
    return false;
}
bool WienerMpodLvHvSnmp::checkIsCrateOid(const DataContainer &data, std::string &simple_oid) const {
    if (data.m_OIDVectorLength >= 7) {
        if (data.m_OIDVector[6] == 19947) {
            simple_oid = convertOidToString(data.m_OIDVector, data.m_OIDVectorLength, 7);
            return true;
        }
    }
    return false;
}
bool WienerMpodLvHvSnmp::checkIsChannelOid(const DataContainer &data) const {
    // 19947.1.3.2.1.x.y.z
    if (data.m_OIDVectorLength >= 12) {
        if (data.m_OIDVector[6] == 19947) {
            if (data.m_OIDVector[7] == 1) {
                if (data.m_OIDVector[8] == 3) {
                    if (data.m_OIDVector[9] == 2) {
                        if (data.m_OIDVector[10] == 1) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool WienerMpodLvHvSnmp::checkIsSystemOid(const DataContainer &data) const {
    // 19947.1.1.x.y.z
    if (data.m_OIDVectorLength >= 10) {
        if (data.m_OIDVector[6] == 19947) {
            if (data.m_OIDVector[7] == 1) {
                if (data.m_OIDVector[8] == 1) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool WienerMpodLvHvSnmp::checkIsMeasurementOid(const std::string &oid_str, std::string &simple_oid) const {
    if ( checkIsCrateOid(oid_str, simple_oid) ) {
        if (simple_oid.substr(0, 8) == "1.3.2.1.") {
            switch ( simple_oid[8] ) {
            case '5':
            case '6':
            case '7':
                return true;
            default:
                break;
            }
        }
    }
    return false;
}
bool WienerMpodLvHvSnmp::checkIsOutputStatusOid(const std::string &oid_str, std::string &simple_oid) const {
    if ( checkIsCrateOid(oid_str, simple_oid) ) {
        if (simple_oid.substr(0, 10) == "1.3.2.1.4.") {
            return true;
        }
    }
    return false;
}

int WienerMpodLvHvSnmp::snmpGetValueIntImpl(const std::string &string_oid) {
    DataContainer data;
    if (m_flagPollingInThreadIsActive) {
        snmpGetValueFromBuffer(string_oid, data);
    }
    else {
        snmpGetValueFromSnmp(string_oid, data);
    }
    if (data.m_type == data_type_int) {
        return data.m_intValue;
    }
    return getIntError();
}

int WienerMpodLvHvSnmp::snmpGetValueIntFromSnmp(const std::string &string_oid) {
    DataContainer data;
    snmpGetValueFromSnmp(string_oid, data);
    if (data.m_type == data_type_int) {
        insertData(data);
        return data.m_intValue;
    }
    return getIntError();
}

bool WienerMpodLvHvSnmp::snmpGetValueFromSnmp(const std::string &string_oid, DataContainer &data) {

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    netsnmp_pdu *response = 0;
    variable_list *vars = 0;
    bool argout = false;

    data.m_type = data_type_undefined;
    data.m_stringValue.clear();
    if (snmpGetImpl(string_oid, &response)) {
        vars = response->variables;
        if (vars) {
            argout = getDataFromSnmpVariable(vars, data);
        }
    }
    if (response) {
        snmp_free_pdu(response);
    }
    return argout;
}

std::string WienerMpodLvHvSnmp::snmpGetValueStringImpl(const std::string &string_oid) {
    DataContainer data;
    if (m_flagPollingInThreadIsActive) {
        snmpGetValueFromBuffer(string_oid, data);
    }
    else {
        snmpGetValueFromSnmp(string_oid, data);
    }
    if (data.m_type == data_type_string) {
        return data.m_stringValue;
    }
    return getStringError();
}
void WienerMpodLvHvSnmp::snmpGetValueFromBuffer(const std::string &string_oid, DataContainer &data) {

    AfThreadsMutexLocker lock(m_dataAccessMutex);

    std::string simple_oid;
    std::map<std::string, DataContainer> ::const_iterator it;
    data.m_type = data_type_undefined;
    data.m_stringValue.clear();
    if (checkIsCrateOid(string_oid, simple_oid)) {
        it = m_readCrateData.find(simple_oid);
        if (it != m_readCrateData.end()) {
            data = it->second;
        }
    }
    else {
        it = m_readGenericData.find(string_oid);
        if (it != m_readGenericData.end()) {
            data = it->second;
        }
    }
}

double WienerMpodLvHvSnmp::snmpGetValueDoubleImpl(const std::string &string_oid) {
    DataContainer data;
    if (m_flagPollingInThreadIsActive) {
        snmpGetValueFromBuffer(string_oid, data);
    }
    else {
        snmpGetValueFromSnmp(string_oid, data);
    }
    if (data.m_type == data_type_double) {
        return data.m_doubleValue;
    }
    return getDoubleError();
}

bool WienerMpodLvHvSnmp::appendToExecutionQueueDouble(const std::string &string_oid, double value) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    DataContainer d(value);
    m_executionQueue.push_back(std::make_pair(string_oid, d));
#ifdef WIENER_MPOD_DEBUG_EXECUTION_QUEUE
    std::cout << "appending " << string_oid << " = " << value << " to execution queue." << std::endl;
#endif
    return true;
}

bool WienerMpodLvHvSnmp::snmpSetValueDoubleImpl(const std::string &string_oid, double value) {
    if ( m_flagPollingInThreadIsActive ) {
        // the value is simply written to the execution queue
        return appendToExecutionQueueDouble(string_oid, value);
    }
    return snmpSetValueDoubleOverSnmp(string_oid, value);
}

bool WienerMpodLvHvSnmp::snmpSetValueDoubleOverSnmp(const std::string &string_oid, double value) {

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    int r_status;
    snmp_pdu *pdu_w; // (remark) must not be freed
    snmp_pdu *response_w;
    float fv;
    bool argout;

    argout = false;
    if (! snmpGetNode(string_oid)) {
        return argout;
    }

    pdu_w = snmp_pdu_create(SNMP_MSG_SET);    // prepare set-request pdu
    pdu_w->community = (u_char*)strdup(m_snmpCommunityName.c_str());
    pdu_w->community_len = strlen((char*)pdu_w->community);


    fv = (float)value;
    snmp_pdu_add_variable(pdu_w,m_OIDVector, m_OIDVectorLength, ASN_OPAQUE_FLOAT,(u_char*)&fv,sizeof(fv));

    /*
     * Send the Request out.
     */
    r_status = snmp_synch_response(m_crate, pdu_w, &response_w);

    if (response_w) {
        if (r_status == STAT_SUCCESS && response_w->errstat == SNMP_ERR_NOERROR) {
            snmpSucceededSetState();
            argout = true;
        } else {
            snmpFailedSetState(r_status, response_w->errstat, string_oid);
        }
        snmp_free_pdu(response_w);
    }
    else {
        snmpFailedSetState(r_status, SNMP_ERR_GENERR, string_oid);
    }
    return argout;
}

bool WienerMpodLvHvSnmp::appendToExecutionQueueInt(const std::string &string_oid, int value) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    DataContainer d(value);
    m_executionQueue.push_back(std::make_pair(string_oid, d));
#ifdef WIENER_MPOD_DEBUG_EXECUTION_QUEUE
    std::cout << "appending " << string_oid << " = " << value << " to execution queue." << std::endl;
#endif

    return true;
}

bool WienerMpodLvHvSnmp::snmpSetValueIntImpl(const std::string &string_oid, int value) {

    if ( m_flagPollingInThreadIsActive ) {
        // the value is simply written to the execution queue
        return appendToExecutionQueueInt(string_oid, value);
    }
    return snmpSetValueIntOverSnmp(string_oid, value);
}

bool WienerMpodLvHvSnmp::snmpSetValueIntOverSnmp(const std::string &string_oid, int value) {

    AfThreadsMutexLocker lock(m_snmpAccessMutex);

    bool argout;
    int r_status;
    snmp_pdu *pdu_w; // (remark)  must not be freed
    snmp_pdu *response_w;
    int iv;

    argout = false;
    if (! snmpGetNode(string_oid)) {
        return argout;
    }

    pdu_w = snmp_pdu_create(SNMP_MSG_SET);    // prepare set-request pdu
    pdu_w->community = (u_char*)strdup(m_snmpCommunityName.c_str());
    pdu_w->community_len = strlen((char*)pdu_w->community);


    iv     = value;
    snmp_pdu_add_variable(pdu_w,m_OIDVector, m_OIDVectorLength, ASN_INTEGER,(u_char*)&iv,sizeof(iv));


    /*
     * Send the Request out.
     */
    r_status = snmp_synch_response(m_crate, pdu_w, &response_w);

    if (response_w) {
        if (r_status == STAT_SUCCESS && response_w->errstat == SNMP_ERR_NOERROR) {
            snmpSucceededSetState();
            argout = true;
        } else {
            snmpFailedSetState(r_status, response_w->errstat, string_oid);
        }
        snmp_free_pdu(response_w);
    }
    else {
        snmpFailedSetState(r_status, SNMP_ERR_GENERR, string_oid);
    }

    return argout;
}

void WienerMpodLvHvSnmp::getOccupiedSlotsImpl(std::vector<size_t> &occupied_slots) const {
    occupied_slots.clear();
    for (size_t i = 0; i < m_moduleTypes.size(); i++) {
        switch (m_moduleTypes[i]) {
        case module_type_lv:
        case module_type_hv:
            occupied_slots.push_back(i);
            break;
        default:
            break;
        }
    }
}


std::string WienerMpodLvHvSnmp::convertModuleEventStatusToString(unsigned long int_status) const {
    std::string result = "off";

    if (checkBitIsSet(int_status, 6)) {
        result = "input error";
    }
    if (checkBitIsSet(int_status, 2)) {
        result = "live insertion";
    }
    if (checkBitIsSet(int_status, 0)) {
        result = "power supply failure";
    }
    if (checkBitIsSet(int_status, 4)) {
        result = "maintenance required";
    }
    if (checkBitIsSet(int_status, 5)) {
        result = "voltage limit exceeded";
    }
    if (checkBitIsSet(int_status, 10)) {
        result = "safety loop open";
    }
    if (checkBitIsSet(int_status, 13)) {
        result = "supply failure";
    }
    if (checkBitIsSet(int_status, 14)) {
        result = "maximum temperature exceeded";
    }

    return result;
}

bool WienerMpodLvHvSnmp::checkDescribesHvModule(std::string &description) const {
    std::transform(description.begin(), description.end(), description.begin(), ::tolower);
    if (description.find("iseg") != std::string::npos) {
        return true;
    }
    return false;
}

bool WienerMpodLvHvSnmp::checkSlotContainsHvModule(unsigned slot_index) {
    if (slot_index < m_moduleTypes.size()) {
        if (m_moduleTypes[slot_index] == module_type_hv) {
            return true;
        }
    }
    return false;
    /*
    // we check if the description contains "iseg"

    std::stringstream cmd;
    cmd << "iso.3.6.1.4.1.19947.1.3.6.1.2." << slot_index + 1;
    std::string res = snmpGetValueStringImpl(cmd.str().c_str());
    if (checkDescribesHvModule(res)) {
#ifdef WIENER_MPOD_DEBUG_FLAG
        std::cout << "WIENER_MPOD_DEBUG_FLAG: slot " << slot_index << " contains a HV module." << std::endl;
#endif
        return true;
    }
#ifdef WIENER_MPOD_DEBUG_FLAG
    std::cout << "WIENER_MPOD_DEBUG_FLAG: slot " << slot_index << " contains a LV module." << std::endl;
#endif
    return false;
    */
}

std::string WienerMpodLvHvSnmp::convertChannelStatusToString(unsigned long int_status, unsigned slot_index) {
    std::string result = "off";

    bool flag_hv = checkSlotContainsHvModule(slot_index);

    if (checkBitIsSet(int_status, 0)) {
        result = "on";
    }
    if (checkBitIsSet(int_status, 11)) {
        result = "ramp up";
    }
    if (checkBitIsSet(int_status, 12)) {
        result = "ramp down";
    }
    if (checkBitIsSet(int_status, 10)) {
        result = "constant current";
    }
    if (flag_hv) {
        /*
        if (checkBitIsSet(int_status, 13)) {
            result = "kill enabled";
        }
        */
        /*
        if (checkBitIsSet(int_status, 15)) {
            result = "adjusting";
        }
        if (checkBitIsSet(int_status, 17)) {
            result = "low current range";
        }
        */
        if (checkBitIsSet(int_status, 16)) {
            result = "constant voltage";
        }
        if (checkBitIsSet(int_status, 5)) {
            result = "current trip";
        }
        if (checkBitIsSet(int_status, 1)) {
            result = "inhibit";
        }
        if (checkBitIsSet(int_status, 14)) {
            result = "emergency";
        }
        if (checkBitIsSet(int_status, 18)) {
            result = "voltage bounds exceeded";
        }
        if (checkBitIsSet(int_status, 4)) {
            result = "voltage limit";
        }
        if (checkBitIsSet(int_status, 19)) {
            result = "current limit exceeded";
        }
    }
    else {
        // low voltage channel
        if (checkBitIsSet(int_status, 9)) {
            result = "timeout occurred";
        }
        if (checkBitIsSet(int_status, 1)) {
            result = "inhibit";
        }
        if (checkBitIsSet(int_status, 2)) {
            result = "sense voltage below minimum";
        }
        if (checkBitIsSet(int_status, 3)) {
            result = "max sense voltage exceeded";
        }
        if (checkBitIsSet(int_status, 4)) {
            result = "max terminal voltage exceeded";
        }
        if (checkBitIsSet(int_status, 5)) {
            result = "max current exceeded";
        }
        if (checkBitIsSet(int_status, 6)) {
            result = "max temperature exceeded";
        }
        if (checkBitIsSet(int_status, 7)) {
            result = "max load power exceeded";
        }
    }
    return result;
}

std::string WienerMpodLvHvSnmp::convertModuleStatusToString(unsigned long int_status) const {
    std::string result = "off";

    if (checkBitIsSet(int_status, 6)) {
        result = "input error";
    }
    if (checkBitIsSet(int_status, 3)) {
        result = "HV on";
    }
    /*
    if (checkBitIsSet(int_status, 15)) {
        result = "kill enabled";
    }
    if (checkBitIsSet(int_status, 0)) {
        result = "adjusting";
    }
    */
    if (! checkBitIsSet(int_status, 9)) {
        result = "ramping";
    }
    if (checkBitIsSet(int_status, 11)) {
        result = "event occurred";
    }
    if (checkBitIsSet(int_status, 2)) {
        result = "live insertion";
    }
    if (! checkBitIsSet(int_status, 8)) {
        result = "sum error";
    }
    if (! checkBitIsSet(int_status, 12)) {
        result = "module error";
    }
    if (checkBitIsSet(int_status, 4)) {
        result = "maintenance required";
    }
    if (checkBitIsSet(int_status, 5)) {
        result = "voltage limit exceeded";
    }
    if (! checkBitIsSet(int_status, 10)) {
        result = "safety loop open";
    }
    if (! checkBitIsSet(int_status, 13)) {
        result = "supply failure";
    }
    if (! checkBitIsSet(int_status, 14)) {
        result = "maximum temperature exceeded";
    }

    return result;
}

void WienerMpodLvHvSnmp::lockAndSetConnectionStatusStructureChanged() {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    switch (m_connectionStatus) {
    case mpod_snmp_connection_status_on:
        m_connectionStatus = mpod_snmp_connection_status_on_structure_changed;
        break;
    case mpod_snmp_connection_status_off:
        m_connectionStatus = mpod_snmp_connection_status_off_structure_changed;
        break;
    default:
        break;
    }
}
void WienerMpodLvHvSnmp::lockAndSetConnectionStatus(mpod_snmp_connection_status_type s) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    m_connectionStatus = s;
}
mpod_snmp_connection_status_type WienerMpodLvHvSnmp::lockAndGetCrateConnectionStatus() {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    return m_connectionStatus;
}
std::string WienerMpodLvHvSnmp::lockAndGetCrateConnectionStatusAsString() {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    return convertConnectionStatusToString(m_connectionStatus);
}
bool WienerMpodLvHvSnmp::lockAndCheckConnectionStatusSwitchedOn() {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    return (m_connectionStatus == mpod_snmp_connection_status_on
            || m_connectionStatus == mpod_snmp_connection_status_on_structure_changed);
}
bool WienerMpodLvHvSnmp::lockAndCheckConnectionStatusOffOrDisconnected() {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    return (m_connectionStatus == mpod_snmp_connection_status_off
            || m_connectionStatus == mpod_snmp_connection_status_disconnected
            || m_connectionStatus == mpod_snmp_connection_status_off_structure_changed);
}

bool WienerMpodLvHvSnmp::lockAndCheckCrateConnectionIsOk() {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    return checkConnectionStatusOk();
}

std::string WienerMpodLvHvSnmp::convertConnectionStatusToString(mpod_snmp_connection_status_type integer_status) const {
    switch (integer_status) {
    case mpod_snmp_connection_status_undefined:
        return "undefined";
    case mpod_snmp_connection_status_error:
        return "error";
    case mpod_snmp_connection_status_timeout:
        return "timeout";
    case mpod_snmp_connection_status_disconnected:
        return "disconnected";
    case mpod_snmp_connection_status_connected:
        return "connected";
    case mpod_snmp_connection_status_on:
        return "on";
    case mpod_snmp_connection_status_off:
        return "off";
    case mpod_snmp_connection_status_on_structure_changed:
        return "on, structure of crate changed";
    case mpod_snmp_connection_status_off_structure_changed:
        return "off, structure of crate changed";
    }
    return "";
}

std::string WienerMpodLvHvSnmp::convertCrateStatusToString(unsigned long int_status) const {
    std::string result = "off";

    if (checkBitIsSet(int_status, 3)) {
        result = "input error";
    }
    if (checkBitIsSet(int_status, 0)) {
        result = "on";
    }
    if (checkBitIsSet(int_status, 9)) {
        result = "reset";
    }
    if (checkBitIsSet(int_status, 2)) {
        result = "local";
    }
    if (checkBitIsSet(int_status, 1)) {
        result = "inhibit";
    }
    if (checkBitIsSet(int_status, 4)) {
        result = "channel error";
    }
    if (checkBitIsSet(int_status, 5)) {
        result = "fan failure";
    }
    if (checkBitIsSet(int_status, 8)) {
        result = "plug and play incompatible";
    }
    if (checkBitIsSet(int_status, 10)) {
        result = "derating";
    }
    if (checkBitIsSet(int_status, 11)) {
        result = "supply error";
    }
    if (checkBitIsSet(int_status, 12)) {
        result = "derating 2";
    }
    if (checkBitIsSet(int_status, 13)) {
        result = "supply error 2";
    }

    return result;
}

bool WienerMpodLvHvSnmp::checkCurrentOidDefinesStatusVariable() const {
    return checkOidDefinesStatusVariable(m_OIDVector, m_OIDVectorLength);
}
bool WienerMpodLvHvSnmp::checkOidDefinesStatusVariable(const oid *name, size_t name_length) const {
    // system status
    // iso.3.6.1.4.1.19947.1.1.2.0  (WIENER-CRATE-MIB::sysStatus.0) =
    if (name_length == 11) {
        if (name[6] == 19947) {
            if (name[7] == 1) {
                if (name[8] == 1) {
                    if (name[9] == 2) {
                        if (name[10] == 0) {
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }
    // module status
    // iso.3.6.1.4.1.19947.1.3.6.1.8.x  (WIENER-CRATE-MIB::moduleStatus.x) =
    // iso.3.6.1.4.1.19947.1.3.6.1.9.x  (WIENER-CRATE-MIB::moduleEventStatus.x) =
    // iso.3.6.1.4.1.19947.1.3.6.1.10.x  (WIENER-CRATE-MIB::moduleEventChannelStatus.x) =
    // channel status
    // iso.3.6.1.4.1.19947.1.3.2.1.4.x  (WIENER-CRATE-MIB::outputStatus.x) =
    if (name_length == 13) {
        if (name[6] == 19947) {
            if (name[7] == 1) {
                if (name[8] == 3) {
                    if (name[9] == 6) {
                        if (name[10] == 1) {
                            switch (name[11]) {
                            case 8:
                            case 9:
                            case 10:
                                return true;
                            default:
                                break;
                            }
                        }
                        return false;
                    }
                    if (name[9] == 2) {
                        if (name[10] == 1) {
                            if (name[11] == 4) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

void WienerMpodLvHvSnmp::insertDataVector(const std::vector<DataContainer> &data_vector) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    std::string simple_oid;
    for (std::vector<DataContainer>::const_iterator it = data_vector.begin(); it != data_vector.end(); it++ ) {
        const DataContainer &data = *it;
        if (checkIsCrateOid(data, simple_oid)) {
            m_readCrateData[simple_oid] = data;
        }
    }
}

void WienerMpodLvHvSnmp::insertData(const DataContainer &data) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    std::string simple_oid;
    if (checkIsCrateOid(data, simple_oid)) {
        m_readCrateData[simple_oid] = data;
    }
}

void WienerMpodLvHvSnmp::executeThread() {
    int polling_interval_ms_fast;
    int polling_interval_ms_intermediate;
    int polling_interval_ms_slow;
    bool flag_join = false;
    bool flag_read_data_fast;
    bool flag_read_data_intermediate;
    bool flag_read_data_slow;
    bool flag_crate_switched_off;
    bool flag_connection_broke_down;
    bool flag_read_generic_data;
    int64_t previous_time_fast = 0;
    int64_t previous_time_intermediate = 0;
    int64_t previous_time_slow = 0;
    int64_t current_time;
    std::vector<std::pair<std::string, DataContainer> > execution_queue;
    std::string oid_str, simple_oid;
    int previous_crate_status_flags = 0, current_crate_status_flags;
    DataContainer data;
    std::vector<DataContainer> data_vector;
    size_t current_read_data_counter;
    size_t previous_read_data_counter = 0;
    size_t total_nb_of_channels;
    size_t nb_bulk_get;
    size_t data_vector_offset;

#ifdef WIENER_MPOD_DEBUG_THREAD
    std::cout << "thread starts" << std::endl;
#endif
    {
        AfThreadsMutexLocker lock(m_dataAccessMutex);
        m_flagThreadIsRunning = true;
        polling_interval_ms_fast = m_pollingIntervalMsFast;
        polling_interval_ms_intermediate = m_pollingIntervalMsIntermediate;
        polling_interval_ms_slow = m_pollingIntervalMsSlow;
        flag_join = m_flagJoinThread;
    }

#ifdef WIENER_MPOD_DEBUG_THREAD
    std::cout << "thread running" << std::endl;
#endif
    flag_read_generic_data = true;
    while ( ! flag_join) {


        {
            AfThreadsMutexLocker lock(m_dataAccessMutex);
            polling_interval_ms_fast = m_pollingIntervalMsFast;
            polling_interval_ms_intermediate = m_pollingIntervalMsIntermediate;
            polling_interval_ms_slow = m_pollingIntervalMsSlow;
            flag_join = m_flagJoinThread;
            execution_queue = m_executionQueue;
            total_nb_of_channels = m_totalNumberOfChannels;
            m_executionQueue.clear();
            if (m_readGenericData.size() == 0) {
                flag_read_generic_data = true;
            }
#ifdef WIENER_MPOD_DEBUG_THREAD
            if (flag_join) {
                std::cout << "thread asked to terminate" << std::endl;
            }
#endif
        }

        if ( ! flag_join ) {
            flag_crate_switched_off = false;
            flag_connection_broke_down = false;
            flag_read_data_fast = false;
            flag_read_data_intermediate = false;
            flag_read_data_slow = false;
            current_time = getTimeStampMsPrecision();
            if (previous_time_fast + polling_interval_ms_fast <= current_time) {
                flag_read_data_fast = true;
                previous_time_fast = current_time;
            }
            if (previous_time_intermediate + polling_interval_ms_intermediate <= current_time) {
                flag_read_data_intermediate = true;
                previous_time_intermediate = current_time;
            }
            if (previous_time_slow + polling_interval_ms_slow <= current_time) {
                flag_read_data_slow = true;
                previous_time_slow = current_time;
            }

            // get crate status and check connection status
            if (flag_read_data_intermediate) {
                oid_str = getSysStatusOid();
                current_crate_status_flags = snmpGetValueIntFromSnmp(oid_str);
                if (current_crate_status_flags == getIntError()) {
                    if (! lockAndCheckCrateConnectionIsOk()) {
                        flag_connection_broke_down = true;
                        previous_crate_status_flags = 0;
                    }
                }
                else {
                    if (current_crate_status_flags != previous_crate_status_flags) {
                        if ( checkBitIsSet(previous_crate_status_flags, 0)
                             && ! checkBitIsSet(current_crate_status_flags, 0)) {
                            flag_crate_switched_off = true;
                        }
                        previous_crate_status_flags = current_crate_status_flags;
                    }
                }
            }

            if (flag_crate_switched_off || flag_connection_broke_down){
                AfThreadsMutexLocker lock(m_dataAccessMutex);
                m_readCrateData.clear();
                m_readGenericData.clear();
                if (flag_connection_broke_down) {
                    m_executionQueue.clear();
                    execution_queue.clear();
                }
            }

            // now write everything in the execution queue
            for ( std::vector<std::pair<std::string, DataContainer> >::const_iterator it = execution_queue.begin(); it != execution_queue.end(); it++) {
                const std::string &string_oid = it->first;
                const DataContainer &dc = it->second;
                switch (dc.m_type) {
                case data_type_double:
#ifdef WIENER_MPOD_DEBUG_EXECUTION_QUEUE
                    std::cout << "executing " << string_oid << " = " << dc.m_doubleValue << std::endl;
#endif
                    snmpSetValueDoubleOverSnmp(string_oid, dc.m_doubleValue);
                    break;
                case data_type_int:
#ifdef WIENER_MPOD_DEBUG_EXECUTION_QUEUE
                    std::cout << "executing " << string_oid << " = " << dc.m_intValue << std::endl;
#endif
                    snmpSetValueIntOverSnmp(string_oid, dc.m_intValue);
                    break;
                default:
                    break;
                }
            }
            if (execution_queue.size()) {
                // if we wrote a value, we have to make sure it is read back in
                flag_read_data_intermediate = true;
                previous_time_intermediate = current_time;
                flag_read_data_slow = true;
                previous_time_slow = current_time;
            }
            execution_queue.clear();

            // now read everything (if connection ok)
            if ( ! flag_connection_broke_down ) {
                if (flag_read_generic_data) {
                    // only once
                    flag_read_generic_data = false;
                    oid_str = getGenericOid();
                    while (oid_str.substr(0, getGenericOid().size()) == getGenericOid()) {
                        oid_str = snmpGetNextOid(oid_str, data);
                        if (oid_str.empty()) {
                            // failed => has to be read again
                            flag_read_generic_data = true;
                            break;
                        }
                        {
                            // succeeded => insert
                            AfThreadsMutexLocker lock(m_dataAccessMutex);
                            m_readGenericData[oid_str] = data;
                        }
                    }
                }
                if (flag_read_data_fast && ! flag_crate_switched_off) {
#ifdef WIENER_MPOD_DEBUG_READ_DATA_FAST
                    std::cout << "@" << current_time << " reading FAST" << std::endl;
#endif
                    // read the current and voltage measurements
                    // iso.3.6.1.4.1.19947.1.3.2.1.5.X
                    // ...
                    // iso.3.6.1.4.1.19947.1.3.2.1.7.X
                    nb_bulk_get = 3 * total_nb_of_channels;
                    oid_str = getOutputMeasurementSenseVoltage();
                    data_vector_offset = 0;
                    while (nb_bulk_get) {
                        oid_str = snmpGetBulk(oid_str, nb_bulk_get, data_vector);
                        insertDataVector(data_vector);
                        appendToMeasuredDataBuffer(data_vector_offset, current_time, data_vector);
                        if (! checkIsMeasurementOid(oid_str, simple_oid)) {
                            break;
                        }
                        nb_bulk_get -= data_vector.size();
                        data_vector_offset += data_vector.size();
                    }
                }
                if (flag_read_data_slow) {
                    // outputIndex ... outputGroup
                    oid_str = "iso.3.6.1.4.1.19947.1.3.2.1.1";
                    nb_bulk_get = 3 * total_nb_of_channels;
                    while (nb_bulk_get) {
                        oid_str = snmpGetBulk(oid_str, nb_bulk_get, data_vector);
                        insertDataVector(data_vector);
                        if (data_vector.size() == 0) {
                            break;
                        }
                        nb_bulk_get -= data_vector.size();
                    }

                    // outputSwitch, outputVoltage, outputCurrent, outputVoltageRiseRate ,
                    //     outputVoltageFallRate, outputSupervisionBehavior, outputSupervisionMinSenseVoltage,
                    //     outputSupervisionMaxSenseVoltage, outputSupervisionMaxTerminalVoltage,
                    //     outputSupervisionMaxCurrent, outputConfigMaxSenseVoltage, outputConfigMaxTerminalVoltage
                    //     outputConfigMaxCurrent, outputSupervisionMaxPower, outputCurrentRiseRate,
                    //     outputCurrentFallRate, outputTripTimeMaxCurrent, outputUserConfig
                    oid_str = "iso.3.6.1.4.1.19947.1.3.2.1.9";
                    nb_bulk_get = 18 * total_nb_of_channels;
                    while (nb_bulk_get) {
                        oid_str = snmpGetBulk(oid_str, nb_bulk_get, data_vector);
                        insertDataVector(data_vector);
                        if (data_vector.size() == 0) {
                            break;
                        }
                        if (!checkIsChannelOid(data_vector.back())) {
                            // should not occur
                            break;
                        }
                        nb_bulk_get -= data_vector.size();
                    }

                }
                if (flag_read_data_intermediate) {
                    current_read_data_counter = 0;
                    // read sys main switch, ... output_number
                    oid_str = "iso.3.6.1.4.1.19947.1.1.1";
                    while (! oid_str.empty()) {
                        oid_str = snmpGetNextOid(oid_str, data);
                        insertData(data);
                        current_read_data_counter++;
                        if ( ! checkIsSystemOid(data)) {
                            break;
                        }
                    }

                    // outputStatus
                    //  no bulk get here as we need this to detect the channels
                    oid_str = "iso.3.6.1.4.1.19947.1.3.2.1.4";
                    oid_str = snmpGetNextOid(oid_str, data);
                    while (! oid_str.empty()) {
                        current_read_data_counter++;
                        if (checkIsOutputStatusOid(oid_str, simple_oid)) {
                            AfThreadsMutexLocker lock(m_dataAccessMutex);
                            m_readCrateData[simple_oid] = data;
                        }
                        else {
                            break;
                        }
                        oid_str = snmpGetNextOid(oid_str, data);
                    }

                    // outputMeasurementTemperature
                    oid_str = "iso.3.6.1.4.1.19947.1.3.2.1.8";
                    nb_bulk_get = total_nb_of_channels;
                    while (nb_bulk_get) {
                        oid_str = snmpGetBulk(oid_str, nb_bulk_get, data_vector);
                        insertDataVector(data_vector);
                        current_read_data_counter += data_vector.size();
                        if (data_vector.size() == 0) {
                            break;
                        }
                        nb_bulk_get -= data_vector.size();
                    }

                    oid_str = "iso.3.6.1.4.1.19947.1.3.3";
                    oid_str = snmpGetNextOid(oid_str, data);
                    while (! oid_str.empty()) {
                        current_read_data_counter++;
                        if (checkIsCrateOid(oid_str, simple_oid)) {
                            AfThreadsMutexLocker lock(m_dataAccessMutex);
                            m_readCrateData[simple_oid] = data;
                        }
                        else {
                            break;
                        }
                        oid_str = snmpGetNextOid(oid_str, data);
                    }
                    if (current_read_data_counter != previous_read_data_counter) {
                        previous_read_data_counter = current_read_data_counter;
                        lockAndSetConnectionStatusStructureChanged();

                        recomputeCrateStructure(true);
                    }
                }
            }

#ifdef WIN32
            ::Sleep(50);
#else
            usleep(50000);
#endif
        }
    }
    {
        AfThreadsMutexLocker lock(m_dataAccessMutex);
        m_flagThreadIsRunning = false;
        m_flagJoinThread = false;
#ifdef WIENER_MPOD_DEBUG_THREAD
        std::cout << "thread terminates" << std::endl;
#endif
    }
}

uint64_t WienerMpodLvHvSnmp::getTimeStampMsPrecision() const {
    timeval now;
    gettimeofday(&now, NULL);
    uint64_t time = now.tv_sec;
    time = time * 1000;
    uint64_t ms = now.tv_usec;
    ms = ms / 1000;
    time = time + ms;
    return time;
}

void WienerMpodLvHvSnmp::freeMeasuredDataBuffers() {
    if ( m_measuredDataTimeBuffer ) {
        delete [] m_measuredDataTimeBuffer;
        m_measuredDataTimeBuffer = 0;
    }
    for (std::vector<double *>::iterator it = m_measuredDataBuffer.begin(); it != m_measuredDataBuffer.end(); it++ ) {
        double *d = *it;
        if (d) {
            delete [] d;
        }
    }
    m_measuredDataBuffer.clear();
    m_measuredDataBufferAllocatedSize = 0;
    m_measuredDataBufferSize = 0;
    m_measuredDataStartPosition = 0;
    m_measuredDataLastReadNumberOfValues = 0;
}

void WienerMpodLvHvSnmp::freeTmpDataBuffers() {
    m_measuredDataTmpAllocateSize = 0;
    if (m_measuredDataTmpValues) {
        delete [] m_measuredDataTmpValues;
        m_measuredDataTmpValues = 0;
    }
    if (m_measuredDataTmpTimeValues) {
        delete [] m_measuredDataTmpTimeValues;
        m_measuredDataTmpTimeValues = 0;
    }
}

void WienerMpodLvHvSnmp::allocateTmpDataBuffers() {
    if (m_measuredDataBufferInitialSize >= m_measuredDataBufferAllocatedSize) {
        allocateTmpDataBuffers(m_measuredDataBufferInitialSize);
    }
    allocateTmpDataBuffers(m_measuredDataBufferAllocatedSize);
}

void WienerMpodLvHvSnmp::allocateTmpDataBuffers(size_t new_allocated_size) {
    if (new_allocated_size > m_measuredDataTmpAllocateSize
            || m_measuredDataTmpValues == 0 || m_measuredDataTmpTimeValues == 0) {

        freeTmpDataBuffers();
        m_measuredDataTmpAllocateSize = new_allocated_size;
        m_measuredDataTmpValues = new double[m_measuredDataTmpAllocateSize];
        m_measuredDataTmpTimeValues = new double[m_measuredDataTmpAllocateSize];
    }
}

void WienerMpodLvHvSnmp::allocateMeasuredDataBuffers(size_t nb, size_t new_allocated_size) {
    if ( m_measuredDataBuffer.size() != nb
            || m_measuredDataBufferAllocatedSize != new_allocated_size ) {
        freeMeasuredDataBuffers();
        if ( nb && new_allocated_size ) {
            m_measuredDataTimeBuffer = new double[new_allocated_size];
            m_measuredDataBuffer.resize(nb, 0);
            for (std::vector<double *>::iterator it = m_measuredDataBuffer.begin(); it != m_measuredDataBuffer.end(); it++ ) {
                *it = new double[new_allocated_size];
            }
            m_measuredDataBufferSize = 0;
            m_measuredDataStartPosition = 0;
            m_measuredDataLastReadNumberOfValues = 0;
        }
        m_measuredDataBufferAllocatedSize = new_allocated_size;
    }
}

void WienerMpodLvHvSnmp::resizeMeasuredDataBuffer(size_t new_allocated_size) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);
    allocateMeasuredDataBuffers(m_totalNumberOfChannels * 3, new_allocated_size);
}

void WienerMpodLvHvSnmp::appendToMeasuredDataBuffer(size_t data_vector_offset, int64_t current_time, const std::vector<DataContainer> &data_vector) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    size_t write_vector_index;
    if (data_vector_offset == 0) {
        // 1st value
        if (m_measuredDataBufferSize >= m_measuredDataBufferAllocatedSize) {
            // ring buffer full
#ifdef WIENER_MPOD_DEBUG_DATA_BUFFER
            std::cout << "ring buffer is full " << std::endl;
#endif
            if ( m_measuredDataStartPosition == m_measuredDataBufferSize ) {
                m_measuredDataStartPosition = 0;
            }
            m_measuredDataStartPosition++;
        }
        else {
            // ring buffer not full
            m_measuredDataBufferSize++;
            m_measuredDataStartPosition = m_measuredDataBufferSize;
        }
        write_vector_index = m_measuredDataStartPosition - 1;
#ifdef WIENER_MPOD_DEBUG_DATA_BUFFER
            std::cout << "write_vector_index == " << write_vector_index << std::endl;
#endif
        if (m_measuredDataTimeBuffer) {
            m_measuredDataTimeBuffer[write_vector_index] = current_time * 0.001;
        }
        for (std::vector<double *>::iterator it = m_measuredDataBuffer.begin(); it != m_measuredDataBuffer.end(); it++ ) {
            double *d = *it;
            if (d) {
                d[write_vector_index] = nan("");
            }
        }
    }
    write_vector_index = m_measuredDataStartPosition - 1;
    if (data_vector_offset < m_measuredDataBuffer.size()) {
        std::vector<double *>::iterator it = m_measuredDataBuffer.begin() + data_vector_offset;
        std::vector<DataContainer>::const_iterator dit = data_vector.begin();
        while (it != m_measuredDataBuffer.end() && dit != data_vector.end()) {
            const DataContainer &dc = *dit;
            if (dc.m_type == data_type_double) {
                double *d = *it;
                if (d) {
                    d[write_vector_index] = dc.m_doubleValue;
                }
            }
            it++;
            dit++;
        }

    }
}

void WienerMpodLvHvSnmp::copyRingBufferToVector(const double *ring_buffer, double *data_buffer) {
    size_t i;
    if (m_measuredDataBufferSize >= m_measuredDataBufferAllocatedSize) {
        // ring buffer full
        if (m_measuredDataStartPosition < m_measuredDataBufferSize) {
            i = m_measuredDataBufferSize - m_measuredDataStartPosition;
            memcpy(data_buffer, ring_buffer + m_measuredDataStartPosition, i * sizeof(double));
            memcpy(data_buffer + i, ring_buffer, (m_measuredDataBufferSize - i) * sizeof(double));
        }
        else {
            memcpy(data_buffer, ring_buffer, m_measuredDataBufferSize * sizeof(double));
        }
    }
    else {
        // ring buffer not full
        memcpy(data_buffer, ring_buffer, m_measuredDataBufferSize * sizeof(double));
    }
}
const double *WienerMpodLvHvSnmp::channelGetMeasurementVector(unsigned vector_index, unsigned *vector_size) {
    allocateTmpDataBuffers();
    if (vector_index < m_measuredDataBuffer.size()) {
        if ( m_measuredDataBuffer[vector_index] ) {
            copyRingBufferToVector(m_measuredDataTimeBuffer, m_measuredDataTmpTimeValues);
            copyRingBufferToVector(m_measuredDataBuffer[vector_index], m_measuredDataTmpValues);
            m_measuredDataLastReadNumberOfValues = m_measuredDataBufferSize;
            if (vector_size) {
                *vector_size = m_measuredDataLastReadNumberOfValues;
            }
            return m_measuredDataTmpValues;
        }
    }
    if (vector_size) {
        *vector_size = 0;
    }
    return 0;
}
const double *WienerMpodLvHvSnmp::channelGetMeasurementCurrentVector(unsigned slot_index, unsigned channel_index, unsigned *vector_size) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    m_measuredDataLastReadNumberOfValues = 0;
    if ( slot_index < m_moduleNumberOfChannels.size() && m_measuredDataTimeBuffer ) {
        channel_index = m_moduleFirstChannelIndex[slot_index] + channel_index;
        channel_index = 2 * m_totalNumberOfChannels + channel_index;
        return channelGetMeasurementVector(channel_index, vector_size);
    }
    if (vector_size) {
        *vector_size = 0;
    }
    return 0;
}
const double *WienerMpodLvHvSnmp::channelGetMeasurementSenseVoltageVector(unsigned slot_index, unsigned channel_index, unsigned *vector_size) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    m_measuredDataLastReadNumberOfValues = 0;
    if ( slot_index < m_moduleNumberOfChannels.size() && m_measuredDataTimeBuffer ) {
        channel_index = m_moduleFirstChannelIndex[slot_index] + channel_index;
        return channelGetMeasurementVector(channel_index, vector_size);
    }
    if (vector_size) {
        *vector_size = 0;
    }
    return 0;
}
const double *WienerMpodLvHvSnmp::channelGetMeasurementTerminalVoltageVector(unsigned slot_index, unsigned channel_index, unsigned *vector_size) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    m_measuredDataLastReadNumberOfValues = 0;
    if ( slot_index < m_moduleNumberOfChannels.size() && m_measuredDataTimeBuffer ) {
        channel_index = m_moduleFirstChannelIndex[slot_index] + channel_index;
        channel_index = m_totalNumberOfChannels + channel_index;
        return channelGetMeasurementVector(channel_index, vector_size);
    }
    if (vector_size) {
        *vector_size = 0;
    }
    return 0;
}
const double *WienerMpodLvHvSnmp::channelGetMeasurementTimeVector(unsigned *vector_size) {
    AfThreadsMutexLocker lock(m_dataAccessMutex);

    if (vector_size) {
        *vector_size = m_measuredDataLastReadNumberOfValues;
    }
    return m_measuredDataTmpTimeValues;
}

