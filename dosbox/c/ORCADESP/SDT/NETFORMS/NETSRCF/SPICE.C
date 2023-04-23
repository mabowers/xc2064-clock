#include "format.h"

static char *switches = "ORSWITCHES \
/2\n \
/L Do not append sheet number to labels\n \
/N Use node names\n \
/I Include unconnected pins\n";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Changed unconnected pins from net "0" to " "
 *      - Added /I switch to allow unconnected pins to be given a net
 *      - Changed so that the GND net is always "0", even with /N set
 *      - Added warnings for names with illegal characters
 *      - Made '$' a legal character (needed for digital simulation)
 *      - Added Version message
 *      - Corrected int to unsigned comparison
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are not checked for length
 *      - Node Numbers are limited to 5 digits
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - Node names are limited to { '_', '$', '0-9' , 'a-z', 'A-Z' } if /N is present
 *
 *      Even if your version of SPICE does not allow alphabetic node names,
 *      you can still give nodes names if they represent numerical
 *      values (such as '14').  These node 'numbers' will not
 *      overlap with the system defined ones since system node numbers
 *      begin at 10000 (except GND, which is always 0).
 *
 *      Unconnected pins will generate a warning and a space, " ", will
 *      be output instead of a number.  If /I is present, then unconnected
 *      pins will not generate a warning and will be given a net number
 *      starting at 32767 and decreasing with each additional unconnected pin.
 *
 *      The .MAP file that is generated is only valid if the /N switch is
 *      not used (since labels will be output to the netlist instead of
 *      system created net numbers if /N is present).
 */

static int sw_L;
static int sw_N;
static int sw_I;

static int net;
static int unet;
static int pos;
static int ch;
static int ok;
static int type;
static int new_type;
static int legal;
static int len;

static int SignalNameStr, Str, GND, SPICE;
static int ALL, NODES;

/*-----------------------*/

WriteSysNet()
{
    net = GetStandardSymbol(NetNumber);
    net = net + 10000;
    WriteInteger(1, net);
    return 0;
}

/*-----------------------*/

ValidateNodeInfo()
{
    legal = 1;

    type = GetStandardSymbol(TypeCode);
    new_type = type;
    if (sw_N == 1)
    {
        legal = SymbolInCharSet(SignalNameStr);

        if (legal != 1) { new_type = 'N'; }
    }
    else
    {
        if (type != 'U') { new_type = 'N'; }
    }
    return 0;
}

/*-----------------------*/

