# mbpoll

> Copyright © 2015 Pascal JEAN aka epsilonRT  
> pascal.jean--AT--btssn.net


## Présentation
mbpoll est un utilitaire en ligne de commande permettant de dialoguer
avec des esclaves ModBus RTU ou TCP.  
Il utilise [libmodbus](http://libmodbus.org/).  
Il s'agit d'un projet multiplateforme, la compilation a été testée sur 
GNU linux x86 et x86_64, Microsoft Windows 7 x86 et GNU linux ARM 6 (Raspbian).  
Bien que la syntaxe de ses options soit très proche du programme modpoll 
de proconX, c'est un projet totalement indépendant.

mbpoll permet de:

- lire des entrées discrètes
- lire et écrire des sorties binaires (*coil*)
- lire des registres d'entrée (*input register*)
- lire et écrire des registres de sortie (*holding register*)

La lecture et l'écriture de registres peut se faire au format décimal,
hexadécimal ou flottant simple précision.

## Installation
Le instructions d'installation se trouvent dans le fichier INSTALL.

##Exemples d'utilisation

La commande ci-dessous permet de lire les registres d'entrée 1 et 2 de
l'esclave à l'adresse 33 connecté en RTU sur /dev/ttyUSB2 (38400 Bd)

---

    mbpoll -a 33 -b 38400 -t 3 -r 1 -c 2 /dev/ttyUSB2
    
    mbpoll 0.1-10 - FieldTalk(tm) Modbus(R) Master Simulator
    Copyright (c) 2015 Pascal JEAN aka epsilonRT
    All rights reserved.
    This software is governed by the CeCILL license <http://www.cecill.info>

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

## Aide
Une aide complète est disponible avec l'option -h:

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
      -c #          Number of values to read (1-125, 1 is default)
      -u            Read the description of the type, the current status, and other
                    information specific to a remote device (RTU only)
      -t 0          Discrete output (coil) data type (binary 0 or 1)
      -t 1          Discrete input data type (binary 0 or 1)
      -t 3          16-bit input register data type
      -t 3:hex      16-bit input register data type with hex display
      -t 3:int      32-bit integer data type in input register table
      -t 3:float    32-bit float data type in input register table
      -t 4          16-bit output (holding) register data type (default)
      -t 4:hex      16-bit output (holding) register data type with hex display
      -t 4:int      32-bit integer data type in output (holding) register table
      -t 4:float    32-bit float data type in output (holding) register table
      -0            First reference is 0 (PDU addressing) instead 1
      -1            Poll only once only, otherwise every poll rate interval
      -l #          Poll rate in ms, ( > 100, 1000 is default)
      -o #          Time-out in seconds (0.01 - 10.00, 1.00 s is default)
    Options for ModBus / TCP : 
      -p #          TCP port number (502 is default)
    Options for ModBus RTU : 
      -b #          Baudrate (1200-921600, 19200 is default)
      -d #          Databits (7 or 8, 8 for RTU)
      -s #          Stopbits (1 or 2, 1 is default)
      -P #          Parity (none, even, odd, even is default)
      -4            RS-485 mode

      -h            Print this help summary page
      -V            Print version and exit
      -v            Verbose mode.  Causes mbpoll to print debugging messages about
                    its progress.  This is helpful in debugging connection...

---
> Ce logiciel est régi par la licence CeCILL soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL telle que diffusée par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

> En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

> A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  à l'utilisation,  à la modification et/ou au
développement et à la reproduction du logiciel par l'utilisateur étant 
donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
manipuler et qui le réserve donc à des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités à charger  et  tester  l'adéquation  du
logiciel à leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 

> Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
pris connaissance de la licence CeCILL, et que vous en avez accepté les
termes.
