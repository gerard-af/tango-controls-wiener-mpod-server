/*----- PROTECTED REGION ID(WienerMpodHvModule::DynAttrUtils.cpp) ENABLED START -----*/
//=============================================================================
//
// file :        WienerMpodHvModuleDynAttrUtils.cpp
//
// description : Dynamic attributes utilities file for the WienerMpodHvModule class
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
//=============================================================================
//                This file is generated by POGO
//        (Program Obviously used to Generate tango Object)
//=============================================================================


#include <WienerMpodHvModule.h>
#include <WienerMpodHvModuleClass.h>

/*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::DynAttrUtils.cpp

//================================================================
//  Attributes managed are:
//================================================================
//  ChannelDynDoubleRead        |  Tango::DevDouble    Scalar
//  ChannelDynDoubleReadWrite   |  Tango::DevDouble    Scalar
//  ChannelDynLongRead          |  Tango::DevLong    Scalar
//  ChannelDynLongReadWrite     |  Tango::DevLong    Scalar
//  ChannelDynStringRead        |  Tango::DevString    Scalar
//  ChannelDynBoolReadWrite     |  Tango::DevBoolean    Scalar
//  ChannelDynEnumReadWrite     |  Tango::DevEnum    Scalar
//  ChannelDynDoubleVectorRead  |  Tango::DevDouble    Spectrum  ( max = 1024)
//================================================================

//    For compatibility reason, this file (WienerMpodHvModuleDynAttrUtils)
//    manage also the dynamic command utilities.
//================================================================
//  The following table gives the correspondence
//  between command and method names.
//
//  Command name  |  Method name
//================================================================
//================================================================

namespace WienerMpodHvModule_ns
{
//=============================================================
//    Add/Remove dynamic attribute methods
//=============================================================

//--------------------------------------------------------
/**
 *    Add a ChannelDynDoubleRead dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynDoubleRead_dynamic_attribute(string attname)
{
    //    Attribute : ChannelDynDoubleRead
    ChannelDynDoubleReadAttrib    *channeldyndoubleread = new ChannelDynDoubleReadAttrib(attname);
    Tango::UserDefaultAttrProp    channeldyndoubleread_prop;
    //    description    not set for ChannelDynDoubleRead
    //    label    not set for ChannelDynDoubleRead
    //    unit    not set for ChannelDynDoubleRead
    //    standard_unit    not set for ChannelDynDoubleRead
    //    display_unit    not set for ChannelDynDoubleRead
    //    format    not set for ChannelDynDoubleRead
    //    max_value    not set for ChannelDynDoubleRead
    //    min_value    not set for ChannelDynDoubleRead
    //    max_alarm    not set for ChannelDynDoubleRead
    //    min_alarm    not set for ChannelDynDoubleRead
    //    max_warning    not set for ChannelDynDoubleRead
    //    min_warning    not set for ChannelDynDoubleRead
    //    delta_t    not set for ChannelDynDoubleRead
    //    delta_val    not set for ChannelDynDoubleRead
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynDoubleRead_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynDoubleRead_dynamic_attribute
    channeldyndoubleread->set_default_properties(channeldyndoubleread_prop);
    //    Not Polled
    channeldyndoubleread->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    ChannelDynDoubleRead_data.insert(make_pair(attname, 0.0));
    add_attribute(channeldyndoubleread);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynDoubleRead dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynDoubleRead_dynamic_attribute(string attname)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevDouble>::iterator ite;
    if ((ite=ChannelDynDoubleRead_data.find(attname))!=ChannelDynDoubleRead_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynDoubleRead_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynDoubleRead_dynamic_attribute
        ChannelDynDoubleRead_data.erase(ite);
    }
}
//--------------------------------------------------------
/**
 *    Add a ChannelDynDoubleReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynDoubleReadWrite_dynamic_attribute(string attname)
{
    //    Attribute : ChannelDynDoubleReadWrite
    ChannelDynDoubleReadWriteAttrib    *channeldyndoublereadwrite = new ChannelDynDoubleReadWriteAttrib(attname);
    Tango::UserDefaultAttrProp    channeldyndoublereadwrite_prop;
    //    description    not set for ChannelDynDoubleReadWrite
    //    label    not set for ChannelDynDoubleReadWrite
    //    unit    not set for ChannelDynDoubleReadWrite
    //    standard_unit    not set for ChannelDynDoubleReadWrite
    //    display_unit    not set for ChannelDynDoubleReadWrite
    //    format    not set for ChannelDynDoubleReadWrite
    //    max_value    not set for ChannelDynDoubleReadWrite
    //    min_value    not set for ChannelDynDoubleReadWrite
    //    max_alarm    not set for ChannelDynDoubleReadWrite
    //    min_alarm    not set for ChannelDynDoubleReadWrite
    //    max_warning    not set for ChannelDynDoubleReadWrite
    //    min_warning    not set for ChannelDynDoubleReadWrite
    //    delta_t    not set for ChannelDynDoubleReadWrite
    //    delta_val    not set for ChannelDynDoubleReadWrite
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynDoubleReadWrite_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynDoubleReadWrite_dynamic_attribute
    channeldyndoublereadwrite->set_default_properties(channeldyndoublereadwrite_prop);
    //    Not Polled
    channeldyndoublereadwrite->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    ChannelDynDoubleReadWrite_data.insert(make_pair(attname, 0.0));
    add_attribute(channeldyndoublereadwrite);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynDoubleReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynDoubleReadWrite_dynamic_attribute(string attname)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevDouble>::iterator ite;
    if ((ite=ChannelDynDoubleReadWrite_data.find(attname))!=ChannelDynDoubleReadWrite_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynDoubleReadWrite_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynDoubleReadWrite_dynamic_attribute
        ChannelDynDoubleReadWrite_data.erase(ite);
    }
}
//--------------------------------------------------------
/**
 *    Add a ChannelDynLongRead dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynLongRead_dynamic_attribute(string attname)
{
    //    Attribute : ChannelDynLongRead
    ChannelDynLongReadAttrib    *channeldynlongread = new ChannelDynLongReadAttrib(attname);
    Tango::UserDefaultAttrProp    channeldynlongread_prop;
    //    description    not set for ChannelDynLongRead
    //    label    not set for ChannelDynLongRead
    //    unit    not set for ChannelDynLongRead
    //    standard_unit    not set for ChannelDynLongRead
    //    display_unit    not set for ChannelDynLongRead
    //    format    not set for ChannelDynLongRead
    //    max_value    not set for ChannelDynLongRead
    //    min_value    not set for ChannelDynLongRead
    //    max_alarm    not set for ChannelDynLongRead
    //    min_alarm    not set for ChannelDynLongRead
    //    max_warning    not set for ChannelDynLongRead
    //    min_warning    not set for ChannelDynLongRead
    //    delta_t    not set for ChannelDynLongRead
    //    delta_val    not set for ChannelDynLongRead
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynLongRead_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynLongRead_dynamic_attribute
    channeldynlongread->set_default_properties(channeldynlongread_prop);
    //    Not Polled
    channeldynlongread->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    ChannelDynLongRead_data.insert(make_pair(attname, 0));
    add_attribute(channeldynlongread);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynLongRead dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynLongRead_dynamic_attribute(string attname)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevLong>::iterator ite;
    if ((ite=ChannelDynLongRead_data.find(attname))!=ChannelDynLongRead_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynLongRead_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynLongRead_dynamic_attribute
        ChannelDynLongRead_data.erase(ite);
    }
}
//--------------------------------------------------------
/**
 *    Add a ChannelDynLongReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynLongReadWrite_dynamic_attribute(string attname)
{
    //    Attribute : ChannelDynLongReadWrite
    ChannelDynLongReadWriteAttrib    *channeldynlongreadwrite = new ChannelDynLongReadWriteAttrib(attname);
    Tango::UserDefaultAttrProp    channeldynlongreadwrite_prop;
    //    description    not set for ChannelDynLongReadWrite
    //    label    not set for ChannelDynLongReadWrite
    //    unit    not set for ChannelDynLongReadWrite
    //    standard_unit    not set for ChannelDynLongReadWrite
    //    display_unit    not set for ChannelDynLongReadWrite
    //    format    not set for ChannelDynLongReadWrite
    //    max_value    not set for ChannelDynLongReadWrite
    //    min_value    not set for ChannelDynLongReadWrite
    //    max_alarm    not set for ChannelDynLongReadWrite
    //    min_alarm    not set for ChannelDynLongReadWrite
    //    max_warning    not set for ChannelDynLongReadWrite
    //    min_warning    not set for ChannelDynLongReadWrite
    //    delta_t    not set for ChannelDynLongReadWrite
    //    delta_val    not set for ChannelDynLongReadWrite
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynLongReadWrite_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynLongReadWrite_dynamic_attribute
    channeldynlongreadwrite->set_default_properties(channeldynlongreadwrite_prop);
    //    Not Polled
    channeldynlongreadwrite->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    ChannelDynLongReadWrite_data.insert(make_pair(attname, 0));
    add_attribute(channeldynlongreadwrite);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynLongReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynLongReadWrite_dynamic_attribute(string attname)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevLong>::iterator ite;
    if ((ite=ChannelDynLongReadWrite_data.find(attname))!=ChannelDynLongReadWrite_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynLongReadWrite_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynLongReadWrite_dynamic_attribute
        ChannelDynLongReadWrite_data.erase(ite);
    }
}
//--------------------------------------------------------
/**
 *    Add a ChannelDynStringRead dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynStringRead_dynamic_attribute(string attname)
{
    //    Attribute : ChannelDynStringRead
    ChannelDynStringReadAttrib    *channeldynstringread = new ChannelDynStringReadAttrib(attname);
    Tango::UserDefaultAttrProp    channeldynstringread_prop;
    //    description    not set for ChannelDynStringRead
    //    label    not set for ChannelDynStringRead
    //    unit    not set for ChannelDynStringRead
    //    standard_unit    not set for ChannelDynStringRead
    //    display_unit    not set for ChannelDynStringRead
    //    format    not set for ChannelDynStringRead
    //    max_value    not set for ChannelDynStringRead
    //    min_value    not set for ChannelDynStringRead
    //    max_alarm    not set for ChannelDynStringRead
    //    min_alarm    not set for ChannelDynStringRead
    //    max_warning    not set for ChannelDynStringRead
    //    min_warning    not set for ChannelDynStringRead
    //    delta_t    not set for ChannelDynStringRead
    //    delta_val    not set for ChannelDynStringRead
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynStringRead_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynStringRead_dynamic_attribute
    channeldynstringread->set_default_properties(channeldynstringread_prop);
    //    Not Polled
    channeldynstringread->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    char array[1];
    array[0] = '\0';
    ChannelDynStringRead_data.insert(make_pair(attname, array));
    add_attribute(channeldynstringread);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynStringRead dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynStringRead_dynamic_attribute(string attname)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevString>::iterator ite;
    if ((ite=ChannelDynStringRead_data.find(attname))!=ChannelDynStringRead_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynStringRead_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynStringRead_dynamic_attribute
        ChannelDynStringRead_data.erase(ite);
    }
}
//--------------------------------------------------------
/**
 *    Add a ChannelDynBoolReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynBoolReadWrite_dynamic_attribute(string attname)
{
    //    Attribute : ChannelDynBoolReadWrite
    ChannelDynBoolReadWriteAttrib    *channeldynboolreadwrite = new ChannelDynBoolReadWriteAttrib(attname);
    Tango::UserDefaultAttrProp    channeldynboolreadwrite_prop;
    //    description    not set for ChannelDynBoolReadWrite
    //    label    not set for ChannelDynBoolReadWrite
    //    unit    not set for ChannelDynBoolReadWrite
    //    standard_unit    not set for ChannelDynBoolReadWrite
    //    display_unit    not set for ChannelDynBoolReadWrite
    //    format    not set for ChannelDynBoolReadWrite
    //    max_value    not set for ChannelDynBoolReadWrite
    //    min_value    not set for ChannelDynBoolReadWrite
    //    max_alarm    not set for ChannelDynBoolReadWrite
    //    min_alarm    not set for ChannelDynBoolReadWrite
    //    max_warning    not set for ChannelDynBoolReadWrite
    //    min_warning    not set for ChannelDynBoolReadWrite
    //    delta_t    not set for ChannelDynBoolReadWrite
    //    delta_val    not set for ChannelDynBoolReadWrite
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynBoolReadWrite_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynBoolReadWrite_dynamic_attribute
    channeldynboolreadwrite->set_default_properties(channeldynboolreadwrite_prop);
    //    Not Polled
    channeldynboolreadwrite->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    ChannelDynBoolReadWrite_data.insert(make_pair(attname, false));
    add_attribute(channeldynboolreadwrite);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynBoolReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynBoolReadWrite_dynamic_attribute(string attname)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevBoolean>::iterator ite;
    if ((ite=ChannelDynBoolReadWrite_data.find(attname))!=ChannelDynBoolReadWrite_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynBoolReadWrite_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynBoolReadWrite_dynamic_attribute
        ChannelDynBoolReadWrite_data.erase(ite);
    }
}
//--------------------------------------------------------
/**
 *    Add a ChannelDynEnumReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynEnumReadWrite_dynamic_attribute(string attname)
{
    //    Attribute : ChannelDynEnumReadWrite
    ChannelDynEnumReadWriteAttrib    *channeldynenumreadwrite = new ChannelDynEnumReadWriteAttrib(attname);
    Tango::UserDefaultAttrProp    channeldynenumreadwrite_prop;
    //    description    not set for ChannelDynEnumReadWrite
    //    label    not set for ChannelDynEnumReadWrite
    //    unit    not set for ChannelDynEnumReadWrite
    //    standard_unit    not set for ChannelDynEnumReadWrite
    //    display_unit    not set for ChannelDynEnumReadWrite
    //    format    not set for ChannelDynEnumReadWrite
    //    max_value    not set for ChannelDynEnumReadWrite
    //    min_value    not set for ChannelDynEnumReadWrite
    //    max_alarm    not set for ChannelDynEnumReadWrite
    //    min_alarm    not set for ChannelDynEnumReadWrite
    //    max_warning    not set for ChannelDynEnumReadWrite
    //    min_warning    not set for ChannelDynEnumReadWrite
    //    delta_t    not set for ChannelDynEnumReadWrite
    //    delta_val    not set for ChannelDynEnumReadWrite
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynEnumReadWrite_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynEnumReadWrite_dynamic_attribute
    {
        vector<string> labels;
        labels.push_back("OFF");
        labels.push_back("ON");
        labels.push_back("CLEAR EMERGENCY");
        labels.push_back("EMERGENCY");
        labels.push_back("CLEAR EVENTS");
        channeldynenumreadwrite_prop.set_enum_labels(labels);
    }
    channeldynenumreadwrite->set_default_properties(channeldynenumreadwrite_prop);
    //    Not Polled
    channeldynenumreadwrite->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    ChannelDynEnumReadWrite_data.insert(make_pair(attname, 0));
    add_attribute(channeldynenumreadwrite);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynEnumReadWrite dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynEnumReadWrite_dynamic_attribute(string attname)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevEnum>::iterator ite;
    if ((ite=ChannelDynEnumReadWrite_data.find(attname))!=ChannelDynEnumReadWrite_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynEnumReadWrite_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynEnumReadWrite_dynamic_attribute
        ChannelDynEnumReadWrite_data.erase(ite);
    }
}
//--------------------------------------------------------
/**
 *    Add a ChannelDynDoubleVectorRead dynamic attribute.
 *
 *  parameter attname: attribute name to be cretated and added.
 *  parameter ptr:     memory buffer used to set attribute value.
 *                     If NULL or not specified, buffer will be allocated.
 */
