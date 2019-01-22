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
import wiener_mpod_help_functions

test_device = tango.DeviceProxy("mpod_test/mpod/1")

crate_status = test_device.command_inout('CrateGetSysStatus')
print 'CrateGetSysStatus() returns \'{0}\''.format(hex(crate_status))
wiener_mpod_help_functions.printCrateStatusFlags(crate_status)

occupied_slots = test_device.command_inout('CrateGetOccupiedSlots')
for module_slot_index in occupied_slots:
    if test_device.command_inout('ModuleGetIsHvModule', module_slot_index):
        ## warning : the module status makes sense only for HV modules
        module_status = test_device.command_inout('ModuleGetStatus', module_slot_index)
        print 'ModuleGetStatus({0}) returns \'{1}\''.format(module_slot_index, hex(module_status))
        wiener_mpod_help_functions.printModuleStatusFlags(module_status)

        ## warning : the module event status makes sense only for HV modules
        module_event_status = test_device.command_inout('ModuleGetEventStatus', module_slot_index)
        print 'ModuleGetEventStatus({0}) returns \'{1}\''.format(module_slot_index, hex(module_event_status))
        wiener_mpod_help_functions.printModuleEventStatusFlags(module_event_status)

        nb_channels_per_module = test_device.command_inout('ModuleGetNumberOfChannels', module_slot_index)
        for channel_index in range(nb_channels_per_module):
            channel_status = test_device.command_inout('ChannelGetStatus', [module_slot_index, channel_index])
            print 'ChannelGetStatus({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, hex(channel_status))
            wiener_mpod_help_functions.printChannelStatusFlagsHV(channel_status)

    else:
        print 'Low voltage modules don\'t use status flags at module level !'

        nb_channels_per_module = test_device.command_inout('ModuleGetNumberOfChannels', module_slot_index)
        for channel_index in range(nb_channels_per_module):
            channel_status = test_device.command_inout('ChannelGetStatus', [module_slot_index, channel_index])
            print 'ChannelGetStatus({0}, {1}) returns \'{2}\''.format(module_slot_index, channel_index, hex(channel_status))
            wiener_mpod_help_functions.printChannelStatusFlagsLV(channel_status)


