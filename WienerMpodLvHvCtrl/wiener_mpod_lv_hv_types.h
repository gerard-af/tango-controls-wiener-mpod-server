/*

    Copyright (c) 2018 - 2019 Analog Flavor EURL

    Contact us for support:
        http://www.analogflavor.com

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of Analog Flavor EURL nor 
      the names of its contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------

*/

#ifndef wiener_mpod_lv_hv_types_h
#define wiener_mpod_lv_hv_types_h

typedef enum {
    mpod_snmp_connection_status_undefined,
    mpod_snmp_connection_status_error,
    mpod_snmp_connection_status_timeout,
    mpod_snmp_connection_status_disconnected,
    mpod_snmp_connection_status_connected,
    mpod_snmp_connection_status_on,
    mpod_snmp_connection_status_off,
    mpod_snmp_connection_status_on_structure_changed,
    mpod_snmp_connection_status_off_structure_changed
} mpod_snmp_connection_status_type;


/*    wiener_mpod_lv_hv_types_h */
#endif   


