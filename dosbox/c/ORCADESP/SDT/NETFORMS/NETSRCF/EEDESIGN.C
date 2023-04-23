#include "format.h"

static char *switches = "ORSWITCHES ";

static char *ver = "VERSION=\"1.20  01-JUN-94\"";
/*
 *  Version 1.20
 *      - Changed ERROR messages to WARNINGS.
 *
 *  Version 1.10a
 *      - Added Version message
 *      - Allow Reference Strings up to 8 characters
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are limited to 8 characters
 *      - Node Names are NOT supported
 *      - Node Numbers are limited to 3 digits (plus the leading 'UN')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - All ASCII keyboard characters are legal
 */

static int len;
static int ReferenceStr;

/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    ReferenceStr = AddSymbol("ReferenceStr");
    SetNumberWidth(3);
    return 0;
}

/*-----------------------*/

WriteHeader()
{
    WriteString(1, "(PATH,OrCAD()");
    WriteCrLf(1);
    WriteString(1, "(COMPONENTS");
    WriteCrLf(1);

    while (LoadInstance())
    {
        CopySymbol(ReferenceString, ReferenceStr);
        PadSpaces(ReferenceStr, 8);

        len = SymbolLength(ReferenceString);
        if (len > 8)
        {
            WriteString(0, "WARNING: Reference is too long '");
            WriteSymbol(0, ReferenceString);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, ReferenceStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }        
        WriteSymbol(1, ReferenceStr);
        WriteString(1, ",");

        len = SymbolLength(ModuleName);
        /* if the ModuleName is empty, then output the PartName */
        if (len > 0)
        {
            WriteSymbol(1, ModuleName);
        }
        else
        {
            WriteSymbol(1, PartName);
        }
        WriteCrLf(1);
    }

    WriteString(1, ")");
    WriteCrLf(1);

    WriteString(1, "(NODES");
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    WriteString(1, "(UN");
    WriteSymbol(1, NetNumber);
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNet()
{
    CopySymbol(ReferenceString, ReferenceStr);
    PadSpaces(ReferenceStr, 8);
    WriteSymbol(1, ReferenceStr);

    WriteString(1, ", ");

    WriteSymbol(1, PinNumberString);
    WriteCrLf(1);
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
    WriteString(1, ")");
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
    WriteString(1, ")");
    WriteCrLf(1);
    WriteString(1, "),OrCAD");
    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
    return 0;
}