//--------------------------------------------------------
void WienerMpodHvModule::add_ChannelDynDoubleVectorRead_dynamic_attribute(string attname, Tango::DevDouble *ptr)
{
    //    Attribute : ChannelDynDoubleVectorRead
    if (ptr==NULL)
        ptr = new Tango::DevDouble[1024];
    ChannelDynDoubleVectorReadAttrib    *channeldyndoublevectorread = new ChannelDynDoubleVectorReadAttrib(attname);
    Tango::UserDefaultAttrProp    channeldyndoublevectorread_prop;
    //    description    not set for ChannelDynDoubleVectorRead
    //    label    not set for ChannelDynDoubleVectorRead
    //    unit    not set for ChannelDynDoubleVectorRead
    //    standard_unit    not set for ChannelDynDoubleVectorRead
    //    display_unit    not set for ChannelDynDoubleVectorRead
    //    format    not set for ChannelDynDoubleVectorRead
    //    max_value    not set for ChannelDynDoubleVectorRead
    //    min_value    not set for ChannelDynDoubleVectorRead
    //    max_alarm    not set for ChannelDynDoubleVectorRead
    //    min_alarm    not set for ChannelDynDoubleVectorRead
    //    max_warning    not set for ChannelDynDoubleVectorRead
    //    min_warning    not set for ChannelDynDoubleVectorRead
    //    delta_t    not set for ChannelDynDoubleVectorRead
    //    delta_val    not set for ChannelDynDoubleVectorRead
    
    /*----- PROTECTED REGION ID(WienerMpodHvModule::att_ChannelDynDoubleVectorRead_dynamic_attribute) ENABLED START -----*/
    
    
    /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::att_ChannelDynDoubleVectorRead_dynamic_attribute
    channeldyndoublevectorread->set_default_properties(channeldyndoublevectorread_prop);
    //    Not Polled
    channeldyndoublevectorread->set_disp_level(Tango::OPERATOR);
    //    Not Memorized
    ChannelDynDoubleVectorRead_data.insert(make_pair(attname, ptr));
    add_attribute(channeldyndoublevectorread);
}
//--------------------------------------------------------
/**
 *    remove a ChannelDynDoubleVectorRead dynamic attribute.
 *
 *  parameter attname: attribute name to be removed.
 *  parameter free_it: memory buffer will be freed if true or not specified.
 */
