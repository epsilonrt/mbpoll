# mbpoll

Copyright © 2015-2026 Pascal JEAN, All rights reserved.

[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/epsilonrt/mbpoll?include_prereleases)](https://github.com/epsilonrt/mbpoll/releases)
[![Build Linux](https://github.com/epsilonrt/mbpoll/actions/workflows/ci_build_linux.yml/badge.svg)](https://github.com/epsilonrt/mbpoll/actions/workflows/ci_build_linux.yml)
[![Build Windows](https://github.com/epsilonrt/mbpoll/actions/workflows/ci_build_windows.yml/badge.svg)](https://github.com/epsilonrt/mbpoll/actions/workflows/ci_build_windows.yml)
[![Build macOS](https://github.com/epsilonrt/mbpoll/actions/workflows/ci_build_macos.yml/badge.svg)](https://github.com/epsilonrt/mbpoll/actions/workflows/ci_build_macos.yml)

---

## Abstract

<a href="https://modbus.org/">
  <img src="doc/modbus.png" alt="Modbus Logo" align="right" valign="top">
</a>

mbpoll is a command line utility to communicate with ModBus slave (RTU or TCP).  
This is a multiplatform project, the compilation was tested on GNU Linux
x86, x86_64, armhf and arm64 (Armbian/Raspbian), Microsoft Windows, and macOS.  

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

### Linux (Debian/Ubuntu)

The recommended way to install mbpoll on Linux (Debian/Ubuntu) is via the APT repository from [piduino.org](http://apt.piduino.org) :

```sh
wget -O- http://www.piduino.org/piduino-key.asc | sudo gpg --dearmor --yes --output /usr/share/keyrings/piduino-archive-keyring.gpg
echo "deb [signed-by=/usr/share/keyrings/piduino-archive-keyring.gpg] http://apt.piduino.org $(lsb_release -c -s) piduino" | sudo tee /etc/apt/sources.list.d/piduino.list
sudo apt update
sudo apt install mbpoll
```

This repository provides `mbpoll` packages for `i386`, `amd64`, `armhf` and `arm64` architectures.

### Download pre-built binaries

You can also download pre-built installable binaries for Linux, macOS and Windows from the [Releases page](https://github.com/epsilonrt/mbpoll/releases).

For Linux, `.deb` packages are available for easy installation.
For macOS, `.pkg` and `.tar.gz` packages are provided.
For Windows, an installer (`mbpoll-setup.exe`) is available.

### Windows installer: SmartScreen warning

mbpoll is an open source project and the Windows installer is **not signed** with a commercial certificate. As a result, Windows and Microsoft Edge may display a warning when downloading or running the installer:

- Edge may block the download and require you to force it.
- Windows Defender SmartScreen may show a warning page and ask you to confirm before installing.

This is normal for open source projects. You can safely bypass these warnings by clicking "More info" then "Run anyway". For more details, see the [documentation](README-WINDOWS.md).

## Installation using Brew on macOS and Linux
Using [Homebrew](https://github.com/Homebrew/brew) to install mbpoll and its dependencies using:

`brew install mbpoll`

## Build from source

For example, for a debian system:

* Install [libmodbus](https://github.com/stephane/libmodbus.git) (Version >= 3.1.7) :

        $ sudo apt-get install build-essential libtool git-core autoconf automake
        $ git clone https://github.com/stephane/libmodbus.git
        $ cd libmodbus
        $ ./autogen.sh
        $ ./configure
        $ make
        $ sudo make install

You can also install it with `apt` if the version of libmodbus is greater than or equal to 3.1.7.
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
        $ sudo dpkg -i *.deb

In some cases, when installing pkg-config for the first time, it may be necessary to set the `PKG_CONFIG_PATH` environment variable before running cmake, so that `pkg_check_modules` can find libmodbus in `/usr/local/lib/`. This can be done with the following command: `export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH`. Make sure to adjust the path if your pkgconfig directory is located elsewhere.

That's all !

For Windows, you can follow the instructions in the [README-WINDOWS.md](README-WINDOWS.md) file.

## Examples

The following command is used to read the input registers 1 and 2 of the
slave at address 33 connected through RTU /dev/ttyUSB2 (38400 Bd)

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

A complete help is available with the -h option.  
On Unix systems, a man page is also available: `man mbpoll`

    usage : mbpoll [ options ] device|host [ writevalues... ] [ options ]

    ModBus(R) Master Simulator.
    Allows to read and write in ModBus slave registers connected by
    serial (RTU only) or TCP.

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
                    for reading, it is possible to give a reference list
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
      -W            Using function 10 for write a single register
      -B            Big endian word order for 32-bit integer and float
      -1            Poll only once only, otherwise every poll rate interval
      -l #          Poll rate in ms, ( > 100, 1000 is default)
      -o #          Time-out in seconds (0.01 - 10.00, 1.00 s is default)
      -q            Quiet mode.  Minimum output only
      -x            Print address (reference) in hexadecimal format
      -Q            Enable MAX_SLAVE quirk (accept slave id 0-255)
      -X            Enable REPLY_TO_BROADCAST quirk (send reply to broadcast)
    Options for ModBus / TCP :
      -p #          TCP port number (502 is default)
    Options for ModBus RTU :
      -b #          Baudrate (1200-921600, 19200 is default)
      -d #          Databits (7 or 8, 8 for RTU)
      -s #          Stopbits (1 or 2, 1 is default)
      -P #          Parity (none, even, odd, even is default)
      -R            RS-485 mode (/RTS on (0) after sending)
      -F            RS-485 mode (/RTS on (0) when sending)

      -h            Print this help summary page
      -V            Print version and exit
      -v            Verbose mode.  Causes mbpoll to print debugging messages about
                    its progress.  This is helpful in debugging connection...


## Contributing

Pull requests are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) before submitting.  
All contributions must target the `dev` branch — `master` is reserved for stable releases only.

## License

mbpoll is free software licensed under the [GNU General Public License v3.0](COPYING).
