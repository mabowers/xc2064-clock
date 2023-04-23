#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Added 'Time Stamp - ' to the comment header for COMPNET in PCB II
 *      - Added code for outputting missing pins on a part
 *      - Added length checks for Part Names, Module Names,
 *        Reference Strings and Pin Names
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are limited to 8 characters
 *      - Module Names are limited to 8 characters
 *      - Reference Strings are limited to 8 characters
 *      - Node Names are limited to 8 characters
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *      - Pin Names are limited to 4 charactes
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal except { '(){} ' }
 */

static int sw_L;

static int prevpin;
static int currpin;

static int type;
static int new_type;
static int legal;
static int len;

static int Str, TStr, PinNumberStr, SignalNameStr, PartNameStr, ModuleNameStr,
           ReferenceStr;
static int ALL, NODES;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Str = AddSymbol("Str");
    TStr = AddSymbol("TStr");
    PinNumberStr = AddSymbol("PinNumberStr");
    SignalNameStr = AddSymbol("SignalNameStr");
    PartNameStr = AddSymbol("PartNameStr");
    ModuleNameStr = AddSymbol("ModuleNameStr");
    ReferenceStr = AddSymbol("ReferenceStr");

    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

    WriteString(1, "( { OrCAD/PCB II Netlist Format");
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
    WriteString(1, "Time Stamp - }");
    WriteCrLf(1);

    SetCharSet("~`!@#$%^&*_-+=[]|\\':;/><.0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    CreatePartDataBase();
    return 0;
}

/*-----------------------*/

ValidateNodeInfo()
{
    /* globals type, legal, and len are set */
    type = GetStandardSymbol(TypeCode);
    new_type = type;

    CopySymbol(SignalNameString, SignalNameStr);
    legal = SymbolInCharSet(SignalNameStr);
    if (legal != 1) { new_type = 'N'; }

    if (new_type == 'L')
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
    if (len > 8) { new_type = 'N'; }
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    ValidateNodeInfo();

    if (legal != 1)
    {
        /* illegal character in the node name */
        WriteString(0, "WARNING: Name contains illegal characters '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");
    }

    if (len > 8)
    {
        WriteString(0, "WARNING: Name is too long '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        SetSymbol(ExitType, "W");
    }

    AddSignalName();
    return 0;
}

/*-----------------------*/

WriteNet()
{
    RecordNode();
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

SetCurrentPinNumber()
{
    int i;
    int len;
    int ch;

    SetCharSet("0123456789");

    currpin = 0;
    CopySymbol(PinNumberString, PinNumberStr);

    i = SymbolInCharSet(PinNumberStr);
    if (i == 1) {
        len = SymbolLength(PinNumberString);

        i = 0;
        do {
            ch = GetSymbolChar(i, PinNumberStr);
            currpin = (currpin * 10) + (ch - 48);

            i = i + 1;
        } while (i < len);
    }

    SetCharSet("~`!@#$%^&*_-+=[]|\\':;/><.0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    int net;

    net = 0;
    SetFirst(ALL);
    do {
        CopySymbol(ReferenceString, Str);
        LoadFieldString(Str);

        WriteString(1, " ( ");
        CopySymbol(TimeStamp, TStr);
        FToUpper(TStr);
        WriteSymbol(1, TStr);
        WriteString(1, " ");

        CopySymbol(ModuleName, ModuleNameStr);
        len = SymbolLength(ModuleName);
        if (len > 8)
        {
            PadSpaces(ModuleNameStr, 8);
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, ModuleName);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, ModuleNameStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }

        CopySymbol(PartName, PartNameStr);
        len = SymbolLength(PartName);
        if (len > 8)
        {
            PadSpaces(PartNameStr, 8);
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, PartName);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, PartNameStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }

        CopySymbol(ReferenceString, ReferenceStr);
        len = SymbolLength(ReferenceString);
        if (len > 8)
        {
            PadSpaces(ReferenceStr, 8);
            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, ReferenceStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }


        len = SymbolLength(ModuleName);
        if (len > 0)
            { WriteSymbol(1, ModuleNameStr); }
        else
            { WriteSymbol(1, PartNameStr); }
        WriteString(1, " ");

        WriteSymbol(1, ReferenceStr);
        WriteString(1, " ");
        WriteSymbol(1, PartNameStr);
        WriteCrLf(1);

        prevpin = 0;
        do {
            /* write out any pins between the previous one and the current one */
            SetCurrentPinNumber();

            prevpin = prevpin + 1;
            if (currpin > prevpin) {
                if (currpin != 0) {
                    SetNumberWidth(1);

                    while (currpin > prevpin) {
                        WriteString(1, "  ( ");

                        WriteInteger(1, prevpin);
                        WriteString(1, " ?");
                        net = net + 1;
                        WriteInteger(1, net);
                        WriteString(1, " )");
                        WriteCrLf(1);

                        prevpin = prevpin + 1;
                    }

                    SetNumberWidth(5);
                }
            }
            prevpin = currpin;

            ValidateNodeInfo();

            WriteString(1, "  ( ");
            CopySymbol(PinNumberString, PinNumberStr);
            len = SymbolLength(PinNumberStr);
            if (len > 4)
            {
                PadSpaces(PinNumberStr, 4);
                WriteString(0, "WARNING: Name is too long '");
                WriteSymbol(0, PinNumberString);
                WriteString(0, "', truncated to ");
                WriteSymbol(0, PinNumberStr);
                WriteCrLf(0);

                SetSymbol(ExitType, "W");
            }
            WriteSymbol(1, PinNumberStr);
            WriteString(1, " ");

            if (new_type == 'L')
            {
                WriteSymbol(1, SignalNameStr);
            }
            if (new_type == 'P')
            {
                WriteSymbol(1, SignalNameStr);
            }
            if (new_type == 'S')
            {
                WriteSymbol(1, SignalNameStr);
            }
            if (new_type == 'N')
            {
                WriteString(1, "N");
                WriteSymbol(1, NetNumber);
            }
            if (new_type == 'U')
            {
                WriteString(1, "?");
                net = net + 1;
                SetNumberWidth(1);
                WriteInteger(1, net);
                SetNumberWidth(5);
            }
            WriteString(1, " )");
            WriteCrLf(1);
        } while (SetNext(NODES));

        WriteString(1, " )");
        WriteCrLf(1);

    } while (SetNext(ALL));

    WriteString(1, ")");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


