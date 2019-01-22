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


occupied_slots = test_device.command_inout('CrateGetOccupiedSlots')
print 'CrateGetOccupiedSlots() returns \'{0}\''.format(occupied_slots)

for slot_index in occupied_slots:
    nb_channels_per_module = test_device.command_inout('ModuleGetNumberOfChannels', slot_index)
    print 'ModuleGetNumberOfChannels({0}) returns \'{1}\''.format(slot_index, nb_channels_per_module)
    print 'ModuleGetIsHvModule({0}) returns \'{1}\''.format(slot_index, test_device.command_inout('ModuleGetIsHvModule', slot_index))


