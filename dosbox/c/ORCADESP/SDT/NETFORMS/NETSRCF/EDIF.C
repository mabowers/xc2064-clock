#include "format.h"

static char *switches = "ORSWITCHES \
/F Keep system generated netnames\n \
/A Allow nonEDIF characters\n \
/K Convert PartFields to Named Properties\n \
/L Do not append sheet number to labels\n \
/N Do not output additional properties\n \
/P Output pin names (instead of pin numbers)\n \
/E Do not create the 'external' library declaration\n";

static char *ver = "VERSION=\"1.20h  12-SEP-94\"";

/*
 *  Version 1.20h
 *      - Changed the way /A is handled.  /A was UPPER casing everything
 *        and I changed it to be case insensitive.
 *  Version 1.20g
 *      - Added code to make sure that NamedProperties get checked for
 *        valid EDIF names.
 *  Version 1.20f
 *      - Added a WriteCrLf(1) to WriteNetListEnd() when using /N.
 *  Version 1.20e
 *      - Added a WARNING message to EdifNameEA().
 *  Version 1.20c
 *      - Added /F switch.
 *  Version 1.20b
 *      - Added /K switch.  This switch causes the Part Fields to get
 *        renamed to "Named Properties". The part field on the schematic
 *        must follow the following convention for naming:
 *                NAME=STRING.    The NAME of the property would be NAME
 *        in this example, and STRING would be the string associated with
 *        the property.
 *  Version 1.20a
 *      - Added /A switch.  Without /A all fieldstrings, part value, and
 *        module names get renamed when illegal characters are found.
 *
 *  Version 1.10d
 *      - Changed default from pin names being output to pin numbers
 *
 *  Version 1.10c
 *      - Added properties for time stamp and field strings
 *      - Added /N switch
 *
 *  Version 1.10b
 *      - Moved comment in the cell definition after the cellType
 *      - Added Version message
 *      - Corrected int to unsigned comparison
 *      - Added /E switch
 *      - Altered such that '...Ref' constructs don't generate 'rename's
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are not checked for length
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - Legal characters are { '_', '0-9', 'a-z', 'A-Z' }
 *      - Remember that case is not significant in EDIF
 *
 *      - Names with illegal characters will be handled as follows:
 *          1  A WARNING will be issued
 *          2  '-' -> 'MINUS'   '+' -> 'PLUS'   '\', '/' -> 'BAR'  all others -> '_'
 *          3  The original name will be RENAMEd to the new
 *
 */

static int sw_L;
static int sw_N;
static int sw_P;
static int sw_E;
static int sw_A;
static int sw_K;

static int temp;
static int legal;
static int ch;
static int net;
static int Str, TempStr, TempStr2, JunkStr;
static int SIGNALS, ALL, NODES;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    SetCharSet("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    Str = AddSymbol("Str");
    TempStr = AddSymbol("TempStr");
    TempStr2 = AddSymbol("TempStr2");
    JunkStr = AddSymbol("JunkStr");

    SIGNALS = AddSymbol( "SIGNALS" );
    SetSymbol( SIGNALS, "SIGNALS" );
    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

    sw_L = SwitchIsSet("L");
    sw_N = SwitchIsSet("N");
    sw_P = SwitchIsSet("P");
    sw_E = SwitchIsSet("E");
    sw_A = SwitchIsSet("A");
    sw_K = SwitchIsSet("K");

    WriteString(1, "(edif ");
    CopySymbol(FileName, Str);
    FToUpper(Str);
    StripExt();
    EdifName1();
    WriteCrLf(1);

    WriteString(1, " (edifVersion 2 0 0)");
    WriteCrLf(1);
    WriteString(1, " (edifLevel 0)");
    WriteCrLf(1);
    WriteString(1, " (keywordMap (keywordLevel 0))");
    WriteCrLf(1);
    WriteString(1, " (status");
    WriteCrLf(1);
    WriteString(1, "  (written");
    WriteCrLf(1);
    WriteString(1, "   (timeStamp 0 0 0 0 0 0)");
    WriteCrLf(1);
    WriteString(1, "   (program \"IFORM.EXE\")");
    WriteCrLf(1);
    WriteString(1, "   (comment \"Original data from OrCAD/SDT schematic\"))");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, TitleString);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, DateString);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, DocumentNumber);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, Revision);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, Organization);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, AddressLine1);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, AddressLine2);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, AddressLine3);
    WriteString(1, "\")");
    WriteCrLf(1);
    WriteString(1, "  (comment \"");
    WriteSymbol(1, AddressLine4);
    WriteString(1, "\"))");
    WriteCrLf(1);

    net = 0;
    return 0;
}