PipeSPICE()
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

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    SignalNameStr = AddSymbol("SignalNameStr");
    Str = AddSymbol("Str");

    SPICE = AddSymbol("SPICE");
    SetSymbol( SPICE, "SPICE" );
    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

    GND = AddSymbol("GND");
    SetSymbol(GND, "GND");

    unet = 32767;

    sw_L = SwitchIsSet("L");
    sw_N = SwitchIsSet("N");
    sw_I = SwitchIsSet("I");

    SetCharSet("_$0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(1);

    WriteString(1, "* ");
    CopySymbol(TitleString, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revised: ");
    WriteSymbol(1, DateString);
    WriteCrLf(1);
    WriteString(1, "* ");
    CopySymbol(DocumentNumber, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revision: ");
    WriteSymbol(1, Revision);
    WriteCrLf(1);
    WriteString(1, "* ");
    WriteSymbol(1, Organization);
    WriteCrLf(1);
    WriteString(1, "* ");
    WriteSymbol(1, AddressLine1);
    WriteCrLf(1);
    WriteString(1, "* ");
    WriteSymbol(1, AddressLine2);
    WriteCrLf(1);
    WriteString(1, "* ");
    WriteSymbol(1, AddressLine3);
    WriteCrLf(1);
    WriteString(1, "* ");
    WriteSymbol(1, AddressLine4);
    WriteCrLf(1);

    FirstPipe();
    ok = AccessKeyWord(SPICE);
    while (ok == 1) {
        PipeSPICE();
        ok = AccessKeyWord(SPICE);
    }
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
    int num;

    AddSignalName();

    type = GetStandardSymbol(TypeCode);
    if (type == 'L')
    {
        num = GetStandardSymbol(NetNumber);
        num = num + 10000;
        WriteInteger(2, num);

        WriteString(2, " ");

        if (sw_N == 0)
        {
            WriteSymbol(2, SignalNameString);
        }
        else
        {
            if (sw_L == 0)
            {
                MakeLocalSignal("_");
                WriteSymbol(2, LocalSignal);
            }
            else
            {
                WriteSymbol(2, SignalNameString);
            }
        }
        WriteCrLf(2);
    }

    if (type == 'P')
    {
        num = GetStandardSymbol(NetNumber);
        num = num + 10000;
        WriteInteger(2, num);

        WriteString(2, " ");

        WriteSymbol(2, SignalNameString);
        WriteCrLf(2);
    }

    if (type == 'S')
    {
        ok = CompareSymbol(GND, SignalNameString);
        if (ok == 0)
            { WriteString(2, "    0 "); }
        else
        {
            num = GetStandardSymbol(NetNumber);
            num = num + 10000;
            WriteInteger(2, num);
            WriteString(2, " ");
        }

        WriteSymbol(2, SignalNameString);
        WriteCrLf(2);
    }

    if (type == 'S')
    {
        ok = CompareSymbol(GND, SignalNameString);
        if (ok != 0)
        {
            /* separate the name from trailing characters */
            CopySymbol(SignalNameString, SignalNameStr);

            ch = ' ';
            pos = FindSymbolChar(ch, SignalNameStr);
            if (pos != -1)
            {
                pos = pos + 1;
                len = SymbolLength(SignalNameStr);
                len = len - 1;
                PackString(pos, len, SignalNameStr, Str);

                pos = pos - 1;
                ch = 0;
                /* kill the trailing space */
                PutSymbolChar(pos, ch, SignalNameStr);
            }
            else
            {
                SetSymbol(Str, "");
            }

            ValidateNodeInfo();
            if (legal != 1)
            {
                net = GetStandardSymbol(NetNumber);
                net = net + 10000;

                /* illegal character in the node name */
                WriteString(0, "WARNING: Name contains illegal characters '");
                WriteSymbol(0, SignalNameStr);
                WriteString(0, "', changed to ");
                WriteInteger(0, net);
                WriteCrLf(0);

                SetSymbol(ExitType, "W");
            }

            WriteSymbol(1, SignalNameStr);

            WriteString(1, " ");
            if (new_type == 'S')
                { WriteSymbol(1, SignalNameStr); }
            else
                { WriteSysNet(); }

            WriteString(1, " 0");

            len = SymbolLength(Str);
            if (len > 0)
            {
                WriteString(1, " ");
                WriteSymbol(1, Str);
            }
            WriteCrLf(1);
        }
    }
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

WriteNodeInfo()
{
    CopySymbol(SignalNameString, SignalNameStr);
    ValidateNodeInfo();

    /* this one is special since GND is always 0 */
    if (type == 'S')
    {
        ok = CompareSymbol(GND, SignalNameString);
        if (ok == 0)
            { WriteString(1, "0"); }
        else
        {
            if (sw_N == 0)
            {
                WriteSysNet();
            }
            else
            {
                ch = ' ';
                pos = FindSymbolChar(ch, SignalNameStr);
                if (pos != -1)
                {
                    ch = 0;
                    /* kill the trailing space */
                    PutSymbolChar(pos, ch, SignalNameStr);
                }
                WriteSymbol(1, SignalNameStr);
            }
        }
    }
    if (new_type == 'L')
    {
        if (sw_L == 0)
        {
            MakeLocalSignal("_");
            WriteSymbol(1, LocalSignal);
        }
        else
        {
            WriteSymbol(1, SignalNameStr);
        }
    }
    if (new_type == 'P')
    {
        WriteSymbol(1, SignalNameString);
    }
    if (new_type == 'N')
    {
        if (type != 'S')
        {
            WriteSysNet();

            if (legal != 1)
            {
                /* illegal character in the node name */
                WriteString(0, "WARNING: Name contains illegal characters '");
                WriteSymbol(0, SignalNameStr);
                WriteString(0, "', changed to ");
                WriteInteger(0, net);
                WriteCrLf(0);
            }
        }
    }
    if (new_type == 'U')
    {
        if (sw_I == 1)
        {
            WriteInteger(1, unet);
            unet = unet - 1;
        }
        else
        {
            WriteString(0, "WARNING: Pin '");
            WriteSymbol(0, PinNameString);
            WriteString(0, "' on Reference '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "' is unconnected");
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        
            WriteString(1, " ");
        }
    }
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    SetFirst(ALL);
    do {
        WriteSymbol(1, ReferenceString);
    
        do {
            WriteString(1, " ");
            WriteNodeInfo();
        } while (SetNext(NODES));

        WriteString(1, " ");
        WriteSymbol(1, PartName);
        WriteCrLf(1);
    } while (SetNext(ALL));

    WriteString(1, ".END");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


