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
# testing high level commands for channels of the WienerMpodLvHvCtrl device server
##

import tango
import time

test_device = tango.DeviceProxy("mpod_test/mpod/1")
module_slot_index = 1
channel_index = 0

# if the DS runs a thread (initiualized with ThreadPollingIntervalMs > 1) we need to wait for the ...set...() commands to be applied
#       this should not take longer than 2 * ThreadPollingIntervalMs
wait_delay = 2
polling_interval = test_device.get_property('ThreadPollingIntervalMs')['ThreadPollingIntervalMs']
if len(polling_interval):
    wait_delay = float(polling_interval[0]) * 0.001
if wait_delay < 0.5:
    wait_delay = 0.5

# command: ChannelGetConfigMaxCurrent
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetConfigMaxCurrent', [module_slot_index, channel_index])
print 'ChannelGetConfigMaxCurrent({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetConfigMaxSenseVoltage
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetConfigMaxSenseVoltage', [module_slot_index, channel_index])
print 'ChannelGetConfigMaxSenseVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetConfigMaxTerminalVoltage
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetConfigMaxTerminalVoltage', [module_slot_index, channel_index])
print 'ChannelGetConfigMaxTerminalVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetCurrent
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetCurrent', [module_slot_index, channel_index])
print 'ChannelGetCurrent({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetCurrentFallRate
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetCurrentFallRate', [module_slot_index, channel_index])
print 'ChannelGetCurrentFallRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetCurrentRiseRate
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetCurrentRiseRate', [module_slot_index, channel_index])
print 'ChannelGetCurrentRiseRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetIndex
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetIndex', [module_slot_index, channel_index])
print 'ChannelGetIndex({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetMeasurementCurrent
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetMeasurementCurrent', [module_slot_index, channel_index])
print 'ChannelGetMeasurementCurrent({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetMeasurementSenseVoltage
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetMeasurementSenseVoltage', [module_slot_index, channel_index])
print 'ChannelGetMeasurementSenseVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetMeasurementTemperature
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetMeasurementTemperature', [module_slot_index, channel_index])
print 'ChannelGetMeasurementTemperature({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetMeasurementTerminalVoltage
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetMeasurementTerminalVoltage', [module_slot_index, channel_index])
print 'ChannelGetMeasurementTerminalVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetStatus
#     input: ModuleSlotIndex, ChannelIndex
#     output: ChannelStatus
res = test_device.command_inout('ChannelGetStatus', [module_slot_index, channel_index])
print 'ChannelGetStatus({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, hex(res))

# command: ChannelGetStatusAsString
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetStatusAsString', [module_slot_index, channel_index])
print 'ChannelGetStatusAsString({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetSupervisionBehavior
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetSupervisionBehavior', [module_slot_index, channel_index])
print 'ChannelGetSupervisionBehavior({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, hex(res))

# command: ChannelGetSupervisionMaxCurrent
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetSupervisionMaxCurrent', [module_slot_index, channel_index])
print 'ChannelGetSupervisionMaxCurrent({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetSupervisionMaxPower
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetSupervisionMaxPower', [module_slot_index, channel_index])
print 'ChannelGetSupervisionMaxPower({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetSupervisionMaxTerminalVoltage
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetSupervisionMaxTerminalVoltage', [module_slot_index, channel_index])
print 'ChannelGetSupervisionMaxTerminalVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetSupervisionMinSenseVoltage
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetSupervisionMinSenseVoltage', [module_slot_index, channel_index])
print 'ChannelGetSupervisionMinSenseVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetSwitch
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetSwitch', [module_slot_index, channel_index])
print 'ChannelGetSwitch({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetTripTime
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetTripTime', [module_slot_index, channel_index])
print 'ChannelGetTripTime({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetUserConfig
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetUserConfig', [module_slot_index, channel_index])
print 'ChannelGetUserConfig({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, hex(res))