/*-----------------------*/

StripExt()
{
    int pos;

    ch = 46;
    pos = FindSymbolChar(ch, Str);
    if (pos != -1) { ch = 0; PutSymbolChar(pos, ch, Str); }
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    CreatePartDataBase();
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;

    if (net != 0) { WriteCrLf(3); }
    net = 1;

    WriteString(3, "     (net ");

    CopySymbol(SignalNameString, Str);

    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal("_");
            SetNumberWidth(5);
            CopySymbol(LocalSignal, Str);
        }
        EdifName3();
    }
    if (i == 'P')
    {
        EdifName3();
    }
    if (i == 'S')
    {
        EdifName3();
    }
    if (i == 'N')
    {
        WriteString(3, "N");
        WriteSymbol(3, NetNumber);
    }
    if (i == 'U')
    {
        WriteString(3, "N");
        WriteSymbol(3, NetNumber);
    }
    WriteCrLf(3);
    WriteString(3, "      (joined");

    if (i == 'P')
    {
        /* tie this net to its port -- if it is a port */
        WriteCrLf(3);
        WriteString(3, "       (portRef ");
        EdifName3NR();
        WriteString(3, ")");
    }

    AddSignalName();
    return 0;
}

/*-----------------------*/

WriteNet()
{
    WriteCrLf(3);
    WriteString(3, "       (portRef ");
    if (sw_P == 1)
        { CopySymbol(PinNameString, Str); }
    else
        { CopySymbol(PinNumberString, Str); }
    EdifName3NR();

    WriteString(3, " (instanceRef ");
    CopySymbol(ReferenceString, Str);
    EdifName3NR();
    WriteString(3, "))");
    
    RecordNode();
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    WriteString(3, "))");
    return 0;
}

/*-----------------------*/

ProcessFieldStrings()
{
    return 0;
}

/*-----------------------*/

EdifName1()
{
    legal = SymbolInCharSet(Str);
    if (legal == 1)
    {
        WriteString(1, "&");
        WriteSymbol(1, Str);
    }
    else
    {
        CopySymbol(Str, TempStr);
        ExceptionsFor("EDIF", TempStr);

        WriteString(0, "WARNING: Name contains illegal characters '");
        WriteSymbol(0, Str);
        WriteString(0, "', changed to ");
        WriteSymbol(0, TempStr);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");

        WriteString(1, "(rename &");
        WriteSymbol(1, TempStr);
        WriteString(1, " \"");
        WriteSymbol(1, Str);
        WriteString(1, "\")");
    }
    return 0;
}

/*-----------------------*/

EdifName1NR()
{
    /* same as EdifName1 except does not generate 'rename' construct */

    legal = SymbolInCharSet(Str);
    if (legal == 1)
    {
        WriteString(1, "&");
        WriteSymbol(1, Str);
    }
    else
    {
        CopySymbol(Str, TempStr);
        ExceptionsFor("EDIF", TempStr);

        WriteString(1, "&");
        WriteSymbol(1, TempStr);
    }
    return 0;
}




/*-----------------------*/

EdifNameEA()
{
    /* same as EdifName1NR except does not generate '&' construct */

    legal = SymbolInCharSet(Str);
    if (legal == 1)
    {
        WriteString(1, "");
        WriteSymbol(1, Str);
    }
    else
    {
        CopySymbol(Str, TempStr);
        ExceptionsFor("EDIF", TempStr);

        WriteString(0, "WARNING: Name contains illegal characters '");
        WriteSymbol(0, Str);
        WriteString(0, "', changed to ");
        WriteSymbol(0, TempStr);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");

        WriteString(1, "");
        WriteSymbol(1, TempStr);
    }
    return 0;
}



/*-----------------------*/

EdifName3()
{
    legal = SymbolInCharSet(Str);
    if (legal == 1)
    {
        WriteString(3, "&");
        WriteSymbol(3, Str);
    }
    else
    {
        CopySymbol(Str, TempStr);
        ExceptionsFor("EDIF", TempStr);

        WriteString(0, "WARNING: Name contains illegal characters '");
        WriteSymbol(0, Str);
        WriteString(0, "', changed to ");
        WriteSymbol(0, TempStr);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");

        WriteString(3, "(rename &");
        WriteSymbol(3, TempStr);
        WriteString(3, " \"");
        WriteSymbol(3, Str);
        WriteString(3, "\")");
    }
    return 0;
}

/*-----------------------*/

