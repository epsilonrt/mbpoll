# Build mbpoll for Windows

## Prerequisites

### Install Visual Studio Code and Build Tools for Visual Studio 2022

To successfully complete this tutorial, you must do the following :

1. Install [Visual Studio Code] (https://code.visualstudio.com/)  
2. Install the [C/C++ extension for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools). You can install the C/C++ extension by searching for 'c++' in the Extensions view (Ctrl+Shift+X). 
3. Install [Build Tools pour Visual Studio 2022](https://visualstudio.microsoft.com/fr/downloads/)  

Installation following the instructions on the page [Configure VS Code for Microsoft C++](https://code.visualstudio.com/docs/cpp/config-msvc)

### Install CMake

1. Download the latest version of [CMake](https://cmake.org/download/)  
2. Install the [CMake Tools extension for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools). . You can install the CMake Tools extension by searching for 'cmake' in the Extensions view (Ctrl+Shift+X). 

### Install pkg-config

This is a step-by-step procedure to get pkg-config working on Windows.

There were multiple versions of the packages available, and in each case I just downloaded the latest version.

1. go to http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/  
2. download the file [pkg-config_0.26-1_win32.zip](http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/pkg-config_0.26-1_win32.zip)  
3. extract the file to C:\Info  
4. download the file [gettext-runtime_0.18.1.1-2_win32.zip](http://ftp.gnome.org/pub/gnome/binaries/win32/dependencies/gettext-runtime_0.18.1.1-2_win32.zip)  
5. extract the file to C:\Info  
6. go to http://ftp.gnome.org/pub/gnome/binaries/win32/glib/2.28  
7. download the file [glib_2.28.8-1_win32.zip](http://ftp.acc.umu.se/pub/gnome/binaries/win32/glib/2.28/glib_2.28.8-1_win32.zip)  
8. extract the file  to C:\Info  
9. Adds the path C:\Info\bin to the PATH environment variable

### Install git for Windows

1. Download the latest version of [Git for Windows](https://gitforwindows.org/)  
2. Install Git for Windows

## Clone the repositories

Open a git bash terminal  :

    git clone https://github.com/epsilonrt/mbpoll.git
    cd mbpoll
    git clone https://github.com/stephane/libmodbus.git

## Build 

1. Open Visual Studio Code and open the folder `mbpoll`.  
2. Choose Visual Studio Community 2022 Realease - x86 as the kit in the Cmake status bar.
3. Choose the Release configuration in the Cmake status bar.
4. Click on the Configure button in the status bar to configure the project.
5. Click on the Build button in the status bar to build the project.

        [main] Building folder: mbpoll 
        [build] Starting build
        [proc] Executing command: C:\Info\CMake\bin\cmake.EXE --build c:/Users/pasca/OneDrive/Documents/src/mbpoll/build --config Release --target ALL_BUILD -j 18 --
        [build] MSBuild version 17.5.0+6f08c67f3 for .NET Framework
        [build] 
        [build]   Microsoft (R) Windows Script Host Version 5.812
        [build]   Copyright (C) Microsoft Corporation. Tous droits réservés.
        [build]   
        [build]   ..\modbus-version.h created.
        [build]   modbus.dll.manifest created.
        [build]   config.h created.
        [build]   
        [build]   Libmodbus configuration completed
        [build]   
        [build]   modbus-data.c
        [build] C:\Users\pasca\OneDrive\Documents\src\mbpoll\libmodbus\src\modbus-data.c(77,6): warning C4273: 'modbus_set_bits_from_byte' : liaison DLL incohérente [C:\Users\pasca\OneDrive\Documents\src\mbpoll\build\mbpoll.vcxproj]
        .......
        [build] LINK : warning LNK4217: le symbole '_modbus_set_bits_from_bytes' défini dans 'modbus-data.obj' est importé par 'modbus.obj' dans la fonction '_modbus_reply' [C:\Users\pasca\OneDrive\Documents\src\mbpoll\build\mbpoll.vcxproj]
        [build]   mbpoll.vcxproj -> C:\Users\pasca\OneDrive\Documents\src\mbpoll\build\Release\mbpoll.exe
        [driver] Build completed: 00:00:04.712
        [build] Build finished with exit code 0

The warnings LNK4217 and C4273 are normal.

## Make Innosetup installer

1. Download the latest version of [Innosetup](https://jrsoftware.org/isdl.php)  
2. Go to the folder `mbpoll\package\win` and open the file `mbpoll.iss` with Innosetup  
3. Download the Microsoft Visual C++ Redistributable latest from https://aka.ms/vs/17/release/vc_redist.x86.exe and copy it to `mbpoll\package\win\tmp\vcredist_x86.exe`  
4. Compile the installer with Innosetup  
5. Get the installer in the folder `mbpoll\package\win\installer`  

