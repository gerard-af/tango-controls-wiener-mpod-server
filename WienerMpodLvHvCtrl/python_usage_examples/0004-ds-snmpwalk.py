#
# This file is part of Tango device class.
# 
# Tango is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Tango is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Tango.  If not, see <http://www.gnu.org/licenses/>.
# 
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 

##
# snmpwalk command
##

import tango
import math

test_device = tango.DeviceProxy("mpod_test/mpod/1")
oid = 'iso'
# oid = 'WIENER-CRATE-MIB::moduleAuxiliaryMeasurementVoltage0.ma1'
oid = test_device.command_inout('SnmpGetNextOID', oid)
translated = test_device.command_inout('SnmpTranslateFromOID', oid)
data_type = ""
while oid != '':
    val = test_device.command_inout('SnmpGetValueDouble', oid)
    data_type = 'double'
    hex_int = ''
    if math.isnan(val):
        val = test_device.command_inout('SnmpGetValueInt', oid)
        data_type = 'integer'
        if val >= 4294967295:
            val = test_device.command_inout('SnmpGetValueString', oid)
            data_type = 'string'
        else:
            hex_int = hex(val);
    translated = test_device.command_inout('SnmpTranslateFromOID', oid)
    if oid == translated:
        print "{0} = {1} ({2}) {3}".format(oid, val, data_type, hex_int)
    else :
        print "{0} ({1}) = {2} ({3}) {4}".format(oid, translated, val, data_type, hex_int)
    oid = test_device.command_inout('SnmpGetNextOID', oid)    


