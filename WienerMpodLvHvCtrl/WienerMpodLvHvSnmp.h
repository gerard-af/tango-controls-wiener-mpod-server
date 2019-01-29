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

#ifndef WienerMpodLvHvSnmp_H
#define WienerMpodLvHvSnmp_H


#include "wiener_mpod_lv_hv_types.h"
#include "AfThreads.h"
#include <string>
#include <vector>
#include <map>
#include <math.h>

#ifdef WIN32
    typedef unsigned long oid;
    typedef signed __int64       int64_t;
    typedef unsigned __int64     uint64_t;
#else
    typedef u_long oid;
#endif
struct snmp_pdu;
struct snmp_session;
struct variable_list;
//struct netsnmp_session;

/**
 *  WienerMpodLvHvSnmp class description:
 *    This class allows to controll the Wiener MPOD and it`s power modules such as
 *    - Wiener LV power supplies
 *    - iseg HV power supplies
 *
 *    It contains low level access functions for directly accessing the
 *    SNMP protocol.
 *    These functions are named `snmp...`.
 *
 *    It also contains high level access functions for controlling:
 *
 *    - the crate functionality. The functions are named `crate...`.
 *    The `crateGet...` functions don`t take arguments.
 *    The `crateSet...` functions need the new value as argument.
 *
 *    - the module functionality. The functions are named `module...`.
 *    The `moduleGet...` functions need the `slot_index` as argument.
 *    The `moduleSet...` functions need the `slot_index` and the new value as argument.
 *
 *    - the channel functionality. The functions are named `channel...`.
 *    The `channelGet...` functions need the `slot_index` and the `channel_index` as arguments.
 *    The `channelSet...` functions need the `slot_index`, the `channel_index` and new value as arguments.
 *
 *    - the group functionality. The functions are named `group...`.
 *    The `groupGet...` functions requires the `groupIndex` as argument.
 *    The `groupSet...` functions need the `groupIndex` ant the new value as argument.
 *
 *    Further information can be found on the manufacturer`s web sites
 *    and in the user manuals for MPOD and the inserted modules.
 */


class WienerMpodLvHvSnmp : public AfThread {

    typedef enum {

        data_type_undefined,
        data_type_double,
        data_type_int,
        data_type_string

    } data_type;

    class DataContainer {
    public:

        data_type m_type;
        int m_intValue;
        double m_doubleValue;
        std::string m_stringValue;
        oid m_OIDVector[128];
        size_t m_OIDVectorLength;

        DataContainer() {
            m_type = data_type_undefined;
            m_OIDVectorLength = 0;
        }
        DataContainer(const double &i) {
            m_type = data_type_double;
            m_doubleValue = i;
            m_OIDVectorLength = 0;
        }
        DataContainer(const int &i) {
            m_type = data_type_int;
            m_intValue = i;
            m_OIDVectorLength = 0;
        }
        DataContainer(const std::string &i) {
            m_type = data_type_string;
            m_stringValue = i;
            m_OIDVectorLength = 0;
        }

    };

    typedef enum {
        module_type_undefined,
        module_type_lv,
        module_type_hv
    } module_type;

    snmp_session *m_session;
    snmp_session *m_crate;
    mpod_snmp_connection_status_type m_connectionStatus;
    oid m_OIDVector[128];
    size_t m_OIDVectorLength;

    std::string m_errorStr;
    size_t m_snmpTimeoutCounter;
    size_t m_snmpTimeoutCounterMax;

    std::vector<module_type> m_moduleTypes;
    std::vector<size_t> m_moduleNumberOfChannels;
    std::vector<size_t> m_moduleFirstChannelIndex;
    size_t m_totalNumberOfChannels;
    bool m_flagCheckOnOff; // the structure of the crate (modules channels) might have changed -> check

    //    IpAddr:    IP Address for the MPOD controller.
    std::string    m_ipAddr;
    unsigned m_portIndex; // default == 161

    //    SnmpCommunityName:    Used SNMP community. This property can limit the access to SNMP variables.
    //  By default `guru` defines the read-write community.
    //  The community `public` prohibits write operations.
    //  These default settings can be changed.
    std::string    m_snmpCommunityName;

    bool m_flagMibFileIsLoaded;
    int m_timeoutIntervalMs;

    std::string mTmpString;
    int *m_cArray;
    unsigned m_cArrayAllocatedSize;

