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

static int net_items;
static int len;
static int ModuleNameStr, PartNameStr, SignalNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    ModuleNameStr = AddSymbol("ModuleNameStr");
    PartNameStr = AddSymbol("PartNameStr");
    SignalNameStr = AddSymbol("SignalNameStr");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    WriteString(1, "%PART");
    WriteCrLf(1);

    while (LoadInstance())
    {
        CopySymbol(ModuleName, ModuleNameStr);
        len = SymbolLength(ModuleName);
        if (len > 15)
        {
            WriteSymbol(1, ModuleName);
            WriteString(1, "  ");
        }
        else
        {
            /* if the ModuleName is empty, then output the PartName */
            if (len > 0)
            {
                PadSpaces(ModuleNameStr, 16);
                WriteSymbol(1, ModuleNameStr);
            }
            else
            {
                CopySymbol(PartName, PartNameStr);
                len = SymbolLength(PartName);
                if (len > 15)
                {
                    WriteSymbol(1, PartName);
                    WriteString(1, "  ");
                }
                else
                {
                    PadSpaces(PartNameStr, 16);
                    WriteSymbol(1, PartNameStr);
                }
            }
        }

        WriteSymbol(1, ReferenceString);
        WriteCrLf(1);
    }

    WriteString(1, "%NET");
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
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

    if (i == 'L')
    {
        len = SymbolLength(SignalNameStr);
        if (len > 23)
        {
            WriteSymbol(1, SignalNameStr);
            WriteString(1, " ");
        }
        else
        {
            PadSpaces(SignalNameStr, 23);
            WriteSymbol(1, SignalNameStr);
        }
    }
    if (i == 'P')
    {
        len = SymbolLength(SignalNameStr);
        if (len > 23)
        {
            WriteSymbol(1, SignalNameStr);
            WriteString(1, " ");
        }
        else
        {
            PadSpaces(SignalNameStr, 23);
            WriteSymbol(1, SignalNameStr);
        }
    }
    if (i == 'S')
    {
        len = SymbolLength(SignalNameStr);
        if (len > 23)
        {
            WriteSymbol(1, SignalNameStr);
            WriteString(1, " ");
        }
        else
        {
            PadSpaces(SignalNameStr, 23);
            WriteSymbol(1, SignalNameStr);
        }
    }
    if (i == 'N')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "                 ");
    }
    if (i == 'U')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "                 ");
    }
    return 0;
}

/*-----------------------*/

WriteNet()
{
    if (net_items == 4)
    {
        WriteCrLf(1);
        WriteString(1, "*                      ");

        net_items = 0;
    }
    WriteString(1, " ");

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
    WriteString(1, "$");

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


