Echo Off
Set OldPath=%PATH%
Set PATH=C:\ORCADEXE
If not %PATH%. == C:\ORCADEXE. Goto ENVERR
Set ORCADEXE=C:\ORCADEXE\
If not %ORCADEXE%. == C:\ORCADEXE\. Goto ENVERR
Set ORCADESP=C:\ORCADESP\
If not %ORCADESP%. == C:\ORCADESP\. Goto ENVERR
Set ORCADUSER=C:\ORCADESP\
If not %ORCADUSER%. == C:\ORCADESP\. Goto ENVERR
Set ORCADPROJ=C:\ORCAD\
If not %ORCADPROJ%. == C:\ORCAD\. Goto ENVERR
Set NoCfgFile=FALSE
If not %NoCfgFile%. == FALSE. Goto ENVERR
Set NoDatFile=FALSE
If not %NoDatFile%. == FALSE. Goto ENVERR
Set NoMrgExe=FALSE
If not %NoMrgExe%. == FALSE. Goto ENVERR
Set NoMrgFile=FALSE
If not %NoMrgFile%. == FALSE. Goto ENVERR
Set NoProj=FALSE
If not %NoProj%. == FALSE. Goto ENVERR
Set NoOldTmplt=FALSE
If not %NoOldTmplt%. == FALSE. Goto ENVERR
Set NoNewTmplt=FALSE
If not %NoNewTmplt%. == FALSE. Goto ENVERR
If not exist C:\ORCAD\TEMPLATE\NUL set NoNewTmplt=TRUE
If not exist C:\ORCAD\TEMPLATE\NUL set NoOldTmplt=TRUE
C:
Cd \ORCAD\TEMPLATE
Cd \ORCAD\TEMPLATE
SDT_C /B
Cd \ORCAD\TUTOR
SDT_C /B
Cd \ORCAD\TEMPLATE

:CHKMRGEXE
Cd C:\ORCADEXE
If not exist C:MERGEDAT.EXE set NoMrgExe=TRUE

:CHKCFG
If %NoNewTmplt%. == TRUE. goto CHKDAT
Cd C:\ORCAD\TEMPLATE
If exist ESP.CFG goto CHKDAT
If %NoOldTmplt%. == TRUE. set NoCfgFile=TRUE
If %NoCfgFile%. == TRUE. goto CHKDAT
Cd C:\ORCAD\TEMPLATE
If not exist C:ESP.CFG set NoCfgFile=TRUE
Cd C:\ORCAD\TEMPLATE
If %NoCfgFile%. == TRUE. goto CHKDAT
Copy C:\ORCAD\TEMPLATE\ESP.CFG .\*.*

:CHKDAT
If exist ORCADESP.DAT goto CHKMRG
If not exist C:\ORCADESP\NUL goto CHKMRG
Cd C:\ORCADESP
If not exist C:ORCADESP.NEW set NoDatFile=TRUE
If %NoNewTmplt%. == TRUE. goto CHKMRG
If %NoDatFile%. == TRUE. goto CHKMRG
Cd C:\ORCAD\TEMPLATE
Copy C:\ORCADESP\ORCADESP.NEW .\ORCADESP.DAT
Cd C:\ORCAD\TUTOR
Copy C:\ORCADESP\ORCADESP.NEW .\ORCADESP.DAT
Cd C:\ORCAD\TEMPLATE

:CHKMRG
If not exist C:\ORCADESP\SDT386.MRG set NoMrgFile=TRUE

If %NoDatFile%. == TRUE. goto CHKPROJ
If %NoMrgExe%. == TRUE. goto CHKPROJ
If %NoMrgFile%. == TRUE. goto CHKPROJ
If %NoTemplate%. == TRUE. goto CHKPROJ
Cls
Echo.
Echo.
Echo Updating ORCADESP.DAT files in design directories...
Echo.
Set ORCADPROJ=C:\ORCAD\
If not %ORCADPROJ%. == C:\ORCAD\. Goto ENVERR
MERGEDAT SDT386.MRG /P
FIXDAT /U /A