    int m_pollingIntervalMsFast;
    int m_pollingIntervalMsIntermediate;
    int m_pollingIntervalMsSlow;
    bool m_flagThreadIsRunning;
    bool m_flagJoinThread;
    bool m_flagPollingInThreadIsActive; /// not read or written by thread
    AfThreadsMutex m_dataAccessMutex;
    AfThreadsMutex m_snmpAccessMutex;

    /// these data structures are used when m_flagPollingInThreadIsActive
    std::map<std::string, DataContainer> m_readCrateData; /// "get" commands where oid starts with "iso.3.6.1.4.1.19947"
    std::map<std::string, DataContainer> m_readGenericData; /// "get" commands where oid starts with something else
    std::vector<std::pair<std::string, DataContainer> > m_executionQueue; /// "set" commands to be sent to the mpod

    /// measurement data buffers
    /// implemented as ringbuffermore
    /// only used when polling is active
    /// we use c vectors as this is compatible with the interface
    double *m_measuredDataTimeBuffer;
    std::vector<double *> m_measuredDataBuffer;
    size_t m_measuredDataBufferInitialSize;
    size_t m_measuredDataBufferAllocatedSize;
    size_t m_measuredDataBufferSize;
    size_t m_measuredDataStartPosition;
    size_t m_measuredDataLastReadNumberOfValues;

    size_t m_measuredDataTmpAllocateSize;
    double *m_measuredDataTmpValues;
    double *m_measuredDataTmpTimeValues;

public:
    WienerMpodLvHvSnmp();
    WienerMpodLvHvSnmp(const std::string &ip_address, int port_index, const std::string &community_name);
    virtual ~WienerMpodLvHvSnmp();

    bool crateConnect(const std::string &ip_address, int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow);
    bool crateConnect(const std::string &ip_address, const std::string &community_name, int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow);
    bool crateConnect(const std::string &ip_address, int port_index, const std::string &community_name, int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow);
    void crateDisconnect();
    mpod_snmp_connection_status_type lockAndGetCrateConnectionStatus();
    std::string lockAndGetCrateConnectionStatusAsString();
    bool lockAndCheckCrateConnectionIsOk();
    std::string lockAndGetErrorString();
    bool lockAndCheckConnectionStatusSwitchedOn();
    bool lockAndCheckConnectionStatusOffOrDisconnected();
    void lockAndSetConnectionStatus(mpod_snmp_connection_status_type s);
    void lockAndSetConnectionStatusStructureChanged();
    void resizeMeasuredDataBuffer(size_t new_allocated_size);

    bool checkIsMibFileLoaded() { return m_flagMibFileIsLoaded; }
    // if interval_in_ms <= 0, the net-snmp library default value will be used
    void snmpSetTimeoutInterval(int interval_in_ms);

