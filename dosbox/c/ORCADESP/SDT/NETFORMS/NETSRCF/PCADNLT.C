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
 *      - All characters are legal for Part Names, Module Names, and Reference Strings
 *      - Legal characters for Node Names are {'+-_$', '0..9', 'a..z', 'A..Z'}
 */

static int sw_L;

static int net_items;
static int len;
static int ModuleNameStr, PrevModuleNameStr, PrevPartNameStr, SignalNameStr,
           Str;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    ModuleNameStr = AddSymbol("ModuleNameStr");
    PrevModuleNameStr = AddSymbol("PrevModuleNameStr");
    PrevPartNameStr = AddSymbol("PrevPartNameStr");
    SignalNameStr = AddSymbol("SignalNameStr");
    Str = AddSymbol("Str");

    SetCharSet("+-_$0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    WriteString(1, "% ");
    CopySymbol(TitleString, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revised: ");
    WriteSymbol(1, DateString);
    WriteCrLf(1);
    WriteString(1, "% ");
    CopySymbol(DocumentNumber, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revision: ");
    WriteSymbol(1, Revision);
    WriteCrLf(1);
    WriteString(1, "% ");
    WriteSymbol(1, Organization);
    WriteCrLf(1);
    WriteString(1, "% ");
    WriteSymbol(1, AddressLine1);
    WriteCrLf(1);
    WriteString(1, "% ");
    WriteSymbol(1, AddressLine2);
    WriteCrLf(1);
    WriteString(1, "% ");
    WriteSymbol(1, AddressLine3);
    WriteCrLf(1);
    WriteString(1, "% ");
    WriteSymbol(1, AddressLine4);
    WriteCrLf(1);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    int first;
    int temp;

    WriteString(1, "BOARD = ORCAD.PCB;");
    WriteCrLf(1);
    WriteCrLf(1);

    WriteString(1, "PARTS");
    WriteCrLf(1);

    /* first pass through, there is no previous module defined */
    first = 1;
    while (LoadInstance())
    {
        /* use the Part Name if there is no ModuleName */
        len = SymbolLength(ModuleName);
        if (len < 1)
        {
            CopySymbol(PartName, ModuleNameStr);
        }
        else
        {
            CopySymbol(ModuleName, ModuleNameStr);
        }

        /* if same as previous, then just continue */
        temp = CompareSymbol(ModuleNameStr, PrevModuleNameStr);
        if (first == 1) { temp = 1; }

        CopySymbol(ModuleNameStr, PrevModuleNameStr);
        if (temp == 0)
        {
            WriteString(1, ",   % ");
            WriteSymbol(1, PrevPartNameStr);
            WriteCrLf(1);
            WriteString(1, "                  ");
        }
        else
        {
            if (first == 0)
            {
                WriteString(1, ";   % ");
                WriteSymbol(1, PrevPartNameStr);
                WriteCrLf(1);
            }

            len = SymbolLength(ModuleNameStr);
            if (len < 15)
            {
                PadSpaces(ModuleNameStr, 15);
            }
            WriteSymbol(1, ModuleNameStr);
            WriteString(1, " = ");
        }

        WriteSymbol(1, ReferenceString);

        CopySymbol(PartName, PrevPartNameStr);
        first = 0;
    }
    if (first == 0)
    {
        /* at least one part was written */
        WriteString(1, ";   % ");
        WriteSymbol(1, PrevPartNameStr);
        WriteCrLf(1);
    }
    WriteCrLf(1);

    WriteString(1, "NETS");
    WriteCrLf(1);
    WriteCrLf(1);
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
            MakeLocalSignal("_");
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

        SetSymbol(ExitType, "W");
        i = 'N';
    }

    if (i == 'L')
    {
        PadSpaces(SignalNameStr, 8);
        WriteSymbol(1, SignalNameStr);
        WriteString(1, "  =");
    }
    if (i == 'P')
    {
        PadSpaces(SignalNameStr, 8);
        WriteSymbol(1, SignalNameStr);
        WriteString(1, "  =");
    }
    if (i == 'S')
    {
        PadSpaces(SignalNameStr, 8);
        WriteSymbol(1, SignalNameStr);
        WriteString(1, "  =");
    }
    if (i == 'N')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "    =");
    }
    if (i == 'U')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "    =");
    }
    return 0;
}

/*-----------------------*/

WriteNet()
{
    if (net_items == 4)
    {
        WriteCrLf(1);
        WriteString(1, "           ");

        net_items = 0;
    }
    WriteString(1, " ");

    WriteSymbol(1, ReferenceString);
    WriteString(1, "/");
    WriteSymbol(1, PinNumberString);

    net_items = net_items + 1;
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    WriteString(1, " ;");
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