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
 *      - Node Names can contain up to 20 characters (see legal characters below)
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All characters are legal for Part Names, Module Names, and Reference Strings
 *      - Legal characters for Node Names are {'/-', '0..9', 'a..z', 'A..Z' }
 */

static int sw_L;

static int net_items;
static int len;
static int SignalNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    SignalNameStr = AddSymbol("SignalNameStr");
    SetCharSet("/-0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    WriteString(1, ".SEARCH P,C");
    WriteCrLf(1);

    while (LoadInstance())
    {
        WriteString(1, ".DD  ");
        WriteSymbol(1, ReferenceString);
        WriteString(1, " ");

        len = SymbolLength(ModuleName);
        if (len > 0)
        {
            WriteSymbol(1, ModuleName);
        }
        else
        {
            /* if the ModuleName is empty, then output the PartName */
            WriteSymbol(1, PartName);
        }
        WriteCrLf(1);
    }
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;
    int legal;

    net_items = 0;
    i = GetStandardSymbol(TypeCode);

    WriteString(1, ".S,");

    CopySymbol(SignalNameString, SignalNameStr);
    legal = SymbolInCharSet(SignalNameStr);
    if (legal != 1)
    {
        WriteString(0, "WARNING: Name contains illegal characters '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");
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
    return 0;
}

/*-----------------------*/

WriteNet()
{
    if (net_items == 8)
    {
        WriteString(1, "!");
        WriteCrLf(1);
        WriteString(1, "       ");

        net_items = 0;
    }
    else
    {
        WriteString(1, ",");
    }

    WriteSymbol(1, ReferenceString);
    WriteString(1, ",");
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