//--------------------------------------------------------
void WienerMpodHvModule::remove_ChannelDynDoubleVectorRead_dynamic_attribute(string attname, bool free_it)
{
    remove_attribute(attname, true, Tango::Util::instance()->_UseDb);
    map<string,Tango::DevDouble *>::iterator ite;
    if ((ite=ChannelDynDoubleVectorRead_data.find(attname))!=ChannelDynDoubleVectorRead_data.end())
    {
        /*----- PROTECTED REGION ID(WienerMpodHvModule::remove_ChannelDynDoubleVectorRead_dynamic_attribute) ENABLED START -----*/
        
        /*----- PROTECTED REGION END -----*/    //    WienerMpodHvModule::remove_ChannelDynDoubleVectorRead_dynamic_attribute
        if (free_it)
            delete[] ite->second;
        ChannelDynDoubleVectorRead_data.erase(ite);
    }
}


//============================================================
//    Tool methods to get pointer on attribute data buffer 
//============================================================
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynDoubleRead data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevDouble *WienerMpodHvModule::get_ChannelDynDoubleRead_data_ptr(string &name)
{
    map<string,Tango::DevDouble>::iterator ite;
    if ((ite=ChannelDynDoubleRead_data.find(name))==ChannelDynDoubleRead_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynDoubleRead_data_ptr()");
    }
    return  &(ite->second);
}
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynDoubleReadWrite data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevDouble *WienerMpodHvModule::get_ChannelDynDoubleReadWrite_data_ptr(string &name)
{
    map<string,Tango::DevDouble>::iterator ite;
    if ((ite=ChannelDynDoubleReadWrite_data.find(name))==ChannelDynDoubleReadWrite_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynDoubleReadWrite_data_ptr()");
    }
    return  &(ite->second);
}
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynLongRead data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevLong *WienerMpodHvModule::get_ChannelDynLongRead_data_ptr(string &name)
{
    map<string,Tango::DevLong>::iterator ite;
    if ((ite=ChannelDynLongRead_data.find(name))==ChannelDynLongRead_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynLongRead_data_ptr()");
    }
    return  &(ite->second);
}
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynLongReadWrite data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevLong *WienerMpodHvModule::get_ChannelDynLongReadWrite_data_ptr(string &name)
{
    map<string,Tango::DevLong>::iterator ite;
    if ((ite=ChannelDynLongReadWrite_data.find(name))==ChannelDynLongReadWrite_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynLongReadWrite_data_ptr()");
    }
    return  &(ite->second);
}
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynStringRead data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevString *WienerMpodHvModule::get_ChannelDynStringRead_data_ptr(string &name)
{
    map<string,Tango::DevString>::iterator ite;
    if ((ite=ChannelDynStringRead_data.find(name))==ChannelDynStringRead_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynStringRead_data_ptr()");
    }
    return  &(ite->second);
}
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynBoolReadWrite data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevBoolean *WienerMpodHvModule::get_ChannelDynBoolReadWrite_data_ptr(string &name)
{
    map<string,Tango::DevBoolean>::iterator ite;
    if ((ite=ChannelDynBoolReadWrite_data.find(name))==ChannelDynBoolReadWrite_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynBoolReadWrite_data_ptr()");
    }
    return  &(ite->second);
}
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynEnumReadWrite data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevEnum *WienerMpodHvModule::get_ChannelDynEnumReadWrite_data_ptr(string &name)
{
    map<string,Tango::DevEnum>::iterator ite;
    if ((ite=ChannelDynEnumReadWrite_data.find(name))==ChannelDynEnumReadWrite_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynEnumReadWrite_data_ptr()");
    }
    return  &(ite->second);
}
//--------------------------------------------------------
/**
 *    Return a pointer on ChannelDynDoubleVectorRead data.
 *
 *  parameter attname: the specified attribute name.
 */
//--------------------------------------------------------
Tango::DevDouble *WienerMpodHvModule::get_ChannelDynDoubleVectorRead_data_ptr(string &name)
{
    map<string,Tango::DevDouble *>::iterator ite;
    if ((ite=ChannelDynDoubleVectorRead_data.find(name))==ChannelDynDoubleVectorRead_data.end())
    {
        TangoSys_OMemStream    tms;
        tms << "Dynamic attribute " << name << " has not been created";
        Tango::Except::throw_exception(
                    (const char *)"ATTRIBUTE_NOT_FOUND",
                    tms.str().c_str(),
                    (const char *)"WienerMpodHvModule::get_ChannelDynDoubleVectorRead_data_ptr()");
    }
    return  ite->second;
}


//=============================================================
//    Add/Remove dynamic command methods
//=============================================================


} //    namespace
