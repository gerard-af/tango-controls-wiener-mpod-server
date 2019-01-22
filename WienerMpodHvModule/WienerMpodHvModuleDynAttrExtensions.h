/*----- PROTECTED REGION ID(WienerMpodHvModuleClass.h) ENABLED START -----*/
//=============================================================================
//
// file :        WienerMpodHvModuleDynAttrExtensions.h
//
// description : 
//
// project :     Uses WienerMpodLvHvCtrl to control a single HV module
//
// This file is part of Tango device class.
// 
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
// 
//
//

#ifndef __WienerMpodHvModuleDynAttrExtensions__h__
#define __WienerMpodHvModuleDynAttrExtensions__h__

#include "WienerMpodHvModuleClass.h"

/*
 * these classes extend the dynamic attributes from WienerMpodHvModuleClass.h.
 *
*/

namespace WienerMpodHvModule_ns
{

class ChannelDynDoubleReadAttribImpl: public ChannelDynDoubleReadAttrib {

    size_t m_channelIndex;
    std::string m_readCommandName;

public:

    ChannelDynDoubleReadAttribImpl(const string &att_name, size_t channel_index, const std::string &read_command_name)
        : ChannelDynDoubleReadAttrib(att_name) {
        m_channelIndex = channel_index;
        m_readCommandName = read_command_name;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelDynDouble(att, m_channelIndex, m_readCommandName);
    }

};

class ChannelDynDoubleReadWriteAttribImpl: public ChannelDynDoubleReadWriteAttrib {

    size_t m_channelIndex;
    std::string m_readCommandName;
    std::string m_writeCommandName;

public:
    ChannelDynDoubleReadWriteAttribImpl(const string &att_name, size_t channel_index, const std::string &read_command_name, const std::string &write_command_name)
        : ChannelDynDoubleReadWriteAttrib(att_name) {
        m_channelIndex = channel_index;
        m_readCommandName = read_command_name;
        m_writeCommandName = write_command_name;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelDynDouble(att, m_channelIndex, m_readCommandName);
    }
    void write(Tango::DeviceImpl *dev,Tango::WAttribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->writeChannelDynDouble(att, m_channelIndex, m_writeCommandName);
    }
};

class ChannelDynLongReadAttribImpl: public ChannelDynLongReadAttrib {

    size_t m_channelIndex;
    std::string m_readCommandName;

public:
    ChannelDynLongReadAttribImpl(const string &att_name, size_t channel_index, const std::string &read_command_name)
        :ChannelDynLongReadAttrib(att_name) {
        m_channelIndex = channel_index;
        m_readCommandName = read_command_name;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelDynLong(att, m_channelIndex, m_readCommandName);
    }
};

class ChannelDynLongReadWriteAttribImpl: public ChannelDynLongReadWriteAttrib {

    size_t m_channelIndex;
    std::string m_readCommandName;
    std::string m_writeCommandName;

public:
    ChannelDynLongReadWriteAttribImpl(const string &att_name, size_t channel_index, const std::string &read_command_name, const std::string &write_command_name)
        : ChannelDynLongReadWriteAttrib(att_name) {
        m_channelIndex = channel_index;
        m_readCommandName = read_command_name;
        m_writeCommandName = write_command_name;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelDynLong(att, m_channelIndex, m_readCommandName);
    }
    void write(Tango::DeviceImpl *dev,Tango::WAttribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->writeChannelDynLong(att, m_channelIndex, m_writeCommandName);
    }
};

class ChannelDynStringReadAttribImpl: public ChannelDynStringReadAttrib {

    size_t m_channelIndex;
    std::string m_readCommandName;

public:
    ChannelDynStringReadAttribImpl(const string &att_name, size_t channel_index, const std::string &read_command_name)
        :ChannelDynStringReadAttrib(att_name) {
        m_channelIndex = channel_index;
        m_readCommandName = read_command_name;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelDynString(att, m_channelIndex, m_readCommandName);
    }
};

class ChannelDynBoolReadWriteAttribEnum: public ChannelDynBoolReadWriteAttrib {

