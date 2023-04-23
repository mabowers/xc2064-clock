#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n \
/I Include unconnected pins\n";

static char *ver = "VERSION=\"1.10   04-SEP-91\"";

/*  Version 1.10
 *      - Added /I switch to allow unconnected pins to be given a net
 *      - Simplified header (ENVIRONMENT and DETAIL)
 *      - Added code for outputting missing pins on a part
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are limited 8 characters
 *      - Node Numbers are limited to 5 digits (plus the leading 'NET')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - Names are not checked for legal characters
 */

static int sw_L;
static int sw_I;

static int prevpin;
static int currpin;

static int SignalNameStr, PinNumberStr, Str;
static int ALL, NODES;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    SignalNameStr = AddSymbol("SignalNameStr");
    PinNumberStr = AddSymbol("PinNumberStr");
    Str = AddSymbol("Str");

    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

    WriteString(1, "{COMPONENT ORCAD.PCB");
    WriteCrLf(1);
    WriteString(1, " {ENVIRONMENT LAYS.PCB}");
    WriteCrLf(1);
    WriteString(1, "  {PDIFvrev 1.30}");
    WriteCrLf(1);
    WriteString(1, "  {DETAIL");
    WriteCrLf(1);
    WriteString(1, "   {SUBCOMP");
    WriteCrLf(1);

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
    int i;

    CopySymbol(SignalNameString, SignalNameStr);

    i = GetStandardSymbol(TypeCode);
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

    i = SymbolLength(SignalNameStr);
    if (i > 14)
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

        WriteString(1, "{I ");

        len = SymbolLength(ModuleName);
        if (len > 0)
        {
            WriteSymbol(1, ModuleName);
        }
        else
        {
            WriteSymbol(1, PartName);
        }
        WriteString(1, ".PRT ");

        WriteSymbol(1, ReferenceString);
        WriteCrLf(1);

        WriteString(1, "{CN");
        WriteCrLf(1);

        prevpin = 0;
        do {
            /* write out any pins between the previous one and the current one */
            SetCurrentPinNumber();

            prevpin = prevpin + 1;
            if (currpin > prevpin) {
                if (currpin != 0) {
                    SetNumberWidth(0);

                    while (currpin > prevpin) {
                        if (prevpin < 10000) { WriteString(1, " "); }
                        if (prevpin < 1000)  { WriteString(1, " "); }
                        if (prevpin < 100)   { WriteString(1, " "); }
                        if (prevpin < 10)    { WriteString(1, " "); }

                        WriteInteger(1, prevpin);
                        WriteString(1, " ?");
                        WriteCrLf(1);

                        prevpin = prevpin + 1;
                    }

                    SetNumberWidth(5);
                }
            }
            prevpin = currpin;

            len = SymbolLength(PinNumberString);
            if (len == 0) { WriteString(1, "     "); }
            if (len == 1) { WriteString(1, "    "); }
            if (len == 2) { WriteString(1, "   "); }
            if (len == 3) { WriteString(1, "  "); }
            if (len == 4) { WriteString(1, " "); }
            WriteSymbol(1, PinNumberString);
            WriteString(1, " ");

            CopySymbol(SignalNameString, SignalNameStr);

            i = GetStandardSymbol(TypeCode);
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
            if (len > 14) { i = 'N'; }

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
                WriteString(1, "NET");
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
                    WriteString(1, "?");
                }
            }
            WriteCrLf(1);
        } while (SetNext(NODES));

        WriteString(1, "}");
        WriteCrLf(1);
        WriteString(1, "}");
        WriteCrLf(1);

    } while (SetNext(ALL));

    WriteString(1, "}");
    WriteCrLf(1);
    WriteString(1, "}");
    WriteCrLf(1);
    WriteString(1, "}");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


