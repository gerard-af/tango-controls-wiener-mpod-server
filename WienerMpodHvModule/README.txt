To build this device server on Linux:

Install net-snmp for your Linux distribution.

Change to the directory of this README.txt file.

> export TANGO_DIR=<home of your tango distribution>

The TANGO_DIR must contain among others "include/tango/tango.h" 
and "Libraries/cppserver/common/tango.opt".

On debian this will be "/usr/share/pogo/references".

> make

This should generate the executable device server "bin/WienerMpodHvModule".
It will be generated in the "bin" directory or in $OUTPUT_DIR if this 
environment variable is defined for instance in tango.opt.

This device server requires a connection to a unning WienerMpodLvHvCtrl device
server. It's name must be configured in the properties.

(Remark) if the connected MPOD crate is switched off, no results are produced.

If "mpod_hv_module_test" is the device defined in your TANGO data base, export the 
TANGO_HOST variable, then start the device server :

> export TANGO_HOST=<IP:port> 
> bin/WienerMpodHvModule mpod_hv_module_test

You might have to define the device server "mpod_hv_module_test" first
(see python_usage_examples/0001-ds-configuration.py for some hints how
this can be done from Python). The device property "WienerMpodLvHvCtrlDevice"
must hold the IP of the WienerMpodLvHvCtrl device. 

The property "ModuleNumberOfChannels" defines how channel attributes will be generated.

Once the device server runs, all of it's attributes can be explore with Jive.


