#include "format.h"

static char *switches = "ORSWITCHES ";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Added handling for 0 parts and/or 0 nets in a design
 *      - Added Version message
 *
 *  A generic dump utility for hierarchical netlist files
 *
 */

static int i;
static int Formatter;

/*------------------------*/

SheetPinMap()
{
    SetPinMap(0, "Unspecified    ");
    SetPinMap(1, "Output         ");
    SetPinMap(2, "Input          ");
    SetPinMap(3, "BiDirectional  ");
    SetPinMap(4, "Open Collector ");
    SetPinMap(5, "Passive        ");
    SetPinMap(6, "Hi-Z           ");
    SetPinMap(7, "Open Emitter   ");
    SetPinMap(8, "Power          ");
    return 0;
}

/*------------------------*/

PinPinMap()
{
    SetPinMap(0, "Input          ");
    SetPinMap(1, "BiDirectional  ");
    SetPinMap(2, "Output         ");
    SetPinMap(3, "Open Collector ");
    SetPinMap(4, "Passive        ");
    SetPinMap(5, "Hi-Z           ");
    SetPinMap(6, "Open Emitter   ");
    SetPinMap(7, "Power          ");
    return 0;
}

/*------------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    WriteString(1, "Initialize - hform");
    WriteCrLf(1);
    
    Formatter = AddSymbol("Formatter");
    MakeInstanceFile();
    return 0;
}

/*------------------------*/

PreFile()
{
    WriteCrLf(1);
    WriteString(1, "Processing - ");
    WriteCrLf(1);
    WriteSymbol(1, FileName);
    WriteCrLf(1);
    return 0;
}

/*------------------------*/

