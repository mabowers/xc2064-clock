#include "format.h"

static char *switches = "ORSWITCHES ";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Added Version message
 *
 *  A generic part-oriented dump utility for linked, flat netlist files
 *
 */

static int i, Formatter;
static int SIGNALS, ALL, NODES;

/*-----------------------*/

PrintNode()
{
    WriteString(1, "Node Is:");
    WriteCrLf(1);

    i = GetStandardSymbol(TypeCode);
    if ( i == 'L' )
    {
       WriteString(1, "Label Node - ");
       WriteSymbol(1, NetNumber);
       WriteString(1, " Node Name - ");
       WriteSymbol(1, SignalNameString);
    }
    if ( i == 'P' )
    {
       WriteString(1, "Port Node - ");
       WriteSymbol(1, NetNumber);
       WriteString(1, " Node Name - ");
       WriteSymbol(1, SignalNameString);
    }
    if ( i == 'S' )
    {
       WriteString(1, "Supply Node - ");
       WriteSymbol(1, NetNumber);
       WriteString(1, " Node Name - ");
       WriteSymbol(1, SignalNameString);
    }
    if ( i == 'N' )
    {
       WriteString(1, "Unnamed Node - ");
       WriteSymbol(1, NetNumber);
    }
    if ( i == 'U' )
    {
       WriteString(1, "Unconnected Node - ");
       WriteSymbol(1, NetNumber);
    }
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Formatter = AddSymbol("Formatter");

    SIGNALS = AddSymbol( "SIGNALS" );
    SetSymbol( SIGNALS, "SIGNALS" );
    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

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

WriteHeader()
{
    CreatePartDataBase();
    WriteString(1, "------------------ Node List ------------------");
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    AddSignalName();

    PrintNode();
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
    WriteString(1, "----------------- End Node List -----------------");
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    SetFirst(ALL);
    do
    {
        if (EndNode()) 
        {
            WriteCrLf(1);
            WriteString(1, "Empty Node for : ");
            WriteSymbol(1, ReferenceString);
            WriteString(1, "     ");
            WriteSymbol(1, PartName);
            WriteString(1, "     ");
            CopySymbol(TimeStamp, Formatter);
            FToUpper(Formatter);
            WriteSymbol(1, Formatter );
            WriteCrLf(1);
        }  
        else
        {
            CopySymbol(ReferenceString, Formatter);
            LoadFieldString(Formatter);

            WriteCrLf(1);
            WriteSymbol(1, ReferenceString);
            WriteString(1, "     ");
            WriteSymbol(1, PartName);
            WriteString(1, "     ");
            CopySymbol(TimeStamp, Formatter);
            FToUpper(Formatter);
            WriteSymbol(1, Formatter );

            WriteCrLf(1);
            WriteString(1, "Reference PinNumber PinName   PartName  Module    PinType");
            WriteCrLf(1);
            WriteString(1, "---------------------------------------------------------");
            WriteCrLf(1);

            do {
                PrintNode();
    
                i = GetStandardSymbol(SignalType);
                if (i == 255)
                {
                    WriteString(1, "**** PIN IS NOT USED ****");
                    WriteCrLf(1);
                }
    
                CopySymbol(ReferenceString, Formatter);
                PadSpaces(Formatter, 10);
                WriteSymbol(1, Formatter);
             
                CopySymbol(PinNumberString, Formatter);
                PadSpaces(Formatter, 10);
                WriteSymbol(1, Formatter);
             
                CopySymbol(PinNameString, Formatter);
                PadSpaces(Formatter, 10);
                WriteSymbol(1, Formatter);
             
                CopySymbol(PartName, Formatter);
                PadSpaces(Formatter, 10);
                WriteSymbol(1, Formatter);
             
                i = SymbolLength(ModuleName);
                if (i == 0)
                {
                    SetSymbol(Formatter, "EMPTY ");
                    PadSpaces(Formatter, 10);
                    WriteSymbol(1, Formatter);
                }
                else
                {
                    CopySymbol(ModuleName, Formatter);
                    PadSpaces(Formatter, 10);
                    WriteSymbol(1, Formatter);
                }

                i = GetStandardSymbol(PinType);
                WriteMap(1, i);
             
                WriteCrLf(1);
            }
            while (SetNext(NODES));
             
            WriteString(1, "--------- END PART ---------");
            WriteCrLf(1);
        }
    }
    while (SetNext(ALL));
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}



