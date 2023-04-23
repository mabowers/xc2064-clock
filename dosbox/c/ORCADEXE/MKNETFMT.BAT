@ECHO OFF
GOTO BEGIN

MKNETFMT.BAT
============

Summary:
  Compiles and links a flat or hierarchical netlist format file.

Features:
* Checks for valid command-line parameters.
* Checks whether the specified compiler (Microsoft C v7.x or 
  Borland C v3.X) is available.
* Checks whether the necessary environment variables are set.
* Displays a usage statement on improper invocation and on request.
* Creates, uses, and deletes the appropriate linker response file.

Notes:
* For the Microsoft compiler, the environment variable BINB should point 
  to the directory that contains the Microsoft C v7.x linker 
  (typically \C700\BINB).
* The netlist format source file (fmtName.C) should be in the directory 
  \ORCADESP\SDT\NETFORMS\NETSRCF for flat formats or 
  \ORCADESP\SDT\NETFORMS\NETSRCH for hierarchical formats 
  (as specified in the type parameter) or in the directory specified in 
  the optional srcPath parameter.
* The resulting object file (fmtName.OBJ) is placed in the directory 
  \ORCADESP\SDT\NETFORMS\NETSRCF for flat formats or 
  \ORCADESP\SDT\NETFORMS\NETSRCH for hierarchical formats 
  (as specified in the type parameter) or in the directory specified in 
  the optional srcPath parameter.
* The common object files should be in \ORCADESP\SDT\NETFORMS\OBJ or in 
  the directory specified in the optional objPath parameter.
* The resulting compiled netlist format and map files (fmtName.EXE and
  fmtName.MAP) are placed in the directory \ORCADESP\SDT\NETFORMS or in
  the directory specified in the optional cmpPath parameter.
* If you want to specify one of the optional path parameters, you must 
  specify all preceding parameters, required and optional (see the
  heading "DOS-related enhancements needed," below).
* The file MKNETFMT.DOC contains complete information about using MKNETFMT.

Usage:
  MKNETFMT compiler type fmtName [ srcPath [ objPath [ cmpPath [ libPath ]]]]
  MKNETFMT [ ? ]

Assumptions:
* The netlist format source file is called "fmtName.C" 
  where fmtName is the string specified in the fmtName parameter
* No "\" is specified at the ends of the optional path parameters 
  or at the end of the BINB environment variable

DOS-related enhancements needed:
* Test for presence of backslash (\) at the ends of the optional 
  parameters, rather than assume it is not included
* Test for existence of a directory in DOS
* Allow user to "skip" arguments on a DOS command line 
  (indicating their absence with placeholders, such as commas)

:BEGIN
REM  Check for usage request
IF '%1%' == '?' GOTO MOREDOC
IF '%1%' == ''  GOTO USAGE

REM  Capture parameters
SET COMPILER=%1%
SET TYPE=%2%
SET FMTNAME=%3%
SET SRCPATH=%4%
SET OBJPATH=%5%
SET CMPPATH=%6%
SET LIBPATH=%7%

:CHKCMP
REM  Check compiler (M or B only)
IF '%COMPILER%' == 'm' SET COMPILER=M
IF '%COMPILER%' == 'b' SET COMPILER=B
FOR %%v IN ( M B ) DO IF '%COMPILER%' == '%%v' GOTO CHKTYP
REM  (Bad compiler parameter)
ECHO MKNETFMT: *** Invalid first parameter (compiler).
ECHO MKNETFMT:     Specify M for Microsoft or B for Borland.
ECHO.
PAUSE
GOTO USAGE

:CHKTYP
REM  Check type (F or H only)
IF '%TYPE%' == 'f' SET TYPE=F
IF '%TYPE%' == 'h' SET TYPE=H
FOR %%v in ( F H ) DO IF '%TYPE%' == '%%v' GOTO CHKFMT
REM  (Bad type parameter)
ECHO MKNETFMT: *** Missing or invalid second parameter (type).
ECHO MKNETFMT:     Specify F for flat or H for hierarchical.
ECHO.
PAUSE
GOTO USAGE

:CHKFMT
Rem  Check format name (required)
IF NOT '%FMTNAME%' == '' GOTO BRANCH
REM  (Missing format parameter)
ECHO MKNETFMT: *** Missing third parameter (fmtName).
ECHO.
PAUSE
GOTO USAGE

:BRANCH
REM  Branch for compiler
IF '%COMPILER%' == 'M' GOTO CHKMSC
IF '%COMPILER%' == 'B' GOTO CHKSPATH

