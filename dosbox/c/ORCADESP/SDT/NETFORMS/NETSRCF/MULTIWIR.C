#include "format.h"

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.20   01-JUN-94\"";
/*
 *  Version 1.20
 *      - Changed ERROR messages to WARNINGS
 *
 *  Version 1.10
 *      - Added Version message
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings plus the Pin Number are limited to 32 characters
 *      - Node Names are limited to 16 characters
 *      - Node Numbers are limited to 5 digits (plus the leading 'N')
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal
 */

static int sw_L;

static int type_code;
static int len;
static int Error, SignalNameStr, ReferenceStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Error = AddSymbol("Error");
    SetSymbol(Error, "E");

    SignalNameStr = AddSymbol("SignalNameStr");
    ReferenceStr = AddSymbol("ReferenceStr");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    int len;

    CopySymbol(SignalNameString, SignalNameStr);

    type_code = GetStandardSymbol(TypeCode);
    if (type_code == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal("-");
            SetNumberWidth(5);
            CopySymbol(LocalSignal, SignalNameStr);
        }
    }

    len = SymbolLength(SignalNameStr);
    if (len > 16)
    {
        WriteString(0, "WARNING: Name is too long '");
        WriteSymbol(0, SignalNameStr);
        WriteString(0, "', changed to N");
        WriteSymbol(0, NetNumber);
        WriteCrLf(0);

        type_code = CompareSymbol(ExitType, Error);
        if (type_code != 0) { SetSymbol(ExitType, "W"); }

        type_code = 'N';
    }
    else
    {
        PadSpaces(SignalNameStr, 16);
    }
    return 0;
}

/*-----------------------*/

WriteNet()
{
    int pin_num_len;
    int ref_len;
    int i;

    if (type_code == 'L')
    {
        WriteSymbol(1, SignalNameStr);
    }
    if (type_code == 'P')
    {
        WriteSymbol(1, SignalNameStr);
    }
    if (type_code == 'S')
    {
        WriteSymbol(1, SignalNameStr);
    }
    if (type_code == 'N')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "          ");
    }
    if (type_code == 'U')
    {
        WriteString(1, "N");
        WriteSymbol(1, NetNumber);
        WriteString(1, "          ");
    }
    WriteString(1, "  ");

    /* the number of chars in the pin number */
    pin_num_len = SymbolLength(PinNumberString);
    /* the number of chars in the reference string */
    ref_len = SymbolLength(ReferenceString);

    if (pin_num_len + ref_len > 31)
    {
        /* the two strings won't fit with one space between them */
        WriteString(0, "WARNING: Reference plus Pin Number is too long '");
        WriteSymbol(0, ReferenceString);
        WriteString(0, "' and '");
        WriteSymbol(0, PinNumberString);
        WriteString(0, "'");
        WriteCrLf(0);

        SetSymbol(ExitType, "W");
    }
    else
    {
        WriteSymbol(1, ReferenceString);

        i = 32 - pin_num_len - ref_len;
        while (i > 0)
        {
            WriteString(1, " ");
            i = i - 1;
        }
        WriteSymbol(1, PinNumberString);
    }

    WriteCrLf(1);
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

WriteNetListEnd()
{
    WriteString(1, "-1");

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}