EdifName3NR()
{
    /* same as EdifName3 except does not generate 'rename' construct */

    legal = SymbolInCharSet(Str);
    if (legal == 1)
    {
        WriteString(3, "&");
        WriteSymbol(3, Str);
    }
    else
    {
        CopySymbol(Str, TempStr);
        ExceptionsFor("EDIF", TempStr);

        WriteString(3, "&");
        WriteSymbol(3, TempStr);
    }
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    int i;
    int len, pos;
    int EqualSign;

    EqualSign = '=';

    WriteString(3, "))))");
    WriteCrLf(3);

    /* tell EDIF what cell makes up the design */
    WriteString(3, " (design ");
    CopySymbol(FileName, Str);
    FToUpper(Str);
    StripExt();
    EdifName3();
    WriteCrLf(3);
    WriteString(3, "  (cellRef ");
    EdifName3NR();
    WriteCrLf(3);
    WriteString(3, "   (libraryRef MAIN_LIB))))");
    WriteCrLf(3);

    if (sw_E == 0)
    {
        /* write the external library */
        WriteString(1, " (external OrCAD_LIB");
        WriteCrLf(1);
        WriteString(1, "  (edifLevel 0)");
        WriteCrLf(1);
        WriteString(1, "  (technology");
        WriteCrLf(1);
        WriteString(1, "   (numberDefinition");
        WriteCrLf(1);
        WriteString(1, "    (scale 1 1 (unit distance))))");

        /* the map for pin types */
        SetPinMap(0, "INPUT");
        SetPinMap(1, "INOUT");
        SetPinMap(2, "OUTPUT");
        SetPinMap(3, "OUTPUT");
        SetPinMap(4, "INOUT");
        SetPinMap(5, "OUTPUT");
        SetPinMap(6, "OUTPUT");
        SetPinMap(7, "INPUT");

        SetAccessType("LOOKUP");
        do {
            WriteCrLf(1);
            WriteString(1, "  (cell ");

            CopySymbol(LookupNameString, Str);
            EdifName1();
            WriteCrLf(1);

            WriteString(1, "   (cellType generic)");
            WriteCrLf(1);
            WriteString(1, "   (comment \"From OrCAD library ");
            WriteSymbol(1, LibraryNameString);
            WriteString(1, "\")");
            WriteCrLf(1);
            WriteString(1, "   (view NetlistView");
            WriteCrLf(1);
            WriteString(1, "    (viewType netlist)");
            WriteCrLf(1);
            WriteString(1, "    (interface");

            LoadFirstPin();
            do {
                WriteCrLf(1);
                WriteString(1, "     (port ");

                if (sw_P == 1)
                    { CopySymbol(PinNameString, Str); }
                else
                    { CopySymbol(PinNumberString, Str); }
                EdifName1();

                WriteString(1, " (direction ");
                i = GetStandardSymbol(PinType);
                WriteMap(1, i);
                WriteString(1, "))");
            } while (LoadPin());

            WriteString(1, ")))");
        } while (NextAccessType());
        WriteString(1, ")");
        WriteCrLf(1);
    }

    /* write the schematic library */
    WriteString(1, " (library MAIN_LIB");
    WriteCrLf(1);
    WriteString(1, "  (edifLevel 0)");
    WriteCrLf(1);
    WriteString(1, "  (technology");
    WriteCrLf(1);
    WriteString(1, "   (numberDefinition");
    WriteCrLf(1);
    WriteString(1, "    (scale 1 1 (unit distance))))");
    WriteCrLf(1);

    WriteString(1, "  (cell ");
    CopySymbol(FileName, Str);
    FToUpper(Str);
    StripExt();
    EdifName1();
    WriteCrLf(1);
    WriteString(1, "   (cellType generic)");
    WriteCrLf(1);
    WriteString(1, "   (view NetlistView");
    WriteCrLf(1);
    WriteString(1, "    (viewType netlist)");
    WriteCrLf(1);
    WriteString(1, "    (interface");

    /* the map for signal types */
    SetPinMap(0, "INOUT");
    SetPinMap(1, "OUTPUT");
    SetPinMap(2, "INPUT");
    SetPinMap(3, "INOUT");

    /* list all of the module ports as part of the interface */
    SetFirst(SIGNALS);
    do {
        i = GetStandardSymbol(TypeCode);
        if (i == 'P')
        {
            WriteCrLf(1);
            WriteString(1, "     (port ");

            CopySymbol(SignalNameString, Str);
            EdifName1();

            WriteString(1, " (direction ");
            i = GetStandardSymbol(SignalType);
            WriteMap(1, i);
            WriteString(1, "))");
        }
    } while (SetNext(SIGNALS));
    WriteString(1, ")");
    WriteCrLf(1);

    WriteString(1, "    (contents");
    WriteCrLf(1);

    /* list all of the instances in the design */
    RewindInstanceFile();
    while (LoadInstance())
    {
        WriteString(1, "     (instance ");
        CopySymbol(ReferenceString, Str);
        EdifName1();
        WriteCrLf(1);

        WriteString(1, "      (viewRef NetlistView");
        WriteCrLf(1);

        WriteString(1, "       (cellRef ");
        CopySymbol(LookupNameString, Str);
        EdifName1NR();
        WriteCrLf(1);

        WriteString(1, "        (libraryRef OrCAD_LIB)))");
        WriteCrLf(1);

        WriteString(1, "      (property PartValue (string \"");
        WriteSymbol(1, PartName);
        WriteString(1, "\"))");
        WriteCrLf(1);

        WriteString(1, "      (property ModuleValue (string \"");
        len = SymbolLength(ModuleName);
        if (len > 0) { 
           if (sw_A == 0) {
              CopySymbol(ModuleName, Str);
              EdifNameEA();
           }
           else
              WriteSymbol(1, ModuleName); 
        }
        else { 
           if (sw_A == 0) {
              CopySymbol(PartName, Str);
              EdifNameEA();
           }
           else
              WriteSymbol(1, PartName); 
        }

        if ( sw_N ) {
           WriteString(1, "\")))");
           WriteCrLf(1);
           }
        else {

           WriteString(1, "\"))");

           WriteCrLf(1);

           WriteString(1, "      (property TimeStampValue (string \"");
           WriteSymbol(1, TimeStamp);
           WriteString(1, "\"))");
           WriteCrLf(1);

           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field1Value (string \"");
              CopySymbol(FieldString1, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else {
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString1, Str);
             CopySymbol(FieldString1, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field1 */
                WriteString(1, "      (property Field1Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else {
                   WriteSymbol(1, Str); 
                }

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }
           WriteString(1, "\"))");
           WriteCrLf(1);

           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field2Value (string \"");
              CopySymbol(FieldString2, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else{
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString2, Str);
             CopySymbol(FieldString2, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field2 */
                WriteString(1, "      (property Field2Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else{
                   WriteSymbol(1, Str); 
                }

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }

           WriteString(1, "\"))");
           WriteCrLf(1);


           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field3Value (string \"");
              CopySymbol(FieldString3, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else{
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString3, Str);
             CopySymbol(FieldString3, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field3 */
                WriteString(1, "      (property Field3Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }

           WriteString(1, "\"))");
           WriteCrLf(1);

           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field4Value (string \"");
              CopySymbol(FieldString4, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else{
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString4, Str);
             CopySymbol(FieldString4, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field4 */
                WriteString(1, "      (property Field4Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else{
                   WriteSymbol(1, Str); 
                }

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }

           WriteString(1, "\"))");
           WriteCrLf(1);

           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field5Value (string \"");
              CopySymbol(FieldString5, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else{
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString5, Str);
             CopySymbol(FieldString5, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field5 */
                WriteString(1, "      (property Field5Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else{
                   WriteSymbol(1, Str); 
                }
             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }

           WriteString(1, "\"))");
           WriteCrLf(1);

           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field6Value (string \"");
              CopySymbol(FieldString6, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else{
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString6, Str);
             CopySymbol(FieldString6, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field6 */
                WriteString(1, "      (property Field6Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else{
                   WriteSymbol(1, Str); 
                }

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }

           WriteString(1, "\"))");
           WriteCrLf(1);


           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field7Value (string \"");
              CopySymbol(FieldString7, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else{
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString7, Str);
             CopySymbol(FieldString7, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field7 */
                WriteString(1, "      (property Field7Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else{
                   WriteSymbol(1, Str); 
                }

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }

           WriteString(1, "\"))");
           WriteCrLf(1);


           /* check for named properties switch */
           if (sw_K == 0){
              /* no /K so output as normal */
              WriteString(1, "      (property Field8Value (string \"");
              CopySymbol(FieldString8, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else{
                 WriteSymbol(1, Str); 
              }
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString8, Str);
             CopySymbol(FieldString8, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field8 */
                WriteString(1, "      (property Field8Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else{
                   WriteSymbol(1, Str); 
                }

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(1, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(1, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(1, TempStr2);
             } /* else */

           }

           WriteString(1, "\")))");
           WriteCrLf(1);
           }
    }

    /* append the nets to the end of this file */
    ConcatFile(1, 3);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}
