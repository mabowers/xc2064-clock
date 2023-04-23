#include "format.h"

static char *switches = "ORSWITCHES ";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Added Version message
 *
 *  A generic net-oriented dump utility for linked, flat netlist files
 *
 */

static int i, Formatter;

/*-----------------------*/

Initialize()
{

    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Formatter = AddSymbol("Formatter");
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

/*-----------------------*/

WriteInstanceFile()
{
    SetAccessType("LIBRARY");
    do
    {
        WriteSymbol(1, LibraryNameString);
        WriteCrLf(1);
        do
        {
            WriteString(1, "    ");
            WriteSymbol(1, ReferenceString);
            WriteString(1, "    ");
            WriteSymbol(1, PartName);
            WriteString(1, "    <");
            WriteSymbol(1, ModuleName);
            WriteString(1, ">");
            WriteCrLf(1);
        }
        while (NextInstance());
    }
    while (NextAccessType());
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

        WriteCrLf(1);
        WriteString(1, "ModuleName: ");
        CopySymbol(ModuleName, Formatter);
        i = SymbolLength(ModuleName);
        if (i == 0)
        {
            SetSymbol(Formatter, "EMPTY");
        }
        WriteSymbol(1, Formatter);
        WriteCrLf(1);

        CopySymbol(TimeStamp, Formatter);
        FToUpper(Formatter);
        PadSpaces(Formatter, 30);
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

/*-----------------------*/

WriteHeader()
{
    WriteString(1, "--------------- Title Block Information -------------");
    WriteCrLf(1);

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

    WriteString(1, "---------------------- Parts --------------------------");
    WriteCrLf(1);
    WriteInstanceFile();
    WriteCrLf(1);

    SortByNumber();
    WriteString(1, "---------- Parts Sorted by Reference Number -----------");
    WriteCrLf(1);
    WriteInstanceFile();
    WriteCrLf(1);

    WriteString(1, "----------------------- Nets --------------------------");
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    i = GetStandardSymbol(TypeCode);

    if (i == 'S')
    {
        WriteString(1, "Power - ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'P')
    {
        WriteString(1, "Port - ");
        WriteSymbol(1, SignalType);
        WriteString(1, " ");
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'L')
    {
        WriteString(1, "Label - ");
        WriteSymbol(1, SignalType);
        WriteString(1, " ");
        WriteSymbol(1, SignalNameString);
        WriteString(1, " ");
        WriteSymbol(1, SheetNumber);
    }
    if (i == 'U')
    {
        WriteString(1, "Unconnected - N");
        WriteSymbol(1, NetNumber);
    }
    if (i == 'N')
    {
        WriteString(1, "Unnamed - N");
        WriteSymbol(1, NetNumber);
    }
    WriteCrLf(1);
    WriteString(1, "Reference  PinNumber  PinName    PartName   Module     PinType");
    WriteCrLf(1);
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNet()
{
    CopySymbol(ReferenceString, Formatter);
    PadSpaces(Formatter, 10);
    WriteSymbol(1, Formatter);
    WriteString(1, " ");

    CopySymbol(PinNumberString, Formatter);
    PadSpaces(Formatter, 10);
    WriteSymbol(1, Formatter);
    WriteString(1, " ");

    CopySymbol(PinNameString, Formatter);
    PadSpaces(Formatter, 10);
    WriteSymbol(1, Formatter);
    WriteString(1, " ");

    CopySymbol(PartName, Formatter);
    PadSpaces(Formatter, 10);
    WriteSymbol(1, Formatter);
    WriteString(1, " ");

    i = SymbolLength(ModuleName);
    if (i == 0)
    {
       SetSymbol(Formatter, "EMPTY ");
       PadSpaces(Formatter, 10);
       WriteSymbol(1, Formatter);
       WriteString(1, " ");
    }
    else
    {
       CopySymbol(ModuleName, Formatter);
       PadSpaces(Formatter, 10);
       WriteSymbol(1, Formatter);
       WriteString(1, " ");
    }

    i = GetStandardSymbol(PinType);
    WriteMap(1, i);

    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    WriteString(1, "----- NET END -----");
    WriteCrLf(1);
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
    WriteString(1, "***** END NETS *****");
    WriteCrLf(1);
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


