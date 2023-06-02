Requires a Modbus server to be running with hostname/ip $MB_SERVER with available read and write to 1st index
aswell as installed xxd. MB_SERVER_ARGS is only to allow for different setups to use different args, like TCP port.
  $ $MBPOLL -S -c 16 -1 -t 3 $MB_SERVER_ARGS $MB_SERVER | xxd
  00000000: (\d*) (\d*) (\d*) (\d*) (\d*) (\d*) (\d*) (\d*)  .* (re)

  $ $MBPOLL -S -c 1 -r 10000 -1 $MB_SERVER_ARGS $MB_SERVER 2> errors.txt | xxd
  $ cat errors.txt
  Read output (holding) register failed: Illegal data address

  $ printf "!" | $MBPOLL -S -r 1 -1 $MB_SERVER_ARGS $MB_SERVER
  mbpoll [\d\.-]+ - FieldTalk\(tm\) Modbus\(R\) Master Simulator (re)
  Copyright (c) 2015-2023 Pascal JEAN, https://github.com/epsilonrt/mbpoll
  This program comes with ABSOLUTELY NO WARRANTY.
  This is free software, and you are welcome to redistribute it
  under certain conditions; type 'mbpoll -w' for details.
  
  Protocol configuration: Modbus TCP
  Slave configuration...: address = [1]
                          start reference = 1, count = 1
  Communication.........: [^,]+, port [\d]+, t/o 1.00 s, poll rate 1000 ms (re)
  Data type.............: 16-bit register, output (holding) register table
  
  Written 1 references.
   (re)
Ends with just an empty line..

  $ $MBPOLL -S -c 1 -1 $MB_SERVER $MB_SERVER_ARGS | xxd
  00000000: 21                                       !

  $ $MBPOLL -S -r 1000 -c 1 -1 $MB_SERVER $MB_SERVER_ARGS | xxd
  Read output (holding) register failed: Illegal data address

  $ printf "?" | $MBPOLL -S -r 1000 -1 -q -q $MB_SERVER_ARGS $MB_SERVER 2> errors.txt
  [1]
  $ cat errors.txt
  Write output (holding) register failed: Illegal data address

  $ $MBPOLL -S -c 1 -1 $MB_SERVER -p 22 | xxd
  [^:]+: Connection failed: .* (re)
Here I get "Connection failed: \x10." Dunno if it's some bug in the libmodbus?