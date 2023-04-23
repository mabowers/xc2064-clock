#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.11   13-DEC-93\"";
/*
 *  Version 1.11
 *      - Changed ERROR messages to WARNINGS
 *  Version 1.10
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names can be up to 16 characters in length
 *      - Module Names can be up to 16 characters in length
 *      - Reference Strings can be up to 16 characters in length
 *      - Node Names can be up to 16 characters in length
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - Note that the Reference and ModuleName must be uppercase only
 *      - All ASCII keyboard characters are legal except { '()[],- ' }
 */

static int sw_L;

static int len;
static int Error, ReferenceStr, ModuleNameStr, PartNameStr, SignalNameStr,
           Str;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Error = AddSymbol("Error");
    SetSymbol(Error, "E");

    ReferenceStr = AddSymbol("ReferenceStr");
    ModuleNameStr = AddSymbol("ModuleNameStr");
    PartNameStr = AddSymbol("PartNameStr");
    SignalNameStr = AddSymbol("SignalNameStr");

    SetCharSet("+_/\\{}<>.`~!@#$%^&*=?0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    Str = AddSymbol("Str");

    CopySymbol(TitleString, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revised: ");
    WriteSymbol(1, DateString);
    WriteCrLf(1);
    CopySymbol(DocumentNumber, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revision: ");
    WriteSymbol(1, Revision);
    WriteCrLf(1);
    WriteSymbol(1, Organization);
    WriteCrLf(1);
    WriteSymbol(1, AddressLine1);
    WriteCrLf(1);
    WriteSymbol(1, AddressLine2);
    WriteCrLf(1);
    WriteSymbol(1, AddressLine3);
    WriteCrLf(1);
    WriteSymbol(1, AddressLine4);
    WriteCrLf(1);
    WriteCrLf(1);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    while (LoadInstance())
    {
        WriteString(1, "[");
        WriteCrLf(1);

        CopySymbol(ReferenceString, ReferenceStr);
        len = SymbolLength(ReferenceString);
        if (len > 16)
        {
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, ReferenceStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
            PadSpaces(ReferenceStr, 16);
        }
        WriteSymbol(1, ReferenceStr);
        WriteCrLf(1);

        len = SymbolLength(ModuleName);
        if (len > 0)
        {
            CopySymbol(ModuleName, ModuleNameStr);
        }
        else
        {
            /* No ModuleName, use the PartName */
            CopySymbol(PartName, ModuleNameStr);
        }

        len = SymbolLength(ModuleNameStr);
        if (len > 16)
        {
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, ModuleNameStr);
            WriteString(0, "', truncated to ");
            PadSpaces(ModuleNameStr, 16);
            WriteSymbol(0, ModuleNameStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }
        WriteSymbol(1, ModuleNameStr);
        WriteCrLf(1);

        CopySymbol(PartName, PartNameStr);
        len = SymbolLength(PartName);
        if (len > 16)
        {
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, PartName);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, PartNameStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
            PadSpaces(PartNameStr, 16);
        }
        WriteSymbol(1, PartNameStr);
        WriteCrLf(1);

        WriteCrLf(1);
        WriteCrLf(1);
        WriteCrLf(1);
        WriteString(1, "]");
        WriteCrLf(1);
    }
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;

    WriteString(1, "(");
    WriteCrLf(1);

    CopySymbol(SignalNameString, SignalNameStr);

    i = GetStandardSymbol(TypeCode);
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
    if (len > 16)
    {
        WriteString(0, "WARNING: Name is too long '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        i = CompareSymbol(ExitType, Error);
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
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNet()
{
    CopySymbol(ReferenceString, ReferenceStr);
    len = SymbolLength(ReferenceString);
    if (len > 16)
    {
        PadSpaces(ReferenceStr, 16);
    }
    WriteSymbol(1, ReferenceStr);
    WriteString(1, ",");
    WriteSymbol(1, PinNumberString);
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    WriteString(1, ")");
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