; Copyright © 2015 epsilonRT, All rights reserved.

#define VERSION "0.1-19-ge00e908"
[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{B380C12E-9574-4C33-B779-CEBBEEAE18C8}
AppName=MBPoll
AppVerName=MBPoll {#VERSION}
AppVersion={#VERSION}
AppPublisher=Pascal Jean aka epsilonRT
AppPublisherURL=http://www.btssn.net/mbpoll
AppSupportURL=http://www.btssn.net/mbpoll
AppUpdatesURL=http://www.btssn.net/mbpoll
DefaultDirName={pf}\MBPoll
SetupIconFile=mbpoll.ico
DisableDirPage=yes
AlwaysShowDirOnReadyPage=yes
OutputDir=installer
OutputBaseFilename=mbpoll-setup-{#VERSION}
Compression=lzma
SolidCompression=yes
DefaultGroupName=MBPoll
ChangesAssociations=yes
ChangesEnvironment=true

[Languages]
Name: en; MessagesFile: compiler:Default.isl; LicenseFile: license.txt

[CustomMessages]
en.MainFiles=MBPoll
en.MainDescription=A command line user interface allows easy communication with ModBus RTU and TCP slave
en.Sources=Source Files
en.Redist=Microsoft Visual C++ 2013 Redistribuable (x86)

[Components]
Name: main; Description: {cm:MainFiles}; Types: full compact custom; Flags: fixed
Name: vc_redist; Description: {cm:Redist}; Types: custom
Name: source; Description: {cm:Sources}; Types: custom

[Tasks]
Name: modifypath; Description: Add application directory to your environmental path; Flags: checkedonce

[Files]
Source: app\*; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
Source: license*.txt; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
Source: mbpoll.ico; DestDir: {app}; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
Source: src\*; DestDir: {app}\src; Flags: ignoreversion recursesubdirs createallsubdirs; Components: source
Source: tmp\vcredist_x86.exe; DestDir: {tmp}; Flags: deleteafterinstall

[Run]
Filename: {tmp}\vcredist_x86.exe; Parameters: "/q /passive /Q:a /c:""msiexec /q /i vcredist.msi"" "; Components: vc_redist; StatusMsg: Installing VC++ 2013 Redistributables...

; ModPathName defines the name of the task defined above.  
; ModPathType defines whether the user or system path will be modified; 
;   this must be either system or user.  
; setArrayLength must specify the total number of dirs to be added;  
; Result[0] contains first directory, Result[1] contains second (optional), etc.
[Code]
const
    ModPathName = 'modifypath';
    ModPathType = 'system';
function ModPathDir(): TArrayOfString;
begin
    setArrayLength(Result, 1)
    Result[0] := ExpandConstant('{app}');
end;
#include "modpath.iss"
