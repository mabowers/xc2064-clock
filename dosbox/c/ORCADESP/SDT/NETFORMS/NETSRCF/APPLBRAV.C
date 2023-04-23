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
 *      - Node Names are not checked for length
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal
 */

static int sw_L;

static int len;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    while (LoadInstance())
    {
        WriteString(1, "*** Desig ");
        len = SymbolLength(ModuleName);
        /* if the ModuleName is empty, then output the PartName */
        if (len > 0)
        {
            WriteSymbol(1, ModuleName);
        }
        else
        {
            WriteSymbol(1, PartName);
        }
        WriteCrLf(1);
        WriteSymbol(1, ReferenceString);
        WriteCrLf(1);
    }
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;

    WriteString(1, "*** NET ");
    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal("-");
            SetNumberWidth(5);
            WriteSymbol(1, LocalSignal);
        }
        else
        {
            WriteSymbol(1, SignalNameString);
        }
    }
    if (i == 'P')
    {
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'S')
    {
        WriteSymbol(1, SignalNameString);
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
    WriteSymbol(1, ReferenceString);
    WriteString(1, " ");
    WriteSymbol(1, PinNumberString);
    WriteCrLf(1);
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

WriteNetListEnd()
{
    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}
