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
# testing high level commands for crates of the WienerMpodLvHvCtrl device server
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


# command: CrateGetFanSpeed
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetFanSpeed() returns \'{0}\''.format(test_device.command_inout('CrateGetFanSpeed'))

# command: CrateSetFanSpeed
#     input: Value
#     output: Uninitialised
new_fan_speed = 1500
print 'CrateSetFanSpeed({0}) returns \'{1}\''.format(new_fan_speed, test_device.command_inout('CrateSetFanSpeed', new_fan_speed))
print 'CrateGetFanSpeed() returns \'{0}\''.format(test_device.command_inout('CrateGetFanSpeed'))

# command: CrateGetNumberOfChannels
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetNumberOfChannels() returns \'{0}\''.format(test_device.command_inout('CrateGetNumberOfChannels'))

# command: CrateGetNumberOfGroups
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetNumberOfGroups() returns \'{0}\''.format(test_device.command_inout('CrateGetNumberOfGroups'))

# command: CrateGetNumberOfModules
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetNumberOfModules() returns \'{0}\''.format(test_device.command_inout('CrateGetNumberOfModules'))

# command: CrateGetSerialNumber
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetSerialNumber() returns \'{0}\''.format(test_device.command_inout('CrateGetSerialNumber'))

# command: CrateGetSysMainSwitch
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetSysMainSwitch() returns \'{0}\''.format(test_device.command_inout('CrateGetSysMainSwitch'))

# command: CrateGetSysStatus
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetSysStatus() returns \'{0}\''.format(test_device.command_inout('CrateGetSysStatus'))

# command: CrateGetSysStatusAsString
#     input: Uninitialised
#     output: Uninitialised
print 'CrateGetSysStatusAsString() returns \'{0}\''.format(test_device.command_inout('CrateGetSysStatusAsString'))

# command: CrateSetSysHardwareReset
#     input: Value
#     output: Uninitialised
## print 'CrateSetSysHardwareReset(1) returns \'{0}\''.format(test_device.command_inout('CrateSetSysHardwareReset', 1))

# command: CrateSetSysMainSwitch
#     input: Value
#     output: Uninitialised
print 'CrateSetSysMainSwitch(1) returns \'{0}\''.format(test_device.command_inout('CrateSetSysMainSwitch', 1))
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
print 'CrateGetSysMainSwitch() returns \'{0}\''.format(test_device.command_inout('CrateGetSysMainSwitch'))


