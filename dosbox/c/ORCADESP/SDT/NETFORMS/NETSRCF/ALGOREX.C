#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels";

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

static int net_items;   /* number of pins written for each net */
static int len;

static int VERSION;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    sw_L = SwitchIsSet("L");
    SetNumberWidth(5);
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
    int i;

    net_items = 0;

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
    if (net_items != 0)
    {
        WriteString(1, ",");
        WriteCrLf(1);
    }

    WriteString(1, "         ");
    WriteSymbol(1, ReferenceString);
    WriteString(1, " (");
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
    WriteString(1, ")-");
    WriteSymbol(1, PinNumberString);

    net_items = 1;
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