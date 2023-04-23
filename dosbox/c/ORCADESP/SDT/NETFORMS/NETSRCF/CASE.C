#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n \
/I Include unconnected pins\n";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Added /I switch to allow unconnected pins to be given a net
 *      - Added code for outputting missing pins on a part
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are not checked for length
 *      - Node Numbers are limited to 5 digits (plus the leading 'X')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal
 */

static int prevpin;
static int currpin;

static int sw_L;
static int sw_I;
static int Str, PinNumberStr;
static int SIGNALS, ALL, NODES;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    WriteString(1, "ASSERTIONS=OFF;VERSION=400;LOCATION=LOC;");
    WriteCrLf(1);

    Str = AddSymbol("Str");
    PinNumberStr = AddSymbol("PinNumberStr");

    SIGNALS = AddSymbol( "SIGNALS" );
    SetSymbol( SIGNALS, "SIGNALS" );
    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

    SetCharSet("0123456789");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    sw_I = SwitchIsSet("I");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    CreatePartDataBase();
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
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
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    int i;
    int len;
    int net;
    
    net = 0;
    SetFirst(ALL);
    do {
        CopySymbol(ReferenceString, Str);
        LoadFieldString(Str);

        WriteString(1, " [SIZE=1;TIMES=1;LOC=(");
        WriteSymbol(1, ReferenceString);
        WriteString(1, ");PLOC=");
        WriteSymbol(1, ReferenceString);
        WriteString(1, ";SHAPE=");

        len = SymbolLength(ModuleName);
        if (len > 0)
            { WriteSymbol(1, ModuleName); }
        else
            { WriteSymbol(1, PartName); }
        WriteString(1, "]");
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
                        WriteString(1, " ");
                        WriteInteger(1, prevpin);
                        WriteString(1, "=NC;");
                        WriteCrLf(1);

                        prevpin = prevpin + 1;
                    }

                    SetNumberWidth(5);
                }
            }
            prevpin = currpin;

            WriteString(1, " ");
            WriteSymbol(1, PinNumberString);
            WriteString(1, "=");

            i = GetStandardSymbol(TypeCode);
            if (i == 'L')
            {
                if (sw_L == 0)
                {
                    SetNumberWidth(1);
                    MakeLocalSignal("_");
                    SetNumberWidth(5);
                    WriteSymbol(1, LocalSignal);
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
                WriteString(1, "X");
                WriteSymbol(1, NetNumber);
            }
            if (i == 'U')
            {
                if (sw_I == 1)
                {
                    net = net + 1;
                    WriteString(1, "UN");
                    WriteInteger(1, net);
                }
                else
                {
                    WriteString(1, "NC");
                }
            }
            WriteString(1, ";");
            WriteCrLf(1);
        } while (SetNext(NODES));

        WriteString(1, " ;");
        WriteCrLf(1);

    } while (SetNext(ALL));

    WriteString(1, ";");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


