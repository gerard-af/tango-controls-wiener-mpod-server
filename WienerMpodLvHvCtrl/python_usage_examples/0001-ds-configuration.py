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

## register the tango device "mpod_test/mpod/1"
dev_info = tango.DbDevInfo()
dev_info.server = 'WienerMpodLvHvCtrl/mpod_test'
dev_info._class = 'WienerMpodLvHvCtrl'
dev_info.name = 'mpod_test/mpod/1'
db.add_device(dev_info)

## this is the new device
test_device = tango.DeviceProxy("mpod_test/mpod/1")

## define th IP address of the MPOD crate
dat = tango.DbDatum()
dat.name = 'IpAddr'
dat.value_string.append('192.168.0.33:10161')
test_device.put_property(dat)

## optional : define the used SNMP community (the default is "guru")
dat = tango.DbDatum()
dat.name = 'SnmpCommunityName'
dat.value_string.append('guru')
test_device.put_property(dat)

## optional : define the ThreadPollingIntervalMs (the default is 1000)
##      Reasonable values are 500 ... 10000.
##      For ThreadPollingIntervalMs = 0 the DS will not communicate from a thread
##      This is useful for debug purposes.
dat = tango.DbDatum()
dat.name = 'ThreadPollingIntervalMs'
dat.value_string.append('1000')
test_device.put_property(dat)

## init the device server with the new properties
test_device.command_inout('Init')

## test if the WIENER-CRATE-MIB file was found
##      this file is required if SNMP commands neet to be translated human readable names 
##      OIDs can always be used
##      The higher level commands all use OIDs.
test_device.get_attribute_list()
if test_device.read_attribute('IsMibFileLoaded').value:
    print 'The WIENER-CRATE-MIB file was loades successfully.'
else:
    print 'WARNING: The WIENER-CRATE-MIB file wasn\'t found.'


## now test the properties
db.get_device_property_list('mpod_test/mpod/1', '*')
db.get_device_property('mpod_test/mpod/1', 'IpAddr')
db.get_device_property('mpod_test/mpod/1', 'SnmpCommunityName')
db.get_device_property('mpod_test/mpod/1', 'ThreadPollingIntervalMs')