# command: ChannelGetVoltage
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetVoltage', [module_slot_index, channel_index])
print 'ChannelGetVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetVoltageFallRate
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetVoltageFallRate', [module_slot_index, channel_index])
print 'ChannelGetVoltageFallRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelGetVoltageRiseRate
#     input: ModuleSlotIndex, ChannelIndex
#     output: Uninitialised
res = test_device.command_inout('ChannelGetVoltageRiseRate', [module_slot_index, channel_index])
print 'ChannelGetVoltageRiseRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetCurrent
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 0.0005
res = test_device.command_inout('ChannelSetCurrent', [module_slot_index, channel_index, val])
print 'ChannelSetCurrent({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
res = test_device.command_inout('ChannelGetCurrent', [module_slot_index, channel_index])
print 'ChannelGetCurrent({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetCurrentFallRate
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 1.1
res = test_device.command_inout('ChannelSetCurrentFallRate', [module_slot_index, channel_index, val])
print 'ChannelSetCurrentFallRate({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetCurrentFallRate', [module_slot_index, channel_index])
print 'ChannelGetCurrentFallRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetCurrentRiseRate
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 1.2
res = test_device.command_inout('ChannelSetCurrentRiseRate', [module_slot_index, channel_index, val])
print 'ChannelSetCurrentRiseRate({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetCurrentRiseRate', [module_slot_index, channel_index])
print 'ChannelGetCurrentRiseRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetSupervisionBehavior
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 17680
res = test_device.command_inout('ChannelSetSupervisionBehavior', [module_slot_index, channel_index, val])
print 'ChannelSetSupervisionBehavior({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetSupervisionBehavior', [module_slot_index, channel_index])
print 'ChannelGetSupervisionBehavior({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, hex(res))

# command: ChannelSetSupervisionMaxCurrent
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 1e-3
res = test_device.command_inout('ChannelSetSupervisionMaxCurrent', [module_slot_index, channel_index, val])
print 'ChannelSetSupervisionMaxCurrent({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetSupervisionMaxCurrent', [module_slot_index, channel_index])
print 'ChannelGetSupervisionMaxCurrent({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetSupervisionMaxTerminalVoltage
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 3.0
res = test_device.command_inout('ChannelSetSupervisionMaxTerminalVoltage', [module_slot_index, channel_index, val])
print 'ChannelSetSupervisionMaxTerminalVoltage({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetSupervisionMaxTerminalVoltage', [module_slot_index, channel_index])
print 'ChannelGetSupervisionMaxTerminalVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetSupervisionMinSenseVoltage
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 1.0
res = test_device.command_inout('ChannelSetSupervisionMinSenseVoltage', [module_slot_index, channel_index, val])
print 'ChannelSetSupervisionMinSenseVoltage({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetSupervisionMinSenseVoltage', [module_slot_index, channel_index])
print 'ChannelGetSupervisionMinSenseVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetSwitch
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 1
res = test_device.command_inout('ChannelSetSwitch', [module_slot_index, channel_index, val])
print 'ChannelSetSwitch({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetSwitch', [module_slot_index, channel_index])
print 'ChannelGetSwitch({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetTripTime
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 100
res = test_device.command_inout('ChannelSetTripTime', [module_slot_index, channel_index, val])
print 'ChannelSetTripTime({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetTripTime', [module_slot_index, channel_index])
print 'ChannelGetTripTime({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetUserConfig
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 8
res = test_device.command_inout('ChannelSetUserConfig', [module_slot_index, channel_index, val])
print 'ChannelSetUserConfig({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetUserConfig', [module_slot_index, channel_index])
print 'ChannelGetUserConfig({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, hex(res))

# command: ChannelSetVoltage
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 4.5
res = test_device.command_inout('ChannelSetVoltage', [module_slot_index, channel_index, val])
print 'ChannelSetVoltage({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetVoltage', [module_slot_index, channel_index])
print 'ChannelGetVoltage({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetVoltageFallRate
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 1.0
res = test_device.command_inout('ChannelSetVoltageFallRate', [module_slot_index, channel_index, val])
print 'ChannelSetVoltageFallRate({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetVoltageFallRate', [module_slot_index, channel_index])
print 'ChannelGetVoltageFallRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)

# command: ChannelSetVoltageRiseRate
#     input: ModuleSlotIndex, ChannelIndex, Value
#     output: Uninitialised
val = 2.0
res = test_device.command_inout('ChannelSetVoltageRiseRate', [module_slot_index, channel_index, val])
print 'ChannelSetVoltageRiseRate({0}, {1}, {2}) returns \'{3}\''.format(module_slot_index, channel_index, val, res)
## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)
res = test_device.command_inout('ChannelGetVoltageRiseRate', [module_slot_index, channel_index])
print 'ChannelGetVoltageRiseRate({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, res)




