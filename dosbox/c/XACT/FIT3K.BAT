@echo Off
rem
rem 
rem SCCS     : @(#)FIT3K.BAT	3.1 Copyright 1994 Xilinx 5/4/94 23:47:44
rem Program  : fit3k
rem Abstract : Convert an XNF design to PLUSASM format.
rem            Then, integrate using FITEQN. FITEQN default
rem            parttype will be used.
rem Date     : 03/29/94
rem Revision : created by Joey Marquez for XEPLD 5.0
rem 
rem
rem
rem 
if %1. == . goto usage

echo x2p %1
x2p %1
if errorlevel 0 goto fit
echo %0 terminating
goto exit

:fit
echo fiteqn %1 %2 %3 %4 %5 %6 %7 %8 %9
fiteqn %1 %2 %3 %4 %5 %6 %7 %8 %9
goto exit

:usage
echo "Abstract: Convert an XNF design to PLUSASM format, then"
echo "          integrate using FITEQN."
echo "Usage   : fit3k <design>[.xnf] [-p parttype] [-u] [-f|-i]"
echo "Example : fit3k test"

:exit
rem * ---------------------------------------------------------------------*
rem * end of file                                                          *
rem * ---------------------------------------------------------------------*
