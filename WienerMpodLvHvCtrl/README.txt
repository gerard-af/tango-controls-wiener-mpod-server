To build this device server on Linux:

Install net-snmp for your Linux distribution.

Change to the directory of this README.txt file.

> export TANGO_DIR=<home of your tango distribution>

The TANGO_DIR must contain among others "include/tango/tango.h" 
and "Libraries/cppserver/common/tango.opt".

On debian this will be "/usr/share/pogo/references".

> make

This should generate the executable device server "bin/WienerMpodLvHvCtrl".
It will be generated in the "bin" directory or in $OUTPUT_DIR if this 
environment variable is defined for instance in tango.opt.

If "mpod_test" is the device defined in your TANGO data base, export the 
TANGO_HOST variable, then start the device server :

> export TANGO_HOST=<IP:port> 
> ./bin/WienerMpodLvHvCtrl mpod_test

You might have to define the device server "mpod_test" first
(see python_usage_examples/0001-ds-configuration.py for some hints how
this can be done from Python). The device property "IpAddr"
must hold the IP of the MPOD crate.

Once the device server runs, open a new window go to the 
python_usage_examples directory.

Run the Python examples. They require the installation of "python-tango".

> python 0002-ds-list-commands.py
...
> python 0008-ds-high-level-group.py


