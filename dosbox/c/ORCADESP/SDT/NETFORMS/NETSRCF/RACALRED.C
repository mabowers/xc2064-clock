#include "format.h"

static char *switches = "ORSWITCHES \
/2\n \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.10b  22-JAN-92\"";

/*  Version 1.10b
 *      - Updated 'WriteNet' (again!) so that lines with exactly
 *        8 entries are handled properly
 *      - Updated 'WriteNet' so it properly handles long lines
 *      - Added Version message
 *      - Added .COM before .REF to component file
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
static int ReferenceStr, PrevRefStr, PrevNumStr, Str;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    ReferenceStr = AddSymbol("ReferenceStr");
    SetNumberWidth(5);

    PrevRefStr = AddSymbol("PrevRefStr");
    PrevNumStr = AddSymbol("PrevNumStr");

    Str = AddSymbol("Str");

    WriteString(1, ".PCB");
    WriteCrLf(1);
    WriteString(1, ".REM ");
    CopySymbol(TitleString, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revised: ");
    WriteSymbol(1, DateString);
    WriteCrLf(1);
    WriteString(1, ".REM ");
    CopySymbol(DocumentNumber, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revision: ");
    WriteSymbol(1, Revision);
    WriteCrLf(1);
    WriteString(1, ".REM ");
    WriteSymbol(1, Organization);
    WriteCrLf(1);
    WriteString(1, ".REM ");
    WriteSymbol(1, AddressLine1);
    WriteCrLf(1);
    WriteString(1, ".REM ");
    WriteSymbol(1, AddressLine2);
    WriteCrLf(1);
    WriteString(1, ".REM ");
    WriteSymbol(1, AddressLine3);
    WriteCrLf(1);
    WriteString(1, ".REM ");
    WriteSymbol(1, AddressLine4);
    WriteCrLf(1);
    WriteString(1, ".CON");
    WriteCrLf(1);
    WriteString(1, ".COD 2");
    WriteCrLf(1);
    WriteCrLf(1);

    WriteString(2, ".PCB");
    WriteCrLf(2);
    WriteString(2, ".REM ");
    CopySymbol(TitleString, Str);
    PadSpaces(Str, 46);
    WriteSymbol(2, Str);
    WriteString(2, "Revised: ");
    WriteSymbol(2, DateString);
    WriteCrLf(2);
    WriteString(2, ".REM ");
    CopySymbol(DocumentNumber, Str);
    PadSpaces(Str, 46);
    WriteSymbol(2, Str);
    WriteString(2, "Revision: ");
    WriteSymbol(2, Revision);
    WriteCrLf(2);
    WriteString(2, ".REM ");
    WriteSymbol(2, Organization);
    WriteCrLf(2);
    WriteString(2, ".REM ");
    WriteSymbol(2, AddressLine1);
    WriteCrLf(2);
    WriteString(2, ".REM ");
    WriteSymbol(2, AddressLine2);
    WriteCrLf(2);
    WriteString(2, ".REM ");
    WriteSymbol(2, AddressLine3);
    WriteCrLf(2);
    WriteString(2, ".REM ");
    WriteSymbol(2, AddressLine4);
    WriteCrLf(2);
    WriteString(2, ".COM");
    WriteCrLf(2);
    WriteString(2, ".REF");
    WriteCrLf(2);
    WriteCrLf(2);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    while (LoadInstance())
    {
        WriteString(2, ".REM ");
        WriteSymbol(2, PartName);
        WriteCrLf(2);

        len = SymbolLength(ReferenceString);
        if (len > 4)
        {
            WriteSymbol(2, ReferenceString);
        }
        else
        {
            CopySymbol(ReferenceString, ReferenceStr);
            PadSpaces(ReferenceStr, 4);
            WriteSymbol(2, ReferenceStr);
        }
        WriteString(2, " ");

        /* use the PartName if there is no ModuleName */
        len = SymbolLength(ModuleName);
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
    WriteString(2, ".EOD");
    WriteCrLf(2);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;

    net_items = 0;
    WriteString(1, ".REM ");

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
        
        WriteString(1, " (local to sheet ");
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
    if (net_items == 8)
    {
        WriteCrLf(1);
        WriteSymbol(1, PrevRefStr);
        WriteString(1, " ");
        WriteSymbol(1, PrevNumStr);
        WriteString(1, " ");

        net_items = 0;
    }

    CopySymbol(ReferenceString, PrevRefStr);
    CopySymbol(PinNumberString, PrevNumStr);

    WriteSymbol(1, ReferenceString);
    WriteString(1, " ");
    WriteSymbol(1, PinNumberString);
    WriteString(1, " ");

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
    WriteString(1, ".EOD");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


