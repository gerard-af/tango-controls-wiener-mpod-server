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
# testing LV channels WienerMpodLvHvCtrl device server
##

import tango
import time
import wiener_mpod_help_functions
test_device = tango.DeviceProxy("mpod_test/mpod/1")

# if the DS runs a thread (initiualized with ThreadPollingIntervalMs > 1) we need to wait for the ...set...() commands to be applied
#       this should not take longer than 2 * ThreadPollingIntervalMs
wait_delay = 2
polling_interval = test_device.get_property('ThreadPollingIntervalMs')['ThreadPollingIntervalMs']
if len(polling_interval):
    wait_delay = float(polling_interval[0]) * 0.001
if wait_delay < 0.5:
    wait_delay = 0.5

def hv_channel_test(module_slot_index, channel_index):
    print 'testing HV channel {0} in module {1}'.format(channel_index, module_slot_index)
    val = 100
    res = test_device.command_inout('ChannelSetVoltage', [module_slot_index, channel_index, val])
    ## switch on
    print '  switching channel on'
    val = 1
    res = test_device.command_inout('ChannelSetSwitch', [module_slot_index, channel_index, val])
    ## wait until channel has ramped up
    time.sleep(wait_delay)
    channel_status = 2048
    while wiener_mpod_help_functions.checkBitIsSet(channel_status, 11):
        time.sleep(wait_delay)
        channel_status = test_device.command_inout('ChannelGetStatus', [module_slot_index, channel_index])
        meas_v = test_device.command_inout('ChannelGetMeasurementSenseVoltage', [module_slot_index, channel_index])
        meas_i = test_device.command_inout('ChannelGetMeasurementCurrent', [module_slot_index, channel_index])
        print '  ramping up ({0} V, {1} A)'.format(meas_v, meas_i)
    ## switch off
    print '  switching channel off'
    val = 0
    res = test_device.command_inout('ChannelSetSwitch', [module_slot_index, channel_index, val])
    ## wait until channel has ramped down
    time.sleep(wait_delay)
    channel_status = 4096
    while wiener_mpod_help_functions.checkBitIsSet(channel_status, 12):
        time.sleep(wait_delay)
        channel_status = test_device.command_inout('ChannelGetStatus', [module_slot_index, channel_index])
        meas_v = test_device.command_inout('ChannelGetMeasurementSenseVoltage', [module_slot_index, channel_index])
        meas_i = test_device.command_inout('ChannelGetMeasurementCurrent', [module_slot_index, channel_index])
        print '  ramping down ({0} V, {1} A)'.format(meas_v, meas_i)

occupied_slots = test_device.command_inout('CrateGetOccupiedSlots')
for module_slot_index in occupied_slots:
    if test_device.command_inout('ModuleGetIsHvModule', module_slot_index):
        nb_channels_per_module = test_device.command_inout('ModuleGetNumberOfChannels', module_slot_index)

        ## the ramp speed is set as a percentage of the nominal voltage        
        val = 1.0
        res = test_device.command_inout('ModuleSetRampSpeedCurrent', [module_slot_index, val])
        for channel_index in range(nb_channels_per_module):
            hv_channel_test(module_slot_index, channel_index)


