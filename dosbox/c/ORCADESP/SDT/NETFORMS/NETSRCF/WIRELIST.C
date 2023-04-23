#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n \
/P Do not output pin numbers for Grid Array parts\n \
/A Abbreviate label descriptions\n";

static char *ver = "VERSION=\"1.10a  01-OCT-91\"";

/*
 *  Version 1.20a
 *      - Changed ERRORS to WARNINGS.
 *  Version 1.10a
 *      - Added /P switch.  Grid array parts have pin numbers that are not
 *        entirely numeric (e.g. 'A14').  If this format is to be used by
 *        a machine, then these 'non-numeric' values might cause problems.
 *      - Added /A switch (Equivalent to /B switch in SDT III.  We cannot
 *        use /B since iform uses it already)
 *      - Added Version message
 *      - Corrected trapping of names that are too long
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names can contain up to 29 characters
 *      - Reference Strings can contain up to 9 characters
 *      - Node Names are not checked for length
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Numbers are limited to 7 characters
 *      - Pin Names are limited to 15 characters.
 *
 *  Characters:
 *      - All characters are legal for Part Names, Module Names, and Reference Strings
 *      - Legal characters for Pin Numbers are { '0..9' } except if /P
 *        switch is set, which will allow all characters
 */

static int sw_L;
static int sw_P;
static int sw_A;

static int pin_type;
static int len;
static int Str, ReferenceStr, PartNameStr, PinNumberStr, PinNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Initialize");
    WriteCrLf(0);

    ReferenceStr = AddSymbol("ReferenceStr");
    PartNameStr = AddSymbol("PartNameStr");
    PinNumberStr = AddSymbol("PinNumberStr");
    PinNameStr = AddSymbol("PinNameStr");

    SetPinMap(0, "Input          ");
    SetPinMap(1, "BiDirectional  ");
    SetPinMap(2, "Output         ");
    SetPinMap(3, "Open Collector ");
    SetPinMap(4, "Passive        ");
    SetPinMap(5, "Hi-Z           ");
    SetPinMap(6, "Open Emitter   ");
    SetPinMap(7, "Power          ");

    SetCharSet("0123456789");
    SetNumberWidth(5);

    Str = AddSymbol("Str");

    WriteString(1, "Wire List");
    WriteCrLf(1);
    WriteCrLf(1);

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
    sw_P = SwitchIsSet("P");
    sw_A = SwitchIsSet("A");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    WriteString(1, "<<< Component List >>>");
    WriteCrLf(1);

    while (LoadInstance())
    {
        CopySymbol(PartName, PartNameStr);
        PadSpaces(PartNameStr, 29);
        len = SymbolLength(PartName);
        if (len > 29)
        {
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, PartName);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, PartNameStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }
        WriteSymbol(1, PartNameStr);
        WriteString(1, " ");

        CopySymbol(ReferenceString, ReferenceStr);
        PadSpaces(ReferenceStr, 9);
        len = SymbolLength(ReferenceString);
        if (len > 9)
        {
            WriteString(0, "WARNING: Reference is too long '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, ReferenceStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }
        WriteSymbol(1, ReferenceStr);
        WriteString(1, " ");

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

    WriteCrLf(1);
    WriteString(1, "<<< Wire List >>>");
    WriteCrLf(1);
    WriteCrLf(1);
    WriteString(1, "  NODE  REFERENCE    PIN #   PIN NAME  ");
    WriteString(1, "         PIN TYPE       PART VALUE");
    WriteCrLf(1);
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;

    WriteString(1, "[");
    WriteSymbol(1, NetNumber);
    WriteString(1, "] ");

    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        SetNumberWidth(1);
        if (sw_L == 0)
        {
            MakeLocalSignal("_");
            WriteSymbol(1, LocalSignal);
        }
        else
        {
            WriteSymbol(1, SignalNameString);
        }

        WriteString(1, " (");
        if (sw_A == 0) {
            WriteString(1, "local to sheet ");
        }
        WriteSymbol(1, SheetNumber);
        WriteString(1, ")");
        SetNumberWidth(5);
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
    int i;

    WriteString(1, "        ");

    CopySymbol(ReferenceString, ReferenceStr);
    PadSpaces(ReferenceStr, 9);
    WriteSymbol(1, ReferenceStr);
    WriteString(1, "       ");

    /* don't check for legal characters unless /P is set */
    CopySymbol(PinNumberString, PinNumberStr);
    if (sw_P == 1)
        { i = SymbolInCharSet(PinNumberStr); }
    else
        { i = 1; }

    if (i == 0)
    {
        WriteString(1, "        ");
    }
    else
    {
        PadSpaces(PinNumberStr, 7);
        len = SymbolLength(PinNumberString);
        if (len > 7)
        {
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, PinNumberString);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, PinNumberStr);
            WriteCrLf(0);
    
            SetSymbol(ExitType, "W");
        }
        WriteSymbol(1, PinNumberStr);
        WriteString(1, " ");
    }

    CopySymbol(PinNameString, PinNameStr);
    PadSpaces(PinNameStr, 15);
    len = SymbolLength(PinNameString);
    if (len > 15)
    {
        WriteString(0, "WARNING: Name is too long '");
        WriteSymbol(0, PinNameString);
        WriteString(0, "', truncated to ");
        WriteSymbol(0, PinNameStr);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");
    }
    WriteSymbol(1, PinNameStr);
    WriteString(1, " ");

    i = GetStandardSymbol(PinType);
    WriteMap(1, i);

    WriteSymbol(1, PartName);
    WriteCrLf(1);
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