  $ $MBPOLL 
  /home/rasmus-larsson/Workspaces/ClujInnovationCentre/mbpoll/build/mbpoll: device or host parameter missing ! Try -h for help.
  [1]

  $ $MBPOLL -h
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
    -S            Stream input or output, in conjunction with -c it will read and create a
                  stream output (Use -O to write regular output somewhere else) with the
                  data from the device, otherwise it will read stdin as a stream input to
                  write to the device
    -O #          Writes standard output to a file
    -1            Poll only once only, otherwise every poll rate interval
    -l #          Poll rate in ms, ( > 100 could be unstable, 1000 is default)
    -o #          Time-out in seconds (0.01 - 10.00, 1.00 s is default)
    -q            Quiet mode. Once: Minimum output only, Twice: Complete silence
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
