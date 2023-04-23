#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are limited to 19 characters
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal
 */

static int sw_L;

static int net_items;
static int SignalNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    SignalNameStr = AddSymbol("SignalNameStr");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int len;
    int i;

    net_items = 0;
    CopySymbol(SignalNameString, SignalNameStr);

    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal("/");
            SetNumberWidth(5);
            CopySymbol(LocalSignal, SignalNameStr);
        }
    }

    SetNumberWidth(4);
    WriteSymbol(1, NetNumber);
    WriteString(1, " ");
    SetNumberWidth(5);

    len = SymbolLength(SignalNameStr);
    if (len > 20)
    {
        WriteString(0, "WARNING: Name is too long '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");
        i = 'N';
    }

    if (i == 'L')
    {
        PadSpaces(SignalNameStr, 20);
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'P')
    {
        PadSpaces(SignalNameStr, 20);
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'S')
    {
        PadSpaces(SignalNameStr, 20);
        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'N')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "              ");
    }
    if (i == 'U')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "              ");
    }
    WriteString(1, " ");
    return 0;
}

/*-----------------------*/

WriteNet()
{
    if (net_items == 4)
    {
        WriteCrLf(1);
        WriteString(1, "                          ");

        net_items = 0;
    }
    else
    {
        if (net_items != 0) { WriteString(1, "  "); }
    }

    WriteSymbol(1, ReferenceString);
    WriteString(1, "-");
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