    double channelGetConfigMaxCurrent(unsigned slot_index, unsigned channel_index);
    double channelGetConfigMaxSenseVoltage(unsigned slot_index, unsigned channel_index);
    double channelGetConfigMaxTerminalVoltage(unsigned slot_index, unsigned channel_index);
    double channelGetCurrent(unsigned slot_index, unsigned channel_index);
    double channelGetCurrentFallRate(unsigned slot_index, unsigned channel_index);
    double channelGetCurrentRiseRate(unsigned slot_index, unsigned channel_index);
    int channelGetIndex(unsigned slot_index, unsigned channel_index);
    double channelGetMeasurementCurrent(unsigned slot_index, unsigned channel_index);
    double channelGetMeasurementSenseVoltage(unsigned slot_index, unsigned channel_index);
    double channelGetMeasurementTemperature(unsigned slot_index, unsigned channel_index);
    double channelGetMeasurementTerminalVoltage(unsigned slot_index, unsigned channel_index);
    int channelGetStatus(unsigned slot_index, unsigned channel_index);
    std::string channelGetStatusAsString(unsigned slot_index, unsigned channel_index);
    int channelGetSupervisionBehavior(unsigned slot_index, unsigned channel_index);
    double channelGetSupervisionMaxCurrent(unsigned slot_index, unsigned channel_index);
    double channelGetSupervisionMaxPower(unsigned slot_index, unsigned channel_index);
    double channelGetSupervisionMaxTerminalVoltage(unsigned slot_index, unsigned channel_index);
    double channelGetSupervisionMinSenseVoltage(unsigned slot_index, unsigned channel_index);
    int channelGetSwitch(unsigned slot_index, unsigned channel_index);
    int channelGetTripTime(unsigned slot_index, unsigned channel_index);
    int channelGetUserConfig(unsigned slot_index, unsigned channel_index);
    double channelGetVoltage(unsigned slot_index, unsigned channel_index);
    double channelGetVoltageFallRate(unsigned slot_index, unsigned channel_index);
    double channelGetVoltageRiseRate(unsigned slot_index, unsigned channel_index);
    bool channelSetCurrent(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetCurrentFallRate(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetCurrentRiseRate(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetSupervisionBehavior(unsigned slot_index, unsigned channel_index, int value);
    bool channelSetSupervisionMaxCurrent(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetSupervisionMaxTerminalVoltage(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetSupervisionMinSenseVoltage(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetSwitch(unsigned slot_index, unsigned channel_index, int value);
    bool channelSetTripTime(unsigned slot_index, unsigned channel_index, int value);
    bool channelSetUserConfig(unsigned slot_index, unsigned channel_index, int value);
    bool channelSetVoltage(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetVoltageFallRate(unsigned slot_index, unsigned channel_index, double value);
    bool channelSetVoltageRiseRate(unsigned slot_index, unsigned channel_index, double value);
    const double *channelGetMeasurementCurrentVector(unsigned slot_index, unsigned channel_index, unsigned *vector_size);
    const double *channelGetMeasurementSenseVoltageVector(unsigned slot_index, unsigned channel_index, unsigned *vector_size);
    const double *channelGetMeasurementTerminalVoltageVector(unsigned slot_index, unsigned channel_index, unsigned *vector_size);
    const double *channelGetMeasurementTimeVector(unsigned *vector_size);
    int crateGetFanSpeed();
    int crateGetNumberOfChannels();
    int crateGetNumberOfGroups();
    int crateGetNumberOfModules();
    std::vector<size_t> crateGetOccupiedSlots();
    std::string crateGetSerialNumber();
    std::string crateGetSysDescr();
    int crateGetSysMainSwitch();
    int crateGetSysStatus();
    std::string crateGetSysStatusAsString();
    bool crateSetFanSpeed(int value);
    bool crateSetSysHardwareReset(int value);
    bool crateSetSysMainSwitch(int value);
    int groupGetIndex(unsigned index);
    bool groupSetSwitch(unsigned group_index, int value);
    double moduleGetAuxiliaryMeasurementVoltage0(unsigned slot_index);
    double moduleGetAuxiliaryMeasurementVoltage1(unsigned slot_index);
    std::string moduleGetDescription(unsigned slot_index);
    int moduleGetEventChannelStatus(unsigned slot_index);
    int moduleGetEventStatus(unsigned slot_index);
    std::string moduleGetEventStatusAsString(unsigned slot_index);
    double moduleGetHardwareLimitCurrent(unsigned slot_index);
    double moduleGetHardwareLimitVoltage(unsigned slot_index);
    int moduleGetIndex(unsigned slot_index);
    bool moduleGetIsHvModule(unsigned slot_index);
    // in general only the 1st is used
    double moduleGetMeasurementTemperature0(unsigned slot_index);
    double moduleGetMeasurementTemperature1(unsigned slot_index);
    double moduleGetMeasurementTemperature2(unsigned slot_index);
    double moduleGetMeasurementTemperature3(unsigned slot_index);
    int moduleGetNumberOfChannels(unsigned slot_index);
    double moduleGetRampSpeedCurrent(unsigned slot_index);
    double moduleGetRampSpeedVoltage(unsigned slot_index);
    int moduleGetStatus(unsigned slot_index);
    std::string moduleGetStatusAsString(unsigned slot_index);
    bool moduleSetDoClear(unsigned slot_index, int value);
    bool moduleSetRampSpeedCurrent(unsigned slot_index, double value);
    bool moduleSetRampSpeedVoltage(unsigned slot_index, double value);

    std::string snmpGetDescription(const char *string_oid);
    std::string snmpGetDescription(const std::string &string_oid) { return snmpGetDescription(string_oid.c_str()); }
    std::string snmpGetNextOid(const std::string &string_oid);
    std::string snmpGetNextOid(const char *string_oid) {
        if (string_oid) {
            return snmpGetNextOid(std::string(string_oid));
        }
        return getStringError();
    }
    std::string snmpGetBulk(const std::string &string_oid, size_t nb, std::vector<DataContainer> &data);
    double snmpGetValueDouble(const char *string_oid) {
        if (string_oid) {
            return snmpGetValueDoubleImpl(std::string(string_oid));
        }
        return getDoubleError();
    }
    double snmpGetValueDouble(const std::string &string_oid) {
        return snmpGetValueDoubleImpl(string_oid);
    }
    int snmpGetValueInt(const char *string_oid) {
        if (string_oid) {
            return snmpGetValueIntImpl(std::string(string_oid));
        }
        return getIntError();
    }
    int snmpGetValueInt(const std::string &string_oid) {
        return snmpGetValueIntImpl(string_oid);
    }
    std::string snmpGetValueString(const char *string_oid) {
        if (string_oid) {
            return snmpGetValueStringImpl(std::string(string_oid));
        }
        return getStringError();
    }
    std::string snmpGetValueString(const std::string &string_oid) {
        return snmpGetValueStringImpl(string_oid);
    }
    bool snmpSetValueDouble(const char *string_oid, double value) {
        if (string_oid) {
            return snmpSetValueDoubleImpl(std::string(string_oid), value);
        }
        return false;
    }
    bool snmpSetValueDouble(const std::string &string_oid, double value) {
        return snmpSetValueDoubleImpl(string_oid, value);
    }
    bool snmpSetValueInt(const char *string_oid, int value) {
        if (string_oid) {
            return snmpSetValueIntImpl(std::string(string_oid), value);
        }
        return false;
    }
    bool snmpSetValueInt(const std::string &string_oid, int value) {
        return snmpSetValueIntImpl(string_oid, value);
    }
    std::string snmpTranslateFromOid(const char *string_oid);
    std::string snmpTranslateFromOid(const std::string &string_oid) { return snmpTranslateFromOid(string_oid.c_str()); }
    std::string snmpTranslateToOid(const char *string_oid);
    std::string snmpTranslateToOid(const std::string &string_oid) { return snmpTranslateToOid(string_oid.c_str()); }

    void snmpInit(void);
    struct snmp_session *snmpOpen(const char* ip_address, int port_index, const char *community_name);
    struct snmp_session *snmpOpen(const std::string &ip_address, int port_index, const std::string &community_name) {
        return snmpOpen(ip_address.c_str(), port_index, community_name.c_str());
    }
    std::string convertOidToString(const oid *name, size_t name_length, size_t offset = 0) const;
    bool compareOidToCurrentOidVector(const oid *name, size_t name_length) const;
    void snmpSucceededSetState();
    void snmpFailedSetState(int rStatus, long snmp_errstat, const std::string &string_oid);
    void setStatusDisconnected(mpod_snmp_connection_status_type new_status);
    void recomputeCrateStructureIfModuleNotDetected(unsigned slot_index, bool flag_command_succeeded);
    bool checkOidDefinesStatusVariable(const oid *name, size_t name_length) const;
    bool checkCurrentOidDefinesStatusVariable() const;
    unsigned char inverseBits(unsigned char ch) const;

    std::string convertConnectionStatusToString(mpod_snmp_connection_status_type integer_status) const;
    std::string convertCrateStatusToString(unsigned long integer_status) const;
    std::string convertModuleEventStatusToString(unsigned long integer_status) const;
    std::string convertModuleStatusToString(unsigned long integer_status) const;
    std::string convertChannelStatusToString(unsigned long integer_status, unsigned slot_index);
    bool checkDescribesHvModule(std::string &description) const;
    bool checkSlotContainsHvModule(unsigned slot_index);
    bool checkBitIsSet(long integer_status, unsigned bit) const {
        return (bool)(integer_status & ( 1 << bit));
    }

    const char *toCString(const std::string &s) { mTmpString = s; return mTmpString.c_str(); }
    const int *toCArray(const std::vector<size_t> &v);

    static double getDoubleError() { return nan(""); }
    static int getIntError() { return -1; }
    static std::string getStringError() { return ""; }

    void executeThread();

    void lockAndSetErrorString(const std::string &s);

private:
    void initAtContruction();

    void setPollingInterval(int thread_polling_interval_ms_fast, int thread_polling_interval_ms_intermediate, int thread_polling_interval_ms_slow);

    bool snmpGetNode(const std::string &string_oid);
    bool snmpSetValueIntImpl(const std::string &string_oid, int value);
    bool snmpSetValueDoubleImpl(const std::string &string_oid, double value);
    double snmpGetValueDoubleImpl(const std::string &string_oid);
    std::string snmpGetValueStringImpl(const std::string &string_oid);
    int snmpGetValueIntImpl(const std::string &string_oid);

    mpod_snmp_connection_status_type getConnectionStatus() const {
        /// must have been locke above
        return m_connectionStatus;
    }
    void setConnectionStatus(mpod_snmp_connection_status_type s) {
        m_connectionStatus = s;
    }
    bool checkConnectionStatusOk() const {
        /// must have been locked above
        return (m_connectionStatus == mpod_snmp_connection_status_on
                || m_connectionStatus == mpod_snmp_connection_status_on_structure_changed
                || m_connectionStatus == mpod_snmp_connection_status_off
                || m_connectionStatus == mpod_snmp_connection_status_connected
                || m_connectionStatus == mpod_snmp_connection_status_off_structure_changed);
    }

    oid *getOIDVector() { return m_OIDVector; }
    const oid *getOIDVector() const { return m_OIDVector; }
    size_t getOIDVectorLength() const { return m_OIDVectorLength; }

    int crateGetSysMainSwitchImpl();
    std::string moduleGetDescriptionImpl(unsigned slot_index);
    int channelGetIndexImpl(unsigned slot_index, unsigned channel_index);
    int channelGetStatusImpl(unsigned slot_index, unsigned channel_index);

    void getOccupiedSlotsImpl(std::vector<size_t> &occupied_slots) const;

    bool snmpGetImpl(const std::string &string_oid, struct snmp_pdu **response);

    void setErrorString(const std::string &s);
    void clearErrorString() { m_errorStr.clear(); }

    bool checkIsCrateOid(const std::string &oid_str, std::string &simple_oid) const;
    bool checkIsCrateOid(const DataContainer &data, std::string &simple_oid) const;
    bool checkIsMeasurementOid(const std::string &oid_str, std::string &simple_oid) const;
    bool checkIsOutputStatusOid(const std::string &oid_str, std::string &simple_oid) const;
    bool checkIsChannelOid(const DataContainer &data) const;
    bool checkIsSystemOid(const DataContainer &data) const;
    void snmpGetValueFromBuffer(const std::string &string_oid, DataContainer &data);
    int snmpGetValueIntFromSnmp(const std::string &string_oid);
    bool appendToExecutionQueueInt(const std::string &string_oid, int value);
    bool snmpSetValueIntOverSnmp(const std::string &string_oid, int value);
    bool appendToExecutionQueueDouble(const std::string &string_oid, double value);
    bool snmpSetValueDoubleOverSnmp(const std::string &string_oid, double value);

    uint64_t getTimeStampMsPrecision() const;

    std::string getOutputMeasurementSenseVoltage() const { return "iso.3.6.1.4.1.19947.1.3.2.1.5"; }
    std::string getSysMainSwitchOid() const { return "iso.3.6.1.4.1.19947.1.1.1.0"; }
    std::string getCrateOid() const { return "iso.3.6.1.4.1.19947"; }
    std::string getSysStatusOid() const { return "iso.3.6.1.4.1.19947.1.1.2.0"; }
    std::string getGenericOid() const { return "iso.3.6.1.2.1.1"; }

    std::string snmpGetNextOid(const std::string &string_oid, DataContainer &data);
    bool getDataFromSnmpVariable(struct variable_list *vars, DataContainer &data) const;
    bool snmpGetValueFromSnmp(const std::string &string_oid, DataContainer &data);

    void recomputeCrateStructure(bool flag_force = false);
    void recomputeCrateStructureIfNecessary(bool flag_crate_presumed_on, bool flag_crate_presumed_disconnected);
    size_t getNumberOfDetecteModules();

    void insertDataVector(const std::vector<DataContainer> &data_vector);
    void insertData(const DataContainer &data);

    void freeTmpDataBuffers();
    void allocateTmpDataBuffers(size_t new_allocated_size);
    void allocateTmpDataBuffers();
    void freeMeasuredDataBuffers();
    void allocateMeasuredDataBuffers(size_t nb, size_t new_allocated_size);
    void appendToMeasuredDataBuffer(size_t data_vector_offset, int64_t current_time, const std::vector<DataContainer> &data_vector);
    void copyRingBufferToVector(const double *ring_buffer, double *data_buffer);
    const double *channelGetMeasurementVector(unsigned vector_index, unsigned *vector_size);

};


#endif   //    WienerMpodLvHvSnmp_H
