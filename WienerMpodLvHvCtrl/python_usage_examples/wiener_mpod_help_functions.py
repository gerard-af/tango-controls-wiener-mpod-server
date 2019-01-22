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

#
#
#

def checkBitIsSet(status_flags, bit):
    return status_flags & (1 << bit)

def printCrateStatusFlags(crate_status):
    if checkBitIsSet(crate_status, 0):
        print 'crate status flag ON is set'
    if checkBitIsSet(crate_status, 1):
        print 'crate status flag INHIBIT is set'
    if checkBitIsSet(crate_status, 2):
        print 'crate status flag LOCAL is set'
    if checkBitIsSet(crate_status, 3):
        print 'crate status flag INPUT ERROR is set'
    if checkBitIsSet(crate_status, 4):
        print 'crate status flag CHANNEL ERROR is set'
    if checkBitIsSet(crate_status, 5):
        print 'crate status flag FAN FAILURE is set'
    if checkBitIsSet(crate_status, 8):
        print 'crate status flag INCOMPAT is set'
    if checkBitIsSet(crate_status, 9):
        print 'crate status flag RESET is set'
    if checkBitIsSet(crate_status, 10):
        print 'crate status flag DERATING is set'
    if checkBitIsSet(crate_status, 11):
        print 'crate status flag SUPPLY FAILURE is set'
    if checkBitIsSet(crate_status, 12):
        print 'crate status flag DERATING 2 is set'
    if checkBitIsSet(crate_status, 13):
        print 'crate status flag SUPPLY FAILURE 2 is set'

def printModuleStatusFlags(module_status):
    if checkBitIsSet(module_status, 0):
        print 'module status flag ADJUST is set'
    if checkBitIsSet(module_status, 2):
        print 'module status flag LIVE INSERTION is set'
    if checkBitIsSet(module_status, 3):
        print 'module status flag HV is set'
    if checkBitIsSet(module_status, 4):
        print 'module status flag SERVICE is set'
    if checkBitIsSet(module_status, 5):
        print 'module status flag V LIMIT is set'
    if checkBitIsSet(module_status, 6):
        print 'module status flag IMPUT ERROR is set'
    if not checkBitIsSet(module_status, 8):
        print 'module status flag BAR(SUM ERROR) is set'
    if not checkBitIsSet(module_status, 9):
        print 'module status flag BAR(RAMP) is set'
    if not checkBitIsSet(module_status, 10):
        print 'module status flag BAR(SAFETY) is set'
    if checkBitIsSet(module_status, 11):
        print 'module status flag EVENT is set'
    if not checkBitIsSet(module_status, 12):
        print 'module status flag BAR(MODULE ERROR) is set'
    if not checkBitIsSet(module_status, 13):
        print 'module status flag BAR(SUPPLY FAILURE) is set'
    if not checkBitIsSet(module_status, 14):
        print 'module status flag BAR(MAX TEMP) is set'
    if checkBitIsSet(module_status, 15):
        print 'module status flag KILL is set'

def printModuleEventStatusFlags(module_event_status):
    if checkBitIsSet(module_event_status, 0):
        print 'module event status flag POWER is set'
    if checkBitIsSet(module_event_status, 2):
        print 'module event status flag LIVE INSERTION is set'
    if checkBitIsSet(module_event_status, 4):
        print 'module event status flag SERVICE is set'
    if checkBitIsSet(module_event_status, 5):
        print 'module event status flag V LIMIT is set'
    if checkBitIsSet(module_event_status, 6):
        print 'module event status flag IMPUT ERROR is set'
    if checkBitIsSet(module_event_status, 10):
        print 'module event status flag SAFETY is set'
    if checkBitIsSet(module_event_status, 13):
        print 'module event status flag SUPPLY FAILURE is set'
    if checkBitIsSet(module_event_status, 14):
        print 'module event status flag MAX TEMP is set'

def printChannelStatusFlagsLV(channel_status):
    if checkBitIsSet(channel_status, 0):
        print 'LV channel status flag ON is set'
    if checkBitIsSet(channel_status, 1):
        print 'LV channel status flag INHIBIT is set'
    if checkBitIsSet(channel_status, 2):
        print 'LV channel status flag MIN SENSE V is set'
    if checkBitIsSet(channel_status, 3):
        print 'LV channel status flag MAX SENSE V is set'
    if checkBitIsSet(channel_status, 4):
        print 'LV channel status flag MAX TERM V is set'
    if checkBitIsSet(channel_status, 5):
        print 'LV channel status flag MAX I is set'
    if checkBitIsSet(channel_status, 6):
        print 'LV channel status flag MAX TEMP is set'
    if checkBitIsSet(channel_status, 7):
        print 'LV channel status flag MAX PLOAD is set'
    if checkBitIsSet(channel_status, 9):
        print 'LV channel status flag TIMEOUT is set'
    if checkBitIsSet(channel_status, 10):
        print 'LV channel status flag CONSTANT CURRENT is set'
    if checkBitIsSet(channel_status, 11):
        print 'LV channel status flag RAMP UP is set'
    if checkBitIsSet(channel_status, 12):
        print 'LV channel status flag RAMP DOWN is set'

def printChannelStatusFlagsHV(channel_status):
    if checkBitIsSet(channel_status, 0):
        print 'HV channel status flag ON is set'
    if checkBitIsSet(channel_status, 1):
        print 'HV channel status flag INHIBIT is set'
    if checkBitIsSet(channel_status, 4):
        print 'HV channel status flag V LIMIT is set'
    if checkBitIsSet(channel_status, 5):
        print 'HV channel status flag TRIP is set'
    if checkBitIsSet(channel_status, 10):
        print 'HV channel status flag CONSTANT CURRENT is set'
    if checkBitIsSet(channel_status, 11):
        print 'HV channel status flag RAMP UP is set'
    if checkBitIsSet(channel_status, 12):
        print 'HV channel status flag RAMP DOWN is set'
    if checkBitIsSet(channel_status, 13):
        print 'HV channel status flag KILL is set'
    if checkBitIsSet(channel_status, 14):
        print 'HV channel status flag EMERGENCY is set'
    if checkBitIsSet(channel_status, 15):
        print 'HV channel status flag ADJUST is set'
    if checkBitIsSet(channel_status, 16):
        print 'HV channel status flag CONSTANT VOLTAGE is set'
    if checkBitIsSet(channel_status, 17):
        print 'HV channel status flag LOW CURRENT RANGE is set'
    if checkBitIsSet(channel_status, 18):
        print 'HV channel status flag V BOUND is set'
    if checkBitIsSet(channel_status, 19):
        print 'HV channel status flag I LIMIT is set'





