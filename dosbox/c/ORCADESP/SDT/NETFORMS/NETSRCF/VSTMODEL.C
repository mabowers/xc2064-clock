#include "format.h"

static char *switches = "ORSWITCHES \
/N Suppress comments\n";

static char *ver = "VERSION=\"1.10a  15-JAN-92\"";

/*  Version 1.10a
 *      - Added ability to use the 1st Field String as the HEX file name
 *        for a ROM
 *      - Corrected netlist creation for ROMs and RAMs
 *        Corrected extraneous semicolons in part delays
 *      - Added additional legal prefixes for module port and label names
 *      - Added ZERO and ONE as legal signal names
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are not checked for length
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal
 *
 *  The HEX file name for a ROM can be placed in the 1st Field String
 *  for the ROM.  If the first character of the String is not a
 *  '|' and it is not a number, then this String will be assumed to be
 *  the ROM's HEX file name.
 */

static int sw_N;

static int ok;
static int len;
static int line;
static int temp;
static int ch;
static int err;
static int isRF;   /* part is a ROM part and  1st part field is a file name */

int FieldStr, Str, PartNameStr, ReferenceStr, IPHStr, IPLStr,
    ONEStr, ZEROStr, VST_MODEL;
int ALL, NODES;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    /* sort the parts by reference number */
    SortByNumber();

    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

    FieldStr = AddSymbol("FieldStr");
    Str = AddSymbol("Str");
    PartNameStr = AddSymbol("PartNameStr");
    ReferenceStr = AddSymbol("ReferenceStr");

    VST_MODEL = AddSymbol("VST_MODEL" );
    SetSymbol( VST_MODEL, "VST_MODEL" );

    IPHStr = AddSymbol("IPHStr");
    SetSymbol(IPHStr, "IPH");

    IPLStr = AddSymbol("IPLStr");
    SetSymbol(IPLStr, "IPL");

    ONEStr = AddSymbol("ONEStr");
    SetSymbol(ONEStr, "ONE");

    ZEROStr = AddSymbol("ZEROStr");
    SetSymbol(ZEROStr, "ZERO");

    /* set to one when an illegal module port */
    /* or label prefix is encountered.        */
    err = 0;

    SetNumberWidth(5);

    sw_N = SwitchIsSet("N");
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
    AddSignalName();
    return 0;
}

/*-----------------------*/

WriteNet()
{
    RecordNode();
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    return 0;
}

/*-----------------------*/

ProcessFieldStrings()
{
    return 0;
}

/*-----------------------*/

PipeVST()
{
    ok = NextPipe();
    if (ok == 1)
    {
        ok = IsKeyWord();
        ok = 1 - ok;
    }

    while (ok == 1)
    {
        WriteSymbol(1, PipeLine);
        WriteCrLf(1);

        ok = NextPipe();
        if (ok == 1)
        {
            ok = IsKeyWord();
            ok = 1 - ok;
        }
    }
    return 0;
}

/*-----------------------*/