    size_t m_channelIndex;
    std::string m_readCommandName;
    std::string m_writeCommandName;
    size_t m_enumValue;

public:
    ChannelDynBoolReadWriteAttribEnum(const string &att_name, size_t channel_index, const std::string &read_command_name, const std::string &write_command_name, size_t enum_value)
        : ChannelDynBoolReadWriteAttrib(att_name) {
        m_channelIndex = channel_index;
        m_readCommandName = read_command_name;
        m_writeCommandName = write_command_name;
        m_enumValue = enum_value;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelDynBoolEnum(att, m_channelIndex, m_readCommandName, m_enumValue);
    }
    void write(Tango::DeviceImpl *dev,Tango::WAttribute &att){
        (static_cast<WienerMpodHvModule *>(dev))->writeChannelDynBoolEnum(att, m_channelIndex, m_writeCommandName, m_enumValue);
    }
};

class ChannelSwitchDynEnumAttrib: public ChannelDynEnumReadWriteAttrib {

    size_t m_channelIndex;

public:
    ChannelSwitchDynEnumAttrib(const string &att_name, size_t channel_index)
        :ChannelDynEnumReadWriteAttrib(att_name) {
        m_channelIndex = channel_index;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelSwitchDynEnum(att, m_channelIndex);
    }
    void write(Tango::DeviceImpl *dev,Tango::WAttribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->writeChannelSwitchDynEnum(att, m_channelIndex);
    }
};

class ChannelTripActionDynEnumAttrib: public ChannelDynEnumReadWriteAttrib {

    size_t m_channelIndex;

public:
    ChannelTripActionDynEnumAttrib(const string &att_name, size_t channel_index)
        :ChannelDynEnumReadWriteAttrib(att_name) {
        m_channelIndex = channel_index;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelTripActionDynEnum(att, m_channelIndex);
    }
    void write(Tango::DeviceImpl *dev,Tango::WAttribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->writeChannelTripActionDynEnum(att, m_channelIndex);
    }
};

class ChannelInhibitActionDynEnumAttrib: public ChannelDynEnumReadWriteAttrib {

    size_t m_channelIndex;

public:
    ChannelInhibitActionDynEnumAttrib(const string &att_name, size_t channel_index)
        :ChannelDynEnumReadWriteAttrib(att_name) {
        m_channelIndex = channel_index;
    }
    void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelInhibitActionDynEnum(att, m_channelIndex);
    }
    void write(Tango::DeviceImpl *dev,Tango::WAttribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->writeChannelInhibitActionDynEnum(att, m_channelIndex);
    }
};

class DynReadDoubleVectorAttrib : public Tango::SpectrumAttr {

    std::string m_readCommandName;

public:
    DynReadDoubleVectorAttrib(const string &att_name, const std::string &read_command_name, size_t vector_size = 1024)
        : SpectrumAttr(att_name.c_str(), Tango::DEV_DOUBLE, Tango::READ, vector_size) {
        m_readCommandName = read_command_name;
    }
    virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readDynDoubleVector(att, m_readCommandName);
    }
    virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty) {
        return true;
    }
};

class ChannelDynReadDoubleVectorAttrib: public Tango::SpectrumAttr {

    size_t m_channelIndex;
    std::string m_readCommandName;

public:
    ChannelDynReadDoubleVectorAttrib(const string &att_name, size_t channel_index, const std::string &read_command_name, size_t vector_size = 1024)
        : SpectrumAttr(att_name.c_str(), Tango::DEV_DOUBLE, Tango::READ, vector_size) {
        m_channelIndex = channel_index;
        m_readCommandName = read_command_name;
    }
    virtual void read(Tango::DeviceImpl *dev,Tango::Attribute &att) {
        (static_cast<WienerMpodHvModule *>(dev))->readChannelDynDoubleVector(att, m_channelIndex, m_readCommandName);
    }
    virtual bool is_allowed(Tango::DeviceImpl *dev,Tango::AttReqType ty) {
        return true;
    }
};

} // end namespace

#endif


