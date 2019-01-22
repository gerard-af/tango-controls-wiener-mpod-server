# tango-controls-wiener-mpod-server
[Tango controls](http://www.tango-controls.org/) device servers for controlling a [Wiener](http://www.wiener-d.com/) MPOD crate and inserted high- or low-voltage modules.

# WienerMpodLvHvCtrl
This is the main device server class. It makes use of the [net-snmp](http://www.net-snmp.org/) library to communicate with the MPOD over a network connection. 

The SNMP crate variables are available as attributes.
The module and channel SNMP variables can be accessed over functions.

See the README.txt file in the WienerMpodLvHvCtrl directory for information about compilation.
The included html documentation gives further details about the device server configuration and the exposed functions and attributes.

The DS configuration is also illustrated in the [wiki](https://github.com/gerard-af/tango-controls-wiener-mpod-server/wiki).

# WienerMpodHvModule
This device class simplifies working with a single Iseg high-voltage modules by exposing the module's and the channels' SNMP 
variables as attributes. The channel attributes are generated dynamically for a given number of channels.

It requires a running WienerMpodLvHvCtrl device server.

See the README.txt file in the WienerMpodHvModule directory for information about compilation.
The included html documentation gives further details about the device server configuration and the exposed attributes.

The DS configuration is also illustrated in the [wiki](https://github.com/gerard-af/tango-controls-wiener-mpod-server/wiki).

# Other Software
Other software for controlling MPOD crates is available. Using this software will familiarize you with your MPOD crate and help you to optimize your Tango-controls setup:
* Easy LV|HV is available from the [Analog Flavor website](http://www.analogflavor.com/en/bespice/easy-lvhv-control-software/).
* iseg SNMPcontrol can be downloaded from the [iseg support page](https://iseg-hv.com/en/support/downloads).

# Support
* contact [Physical Instruments](https://www.physical-instruments.fr/) for support in France.
* contact [Analog Flavor](http://www.analogflavor.com/) for international support.

# Further Information
* the WIENER-CRATE-MIB.txt file from the [Wiener website](http://www.wiener-d.com/) explains all used SNMP variables in details.
* the MPOD manual from the [Wiener website](http://www.wiener-d.com/) explains how to use an MPOD.
* the user guide of your [Iseg](http://iseg-hv.com) module explains how to use your high-voltage module.
