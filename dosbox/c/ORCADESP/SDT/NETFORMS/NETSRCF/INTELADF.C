#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n \
/N Suppress comments\n \
/I Include unconnected pins\n";

static char *ver = "VERSION=\"1.20   22-APR-94\"";

/*
 *  Version 1.20
 *      - Changed the ERROR messages to WARNINGS
 *  Version 1.10
 *      - Added /I switch to allow unconnected pins to be given a net
 *      - Added 'unet' to make certain unconnected pins are in unique nets
 *      - Added Version message
 *      - Corrected int to unsigned comparison
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are not checked for length
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal
 */

static int sw_L;
static int sw_N;
static int sw_I;

static int ok;
static int unet;
static int PartNameStr, SignalNameStr, OUT, FBK, Str, EQUATIONS;
static int SIGNALS, ALL, NODES;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    PartNameStr = AddSymbol("PartNameStr");
    SignalNameStr = AddSymbol("SignalNameStr");
    Str = AddSymbol("Str");
    EQUATIONS = AddSymbol( "EQUATIONS" );
    SetSymbol( EQUATIONS, "EQUATIONS" );

    OUT = AddSymbol("OUT");
    SetSymbol(OUT, "OUT");
    FBK = AddSymbol("FBK");
    SetSymbol(FBK, "FBK");

    SIGNALS = AddSymbol( "SIGNALS" );
    SetSymbol( SIGNALS, "SIGNALS" );
    ALL = AddSymbol( "ALL" );
    SetSymbol( ALL, "ALL" );
    NODES = AddSymbol( "NODES" );
    SetSymbol( NODES, "NODES" );

    SetNumberWidth(1);

    unet = 0;

    sw_L = SwitchIsSet("L");
    sw_N = SwitchIsSet("N");
    sw_I = SwitchIsSet("I");
    return 0;
}

/*-----------------------*/

WriteHeader()
{

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

    WriteString(1, "OPTIONS:");
    WriteSymbol(1, AddressLine3);
    WriteCrLf(1);
    WriteString(1, "PART:");
    WriteSymbol(1, AddressLine4);
    WriteCrLf(1);
    WriteCrLf(1);

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

HandleInputs()
{
    int i;

    /* list the input module ports */
    WriteString(1, "INPUTS:");
    WriteCrLf(1);
    SetFirst(SIGNALS);
    do {
        i = GetStandardSymbol(TypeCode);
        if (i == 'P')
        {
            i = GetStandardSymbol(SignalType);
            if (i == 2)
            {
                WriteString(1, "    ");
                WriteSymbol(1, SignalNameString);
                WriteCrLf(1);
            }
        }
    } while (SetNext(SIGNALS));
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleOutputs()
{
    int i;

    /* list the output module ports */
    WriteString(1, "OUTPUTS:");
    WriteCrLf(1);
    SetFirst(SIGNALS);
    do {
        i = GetStandardSymbol(TypeCode);
        if (i == 'P')
        {
            i = GetStandardSymbol(SignalType);
            if (i == 1)
            {
                WriteString(1, "    ");
                WriteSymbol(1, SignalNameString);
                WriteCrLf(1);
            }
        }
    } while (SetNext(SIGNALS));
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNodeInfo()
{
    int i;
    int pos;

    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (sw_L == 0)
        {
            MakeLocalSignal(".");
            WriteSymbol(1, LocalSignal);
        }
        else
        {
            WriteSymbol(1, SignalNameString);
        }
    }
    if (i == 'P')
    {
        CopySymbol(SignalNameString, SignalNameStr);

        /* signal may appear as 'asdfas@##' -- remove the '@' and beyond */
        i = 64;
        pos = FindSymbolChar(i, SignalNameStr);
        if (pos != -1) { i = 0; PutSymbolChar(pos, i, SignalNameStr); }

        WriteSymbol(1, SignalNameStr);
    }
    if (i == 'S')
    {
        WriteSymbol(1, SignalNameString);
    }
    if (i == 'N')
    {
        WriteString(1, ".");
        WriteSymbol(1, NetNumber);
    }
    if (i == 'U')
    {
        if (sw_I == 0)
        {
            WriteString(0, "WARNING: Pin ");
            WriteSymbol(0, PinNumberString);
            WriteString(0, " on Reference '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "' is unconnected");
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }

        unet = unet + 1;
        WriteString(1, "UN");
        WriteInteger(1, unet);
    }
    return 0;
}

/*-----------------------*/

PipeADF()
{
    ok = NextPipe();
    if (ok == 1)
    {
        ok = IsKeyWord();
        ok = 1 - ok;
    }

    while (ok == 1)
    {
        WriteSymbol(1, PipeLine);
        WriteCrLf(1);

        ok = NextPipe();
        if (ok == 1)
        {
            ok = IsKeyWord();
            ok = 1 - ok;
        }
    }
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
    int i;
    int first;
    int cnt;

    /* write the input ports */
    HandleInputs();

    /* write the output ports */
    HandleOutputs();

    /* list the network */
    WriteString(1, "NETWORK:");
    WriteCrLf(1);

    SetFirst(ALL);
    cnt = 0;
    do {
        cnt = cnt + 1;

        first = 1;
        do {
            /* output signal if pin is named 'OUT' */
            i = CompareSymbol(OUT, PinNameString);
            if (i == 0)
            {
                if (first == 0) { WriteString(1, ","); }
                WriteNodeInfo();

                first = 0;
            }
        } while (SetNext(NODES));

        SetFirst(NODES);
        do {
            /* output signal if pin is named 'FBK' */
            i = CompareSymbol(FBK, PinNameString);
            if (i == 0)
            {
                if (first == 0) { WriteString(1, ","); }
                WriteNodeInfo();

                first = 0;
            }
        } while (SetNext(NODES));
        if (first == 0) {WriteString(1, "="); }

        CopySymbol(PartName, PartNameStr);
        ExceptionsFor("ADF", PartNameStr);
        WriteSymbol(1, PartNameStr);

        WriteString(1, "(");

        SetFirst(NODES);
        first = 1;
        do {
            i = CompareSymbol(OUT, PinNameString);
            if (i != 0)
            {
                i = CompareSymbol(FBK, PinNameString);
                if (i != 0)
                {
                    /* any symbol that is not 'OUT' or 'FBK' */
                    if (first == 0) { WriteString(1, ","); }

                    WriteNodeInfo();
                    first = 0;
                }
            }
        } while (SetNext(NODES));

        WriteString(1, ")");

        if (sw_N == 0)
        {
            WriteString(1, " % SYM ");
            WriteInteger(1, cnt);
            WriteString(1, " %");
        }
        WriteCrLf(1);

    } while (SetNext(ALL));
    WriteCrLf(1);

    WriteString(1, "EQUATIONS:");
    WriteCrLf(1);

    /* now the equations */
    FirstPipe();
    ok = AccessKeyWord(EQUATIONS);
    while (ok == 1) {
        PipeADF();
        ok = AccessKeyWord(EQUATIONS);
    }
   
    WriteString(1, "END$");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


