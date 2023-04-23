#include "format.h"

static char *switches = "ORSWITCHES \
/2\n \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.20   01-JUN-94\"";
/*
 *  Version 1.20
 *      - Changed ERROR messages to WARNINGS.
 *  Version 1.10
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names can contain up to 19 characters
 *      - Module Names can contain up to 19 characters
 *      - Reference Strings can contain up to 19 characters
 *      - Node Names can contain up to 8 characters (see legal characters below)
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All characters are legal for Part Names, Module Names, and Reference Strings
 *      - Legal characters for Node Names are { '+-', '0..9', 'a..z', 'A..Z' }
 */

static int sw_L;

static int net_items;
static int len;
static int Error, PartNameStr, ModuleNameStr, ReferenceStr, SignalNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Error = AddSymbol("Error");
    SetSymbol(Error, "E");

    /* used to pad their standard symbol namesakes with spaces */
    PartNameStr = AddSymbol("PartNameStr");
    ModuleNameStr = AddSymbol("ModuleNameStr");
    ReferenceStr = AddSymbol("ReferenceStr");
    SignalNameStr = AddSymbol("SignalNameStr");

    /* set up the legal character set */
    SetCharSet("+-0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    while (LoadInstance())
    {
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

        CopySymbol(ReferenceString, ReferenceStr);
        PadSpaces(ReferenceStr, 19);
        len = SymbolLength(ReferenceString);
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
            PadSpaces(PartNameStr, 19);
            WriteSymbol(2, PartNameStr);
        }

        WriteString(2, " 000     000     0");
        WriteCrLf(2);
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
    if (len > 8)
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
    else
    {
        PadSpaces(SignalNameStr, 8);
    }
    WriteString(1, "/");

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
        WriteString(1, "  ");
    }
    if (i == 'U')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "  ");
    }
    return 0;
}

/*-----------------------*/

WriteNet()
{
    if (net_items == 8)
    {
        WriteString(1, ",");
        WriteCrLf(1);
        WriteString(1, "          ");

        net_items = 0;
    }
    else
    {
        WriteString(1, " ");
    }

    WriteSymbol(1, ReferenceString);
    WriteString(1, "(");
    WriteSymbol(1, PinNumberString);
    WriteString(1, ")");

    net_items = net_items + 1;
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    WriteString(1, ";");
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