WriteNetNode()
{
    i = GetStandardSymbol(TypeCode);
    if (i == 'P')
    {
        WriteString(1, "Port   - ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'L')
    {
        WriteString(1, "Label  - ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'S')
    {
        WriteString(1, "Supply - ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'N')
    {
        WriteString(1, "Part   - ");
        WriteSymbol(1, PinNameString);
        WriteString(1, "  ");
        WriteSymbol(1, PinNumberString);
        WriteString(1, "  ");
        WriteSymbol(1, ReferenceString);
    }
    if (i == 'U')
    {
        WriteString(1, "Sheet  - ");
        WriteSymbol(1, PinNameString);
        WriteString(1, "  ");
        WriteSymbol(1, PinNumberString);
        WriteString(1, "  ");
        WriteSymbol(1, ReferenceString);
    }
    WriteCrLf(1);
    return 0;
}

/*------------------------*/

WriteChildPin()
{
    WriteString(1, "     ");

    CopySymbol(PinNameString, Formatter);
    PadSpaces(Formatter, 10);
    WriteSymbol(1, Formatter);

    i = GetStandardSymbol(PinType);
    WriteMap(1, i);

    WriteCrLf(1);
    return 0;
}

/*------------------------*/

WritePin()
{
    WriteString(1, "     ");

    CopySymbol(ReferenceString, Formatter);
    PadSpaces(Formatter, 12);
    WriteSymbol(1, Formatter);

    CopySymbol(PinNumberString, Formatter);
    PadSpaces(Formatter, 12);
    WriteSymbol(1, Formatter);

    CopySymbol(PinNameString, Formatter);
    PadSpaces(Formatter, 12);
    WriteSymbol(1, Formatter);

    CopySymbol(PartName, Formatter);
    PadSpaces(Formatter, 12);
    WriteSymbol(1, Formatter);

    i = SymbolLength(ModuleName);
    if (i == 0)
    {
        SetSymbol(Formatter, "EMPTY ");
        PadSpaces(Formatter, 12);
        WriteSymbol(1, Formatter);
    }
    else
    {
        CopySymbol(ModuleName, Formatter);
        PadSpaces(Formatter, 12);
        WriteSymbol(1, Formatter);
    }

    i = GetStandardSymbol(PinType);
    WriteMap(1, i);

    WriteCrLf(1);
    return 0;
}

/*------------------------*/

WriteSignal()
{
    i = GetStandardSymbol(TypeCode);
    WriteString(1, "Net Number - ");
    WriteSymbol(1, NetNumber);
    WriteString(1, "     ");

    if (i == 'L')
    {
        WriteString(1, "Label Node - ");
        WriteString(1, " Node Name - ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'P')
    {
        WriteString(1, "Port Node - ");
        WriteString(1, " Node Name - ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'S')
    {
        WriteString(1, "Supply Node - ");
        WriteString(1, " Node Name - ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'N')
    {
        WriteString(1, "UnNamed Node - ");
    }
    if (i == 'U')
    {
        WriteString(1, "UnConnected Node - ");
    }
    WriteCrLf(1);
    return 0;
}

/*------------------------*/

Title()
{
    WriteSymbol(1, SheetNumber);
    WriteString(1, "/");
    WriteSymbol(1, TotalSheets);
    WriteString(1, " ");
    WriteSymbol(1, SheetSize);
    WriteCrLf(1);

    WriteSymbol(1, DateString);
    WriteString(1, " ");
    WriteSymbol(1, DocumentNumber);
    WriteString(1, " ");
    WriteSymbol(1, Revision);
    WriteCrLf(1);

    WriteSymbol(1, TitleString);
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
    return 0;
}

/*------------------------*/

PostFile()
{
    WriteCrLf(1);
    WriteString(1, "Processed - ");
    WriteSymbol(1, FileName);
    WriteCrLf(1);

    WriteString(1, "Title Block :");
    WriteCrLf(1);
    Title();

    WriteCrLf(1);
    i = FirstChild();
    if (i == 0)
    {
        WriteString(1, "No Children.");
        WriteCrLf(1);
    }
    else
    {
        WriteString(1, "----- Children -----");
        WriteCrLf(1);
        SheetPinMap();
        do {
            WriteString(1, "--------------------");
            WriteCrLf(1);
            WriteSymbol(1, ReferenceString);
            WriteString(1, "  ");
            WriteSymbol(1, PartName);
            WriteCrLf(1);

            i = ChildPinCount();
            WriteString(1, "ChildPinCount - ");
            WriteInteger(1, i);
            WriteCrLf(1);
            do {
                WriteString(1, "Child Pin      : ");
                WriteChildPin();
                WriteString(1, "Signal         : ");

                i = SetSignal();
                if (i == 1)
                {
                    WriteSignal();
                }
                else
                {
                    WriteString(1, "Child Pin is not used.");
                    WriteCrLf(1);
                }
                WriteString(1, "Child Pin Info : ");
                WriteCrLf(1);
                WritePin();
                WriteCrLf(1);
            } while (NextChildPin());
            WriteCrLf(1);
        } while (NextChild());
    }
    WriteCrLf(1);

    i = FirstPart();
    if (i == 0)
    {
        WriteString(1, "No Parts.");
        WriteCrLf(1);
    }
    else
    {
        WriteString(1, "----- Parts -----");
        WriteCrLf(1);
        PinPinMap();
        do {
            WriteString(1, "--------------------");
            WriteCrLf(1);
            WriteSymbol(1, ReferenceString);
            WriteString(1, "  ");
            WriteSymbol(1, PartName);
            WriteCrLf(1);

            i = PinCount();
            WriteString(1, "PinCount - ");
            WriteInteger(1, i);
            WriteCrLf(1);
            do {
                WriteString(1, "Signal        : ");
                i = SetSignal();
                if (i == 1)
                {
                    WriteSignal();
                }
                else
                {
                    WriteString(1, "Pin is not used.");
                    WriteCrLf(1);
                }
                WriteString(1, "Part Pin Info : ");
                WriteCrLf(1);
                WritePin();
            } while (NextPin());

            WriteCrLf(1);
        } while (NextPart());
    }
    WriteCrLf(1);

    i = FirstNet();
    if (i == 0)
    {
        WriteString(1, "No nets.");
        WriteCrLf(1);
    }
    else
    {
        do {
            WriteString(1, "----- Nets -----");
            WriteCrLf(1);
            WriteString(1, "Net Number - ");
            WriteSymbol(1, NetNumber);
            WriteCrLf(1);

            do {
                WriteNetNode();
            } while (NextNode());
        } while (NextNet());
    }
    return 0;
}

/*------------------------*/

WriteInstanceFile()
{
    int len;

    WriteCrLf(1);
    WriteCrLf(1);
    SetAccessType("LIBRARY");
    do {
        len = SymbolLength(LibraryNameString);
        if (len > 0) {
            WriteSymbol(1, LibraryNameString);
            WriteCrLf(1);
            do {
                WriteString(1, "    ");
                WriteSymbol(1, ReferenceString);
                WriteString(1, "    ");
                WriteSymbol(1, PartName);
                WriteString(1, "    <");
                WriteSymbol(1, ModuleName);
                WriteString(1, ">");
                WriteCrLf(1);
            } while (NextInstance());
        }
    } while (NextAccessType());
    WriteCrLf(1);
    WriteCrLf(1);

    RewindInstanceFile();

    while (LoadInstance())
    {
        CopySymbol(ReferenceString, Formatter);
        PadSpaces(Formatter, 10);
        WriteSymbol(1, Formatter);

        CopySymbol(PartName, Formatter);
        PadSpaces(Formatter, 10);
        WriteSymbol(1, Formatter);

        CopySymbol(ModuleName, Formatter);
        i = SymbolLength(ModuleName);
        if (i == 0)
        {
            SetSymbol(Formatter, "EMPTY");
        }
        PadSpaces(Formatter, 10);
        WriteSymbol(1, Formatter);

        CopySymbol(TimeStamp, Formatter);
        PadSpaces(Formatter, 10);
        WriteSymbol(1, Formatter);

        WriteCrLf(1);
        WriteString(1, "    <");
        WriteSymbol(1, FieldString1);
        WriteString(1, "> ");
        WriteString(1, "<");
        WriteSymbol(1, FieldString2);
        WriteString(1, "> ");
        WriteString(1, "<");
        WriteSymbol(1, FieldString3);
        WriteString(1, "> ");
        WriteString(1, "<");
        WriteSymbol(1, FieldString4);
        WriteString(1, "> ");
        WriteString(1, "<");
        WriteSymbol(1, FieldString5);
        WriteString(1, "> ");
        WriteString(1, "<");
        WriteSymbol(1, FieldString6);
        WriteString(1, "> ");
        WriteString(1, "<");
        WriteSymbol(1, FieldString7);
        WriteString(1, "> ");
        WriteString(1, "<");
        WriteSymbol(1, FieldString8);
        WriteString(1, "> ");
        WriteCrLf(1);
    }
    return 0;
}

/*------------------------*/

PostProcess()
{
    WriteInstanceFile();

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}

