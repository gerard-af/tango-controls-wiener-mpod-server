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
# configure tango for testing the WienerMpodLvHvCtrl device server
##

import tango

db = tango.Database()

## register the tango device "mpod_hv_module_test/hv/1"
dev_info = tango.DbDevInfo()
dev_info.server = 'WienerMpodHvModule/mpod_hv_module_test'
dev_info._class = 'WienerMpodHvModule'
dev_info.name = 'mpod_hv_module_test/hv/1'
db.add_device(dev_info)

## this is the new device
test_device = tango.DeviceProxy('mpod_hv_module_test/hv/1')

## define the nqme of the corresponding WienerMpodLvHvCtrl device
dat = tango.DbDatum()
dat.name = 'WienerMpodLvHvCtrlDevice'
dat.value_string.append('mpod_test/mpod/1')
test_device.put_property(dat)

## define the nqme of the corresponding WienerMpodLvHvCtrl deviceslot index in the MPOD crate
dat = tango.DbDatum()
dat.name = 'CrateSlotIndex'
dat.value_string.append('6')
test_device.put_property(dat)

## define the nqme of the corresponding WienerMpodLvHvCtrl deviceslot index in the MPOD crate
dat = tango.DbDatum()
dat.name = 'ModuleNumberOfChannels'
dat.value_string.append('32')
test_device.put_property(dat)

## init the device server with the new properties
test_device.command_inout('Init')


test_device.get_attribute_list()

## now test the properties
db.get_device_property_list('mpod_hv_module_test/hv/1', '*')
db.get_device_property('mpod_hv_module_test/hv/1', 'WienerMpodLvHvCtrlDevice')
db.get_device_property('mpod_hv_module_test/hv/1', 'CrateSlotIndex')
db.get_device_property('mpod_hv_module_test/hv/1', 'ModuleNumberOfChannels')