:CHKPROJ
If %NoProj%. == FALSE. goto CHKTMPLT
Cls
Echo.
Echo.
Echo.
Echo ERROR: It appears that the project directory shown below does not exist.
Echo The conversions of schematics and libraries in design directories
Echo was therefore aborted.
Echo.
Echo C:\ORCAD
Echo.
Pause
:CHKTMPLT
If %NoNewTmplt%. == FALSE. goto CHK4ERR
Cls
Echo.
Echo.
Echo ERROR: The TEMPLATE directory was not found in
Echo.
Echo C:\ORCAD
Echo.
Echo This directory is necessary for the ESP Design Environment to
Echo operate. If you are planning to use ESP, you should consider
Echo reinstalling ESP and specifying the path shown above for the project
Echo directory when prompted for it.
Echo.
Echo.
Pause

:CHK4ERR
If %NoCfgFile%. == TRUE. goto FILEERR
If %NoDatFile%. == TRUE. goto FILEERR
If %NoMrgExe%. == TRUE. goto FILEERR
If %NoMrgFile%. == TRUE. goto FILEERR
Goto END
:FILEERR

Echo ERROR: Unable to find the following file(s).
Echo.
If %NoMrgExe%. == TRUE. echo C:\ORCADESP\MERGEDAT.EXE
If %NoMrgFile%. == TRUE. echo C:\ORCADESP\SDT386.MRG
If %NoDatFile%. == TRUE. echo C:\ORCAD\TEMPLATE\ORCADESP.DAT
If %NoCfgFile%. == TRUE. echo C:\ORCAD\TEMPLATE\ESP.CFG
Echo.
Echo The files listed above are critical for this update process to
Echo complete.
Echo.
Pause
:MRGEXE
If not %NoMrgExe%. == TRUE. goto MRG
Echo.
Echo MERGEDAT.EXE is used to update the ORCADESP.DAT file to reflect the
Echo presence of the SDT 386+ product. This results in different buttons
Echo and local configurations on the SDT 386+ screen.
Echo.
Pause
:MRG
If not %NoMrgFile%. == TRUE. goto CFG
Echo.
Echo The SDT386.MRG file is used to update the screen buttons, SDT 386+
Echo executable references, and SDT 386+ local configurations for ESP.
Echo.
Pause
:CFG
If not %NoCfgFile%. == TRUE. goto DAT
Echo.
Echo The ESP.CFG file contains the configuration information for the ESP
Echo Design Environment and should be present only in the TEMPLATE
Echo directory.
Echo.
Pause
:DAT
If not %NoDatFile%. == TRUE. goto CONVERT
Echo.
Echo The ORCADESP.DAT file may have been missed if
Echo.
Echo C:\ORCAD\TEMPLATE
Echo.
Echo is not where you normally store your project directories. You will
Echo need to manually update the ORCADESP.DAT file in each of your
Echo design directories.
Echo.
Echo For instructions on manually updating the ORCADESP.DAT file,
Echo please see OrCAD Technical Note #45. If you do not have a
Echo copy, our Technical Support BBS or our Customer Service
Echo department can provide you with one.
Echo.
Pause
Goto END

:ENVERR
Cls
Echo.
Echo.
Echo ERROR: It appears that there is not enough environment space to
Echo temporarily set environment variables required to perform
Echo the automatic conversion of the files you have indicated.
Echo You may need to increase your environment size by adding
Echo or modifying the SHELL statement in your CONFIG.SYS file
Echo as described in your DOS manual.
Echo.
Echo If you do not wish to modify your CONFIG.SYS file, you may
Echo also run this batch file from the DOS prompt using the
Echo following command line:
Echo.
Echo command /e:4096 /c sdtconv.bat
Echo.
Echo You will need to be in the directory shown below to run
Echo the conversion/update batch file:
Echo.
Echo C:\ORCAD\TEMPLATE
Echo.
Echo If environment space is still a problem, increase the
Echo value following /e: on the command shown above and try again.
Echo.
Echo.
Pause
Goto END

:END
Cd C:\ORCAD\TEMPLATE
Set NoCfgFile=
Set NoDatFile=
Set NoMrgExe=
Set NoMrgFile=
Set NoProj=
Set NoNewTmplt=
Set NoOldTmplt=
Set PATH=%ORCADEXE%;%OldPath%
Set OldPath=
Set ORCADEXE=C:\ORCADEXE\
Set ORCADESP=C:\ORCADESP\
Set ORCADUSER=C:\ORCADESP\
Set ORCADPROJ=C:\ORCAD\
