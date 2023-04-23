#include "format.h"

static char *switches = "ORSWITCHES \
/2\n \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.20   01-JUN-94\"";
/*
 *  Version 1.20
 *      Changed ERROR messages to WARNINGS.
 *
 *  Version 1.10
 *      - Corrected handling of /L switch when more than 5 pins in a net
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are limited to 19 characters
 *      - Module Names are limited to 19 characters
 *      - Reference Strings are limited to 19 characters
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
static int PartNameStr, ModuleNameStr, ReferenceStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    /* used to pad their standard symbol namesakes with spaces */
    PartNameStr = AddSymbol("PartNameStr");
    ModuleNameStr = AddSymbol("ModuleNameStr");
    ReferenceStr = AddSymbol("ReferenceStr");

    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    WriteString(2, "# OrCAD Formatted Netlist for MENTOR Board Station V6");
    WriteCrLf(2);
    WriteString(2, "# Reference         .... Value Field         Module Field");
    WriteCrLf(2);

    while (LoadInstance())
    {
        CopySymbol(ReferenceString, ReferenceStr);
        PadSpaces(ReferenceStr, 19);
        len = SymbolLength(ReferenceStr);
        if (len > 19)
        {
            WriteString(0, "WARNING: Reference is too long '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, ReferenceStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }
        WriteSymbol(2, ReferenceStr);
        WriteString(2, " PART ");

        CopySymbol(PartName, PartNameStr);
        PadSpaces(PartNameStr, 19);
        len = SymbolLength(PartName);
        if (len > 19)
        {
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, PartName);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, PartNameStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }
        WriteSymbol(2, PartNameStr);
        WriteString(2, " ");

        /* if the ModuleName is empty, then use the PartName */
        len = SymbolLength(ModuleName);
        if (len > 0)
        {
            CopySymbol(ModuleName, ModuleNameStr);
            PadSpaces(ModuleNameStr, 19);

            if (len > 19)
            {
                WriteString(0, "WARNING: Name is too long '");
                WriteSymbol(0, ModuleName);
                WriteString(0, "', truncated to ");
                WriteSymbol(0, ModuleNameStr);
                WriteCrLf(0);

                SetSymbol(ExitType, "W");
            }
            WriteSymbol(2, ModuleNameStr);
        }
        else
        {
            len = SymbolLength(PartName);
            if (len > 19)
            {
                WriteSymbol(2, PartNameStr);
            }
            else
            {
                WriteSymbol(2, PartName);
            }
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
    WriteString(1, "NET '");

    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal("_");
            WriteSymbol(1, LocalSignal);
            SetNumberWidth(5);
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
    WriteString(1, "'");
    return 0;
}

/*-----------------------*/

WriteNet()
{
    int i;

    if (net_items == 5)
    {
        WriteCrLf(1);
        WriteString(1, "NET '");

        i = GetStandardSymbol(TypeCode);
        if (i == 'L')
        {
            if (sw_L == 0)
                { WriteSymbol(1, LocalSignal); }
            else
                { WriteSymbol(1, SignalNameString); }
        }
        if (i == 'P')
        {
            WriteSymbol(1, SignalNameString);
        }
        if (i == 'S')
        {
            WriteSymbol(1, SignalNameString);
        }
        if (i == 'U')
        {
            WriteString(1, "N");
            WriteSymbol(1, NetNumber);
        }
        if (i == 'N')
        {
            WriteString(1, "N");
            WriteSymbol(1, NetNumber);
        }
        WriteString(1, "' ");

        net_items = 0;
    }
    else
    {
        WriteString(1, " ");
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