HandleFieldCommands()
{
    int i;
    int ch;

    i = 0;

    len = SymbolLength(FieldString1);
    if (len > 0)
    {
        CopySymbol(FieldString1, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }

    len = SymbolLength(FieldString2);
    if (len > 0)
    {
        CopySymbol(FieldString2, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            i = PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }

    len = SymbolLength(FieldString3);
    if (len > 0)
    {
        CopySymbol(FieldString3, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }

    len = SymbolLength(FieldString4);
    if (len > 0)
    {
        CopySymbol(FieldString4, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }

    len = SymbolLength(FieldString5);
    if (len > 0)
    {
        CopySymbol(FieldString5, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }

    len = SymbolLength(FieldString6);
    if (len > 0)
    {
        CopySymbol(FieldString6, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }

    len = SymbolLength(FieldString7);
    if (len > 0)
    {
        CopySymbol(FieldString7, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }

    len = SymbolLength(FieldString8);
    if (len > 0)
    {
        CopySymbol(FieldString8, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch == 124)
        {
            len = len - 1;
            PackString(1, len, FieldStr, Str);
            WriteSymbol(1, Str);
            WriteCrLf(1);
        }
    }
    return 0;
}

/*-----------------------*/

HandleFieldDelays()
{
    int i;
    int len;
    int ch;
    int semi;

    i = 0;
    semi = 0;

    if (isRF == 0)
    {
        /* only do this if not already used as the HEX file name for a ROM */
        len = SymbolLength(FieldString1);
        if (len > 0)
        {
            CopySymbol(FieldString1, FieldStr);
            ch = GetSymbolChar(i, FieldStr);
            if (ch != 124)
            {
                WriteString(1, ";");
                semi = 1;

                WriteSymbol(1, FieldString1);
            }
        }
    }

    len = SymbolLength(FieldString2);
    if (len > 0)
    {
        CopySymbol(FieldString2, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch != 124)
        { 
            if (semi == 1) { WriteString(1, ","); }
            else           { WriteString(1, ";"); semi = 1; }
            WriteSymbol(1, FieldString2);
        }
    }

    len = SymbolLength(FieldString3);
    if (len > 0)
    {
        CopySymbol(FieldString3, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch != 124)
        { 
            if (semi == 1) { WriteString(1, ","); }
            else           { WriteString(1, ";"); semi = 1; }
            WriteSymbol(1, FieldString3);
        }
    }

    len = SymbolLength(FieldString4);
    if (len > 0)
    {
        CopySymbol(FieldString4, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch != 124)
        { 
            if (semi == 1) { WriteString(1, ","); }
            else           { WriteString(1, ";"); semi = 1; }
            WriteSymbol(1, FieldString4);
        }
    }

    len = SymbolLength(FieldString5);
    if (len > 0)
    {
        CopySymbol(FieldString5, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch != 124)
        { 
            if (semi == 1) { WriteString(1, ","); }
            else           { WriteString(1, ";"); semi = 1; }
            WriteSymbol(1, FieldString5);
        }
    }

    len = SymbolLength(FieldString6);
    if (len > 0)
    {
        CopySymbol(FieldString6, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch != 124)
        { 
            if (semi == 1) { WriteString(1, ","); }
            else           { WriteString(1, ";"); semi = 1; }
            WriteSymbol(1, FieldString6);
        }
    }

    len = SymbolLength(FieldString7);
    if (len > 0)
    {
        CopySymbol(FieldString7, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch != 124)
        { 
            if (semi == 1) { WriteString(1, ","); }
            else           { WriteString(1, ";"); semi = 1; }
            WriteSymbol(1, FieldString7);
        }
    }

    len = SymbolLength(FieldString8);
    if (len > 0)
    {
        CopySymbol(FieldString8, FieldStr);
        ch = GetSymbolChar(i, FieldStr);
        if (ch != 124)
        { 
            if (semi == 1) { WriteString(1, ","); }
            else           { WriteString(1, ";"); semi = 1; }
            WriteSymbol(1, FieldString8);
        }
    }
    return 0;
}

/*-----------------------*/

CheckSignal()
{
    int c;

    CopySymbol(SignalNameString, Str);
    FToUpper(Str);

    temp = 0;
    ch = GetSymbolChar(temp, Str);
    if (ch == 'P') { ok = 1; }
    if (ch == 'L') { ok = 1; }
    if (ch == 'N') { ok = 1; }

    if (ch == '*')
    {
        temp = 1;
        c = GetSymbolChar(temp, Str);

        if (c == 'P') { ok = 1; }
        if (c == 'L') { ok = 1; }
        if (c == 'N') { ok = 1; }
    }

    if (ch == '~')
    {
        temp = 1;
        c = GetSymbolChar(temp, Str);

        if (c == 'P') { ok = 1; }
        if (c == 'L') { ok = 1; }
        if (c == 'N') { ok = 1; }
    }

    if (ch == 'I')
    {
        temp = CompareSymbol(Str, IPLStr);
        if (temp == 0) { ok = 1; };

        temp = CompareSymbol(Str, IPHStr);
        if (temp == 0) { ok = 1; };
    }

    if (ch == 'Z')
    {
        ok = 1 + CompareSymbol(Str, ZEROStr);
    }

    if (ch == 'O')
    {
        ok = 1 + CompareSymbol(Str, ONEStr);
    }
    return 0;
}

/*-----------------------*/

WriteNodeInfo()
{
    int i;

    /* Str must begin with 'P', 'L', 'N', '*P', '*L', '*N', '~P', */
    /* '~L', '~N', or be named 'IPL', 'IPH', 'ONE', or 'ZERO'.    */
    ok = 0;
    CheckSignal();
    
    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (ok == 0)
        {
            if (err == 0)
            {
                err = 1;

                /* put out legal prefixes */
                WriteString(0, "Signal names must begin with one of the following prefixes:");
                WriteCrLf(0);
                WriteString(0, "     'P',  'L',  'N',");
                WriteCrLf(0);
                WriteString(0, "    '*P', '*L', '*N',");
                WriteCrLf(0);
                WriteString(0, "    '~P', '~L', '~N',");
                WriteCrLf(0);
                WriteString(0, "or have the name:");
                WriteCrLf(0);
                WriteString(0, "    'IPL', 'IPH', 'ONE' or 'ZERO'");
                WriteCrLf(0);
            }

            WriteString(0, "ERROR: Illegal signal name '");
            WriteSymbol(0, SignalNameString);
            WriteString(0, "'");
            WriteCrLf(0);

            SetSymbol(ExitType, "E");
        }

        len = SymbolLength(SignalNameString);
        line = line + len;

        WriteSymbol(1, SignalNameString);
    }
    if (i == 'P')
    {
        if (ok == 0)
        {
            if (err == 0)
            {
                err = 1;

                /* put out legal prefixes */
                WriteString(0, "Signal names must begin with one of the following prefixes:");
                WriteCrLf(0);
                WriteString(0, "     'P',  'L',  'N',");
                WriteCrLf(0);
                WriteString(0, "    '*P', '*L', '*N',");
                WriteCrLf(0);
                WriteString(0, "    '~P', '~L', '~N',");
                WriteCrLf(0);
                WriteString(0, "or have the name:");
                WriteCrLf(0);
                WriteString(0, "    'IPL', 'IPH', 'ONE' or 'ZERO'");
                WriteCrLf(0);
            }

            WriteString(0, "ERROR: Illegal signal name '");
            WriteSymbol(0, SignalNameString);
            WriteString(0, "'");
            WriteCrLf(0);

            SetSymbol(ExitType, "E");
        }

        len = SymbolLength(SignalNameString);
        line = line + len;

        WriteSymbol(1, SignalNameString);
    }
    if (i == 'S')
    {
        WriteString(0, "ERROR: Signal '");
        WriteSymbol(0, SignalNameString);
        WriteString(0, "' is a power net");
        WriteCrLf(0);

        SetSymbol(ExitType, "E");

        len = SymbolLength(SignalNameString);
        line = line + len;

        WriteSymbol(1, SignalNameString);
    }
    if (i == 'N')
    {
        WriteString(0, "ERROR: Pin ");
        WriteSymbol(0, PinNumberString);
        WriteString(0, " on Reference '");
        WriteSymbol(0, ReferenceString);
        WriteString(0, "' is connected to an unnamed net");
        WriteCrLf(0);

        SetSymbol(ExitType, "E");
        
        line = line + 6;

        WriteString(1, "?");
        WriteSymbol(1, NetNumber);
    }
    if (i == 'U')
    {
        WriteString(0, "ERROR: Pin ");
        WriteSymbol(0, PinNumberString);
        WriteString(0, " on Reference '");
        WriteSymbol(0, ReferenceString);
        WriteString(0, "' is unconnected");
        WriteCrLf(0);

        SetSymbol(ExitType, "E");
        
        line = line + 2;

        WriteString(1, "??");
    }
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    int i;
    int first;
    int ch;
    int ptype;
    int len;

    /* write any header info in pipe commands */
    FirstPipe();
    ok = AccessKeyWord(VST_MODEL);
    while (ok == 1) {
        PipeVST();
        ok = AccessKeyWord(VST_MODEL);
    }

    RewindInstanceFile();
    while (LoadInstance())
    {
        CopySymbol(ReferenceString, ReferenceStr);
        SetIndexByRef(ReferenceStr);

        /* check for any commands in the Part Fields */
        HandleFieldCommands();

        CopySymbol(PartName, PartNameStr);
        ExceptionsFor("MOD", PartNameStr);
        WriteSymbol(1, PartNameStr);
        WriteString(1, "(");

        line = SymbolLength(PartNameStr);
        line = line + 1;

        /* Find out if this part is a ROM.  If so, then the first Field    */
        /* String will be treated as the HEX file name if the first        */
        /* character is not a number.                                      */
        isRF = 0;

        i = 0;
        ch = GetSymbolChar(i, PartNameStr);
        if (ch == 'R')
        {
            i = 1;
            ch = GetSymbolChar(i, PartNameStr);
            if (ch == 'O')
            {
                i = 2;
                ch = GetSymbolChar(i, PartNameStr);
                if (ch == 'M')
                {
                    /* is there data in the Field String? */
                    CopySymbol(FieldString1, FieldStr);
                    len = SymbolLength(FieldStr);
                    if (len > 0)
                    {
                        i = 0;
                        ch = GetSymbolChar(i, FieldStr);
                        if (ch != 124)
                        {
                            /* ch is not '|' */
                            if (ch < 48)
                            {
                                /* ch is a not a number */
                                isRF = 1;

                                /* write the file name out */
                                WriteSymbol(1, FieldStr);
                                WriteString(1, ";");

                                line = line + len;                             
                                line = line + 1;
                            }

                            if (ch > 57)
                            {
                                /* ch is a not a number */
                                isRF = 1;

                                /* write the file name out */
                                WriteSymbol(1, FieldStr);
                                WriteString(1, ";");

                                line = line + len;                             
                                line = line + 1;
                            }
                        }
                    }                
                }
            }
        }

        first = 1;
        SetFirst(NODES);
        do {
            i = GetStandardSymbol(PinType);

            if (first != 1)
            {
                if (i != ptype)
                    { ptype = i; WriteString(1, ";"); }
                else
                    { WriteString(1, ","); }
            }
            else
            {
                ptype = i;
            }

            if (line > 70)
            {
                line = 10;
                WriteCrLf(1);
                WriteString(1, "          ");
            }

            WriteNodeInfo();
            first = 0;
        } while (SetNext(NODES));

        /* if not a logical device, then set delays */
        i = 0;
        ch = GetSymbolChar(i, PartNameStr);
        if (ch != 'L')
        {
            HandleFieldDelays();
        }
        WriteString(1, ");");

        if (sw_N == 0)
        {
            WriteSymbol(1, ReferenceString);
        }
        WriteCrLf(1);
    }

    WriteString(1, "%");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


