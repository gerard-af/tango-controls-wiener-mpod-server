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
# listing all commands of the WienerMpodLvHvCtrl device server
##

import tango

test_device = tango.DeviceProxy("mpod_test/mpod/1")

## show all available commands
all_commands = test_device.command_list_query()

print ">>> commands related to the underlying SNMP protocol:"
for single_command in all_commands:
    command_name = single_command.cmd_name
    if command_name.startswith('Snmp'):
        print "command: {0}".format(single_command.cmd_name)
        print "    input: {0}".format(single_command.in_type_desc)
        print "    output: {0}".format(single_command.out_type_desc)
print ">>> commands related to the crate commands:"
for single_command in all_commands:
    command_name = single_command.cmd_name
    if command_name.startswith('Crate'):
        print "command: {0}".format(single_command.cmd_name)
        print "    input: {0}".format(single_command.in_type_desc)
        print "    output: {0}".format(single_command.out_type_desc)
print ">>> commands related to the module commands:"
for single_command in all_commands:
    command_name = single_command.cmd_name
    if command_name.startswith('Module'):
        print "command: {0}".format(single_command.cmd_name)
        print "    input: {0}".format(single_command.in_type_desc)
        print "    output: {0}".format(single_command.out_type_desc)
print ">>> commands related to the channel commands:"
for single_command in all_commands:
    command_name = single_command.cmd_name
    if command_name.startswith('Channel'):
        print "command: {0}".format(single_command.cmd_name)
        print "    input: {0}".format(single_command.in_type_desc)
        print "    output: {0}".format(single_command.out_type_desc)
print ">>> commands related to the group commands:"
for single_command in all_commands:
    command_name = single_command.cmd_name
    if command_name.startswith('Group'):
        print "command: {0}".format(single_command.cmd_name)
        print "    input: {0}".format(single_command.in_type_desc)
        print "    output: {0}".format(single_command.out_type_desc)
print ">>> other commands:"
for single_command in all_commands:
    command_name = single_command.cmd_name
    if ((not command_name.startswith('Snmp')) 
        and (not command_name.startswith('Crate')) 
        and (not command_name.startswith('Module')) 
        and (not command_name.startswith('Channel'))
        and (not command_name.startswith('Group'))):
        print "command: {0}".format(single_command.cmd_name)
        print "    input: {0}".format(single_command.in_type_desc)
        print "    output: {0}".format(single_command.out_type_desc)



