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
# testing high level commands for modules of the WienerMpodLvHvCtrl device server
##

import tango
import time

test_device = tango.DeviceProxy("mpod_test/mpod/1")
module_slot_index = 1

# if the DS runs a thread (initiualized with ThreadPollingIntervalMs > 1) we need to wait for the ...set...() commands to be applied
#       this should not take longer than 2 * ThreadPollingIntervalMs
wait_delay = 2
polling_interval = test_device.get_property('ThreadPollingIntervalMs')['ThreadPollingIntervalMs']
if len(polling_interval):
    wait_delay = float(polling_interval[0]) * 0.001
if wait_delay < 0.5:
    wait_delay = 0.5

# command: ModuleGetAuxiliaryMeasurementVoltage0
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetAuxiliaryMeasurementVoltage0({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetAuxiliaryMeasurementVoltage0', module_slot_index))

# command: ModuleGetAuxiliaryMeasurementVoltage1
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetAuxiliaryMeasurementVoltage1({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetAuxiliaryMeasurementVoltage1', module_slot_index))

# command: ModuleGetDescription
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetDescription({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetDescription', module_slot_index))

# command: ModuleGetEventChannelStatus
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetEventChannelStatus({0}) returns \'{1}\''.format(module_slot_index, hex(test_device.command_inout('ModuleGetEventChannelStatus', module_slot_index)))

# command: ModuleGetEventStatus
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetEventStatus({0}) returns \'{1}\''.format(module_slot_index, hex(test_device.command_inout('ModuleGetEventStatus', module_slot_index)))

# command: ModuleGetEventStatusAsString
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetEventStatusAsString({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetEventStatusAsString', module_slot_index))

# command: ModuleGetHardwareLimitCurrent
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetHardwareLimitCurrent({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetHardwareLimitCurrent', module_slot_index))

# command: ModuleGetHardwareLimitVoltage
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetHardwareLimitVoltage({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetHardwareLimitVoltage', module_slot_index))

# command: ModuleGetIndex
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetIndex({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetIndex', module_slot_index))

# command: ModuleGetMeasurementTemperature0
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetMeasurementTemperature0({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetMeasurementTemperature0', module_slot_index))

# command: ModuleGetMeasurementTemperature1
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetMeasurementTemperature1({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetMeasurementTemperature1', module_slot_index))

# command: ModuleGetMeasurementTemperature2
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetMeasurementTemperature2({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetMeasurementTemperature2', module_slot_index))

# command: ModuleGetMeasurementTemperature3
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetMeasurementTemperature3({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetMeasurementTemperature3', module_slot_index))

# command: ModuleGetRampSpeedCurrent
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetRampSpeedCurrent({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetRampSpeedCurrent', module_slot_index))

# command: ModuleGetRampSpeedVoltage
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetRampSpeedVoltage({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetRampSpeedVoltage', module_slot_index))

# command: ModuleGetStatus
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetStatus({0}) returns \'{1}\''.format(module_slot_index, hex(test_device.command_inout('ModuleGetStatus', module_slot_index)))

# command: ModuleGetStatusAsString
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetStatusAsString({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetStatusAsString', module_slot_index))

# command: ModuleSetDoClear
#     input: Slot index, Value
#     output: Uninitialised
print 'ModuleSetDoClear({0}, 1) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleSetDoClear', [module_slot_index, 1]))
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)

# command: ModuleSetRampSpeedCurrent
#     input: Slot index, Value
#     output: Uninitialised
print 'ModuleSetRampSpeedCurrent({0}, 10.5) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleSetRampSpeedCurrent', [module_slot_index, 10.5]))
print 'ModuleGetRampSpeedCurrent({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetRampSpeedCurrent', module_slot_index))

# command: ModuleSetRampSpeedVoltage
#     input: Slot index, Value
#     output: Uninitialised
print 'ModuleSetRampSpeedVoltage({0}, 0.5) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleSetRampSpeedVoltage', [module_slot_index, 0.5]))
print 'ModuleGetRampSpeedVoltage({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetRampSpeedVoltage', module_slot_index))

# command: ModuleGetIsHvModule
#     input: Slot index
#     output: Uninitialised
print 'ModuleGetIsHvModule({0}) returns \'{1}\''.format(module_slot_index, test_device.command_inout('ModuleGetIsHvModule', module_slot_index))



