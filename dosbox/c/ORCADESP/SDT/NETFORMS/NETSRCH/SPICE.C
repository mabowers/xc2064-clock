#include "format.h"

static char *switches = "ORSWITCHES \
/2\n \
/L Do not append sheet number to labels\n \
/N Use node names\n \
/I Include unconnected pins\n";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10a
 *      - Changed unconnected pins from net "0" to " "
 *      - Added /I switch to allow unconnected pins to be given a net
 *      - Changed so that the GND net is always "0", even with /N set
 *      - Altered PostFile to handle sheets with zero parts
 *      - Corrected format of subcircuit calls
 *      - Added warnings for names with illegal characters
 *      - Added code to strip path information from sheet filenames
 *      - Made '$' a legal character (needed for digital simulation)
 *      - Removed unneeded call to MakeInstanceFile()
 *      - Added Version message
 *      - Corrected int to unsigned comparison
 *      - Corrected output of unused pins to file 3
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

static int type;
static int new_type;
static int first;
static int ok;
static int legal;
static int len;
static int net;
static int unet;
static int pos;
static int ch;
static int num;
static int SignalNameStr, TempStr, Str, GND, SPICE;

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

StripExt()
{
    ch = 46;
    pos = FindSymbolChar(ch, Str);
    if (pos != -1) { ch = 0; PutSymbolChar(pos, ch, Str); }
    return 0;
}

/*-----------------------*/

ValidateNetInfo()
{
    legal = 1;

    type = GetStandardSymbol(NetCode);
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

WriteSysNet()
{
    net = GetStandardSymbol(NetNumber);
    net = net + 10000;
    WriteInteger(3, net);
    return 0;
}

/*-----------------------*/

WritePower()
{
    int ch;
    int pos;

    type = GetStandardSymbol(NetCode);
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
                len = SymbolLength(SignalNameStr);
                len = len - 1;
                pos = pos + 1;
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

            ValidateNetInfo();
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

            WriteSymbol(3, SignalNameStr);

            WriteString(3, " ");
            if (new_type == 'S')
                { WriteSymbol(3, SignalNameStr); }
            else
                { WriteSysNet(); }

            WriteString(3, " 0");

            len = SymbolLength(Str);
            if (len > 0)
            {
                WriteString(3, " ");
                WriteSymbol(3, Str);
            }
            WriteCrLf(3);
        }
    }
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

WriteNodeInfo()
{
    CopySymbol(SignalNameString, SignalNameStr);
    ValidateNodeInfo();

    /* this one is special since GND is always 0 */
    if (type == 'S')
    {
        ok = CompareSymbol(GND, SignalNameString);
        if (ok == 0)
            { WriteString(3, "0"); }
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
                WriteSymbol(3, SignalNameStr);
            }
        }
    }
    if (new_type == 'L')
    {
        if (sw_L == 0)
        {
            MakeLocalSignal("_");
            WriteSymbol(3, LocalSignal);
        }
        else
        {
            WriteSymbol(3, SignalNameStr);
        }
    }
    if (new_type == 'P')
    {
        WriteSymbol(3, SignalNameStr);
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
            WriteInteger(3, unet);
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
        
            WriteString(3, " ");
        }
    }
    return 0;
}

/*-----------------------*/

WriteParms()
{
    ok = FirstChild();
    if (ok == 0) { return 0; }

    WriteString(3, ".SUBCKT ");
    CopySymbol(FileName, Str);
    FToUpper(Str);
    StripPath(Str);
    StripExt();
    WriteSymbol(3, Str);

    FirstChildPin();
    do {
        WriteString(3, " ");

        CopySymbol(PinNameString, Str);
        legal = SymbolInCharSet(Str);

        if ((legal + sw_N) == 2)
        {
            WriteSymbol(3, PinNameString);
        }
        else
        {
            ok = FirstNet();
            if (ok == 1) {
                do {
                    type = GetStandardSymbol(NetCode);
                    if (type == 'P')
                    {
                        ok = CompareSymbol(SignalNameString, Str);
                        if (ok == 0) { WriteSysNet(); }
                    }
                } while (NextNet());
            }
        }
    } while (NextChildPin());
    WriteCrLf(3);
    return 0;
}

/*-----------------------*/

WriteMAPFile()
{
    type = GetStandardSymbol(NetCode);
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
        WriteString(2, " (sheet ");
        CopySymbol(FileName, Str);
        FToUpper(Str);
        StripExt();
        WriteSymbol(2, Str);
        WriteString(2, ")");
        WriteCrLf(2);
    }

    if (type == 'P')
    {
        num = GetStandardSymbol(NetNumber);
        num = num + 10000;
        WriteInteger(2, num);

        WriteString(2, " ");
        WriteSymbol(2, SignalNameString);
        WriteString(2, " (sheet ");
        CopySymbol(FileName, Str);
        FToUpper(Str);
        StripExt();
        WriteSymbol(2, Str);
        WriteString(2, ")");
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
        WriteString(2, " (sheet ");
        CopySymbol(FileName, Str);
        FToUpper(Str);
        StripExt();
        WriteSymbol(2, Str);
        WriteString(2, ")");
        WriteCrLf(2);
    }
    return 0;
}

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    SetTraversal("ROOT");

    SignalNameStr = AddSymbol("SignalNameStr");
    TempStr = AddSymbol("TempStr");
    Str = AddSymbol("Str");

    GND = AddSymbol("GND");
    SetSymbol(GND, "GND");

    SPICE = AddSymbol("SPICE");
    SetSymbol(SPICE, "SPICE" );

    unet = 32767;

    sw_L = SwitchIsSet("L");
    sw_N = SwitchIsSet("N");
    sw_I = SwitchIsSet("I");

    SetCharSet("_$0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(1);
    first = 1;
    net = 0;
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
    if (first == 1)
    {
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
    }
    else
    {
        WriteParms();
    }

    /* output the power sources and .MAP file */
    ok = FirstNet();
    if (ok == 1) {
        do {
            WritePower();
            WriteMAPFile();
        } while (NextNet());
    }

    /* write out the parts */
    ok = FirstPart();
    if (ok == 1) {
        do {
            WriteSymbol(3, ReferenceString);

            FirstPin();    
            do {
                SetSignal();

                WriteString(3, " ");
                WriteNodeInfo();
            } while (NextPin());

            WriteString(3, " ");
            WriteSymbol(3, PartName);
            WriteCrLf(3);
        } while (NextPart());
    }

    /* write out the subcircuit calls */
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
            WriteString(3, "X");

            CopySymbol(ReferenceString, Str);
            FToUpper(Str);
            StripExt();
            WriteSymbol(3, Str);

            FirstChildPin();
            do {
                SetSignal();

                WriteString(3, " ");
                WriteNodeInfo();
            } while (NextChildPin());

            WriteString(3, " ");
            CopySymbol(PartName, Str);
            FToUpper(Str);
            StripPath(Str);
            StripExt();
            WriteSymbol(3, Str);

            WriteCrLf(3);
            ok = NextChild();
        }
    }

    if (first != 1)
    {
        WriteString(3, ".ENDS");
        WriteCrLf(3);
    }
    first = 0;
    return 0;
}

/*-----------------------*/

PostProcess()
{
    FirstPipe();
    ok = AccessKeyWord(SPICE);
    while (ok == 1) {
        PipeSPICE();
        ok = AccessKeyWord(SPICE);
    }

    WriteString(3, ".END");
    WriteCrLf(3);

    ConcatFile(1, 3);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


