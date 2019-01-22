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
# testing low level commands of the WienerMpodLvHvCtrl device server
##

import tango
import time

test_device = tango.DeviceProxy("mpod_test/mpod/1")

# if the DS runs a thread (initiualized with ThreadPollingIntervalMs > 1) we need to wait for the ...set...() commands to be applied
#       this should not take longer than 2 * ThreadPollingIntervalMs
wait_delay = 2
polling_interval = test_device.get_property('ThreadPollingIntervalMs')['ThreadPollingIntervalMs']
if len(polling_interval):
    wait_delay = float(polling_interval[0]) * 0.001
if wait_delay < 0.5:
    wait_delay = 0.5

# command: SnmpGetDescription
#     input: OID as string
#     output: Description from MIB file.
test_device.command_inout('SnmpGetDescription', 'iso.3.6.1.4.1.19947.1.3.2.1.2.101')
test_device.command_inout('SnmpGetDescription', 'WIENER-CRATE-MIB::outputName.u100')

# command: SnmpGetNextOID
#     input: CurrentOID
#     output: NextOID
oid = 'iso'
print 'SnmpGetNextOID(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetNextOID', oid))
oid = 'WIENER-CRATE-MIB::moduleAuxiliaryMeasurementVoltage0'
print 'SnmpGetNextOID(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetNextOID', oid))

# command: SnmpGetValueDouble
#     input: OID name
#     output: Readout
oid = 'iso.3.6.1.4.1.19947.1.3.2.1.5.101'
print 'SnmpGetValueDouble(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueDouble', oid))
oid = 'WIENER-CRATE-MIB::outputMeasurementSenseVoltage.u100'
print 'SnmpGetValueDouble(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueDouble', oid))

# command: SnmpGetValueInt
#     input: OID name
#     output: Readout
oid = 'iso.3.6.1.4.1.19947.1.1.1.0'
print 'SnmpGetValueInt(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueInt', oid))
oid = 'WIENER-CRATE-MIB::sysMainSwitch.0'
print 'SnmpGetValueInt(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueInt', oid))

# command: SnmpGetValueString
#     input: OID name
#     output: Readout
oid = 'iso.3.6.1.2.1.1.1.0'
print 'SnmpGetValueString(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueString', oid))
oid = 'SNMPv2-SMI::mib-2.1.1.0'
print 'SnmpGetValueString(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueString', oid))

# command: SnmpSetValueDouble
#     input: Variable name and value to set
#     output: Success
oid = 'iso.3.6.1.4.1.19947.1.3.2.1.10.301'
print 'SnmpSetValueDouble(\'{0}\', 1027.0) returns \'{1}\''.format(oid, test_device.command_inout('SnmpSetValueDouble', [[1027.0], [oid]]))
## it takes some time until the the new value is taken into account (write and read to device)
time.sleep(wait_delay)
print 'SnmpGetValueDouble(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueDouble', oid))
oid = 'WIENER-CRATE-MIB::outputVoltage.u300'
print 'SnmpSetValueDouble(\'{0}\', 1327.0) returns \'{1}\''.format(oid, test_device.command_inout('SnmpSetValueDouble', [[1327.0], [oid]]))
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
print 'SnmpGetValueDouble(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueDouble', oid))

# command: SnmpSetValueInt
#     input: Variable name and value to set
#     output: Success
oid = 'iso.3.6.1.4.1.19947.1.1.1.0'
print 'SnmpSetValueInt(\'{0}\', 0) returns \'{1}\''.format(oid, test_device.command_inout('SnmpSetValueInt', [[0], [oid]]))
## it takes some time until the the new value is taken into account (write and read to device)
time.sleep(wait_delay)
print 'SnmpGetValueInt(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueInt', oid))
oid = 'WIENER-CRATE-MIB::sysMainSwitch.0'
print 'SnmpSetValueInt(\'{0}\', 1) returns \'{1}\''.format(oid, test_device.command_inout('SnmpSetValueInt', [[1], [oid]]))
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
print 'SnmpGetValueInt(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpGetValueInt', oid))

# command: SnmpTranslateFromOID
#     input: OID as string
#     output: Human readable string
oid = 'iso.3.6.1.4.1.19947.1.1.1.0'
print 'SnmpTranslateFromOID(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpTranslateFromOID', oid))
oid = 'WIENER-CRATE-MIB::sysMainSwitch.0'
print 'SnmpTranslateFromOID(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpTranslateFromOID', oid))

# command: SnmpTranslateToOID
#     input: Human readable string
#     output: OID as string
oid = 'iso.3.6.1.4.1.19947.1.1.1.0'
print 'SnmpTranslateToOID(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpTranslateToOID', oid))
oid = 'WIENER-CRATE-MIB::sysMainSwitch.0'
print 'SnmpTranslateToOID(\'{0}\') returns \'{1}\''.format(oid, test_device.command_inout('SnmpTranslateToOID', oid))



