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
group_index = 0

# if the DS runs a thread (initiualized with ThreadPollingIntervalMs > 1) we need to wait for the ...set...() commands to be applied
#       this should not take longer than 2 * ThreadPollingIntervalMs
wait_delay = 2
polling_interval = test_device.get_property('ThreadPollingIntervalMs')['ThreadPollingIntervalMs']
if len(polling_interval):
    wait_delay = float(polling_interval[0]) * 0.001
if wait_delay < 0.5:
    wait_delay = 0.5

# command: GroupGetIndex
#     input: Group index
#     output: Uninitialised
res = test_device.command_inout('GroupGetIndex', group_index)
print 'GroupGetIndex({0}) returns \'{1}\''.format(group_index, res)

# command: GroupSetSwitch
#     input: Group index, Value
#     output: Uninitialised
val = 1
res = test_device.command_inout('GroupSetSwitch', [group_index, val])
print 'GroupSetSwitch({0}, {1}) returns \'{2}\''.format(group_index, val, res)

## it takes some time until the the new value is taken into account (write and read to device)
print ' ... waiting before comparing read and written value ...'
time.sleep(wait_delay)

