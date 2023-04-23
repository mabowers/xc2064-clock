#include "format.h"

static char *switches = "ORSWITCHES \
/F Keep system generated netnames\n \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.20b  07-JUL-94\"";

/*
 *  Version 1.20a
 *      - Added /F switch.
 *  Version 1.20
 *      - Changed ERROR messages to WARNINGS.
 *  Version x1.11
 *      - Part list & Net list are written to 1 file now.
 *  Version x1.10
 *  Modified PADSASC0.C version 1.10c 
 *      - Added closing asterisks for keywords
 *      - Extended allowable character set for reference strings
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names not checked for length
 *      - Reference Strings are limited to 6 characters
 *      - Node Names are limited to 12 characters
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - Node names can only use these characters { '~!#$%_-=+|/.:;<>',', 'A-Z', 'a-z', '0-9' }
 *      - References can only use these characters { '~!#$%_-=+|/.:;<>','A-Z', 'a-z', '0-9' }
 */

static int sw_L;

static int net_items;
static int Error, ReferenceStr, SignalNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Error = AddSymbol("Error");
    SetSymbol(Error, "E");

    /* used to pad their standard symbol namesakes with spaces */
    ReferenceStr = AddSymbol("ReferenceStr");
    SignalNameStr = AddSymbol("SignalNameStr");

    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    int legal;
    int len;

    SetCharSet("~!#$%_-=+|/.:;<>0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

    WriteString(1, "*PADS-PCB*");
    WriteCrLf(1);
    WriteString(1, "*PART*");
    WriteCrLf(1);

    while (LoadInstance())
    {
        CopySymbol(ReferenceString, ReferenceStr);
        legal = SymbolInCharSet(ReferenceStr);
        PadSpaces(ReferenceStr, 6);
        if (legal != 1)
        {
            WriteString(0, "WARNING: Reference contains illegal characters '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "'");
            WriteCrLf(0);
            SetSymbol(ExitType, "W");
        }
        else
        {
            len = SymbolLength(ReferenceString);
            if (len > 6)
            {
                WriteString(0, "WARNING: Reference is too long '");
                WriteSymbol(0, ReferenceString);
                WriteString(0, "', truncated to ");
                WriteSymbol(0, ReferenceStr);
                WriteCrLf(0);

                SetSymbol(ExitType, "W");
            }
        }
        WriteSymbol(1, ReferenceStr);
        WriteString(1, " ");

        /* if the ModuleName is empty, then use the PartName */
        len = SymbolLength(ModuleName);
        if (len > 0)
        {
            WriteSymbol(1, ModuleName);
        }
        else
        {
            WriteSymbol(1, PartName);
        }
        WriteCrLf(1);
    }

/*    WriteString(1, "*END*");   */
    WriteCrLf(1);

    SetCharSet("~!#$%_-=+|/.:;<>0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

/*    WriteString(1, "*PADS-PCB*");
    WriteCrLf(1);                   */
    WriteString(1, "*NET*");
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;
    int legal;
    int len;

    net_items = 0;
    WriteString(1, "*SIGNAL* ");

    i = GetStandardSymbol(TypeCode);

    CopySymbol(SignalNameString, SignalNameStr);
    legal = SymbolInCharSet(SignalNameStr);
    if (legal != 1)
    {
        /* illegal character in the node name */
        WriteString(0, "WARNING: Name contains illegal characters '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        i = CompareSymbol(ExitType, Error);
        if (i != 0) { SetSymbol(ExitType, "W"); }

        i = 'N';
    }

    if (i == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal("-");
            SetNumberWidth(5);
            CopySymbol(LocalSignal, SignalNameStr);
        }
    }

    len = SymbolLength(SignalNameStr);
    if (len > 47)
    {
        WriteString(0, "WARNING: Name is too long '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        i = CompareSymbol(ExitType, Error);
        if (i != 0) { SetSymbol(ExitType, "W"); }

        i = 'N';
    }

    if (i == 'L')
    {
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'P')
    {
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'S')
    {
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'N')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
    }
    if (i == 'U')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
    }
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNet()
{
    int i;
    int legal;
    int len;

    if (net_items == 8)
    {
        WriteCrLf(1);
        net_items = 0;
    }

    if (net_items != 0)
    {
        WriteString(1, " ");
    }

    CopySymbol(ReferenceString, ReferenceStr);
    i = SymbolLength(ReferenceString);
    if (i > 6)
    {
        PadSpaces(ReferenceStr, 6);
    }
    WriteSymbol(1, ReferenceStr);
    WriteString(1, ".");
    WriteSymbol(1, PinNumberString);

    net_items = net_items + 1;
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

ProcessFieldStrings()
{
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    WriteString(1, "*END*");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


