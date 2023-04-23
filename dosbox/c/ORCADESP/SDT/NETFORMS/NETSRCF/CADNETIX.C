#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.20   01-JUN-94\"";
/*
 *  Version 1.20
 *      - Changed the ERROR messages to WARNINGS
 *  Version 1.10
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names can contain up to 17 characters
 *      - Module Names can contain up to 15 characters
 *      - Reference Strings plus Pin Numbers can contain up to 12 characters
 *      - Node Names can contain up to 16 characters
 *      - Node Numbers are not checked for length
 *      - Pin Numbers can contain up to 3 digits
 *      - All ASCII characters are legal
 */

static int sw_L;

static int net_items;
static int len;
static int Error, PartNameStr, ModuleNameStr, ReferenceStr, NetNumberStr,
           SignalNameStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Error = AddSymbol("Error");
    SetSymbol(Error, "E");

    /* used to pad their standard symbol namesakes with spaces */
    PartNameStr = AddSymbol("PartNameStr");
    ModuleNameStr = AddSymbol("ModuleNameStr");
    ReferenceStr = AddSymbol("ReferenceStr");

    NetNumberStr = AddSymbol("NetNumberStr");
    SignalNameStr = AddSymbol("SignalNameStr");

    SetNumberWidth(1);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    WriteString(1, "PARTS LIST");
    WriteCrLf(1);
    WriteCrLf(1);

    while (LoadInstance())
    {
        CopySymbol(PartName, PartNameStr);
        PadSpaces(PartNameStr, 17);
        len = SymbolLength(PartName);
        if (len > 17)
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

        /* if the ModuleName is empty, then use the PartName again */
        len = SymbolLength(ModuleName);
        if (len > 0)
        {
            CopySymbol(ModuleName, ModuleNameStr);
            PadSpaces(ModuleNameStr, 15);

            if (len > 15)
            {
                WriteString(0, "WARNING: Name is too long '");
                WriteSymbol(0, ModuleName);
                WriteString(0, "', truncated to ");
                WriteSymbol(0, ModuleNameStr);
                WriteCrLf(0);

                SetSymbol(ExitType, "W");
            }
            WriteSymbol(1, ModuleNameStr);
        }
        else
        {
            PadSpaces(PartNameStr, 15);
            WriteSymbol(1, PartNameStr);
        }
        WriteString(1, " ");

        WriteSymbol(1, ReferenceString);
        WriteCrLf(1);
    }

    WriteString(1, "EOS");
    WriteCrLf(1);
    WriteString(1, "NET LIST");
    WriteCrLf(1);
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int i;

    net_items = 0;
    CopySymbol(SignalNameString, SignalNameStr);

    i = GetStandardSymbol(TypeCode);
    if (i == 'L')
    {
        if (sw_L == 0)
        {
            MakeLocalSignal("_");
            CopySymbol(LocalSignal, SignalNameStr);
        }
    }

    len = SymbolLength(SignalNameStr);
    if (len > 16)
    {
        WriteString(0, "WARNING: Name is too long '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to NODE ");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        i = CompareSymbol(ExitType, Error);
        if (i != 0) { SetSymbol(ExitType, "W"); }

        i = 'N';
    }

    if (i == 'L')
    {
        WriteString(1, "NODENAME ");
        PadSpaces(SignalNameStr, 16);
        WriteSymbol(1, SignalNameStr);
        WriteString(1, " $");
    }
    if (i == 'P')
    {
        WriteString(1, "NODENAME ");
        PadSpaces(SignalNameStr, 16);
        WriteSymbol(1, SignalNameStr);
        WriteString(1, " $");
    }
    if (i == 'S')
    {
        WriteString(1, "NODENAME ");
        PadSpaces(SignalNameStr, 16);
        WriteSymbol(1, SignalNameStr);
        WriteString(1, " $");
    }
    if (i == 'N')
    {
        WriteString(1, "NODE     ");
        CopySymbol(NetNumber, NetNumberStr);
        PadSpaces(NetNumberStr, 16);
        WriteSymbol(1, NetNumberStr);
        WriteString(1, " $");
    }
    if (i == 'U')
    {
        WriteString(1, "NODE     ");
        CopySymbol(NetNumber, NetNumberStr);
        PadSpaces(NetNumberStr, 16);
        WriteSymbol(1, NetNumberStr);
        WriteString(1, " $");
    }
    WriteCrLf(1);

    /* the leading 2 spaces for this net */
    WriteString(1, "  ");
    return 0;
}

/*-----------------------*/

WriteNet()
{
    int pin_num_len;
    int ref_len;
    int i;

    if (net_items == 4)
    {
        WriteString(1, "   $");
        WriteCrLf(1);
        WriteString(1, "  ");

        net_items = 0;
    }
    WriteString(1, "  ");

    /* the number of chars in the pin number */
    pin_num_len = SymbolLength(PinNumberString);
    /* the number of chars in the reference string */
    ref_len = SymbolLength(ReferenceString);

    CopySymbol(ReferenceString, ReferenceStr);

    if (pin_num_len + ref_len > 11)
    {
        /* the two strings won't fit with one space between them */
        WriteString(0, "WARNING: Reference plus Pin Number is too long '");
        WriteSymbol(0, ReferenceString);
        WriteString(0, "' and '");
        WriteSymbol(0, PinNumberString);
        WriteString(0, "'");
        WriteCrLf(0);

        SetSymbol(ExitType, "W");

        if (pin_num_len == 1)
        {
            PadSpaces(ReferenceStr, 10);
            ref_len = 10;
        }
        if (pin_num_len == 2)
        {
            PadSpaces(ReferenceStr, 9);
            ref_len = 9;
        }
        if (pin_num_len == 3)
        {
            PadSpaces(ReferenceStr, 8);
            ref_len = 8;
        }
        if (pin_num_len == 4)
        {
            PadSpaces(ReferenceStr, 7);
            ref_len = 7;
        }
    }

    WriteSymbol(1, ReferenceStr);

    i = 12 - pin_num_len - ref_len;
    while (i > 0)
    {
        WriteString(1, " ");
        i = i - 1;
    }
    WriteSymbol(1, PinNumberString);

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
    WriteString(1, "EOS");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}

