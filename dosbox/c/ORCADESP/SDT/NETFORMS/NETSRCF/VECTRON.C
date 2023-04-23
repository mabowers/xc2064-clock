#include "format.h"

static char *switches = "ORSWITCHES \
/2\n \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.20   01-JUN-94\"";

/*
 *  Version 1.20
 *      - Changed ERROR messages to WARNINGS.
 *
 *  Version 1.10
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings can contain up to 8 characters
 *      - Node Names can contain up to 12 characters
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All characters are legal
 */

static int sw_L;

static int net_items;
static int len;
static int Error, ReferenceStr, SignalNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Error = AddSymbol("Error");
    SetSymbol(Error, "E");

    ReferenceStr = AddSymbol("ReferenceStr");
    SignalNameStr = AddSymbol("SignalNameStr");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    while (LoadInstance())
    {
        CopySymbol(ReferenceString, ReferenceStr);
        len = SymbolLength(ReferenceString);
        if (len > 8)
        {
            WriteString(0, "WARNING: Reference is too long '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, ReferenceStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }

        PadSpaces(ReferenceStr, 8);
        WriteSymbol(2, ReferenceStr);
        WriteString(2, " ");

        len = SymbolLength(ModuleName);
        /* use the Part Name if the Module Name is not present */
        if (len > 0)
        {
            WriteSymbol(2, ModuleName);
        }
        else
        {
            WriteSymbol(2, PartName);
        }
        WriteCrLf(2);
    }
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;

    net_items = 0;
    WriteString(1, "*");

    CopySymbol(SignalNameString, SignalNameStr);

    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal(".");
            SetNumberWidth(5);
            CopySymbol(LocalSignal, SignalNameStr);
        }
    }

    len = SymbolLength(SignalNameStr);
    if (len > 12)
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
        PadSpaces(SignalNameStr, 12);
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'P')
    {
        PadSpaces(SignalNameStr, 12);
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'S')
    {
        PadSpaces(SignalNameStr, 12);
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'N')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "      ");
    }
    if (i == 'U')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "      ");
    }
    return 0;
}

/*-----------------------*/

WriteNet()
{
    if (net_items == 8)
    {
        WriteCrLf(1);
        WriteString(1, "             ");

        net_items = 0;
    }
    WriteString(1, " ");

    len = SymbolLength(ReferenceString);
    if (len > 8)
    {
        CopySymbol(ReferenceString, ReferenceStr);
        PadSpaces(ReferenceStr, 8);
        WriteSymbol(1, ReferenceStr);
    }
    else
    {
        WriteSymbol(1, ReferenceString);
    }
    WriteString(1, " ");
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
    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


