#include "format.h"

static char *switches = "ORSWITCHES \
/A Allow nonEDIF characters\n \
/K Convert PartFields to Named Properties\n \
/L Do not append sheet number to labels\n \
/N Do not output additional properties\n \
/P Output pin names (instead of pin numbers)\n \
/E Do not create the 'external' library declaration\n";

static char *ver = "VERSION=\"1.20d  12-SEP-94\"";

/*
 *  Version 1.20d
 *      - Changed the way /A is handled.  /A was UPPER casing everything
 *        and I changed it to be case insensitive.
 *  Version 1.20c
 *      - Added code to make sure that NamedProperties get checked for
 *        valid EDIF names.
 *  Version 1.20b
 *      - Added WARNING message to EdifNameEA().
 *  Version 1.20a
 *      - Added /K switch.  This switch causes the Part Fields to get
 *        renamed to "Named Properties". The part field on the schematic
 *        must follow the following convention for naming:
 *                NAME=STRING.    The NAME of the property would be NAME
 *        in this example, and STRING would be the string associated with
 *        the property.
 *      - Added /A switch.  Without /A all fieldstrings, part value, and
 *        module names get renamed when illegal characters are found.
 *
 *  Version 1.10f
 *      - Changed /N to write to file "3" instead of "1".
 *
 *  Version 1.10e
 *      - Changed default to output pin numbers rather that pin names
 *
 *  Version 1.10d
 *      - Report all elements on a net inside the NextNode() loop
 *
 *  Version 1.10c
 *      - Added properties for time stamp and field strings
 *      - Added /N switch
 *
 *  Version 1.10b
 *      - Altered PostFile to handle sheets with zero parts
 *      - Moved comment in the cell definition after the cellType
 *      - Added handling for 0 parts and/or 0 nets in a design
 *      - Added code to strip path information from sheet filenames
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

static int legal;
static int len;
static int ch;
static int ok;
static int first;
static int SignalNameStr, TempStr, Str, DesignStr;
static int TempStr2, JunkStr;

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
        WriteString(3, "");
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

        WriteString(3, "");
        WriteSymbol(3, TempStr);
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

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    MakeInstanceFile();
    SetTraversal("LEAF");

    SignalNameStr = AddSymbol("SignalNameStr");
    TempStr = AddSymbol("TempStr");
    Str = AddSymbol("Str");
    JunkStr = AddSymbol("JunkStr");
    TempStr2 = AddSymbol("TempStr2");
    DesignStr = AddSymbol("DesignStr");

    sw_L = SwitchIsSet("L");
    sw_N = SwitchIsSet("N");
    sw_P = SwitchIsSet("P");
    sw_E = SwitchIsSet("E");
    sw_A = SwitchIsSet("A");
    sw_K = SwitchIsSet("K");

    SetCharSet("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    first = 1;
    return 0;
}

/*-----------------------*/

PreFile()
{
    return 0;
}

/*-----------------------*/

