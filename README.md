# mbpoll

Copyright © 2015-2023 Pascal JEAN, All rights reserved.


## Abstract

mbpoll is a command line utility to communicate with ModBus slave (RTU or TCP).  
This is a multiplatform project, the compilation was tested on GNU Linux
x86, x86_64, armhf and arm64 (Armbian/Raspbian), Microsoft Windows, and Mac OSX.  

Development of major version 1 of mbpoll is complete, **version 2 using libmodbuspp is under development.** Proposals for new features will be transferred to this new branch.

mbpoll can:

- read discrete inputs
- read and write binary outputs (*coil*)
- read input registers
- read and write output registers (*holding register*)

The reading and writing registers may be in decimal, hexadecimal or 
floating single precision.

> **Note:** mbpoll's output syntax and command line option syntax is similar to the original modpoll command line program published by proconX. However mbpoll is a completely independent project and based on different source code than the original modpoll program. mbpoll is distributed under the GPL license, but the original modpoll program is not covered by the GPL license.


## Quickstart guide

The fastest and safest way to install mbpoll is to use the APT 
repository from [piduino.org](http://apt.piduino.org), so you should do the following :

    wget -O- http://www.piduino.org/piduino-key.asc | sudo apt-key add -
    sudo add-apt-repository 'deb http://apt.piduino.org stretch piduino'
    sudo apt update
    sudo apt install mbpoll

This repository provides `mbpoll` and `libmodbus` (version 3.1.4) packages for 
`i386`, `amd64`, `armhf` and `arm64` architectures.
In the above commands, the repository is a Debian Stretch distribution, but you 
can also choose Ubuntu Trusty, Xenial or Bionic by replacing `stretch` with 
`trusty`, `xenial` or `bionic`.  
It may be necessary to install the `software-properties-common` 
package for `add-apt-repository`.

For Raspbian you have to do a little different :

    wget -O- http://www.piduino.org/piduino-key.asc | sudo apt-key add -
    echo 'deb http://raspbian.piduino.org stretch piduino' | sudo tee /etc/apt/sources.list.d/piduino.list
    sudo apt update
    sudo apt install mbpoll

The Raspbian repository provides Piduino packages for `armhf` architecture for Stretch only.

## Build from source

For example, for a debian system:

* Install [libmodbus](https://github.com/stephane/libmodbus.git) (Version >= 3.1.4) :

        $ sudo apt-get install build-essential libtool git-core autoconf automake
        $ git clone https://github.com/stephane/libmodbus.git
        $ cd libmodbus
        $ ./autogen.sh
        $ ./configure
        $ make
        $ sudo make install

You can also install it with `apt` if the version of libmodbus is greater than or equal to 3.1.4.
For example to query a debian system:

    $ apt-cache show libmodbus-dev

* Install [piduino](https://github.com/epsilonrt/piduino/tree/dev) **only if you want to manage the RS485 with a GPIO signal**:

        $ sudo apt-get install cmake libcppdb-dev pkg-config libsqlite3-dev sqlite3 libudev-dev
        $ git clone https://github.com/epsilonrt/piduino.git
        $ cd piduino 
        $ git checkout dev
        $ mkdir build
        $ cd build
        $ cmake ..
        $ make
        $ sudo make install
    
* Generate Makefile with cmake:

        $ sudo apt-get install cmake pkg-config
        $ cd mbpoll
        $ mkdir build
        $ cd build
        $ cmake ..

* Compile and install mbpoll:

        $ make
        $ sudo make install
        $ sudo ldconfig

If you prefer, you can in the place of direct compilation create a package and install it:

        $ make package
        $ sudo dpkg -i * .deb

In some cases, when installing pkg-config for the first time, it may be necessary to set the $PKG_CONFIG_PATH environment variable before running CMAKE, so pkg_check_module it will be able to find the libmodbus at /usr/local/lib/ directory. This can be done by the following command: export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH. Make sure to adjust the path /usr/local/lib/pkgconfig if your pkgconfig is located in a different path.

That's all !

For Windows, you can follow the instructions in the [README-WINDOWS.md](README-WINDOWS.md) file.

## Examples

The following command is used to read the input registers 1 and 2 of the
slave at address 33 connected through RTU /dev/ttyUSB2 (38400 Bd)

---

        $ mbpoll -a 33 -b 38400 -t 3 -r 1 -c 2 /dev/ttyUSB2
        
        mbpoll 1.5 -  Modbus® Master Simulator
        Copyright (c) 2015-2023 Pascal JEAN, https://github.com/epsilonrt/mbpoll
        This program comes with ABSOLUTELY NO WARRANTY.
        This is free software, and you are welcome to redistribute it
        under certain conditions; type 'mbpoll -w' for details.

        Protocol configuration: Modbus RTU
        Slave configuration...: address = [33]
                                start reference = 1, count = 2
        Communication.........: /dev/ttyUSB2, 38400-8E1 
                                t/o 1.00 s, poll rate 1000 ms
        Data type.............: 16-bit register, input register table

        -- Polling slave 33... Ctrl-C to stop)
        [1]: 	9997
        [2]: 	10034
        -- Polling slave 33... Ctrl-C to stop)
        [1]: 	10007
        [2]: 	10034
        -- Polling slave 33... Ctrl-C to stop)
        [1]: 	10007
        [2]: 	10034
        -- Polling slave 33... Ctrl-C to stop)
        [1]: 	10007
        [2]: 	10034
        ^C--- /dev/ttyUSB2 poll statistics ---
        4 frames transmitted, 4 received, 0 errors, 0.0% frame loss

        everything was closed.
        Have a nice day !