:CHKMSC
REM  Check for availability of Microsoft C v7.x compiler
IF '%LIB%'     == '' GOTO MSCERR
IF '%INCLUDE%' == '' GOTO MSCERR
IF '%INIT%'    == '' GOTO MSCERR

:CHKMSENV
REM  Check for access to Microsoft C v7.x linker
IF '%BINB%' == '' GOTO MSENVERR

:CHKSPATH
REM  Check for srcPath parameter
IF '%SRCPATH%' == '' SET SRCPATH=\ORCADESP\SDT\NETFORMS\NETSRC%TYPE%

:CHKSRC
REM  Check for source code
ECHO MKNETFMT: Checking for netlist format source file ...
IF NOT EXIST %SRCPATH%\%FMTNAME%.C GOTO SRCERR
ECHO MKNETFMT: Source file found.

:CHKOPATH
REM  Check for objPath parameter
IF '%COMPILER%' == 'M' SET OBJDIR=OBJ
IF '%COMPILER%' == 'B' SET OBJDIR=BRLNDOBJ
IF '%OBJPATH%' == '' SET OBJPATH=\ORCADESP\SDT\NETFORMS\%OBJDIR%

:CHKOBJ
REM  Check for required object files
IF '%TYPE%' == 'F' SET TSR=TSR
IF '%TYPE%' == 'H' SET TSR=HTSR
ECHO MKNETFMT: Checking for required object files ...
FOR %%v IN ( %TSR% CONTEXT RESIDENT STUBS ) DO IF NOT EXIST %OBJPATH%\%%v.OBJ GOTO OBJERR
ECHO MKNETFMT: Object files found.

:CHKCPATH
REM  Check for cmpPath parameter
IF '%CMPPATH%' == '' SET CMPPATH=\ORCADESP\SDT\NETFORMS

:CHKLPATH
REM  Check for libPath parameter (for Borland)
IF '%COMPILER%' == 'M' GOTO OLDOUT
IF '%LIBPATH%' == '' SET LIBPATH=\BORLANDC\LIB
REM  Check for small-memory-model object file
IF NOT EXIST %LIBPATH%\C0S.OBJ GOTO B-OBJERR

:OLDOUT
REM  Delete any previous version of the output file
IF EXIST MKNETFMT.OUT DEL MKNETFMT.OUT

:COMPILE
REM  Compile the netlist format source code
ECHO MKNETFMT: Compiling %SRCPATH%\%FMTNAME%.C ...
ECHO MKNETFMT: Compiler messages: >>MKNETFMT.OUT
ECHO. >>MKNETFMT.OUT
IF EXIST %FMTNAME%.OBJ DEL %FMTNAME%.OBJ
IF '%COMPILER%' == 'B' GOTO B-CMP
:M-CMP
cl /c /Zp1 /W3 /DLINT_ARGS %SRCPATH%\%FMTNAME%.C >>MKNETFMT.OUT
IF ERRORLEVEL 1 GOTO COMPERR
GOTO ENDCMP
:B-CMP
bcc -c %SRCPATH%\%FMTNAME%.C >>MKNETFMT.OUT
IF ERRORLEVEL 1 GOTO COMPERR
:ENDCMP
IF NOT EXIST %FMTNAME%.OBJ GOTO COMPERR2
ECHO MKNETFMT: Compilation complete.

REM  Allow for srcPath as the current directory
REN %FMTNAME%.OBJ TMP.OBJ >>TMP.OUT
COPY TMP.OBJ %SRCPATH%\%FMTNAME%.OBJ >>TMP.OUT

REM  Create the appropriate linker response file
ECHO MKNETFMT: Creating linker response file ...
IF EXIST TMP.LNK DEL TMP.LNK

REM  Write the name of the Borland small-memory model object file (Borland only)
IF '%COMPILER%' == 'M' GOTO REQOBJ
ECHO %LIBPATH%\C0S.OBJ + >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR

:REQOBJ
REM  Write the names of the four required OrCAD object files
ECHO %OBJPATH%\%TSR%.OBJ + >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR
ECHO %OBJPATH%\CONTEXT.OBJ + >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR
ECHO %OBJPATH%\RESIDENT.OBJ + >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR
ECHO %OBJPATH%\STUBS.OBJ + >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR

REM  Write the name of the netlist-format object file created above
ECHO %SRCPATH%\%FMTNAME%.OBJ >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR

REM  Write the names for the executable and map files
ECHO %FMTNAME% >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR
ECHO %FMTNAME% >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR

REM  Write an additional linker response line (Borland only)
IF '%COMPILER%' == 'M' GOTO ENDLRF
ECHO CS >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR

:ENDLRF
REM  End the linker response file
ECHO ;. >>TMP.LNK
IF ERRORLEVEL 1 GOTO FILEERR

REM  Link the object files
ECHO MKNETFMT: Linking object files ...
ECHO. >>MKNETFMT.OUT
ECHO ========================================= >>MKNETFMT.OUT
ECHO. >>MKNETFMT.OUT
ECHO MKNETFMT: Linker messages: >>MKNETFMT.OUT
IF EXIST %FMTNAME%.EXE DEL %FMTNAME%.EXE
IF EXIST %FMTNAME%.MAP DEL %FMTNAME%.MAP
IF '%COMPILER%' == 'B' GOTO B-LNK
:M-LNK
%BINB%\link /st:4096 /MA /CODEVIEW @TMP.LNK >>MKNETFMT.OUT
IF ERRORLEVEL 1 GOTO LINKERR
GOTO ENDLNK
:B-LNK
tlink -L%LIBPATH% @TMP.LNK >>MKNETFMT.OUT
IF ERRORLEVEL 1 GOTO LINKERR
:ENDLNK
FOR %%v IN ( MAP EXE ) DO IF NOT EXIST %FMTNAME%.%%v GOTO LINKERR2
ECHO MKNETFMT: Linking complete.

REM  Allow for cmpPath as the current directory
FOR %%v IN ( MAP EXE ) DO REN %FMTNAME%.%%v TMP.* >>TMP.OUT
FOR %%v IN ( MAP EXE ) DO COPY TMP.%%v %CMPPATH%\%TYPE%%FMTNAME%.* >>TMP.OUT

:DONE
ECHO MKNETFMT: Finished.
ECHO MKNETFMT: Check the file MKNETFMT.OUT for compiler and linker messages.
GOTO CLEANUP

:MSCERR
ECHO OFF
ECHO MKNETFMT: *** One of the Microsoft C v7.x environment variables is not set:
ECHO MKNETFMT:     LIB
ECHO MKNETFMT:     INCLUDE
ECHO MKNETFMT:     INIT
ECHO MKNETFMT:
ECHO MKNETFMT:     Commands to set these environment variables are added to your 
ECHO MKNETFMT:     AUTOEXEC.BAT file when you install Microsoft C v7.x. Their 
ECHO MKNETFMT:     absence may indicate that the compiler is unavailable to 
ECHO MKNETFMT:     MKNETFMT.  See your Microsoft C v7.x documentation for 
ECHO MKNETFMT:     information about setting these environment variables.
PAUSE
GOTO CLEANUP

:MSENVERR
ECHO OFF
ECHO MKNETFMT: *** The BINB environment variable is not set. BINB must point
ECHO MKNETFMT:     to the directory that contains the Microsoft C v7.x linker
ECHO MKNETFMT:     (typically \C700\BINB).  Add the following line to your
ECHO MKNETFMT:     AUTOEXEC.BAT file or enter it at the DOS prompt before you
ECHO MKNETFMT:     run MKNETFMT:
ECHO MKNETFMT:
ECHO MKNETFMT:        SET BINB=pathToMSCLinker
ECHO MKNETFMT:
ECHO MKNETFMT:     As with the other environment variables created and used by
ECHO MKNETFMT:     the Microsoft C v7.x product, do not include a backslash (\)
ECHO MKNETFMT:     at the end of the pathToMSCLinker.
PAUSE
GOTO CLEANUP

:SRCERR
ECHO OFF
ECHO MKNETFMT: *** The specified source file was not found:
ECHO MKNETFMT:     %SRCPATH%\%FMTNAME%.C
PAUSE
GOTO USAGE

:OBJERR
ECHO OFF
ECHO MKNETFMT: *** One of the required object files was not found:
ECHO MKNETFMT:     %OBJPATH%\%TSR%.OBJ
ECHO MKNETFMT:     %OBJPATH%\CONTEXT.OBJ
ECHO MKNETFMT:     %OBJPATH%\RESIDENT.OBJ
ECHO MKNETFMT:     %OBJPATH%\STUBS.OBJ
PAUSE
GOTO USAGE

:B-OBJERR
ECHO OFF
ECHO MKNETFMT: *** Borland's small-memory-model object file was not found:
ECHO MKNETFMT:     %LIBPATH%\C0S.OBJ
PAUSE
GOTO USAGE