PostFile()
{
    int i;
    int EqualSign, pos;

    EqualSign = '=';

    if (first == 1)
    {
        /* write the schematic library */
        WriteString(3, " (library MAIN_LIB");
        WriteCrLf(3);
        WriteString(3, "  (edifLevel 0)");
        WriteCrLf(3);
        WriteString(3, "  (technology");
        WriteCrLf(3);
        WriteString(3, "   (numberDefinition");
        WriteCrLf(3);
        WriteString(3, "    (scale 1 1 (unit distance))))");
    }
    WriteCrLf(3);

    ok = LastFile();
    if (ok == 1)
    {
        /* this is the root sheet -- no pins upward in the hierarchy */
        WriteString(1, "(edif ");
        CopySymbol(FileName, Str);
        CopySymbol(FileName, DesignStr);
        FToUpper(Str);
        StripPath(Str);
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
        WriteString(1, "   (program \"HFORM.EXE\")");
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
    }

    WriteString(3, "  (cell ");
    CopySymbol(FileName, Str);
    FToUpper(Str);
    StripPath(Str);
    StripExt();
    EdifName3();
    WriteCrLf(3);
    WriteString(3, "   (cellType generic)");
    WriteCrLf(3);
    WriteString(3, "   (view NetlistView");
    WriteCrLf(3);
    WriteString(3, "    (viewType netlist)");
    WriteCrLf(3);
    WriteString(3, "    (interface");

    /* write out the ports */
    ok = FirstChild();
    if (ok == 1)
    {
        ok = FirstChild();

        CopySymbol(FileName, Str);
        StripExt();
        CopySymbol(Str, TempStr);

        CopySymbol(PartName, Str);
        StripExt();

        /* if the same, then these are the module ports for this sheet */
        ok = CompareSymbol(TempStr, Str);
        if (ok == 0)
        {
            /* the map for pin types */
            SetPinMap(0, "INOUT");
            SetPinMap(1, "OUTPUT");
            SetPinMap(2, "INPUT");
            SetPinMap(3, "INOUT");
            SetPinMap(4, "OUTPUT");
            SetPinMap(5, "INOUT");
            SetPinMap(6, "OUTPUT");
            SetPinMap(7, "INPUT");

            FirstChildPin();
            do {
                WriteCrLf(3);
                WriteString(3, "     (port ");

                if (sw_P == 1)
                    { CopySymbol(PinNameString, Str); }
                else
                    { CopySymbol(PinNumberString, Str); }
                EdifName3();

                WriteString(3, " (direction ");
                i = GetStandardSymbol(PinType);
                WriteMap(3, i);
                WriteString(3, "))");
            } while (NextChildPin());
        }
    }
    WriteString(3, ")");
    WriteCrLf(3);

    /* write out the instances of library parts */
    SetPinMap(0, "INOUT");
    SetPinMap(1, "OUTPUT");
    SetPinMap(2, "INPUT");
    SetPinMap(3, "INOUT");

    WriteString(3, "    (contents");

    ok = FirstPart();
    if (ok == 1) {
        do {
            CopySymbol(ReferenceString, Str);
            LoadFieldString(Str);

            WriteCrLf(3);
            WriteString(3, "     (instance ");
            EdifName3();
            WriteCrLf(3);

            WriteString(3, "      (viewRef NetlistView");
            WriteCrLf(3);

            WriteString(3, "       (cellRef ");
            CopySymbol(LookupNameString, Str);
            EdifName3NR();
            WriteCrLf(3);

            WriteString(3, "        (libraryRef OrCAD_LIB)))");
            WriteCrLf(3);

            WriteString(3, "      (property PartValue (string \"");
            WriteSymbol(3, PartName);
            WriteString(3, "\"))");
            WriteCrLf(3);

            WriteString(3, "      (property ModuleValue (string \"");
            len = SymbolLength(ModuleName);
            if (len > 0)
                { WriteSymbol(3, ModuleName); }
            else
                { WriteSymbol(3, PartName); }
            if ( sw_N ) {
/*                WriteString(1, "\")))");   */
                WriteString(3, "\")))");
                }
            else {
                WriteString(3, "\"))");

                WriteCrLf(3);

                WriteString(3, "      (property TimeStampValue (string \"");
                WriteSymbol(3, TimeStamp);
                WriteString(3, "\"))");
                WriteCrLf(3);

           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field1Value (string \"");
              CopySymbol(FieldString1, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString1, Str);
             CopySymbol(FieldString1, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);  
             if (pos == 255){
             /* no equal sign, so default back to Field1 */
                WriteString(3, "      (property Field1Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }
           WriteString(3, "\"))");
           WriteCrLf(3);

           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field2Value (string \"");
              CopySymbol(FieldString2, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString2, Str);
             CopySymbol(FieldString2, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field2 */
                WriteString(3, "      (property Field2Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }

           WriteString(3, "\"))");
           WriteCrLf(3);


           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field3Value (string \"");
              CopySymbol(FieldString3, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString3, Str);
             CopySymbol(FieldString3, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field3 */
                WriteString(3, "      (property Field3Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }

           WriteString(3, "\"))");
           WriteCrLf(3);

           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field4Value (string \"");
              CopySymbol(FieldString4, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString4, Str);
             CopySymbol(FieldString4, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field4 */
                WriteString(3, "      (property Field4Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }

           WriteString(3, "\"))");
           WriteCrLf(3);

           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field5Value (string \"");
              CopySymbol(FieldString5, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString5, Str);
             CopySymbol(FieldString5, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field5 */
                WriteString(3, "      (property Field5Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }

           WriteString(3, "\"))");
           WriteCrLf(3);

           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field6Value (string \"");
              CopySymbol(FieldString6, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString6, Str);
             CopySymbol(FieldString6, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field6 */
                WriteString(3, "      (property Field6Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }

           WriteString(3, "\"))");
           WriteCrLf(3);


           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field7Value (string \"");
              CopySymbol(FieldString7, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString7, Str);
             CopySymbol(FieldString7, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field7 */
                WriteString(3, "      (property Field7Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }

           WriteString(3, "\"))");
           WriteCrLf(3);


           /* check for named properties switch */
           if (sw_K == 0){
              WriteString(3, "      (property Field8Value (string \"");
              CopySymbol(FieldString8, Str);
              if (sw_A == 0) {
                 EdifNameEA();
              }
              else
                 WriteSymbol(3, Str); 
           }
           else{
           /* we have to Name the Property */
             CopySymbol(FieldString8, Str);
             CopySymbol(FieldString8, TempStr2);
             pos = FindSymbolChar(EqualSign, Str);
             if (pos == 255){
             /* no equal sign, so default back to Field8 */
                WriteString(3, "      (property Field8Value (string \"");
                if (sw_A == 0) {
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, Str); 

             } /* if */
             else{
             /* we have an equal sign, so lets name the property */
                WriteString(3, "      (property ");
                PackString(0, pos-1, Str, TempStr);
                if (sw_A == 0) {
                   CopySymbol(TempStr, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr);

                /* now get the string to the right of the EqualSign */
                PackString(0, pos, TempStr2, JunkStr);
                WriteString(3, " (string \"");
                if (sw_A == 0) {
                   CopySymbol(TempStr2, Str);
                   EdifNameEA();
                }
                else
                   WriteSymbol(3, TempStr2);
             } /* else */

           }

           WriteString(3, "\")))");
           WriteCrLf(3);
           }

        } while (NextPart());
    }

    /* write out the instances of sheets */
    ok = FirstChild();
    if (ok == 1)
    {
        CopySymbol(FileName, Str);
        StripExt();
        CopySymbol(Str, TempStr);
        
        CopySymbol(PartName, Str);
        StripExt();

        /* skip if this is the module ports for the current sheet */
        ok = CompareSymbol(Str, TempStr);
        if (ok == 0)
            { ok = NextChild(); }
        else
            { ok = 1; }

        while (ok == 1)
        {
            WriteCrLf(3);
            WriteString(3, "     (instance ");
            CopySymbol(ReferenceString, Str);
            EdifName3();
            WriteCrLf(3);

            WriteString(3, "      (viewRef NetlistView");
            WriteCrLf(3);

            WriteString(3, "       (cellRef ");
            CopySymbol(PartName, Str);
            FToUpper(Str);
            StripPath(Str);
            StripExt();
            EdifName3NR();
            WriteString(3, ")))");

            ok = NextChild();
        }
    }

    /* write out the nets */
    ok = FirstNet();
    if (ok == 1) {
        do {
            WriteCrLf(3);
            WriteString(3, "     (net ");

            CopySymbol(NetNameString, Str);

            i = GetStandardSymbol(NetCode);
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

            ok = FirstNode();
            if (ok == 1)
            {
                do {
                    i = GetStandardSymbol(TypeCode);

                    if (i == 'N')
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
                    }
                    if (i == 'U')
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
                    }
                 if (i == 'P')
                    {
                    /* tie this net to its port -- if it is a port */
                    WriteCrLf(3);
                    WriteString(3, "       (portRef ");
                    CopySymbol(SignalNameString, Str);
                    EdifName3NR();
                    WriteString(3, ")");
                    }
                } while (NextNode());
            }

            WriteString(3, "))");
        } while (NextNet());
    }
    WriteString(3, ")))");

    first = 0;
    return 0;
}

/*-----------------------*/

PostProcess()
{
    int i;

    WriteString(3, ")");
    WriteCrLf(3);

    /* tell EDIF what cell makes up the design */
    WriteString(3, " (design ");
    CopySymbol(DesignStr, Str);
    FToUpper(Str);
    StripPath(Str);
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
            i = SymbolLength(LookupNameString);
            if (i > 0) {

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
            }
        } while (NextAccessType());
        WriteString(1, ")");
        WriteCrLf(1);
    }

    ConcatFile(1, 3);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}