## Help

A complete help is available with the -h option:

    usage : mbpoll [ options ] device|host [ writevalues... ] [ options ]

    ModBus Master Simulator. It allows to read and write in ModBus slave registers
                             connected by serial (RTU only) or TCP.

    Arguments :
      device        Serial port when using ModBus RTU protocol
                      COM1, COM2 ...              on Windows
                      /dev/ttyS0, /dev/ttyS1 ...  on Linux
                      /dev/ser1, /dev/ser2 ...    on QNX
      host          Host name or dotted IP address when using ModBus/TCP protocol
      writevalues   List of values to be written.
                    If none specified (default) mbpoll reads data.
                    If negative numbers are provided, it will precede the list of
                    data to be written by two dashes ('--'). for example :
                    mbpoll -t4:int /dev/ttyUSB0 -- 123 -1568 8974 -12
    General options : 
      -m #          mode (rtu or tcp, TCP is default)
      -a #          Slave address (1-255 for rtu, 0-255 for tcp, 1 is default)
                    for reading, it is possible to give an address list
                    separated by commas or colons, for example :
                    -a 32,33,34,36:40 read [32,33,34,36,37,38,39,40]
      -r #          Start reference (1 is default)
                    for reading, it is possible to give an address list
                    separated by commas or colons
      -c #          Number of values to read (1-125, 1 is default)
      -u            Read the description of the type, the current status, and other
                    information specific to a remote device (RTU only)
      -t 0          Discrete output (coil) data type (binary 0 or 1)
      -t 1          Discrete input data type (binary 0 or 1)
      -t 3          16-bit input register data type
      -t 3:int16    16-bit input register data type with signed int display
      -t 3:hex      16-bit input register data type with hex display
      -t 3:string   16-bit input register data type with string (char) display
      -t 3:int      32-bit integer data type in input register table
      -t 3:float    32-bit float data type in input register table
      -t 4          16-bit output (holding) register data type (default)
      -t 4:int16    16-bit output (holding) register data type with signed int display
      -t 4:hex      16-bit output (holding) register data type with hex display
      -t 4:string   16-bit output (holding) register data type with string (char) display
      -t 4:int      32-bit integer data type in output (holding) register table
      -t 4:float    32-bit float data type in output (holding) register table
      -0            First reference is 0 (PDU addressing) instead 1
      -B            Big endian word order for 32-bit integer and float
      -1            Poll only once only, otherwise every poll rate interval
      -l #          Poll rate in ms, ( > 100, 1000 is default)
      -o #          Time-out in seconds (0.01 - 10.00, 1.00 s is default)
      -q            Quiet mode.  Minimum output only
    Options for ModBus / TCP : 
      -p #          TCP port number (502 is default)
    Options for ModBus RTU : 
      -b #          Baudrate (1200-921600, 19200 is default)
      -d #          Databits (7 or 8, 8 for RTU)
      -s #          Stopbits (1 or 2, 1 is default)
      -P #          Parity (none, even, odd, even is default)
      -R [#]        RS-485 mode (/RTS on (0) after sending)
                     Optional parameter for the GPIO RTS pin number
      -F [#]        RS-485 mode (/RTS on (0) when sending)
                     Optional parameter for the GPIO RTS pin number

      -h            Print this help summary page
      -V            Print version and exit
      -v            Verbose mode.  Causes mbpoll to print debugging messages about
                    its progress.  This is helpful in debugging connection...

---
> Copyright © 2015-2023 Pascal JEAN, All rights reserved.

> mbpoll is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

> mbpoll is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

> You should have received a copy of the GNU General Public License
along with mbpoll. If not, see <http://www.gnu.org/licenses/>.