:MOREDOC
ECHO OFF
REM  Display the file MKNETFMT.DOC in %ORCADESP% or in the current directory
REM  The ORCADESP environment variable includes a trailing backslash
IF '%ORCADESP%' == '' GOTO MOREDOC2
IF NOT EXIST %ORCADESP%SDT\MKNETFMT.DOC GOTO MOREDOC2
TYPE %ORCADESP%SDT\MKNETFMT.DOC | MORE
GOTO CLEANUP

:MOREDOC2
ECHO OFF
IF NOT EXIST .\MKNETFMT.DOC GOTO USAGE
TYPE .\MKNETFMT.DOC | MORE
GOTO CLEANUP

:USAGE
ECHO OFF
CLS 
ECHO Usage:
ECHO   MKNETFMT compiler type fmtName [ srcPath [ objPath [ cmpPath [ libPath ]]]]
ECHO   MKNETFMT [ ? ]
ECHO.
ECHO Parameters:
ECHO   compiler   M specifies the Microsoft C v7.x compiler and 
ECHO              B specifies the Borland C v3.X compiler 
ECHO              (specify exactly one, M or B)
ECHO   type       F specifies a flat format and 
ECHO              H specifies a hierarchical format 
ECHO              (specify exactly one, F or H)
ECHO   fmtName    Name of the netlist format source code file, excluding the 
ECHO              dot and extension (.C)
ECHO   srcPath    Relative or absolute path to the directory containing the 
ECHO              netlist format source file, fmtName.C
ECHO              (optional; defaults to \ORCADESP\SDT\NETFORMS\NETSRCF 
ECHO              for flat formats, and to \ORCADESP\SDT\NETFORMS\NETSRCH 
ECHO              for hierarchical formats [as specified in the type parameter])
ECHO   objPath    Relative or absolute path to the directory containing the 
ECHO              required object files 
ECHO              (optional; defaults to \ORCADESP\SDT\NETFORMS\OBJ)
PAUSE
CLS 
ECHO Usage:
ECHO   MKNETFMT compiler type fmtName [ srcPath [ objPath [ cmpPath [ libPath ]]]]
ECHO   MKNETFMT [ ? ]
ECHO.
ECHO Parameters (continued):
ECHO   cmpPath    Relative or absolute path to the directory in which the 
ECHO              compiled netlist format and map files are to be stored 
ECHO              (optional; defaults to \ORCADESP\SDT\NETFORMS)
ECHO   libPath    Relative or absolute path to the directory containing the Borland
ECHO              library files (checked if the compiler parameter is set to B) 
ECHO              (optional; defaults to \BORLANDC\LIB)
ECHO   ?          Display additional information about MKNETFMT's operation.
ECHO.
ECHO Notes:
ECHO * Do not include a backslash (\) at the end of any directories you specify in
ECHO   the optional path parameters or at the end of the BINB environment variable.
ECHO * If you use version 6.X of the Microsoft C compiler, MKNETFMT may appear to
ECHO   stop after the message "Linking object files ..." displays.  If so, just
ECHO   press Enter to continue and finish the process. 
ECHO * Check the file MKNETFMT.OUT for messages from the compiler and linker.
ECHO * Enter "MKNETFMT ?" for more information about MKNETFMT.
ECHO.
GOTO CLEANUP

:COMPERR2
ECHO OFF
ECHO MKNETFMT: *** The expected object file was not found:
ECHO MKNETFMT:     .\%FMTNAME%.OBJ
GOTO COMPERR

:COMPERR
ECHO OFF
SET PROBLEM=compilation
GOTO SHOWMSG

:FILEERR
ECHO OFF
SET PROBLEM=file-creation
GOTO SHOWMSG

:LINKERR2
ECHO OFF
ECHO MKNETFMT: *** One of the expected output files was not found:
ECHO MKNETFMT:     .\%FMTNAME%.MAP
ECHO MKNETFMT:     .\%FMTNAME%.EXE
GOTO LINKERR

:LINKERR
ECHO OFF
SET PROBLEM=link
GOTO SHOWMSG

:SHOWMSG
ECHO OFF
ECHO MKNETFMT: *** A problem occurred in the %PROBLEM% phase (see MKNETFMT.OUT).
GOTO CLEANUP

:CLEANUP
REM  Clear environment variables and delete temporary files
ECHO OFF
SET COMPILER=
SET TYPE=
SET FMTNAME=
SET SRCPATH=
SET OBJPATH=
SET OBJDIR=
SET CMPPATH=
SET LIBPATH=
SET TSR=
SET PROBLEM=
FOR %%v IN ( LNK OUT OBJ MAP EXE ) DO IF EXIST TMP.%%v DEL TMP.%%v

:EXIT
