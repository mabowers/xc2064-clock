#include "format.h"
#include <stdio.h>
#include <stdlib.h> /* malloc() */
#include <string.h> /* strstr(), strcmp() */

static char *switches = "ORSWITCHES \
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.22   30-AUG-94\"";

/*  Revision Changes:
 *      - Added code for outputting missing pins on a part
 *      - Added Version message
 *      - Copied PLDNET.C to OHDLNET.C (as a starting point for OHDLNET.C)
 *      - Added support for pad symbols
 *      - Signals attached to pad symbols are NOT appended with _<SheetNumber>
 *      - Changed the writing of pipe text to use a user_symbol instead of the standard_symbol
 *        PipeLine.  Apearantly the WriteSymbol() function behaves differently depending on
 *        wether the symbol being written is is a standard or user symbol.  The standard symbols
 *        are uppercased when written out, while the user symbols are not.  PLD uses lowercase
 *        characters i~n as index variables.  PLD equations can be placed on the schematic,
 *        therefor, lowercase characters must be written to the netlist (PLD source file).
 *      - Netnames are now checked for illegal characters
 *
 *  Capacities:
 *      - Part Names are not checked for length
 *      - Module Names are not checked for length
 *      - Reference Strings are not checked for length
 *      - Node Names are not checked for length
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - Legal characters are { '_', '0-9', 'a-z', 'A-Z' }
 *      - Names with illegal characters will generate a warning message
 *
 *  Notes on pad symbol support:
 *      - All symbols will have the library lookup name checked.  If the
 *        lookup name has "PAD" embedded in it, the symbol is determined
 *        to be a PAD.
 *      - The value is then checked for "PAD" (even if the library lookup
 *        already verified it is a pad), again to determine if the
 *        symbol is a pad symbol. (The value will override if both have
 *        the string "PAD" embedded.)
 *      - The format of the library lookup name/value string will be:
 *        format:   [<type> PAD {<pin number>}] | [<pin number>]
 *        The pin number is assumed to be the remaining characters following
 *        the string "PAD" in the value field.  If "PAD" does not exist in
 *        the value, then the entire contents of the value field is taken as
 *        the pin number.  Alphanumerics of any length are allowed.
 *      - If the library lookup name indicates the symbol is a pad, then
 *        the value string does not need to indicate it also.  As a matter
 *        of fact, in this situation the contents of the value string will
 *        be the pin number.  If the library lookup name is "BIPAD" and the
 *        value is "16", then the pad type is "BI", and it is connected to
 *        pin 16.  If the value does not specify a pin number (in other
 *        words both strings say something like "BIPAD", then the resulting
 *        pin assignment will be null.
 *      - Here are some examples:
 *
 *        lookup name   value string   formatter string
 *          BIPAD       BIPAD          PAD (<sig>, "BI")
 *          BIPAD       BIPAD23        PAD (<sig>, "BI", "23")
 *          BIPAD       23             PAD (<sig>, "BI", "23")
 *          BIPAD       XPAD           PAD (<sig>, "X")
 *          SOMEPART    BIPAD23        PAD (<sig>, "BI", "23") NOTE that SOMEPART has one pin
 *          SOMEPART    SOMEPART       SOMEPART (<sig>)
 *
 *        The last example shows a typical part, not the PAD exception.
 *
 * NOTE: Any signal that is attached to a pad symbol is NOT appended
 *       with an _<SheetNumber>.  This is done so that the signal name
 *       the customer places on his schematic is the signal name that
 *       is placed in the source code.  If an _<SheetNumber> were to be
 *       placed on the signal name, the customer would have to wait until
 *       after he compiled his netlist to see what the resulting signal
 *       names would be.  The customer needs to know what his signal names
 *       are if he is going to create test vectors.  Also, if he makes a
 *       change to his schemaics (assuming it is a hierarchy) the sheet
 *       numbers may change, causing more confusion.
 *
 *       The only thing the customer should have to be wary of is not
 *       naming a module port with the same name as a signal connected
 *       to a pad symbol.  If he does, there will be an implied connection
 *       in the netlist due to the same signal name being used in multiple
 *       nets.
 */

static int prevpin;
static int currpin;
static int type;
static int FieldStr, PinNumberStr, PartNameStr, RefStr, Str, Kwd, NetName, NewNetName;
static int SIGNALS, ALL, NODES, NETS;
static int PLD, VECTORS;
static int PadFlag, PadIndex, PadTypeStr, PadNumStr, PadStr;
static int TempPadTypeStr, TempNameString, TempString;
static int UserSymbolPipeLine;

static int sw_L;
static int ok;


/* --------------------------- */
/* List functions (C specific) */
/* --------------------------- */

/* These routines build a linked list of all signal names connected to pad
 * symbols.  A pad symbol is defined as any part which contains the string
 * "PAD" (in part or whole) in either the library lookup name or the part
 * value field.
 *
 * A linked list is created containing each signal name as they are
 * determined to be connected to a pad.  Later before a signal is to be
 * written, this list is checked.  If the signal is not contained in the
 * list, then an _<SheetNumber> is written after the signal (in order
 * to localize the signal).
 */

struct Node
{
    struct Node *next;
    char *name;
};

struct Node *head;

/*-----------------------*/

/* Move a formatter symbol to a array of characters.
 *
 * Entry: 'TempString' is a formatter symbol containing a string.
 *
 * Exit:  'MakeCString' will point to a C string with sthe same contents
 *         as 'TempString'.  Subsequent calls will overwrite the C string.
 */

char *MakeCString()
{
    static char s[256];
    int i,l;

    l = SymbolLength(TempString);

    for (i=0; i<l && i<255; i++)
        s[i] = GetSymbolChar(i, TempString);

    s[l] = (char) 0;

    return s;
}

/*-----------------------*/

/* Check to see if the current net has a pad symbol connected to it.
 *
 * Entry: none.
 *
 * Exit:  'PadSymbol' will be one if the LookupNameString or PartNameStr
 *         contains the string "PAD".  Zero is returned otherwise, or
 *         if the node is net is not a labeled net.
 */

int PadSymbol()
{
    int type;
    char *p;

    type = GetStandardSymbol(TypeCode);

    if (type != 'L') return 0;

    CopySymbol(LookupNameString, TempString);
    p = MakeCString();
    if (strstr(p, "PAD")) return 1;

    CopySymbol(PartName, TempString);
    p = MakeCString();
    if (strstr(p, "PAD")) return 1;

    return 0;
}

/*-----------------------*/

/* Store the net name in a linked list.
 *
 * Entry: 'SignalNameString' is a formatter symbol containing the
 *         name of the current net.
 *
 * Exit:  If the string in 'SignalNameString' is not already contained
 *         in the list, add it.
 */

SaveNode()
{
    char *p;
    struct Node *curr, *new;
    char *s;

    CopySymbol(SignalNameString, TempString);
    p = MakeCString();
    s = (char *) malloc (strlen(p)+1);
    if (!s) 
    {
        printf("Memory limit.\n");
        SetSymbol(ExitType, "E");
        return 0;
    }
    strcpy (s, p);

    if (head)
    {
        if (strcmp(head->name, p)==0)
                return 0;
        for (curr=head; curr->next; curr=curr->next)
            if (strcmp(curr->next->name,p)==0) 
                return 0;
    }

    new = (struct Node *) malloc (sizeof(struct Node));
    if (!new) 
    {
        printf("Memory limit.\n");
        SetSymbol(ExitType, "E");
        return 0;
    }
    new->name = s;
    new->next = (struct Node *) 0;

    if (!head) head = new;
    else curr->next = new;

    return 0;
}

/*-----------------------*/

/* Determine if the signal is stored in the list of signals (meaning
 * the signal is atached to a PAD symbol).
 *
 * Entry: 'SignalNameString' is a formatter symbol containing the name
 *         of the current signal.
 *
 * Exit:  'PadSignal' is one if the signal is connected to a pad symbol,
 *         zero otherwise.
 */

int PadSignal()
{
    char *p;
    struct Node *curr;

    CopySymbol(SignalNameString, TempString);
    p = MakeCString();

    for (curr=head; curr; curr=curr->next)
        if (strcmp(curr->name, p)==0)
            return 1;

    return 0;
}


/*-----------------------*/

FreeList()
{
    struct Node *curr, *temp;

    temp = (struct Node *) 0;

    for (curr=head; curr; curr=curr->next)
    {
        if (temp)
        {
            free(temp->name);
            free(temp);
        }

        temp = curr;
    }

    if (temp)
    {
        free(temp->name);
        free(temp);
    }

    return 0;
}


/* --------------------- */
/* Standard functions    */
/* --------------------- */

/*-----------------------*/

Initialize()
{

    FieldStr = AddSymbol("FieldStr");
    PinNumberStr = AddSymbol("PinNumberStr");
    PartNameStr = AddSymbol("PartNameStr");
    Str = AddSymbol("Str");
    Kwd = AddSymbol("Kwd");
    NetName = AddSymbol("NetName");
    NewNetName = AddSymbol("NewNetName");
    RefStr = AddSymbol("RefStr");

    PadTypeStr = AddSymbol("PadTypeStr");
    PadNumStr = AddSymbol("PadNumStr");
    PadStr = AddSymbol("PadStr");
    SetSymbol(PadStr, "PAD");
    TempNameString = AddSymbol("TempNameString");
    TempPadTypeStr = AddSymbol("TempPadTypeStr");
    TempString = AddSymbol("TempString");
    UserSymbolPipeLine = AddSymbol("UserSymbolPipeLine");

    PLD = AddSymbol("PLD");
    SetSymbol(PLD, "PLD");
    VECTORS = AddSymbol("VECTORS");
    SetSymbol(VECTORS, "VECTORS");

    SIGNALS = AddSymbol("SIGNALS");
    SetSymbol(SIGNALS, "SIGNALS");
    ALL = AddSymbol("ALL");
    SetSymbol(ALL, "ALL");
    NODES = AddSymbol("NODES");
    SetSymbol(NODES, "NODES");
    NETS = AddSymbol("NETS");
    SetSymbol(NETS, "NETS");

    head = (struct Node *) 0;


    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    AddSymbol("FieldStr");
    AddSymbol("PinNumberStr");
    AddSymbol("PartNameStr");
    AddSymbol("Str");

    SetCharSet("_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");

    WriteString(1, "|| ");
    CopySymbol(TitleString, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revised: ");
    WriteSymbol(1, DateString);
    WriteCrLf(1);
    WriteString(1, "|| ");
    CopySymbol(DocumentNumber, Str);
    PadSpaces(Str, 46);
    WriteSymbol(1, Str);
    WriteString(1, "Revision: ");
    WriteSymbol(1, Revision);
    WriteCrLf(1);
    WriteString(1, "|| ");
    WriteSymbol(1, Organization);
    WriteCrLf(1);
    WriteString(1, "|| ");
    WriteSymbol(1, AddressLine1);
    WriteCrLf(1);
    WriteString(1, "|| ");
    WriteSymbol(1, AddressLine2);
    WriteCrLf(1);
    WriteString(1, "|| ");
    WriteSymbol(1, AddressLine3);
    WriteCrLf(1);
    WriteString(1, "|| ");
    WriteSymbol(1, AddressLine4);
    WriteCrLf(1);
    WriteString(1, "|| ");
    WriteCrLf(1);

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
    int legal;

    CopySymbol(SignalNameString, Str);
    legal = SymbolInCharSet(Str);
    if (legal == 0)
    {
        WriteString(0, "WARNING: Name contains illegal characters '");
        WriteSymbol(0, Str);
        WriteString(0, "'");
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

SetPadFlags()
{
    int i, j, l, ch, chp;

    i = 0;
    j = 0;

    PadFlag  = 0;
    PadIndex = 0;

    l = SymbolLength(TempNameString);

    ch  = GetSymbolChar(i, TempNameString);
    chp = GetSymbolChar(j, PadStr);

    while (i < l)
    {
        if (ch == chp)
        {
            j = j+1;
        }
        else
        {
            j = 0;
        }

        if (j >= 3)
        {
            PadFlag  = 1;
            PadIndex = i-3;

            return 1;
        }

        i = i+1;

        ch  = GetSymbolChar(i, TempNameString);
        chp = GetSymbolChar(j, PadStr);
    }

    return 0;
}


/*-----------------------*/

FindPartName()
{
    int i, j, k, l, ch;

    PutSymbolChar(0,0,PadTypeStr);
    PutSymbolChar(0,0,PadNumStr);

    SetPadFlags();      /* Sets (or clears) PadFlag and PadIndex. */

    i = 0;
    l = SymbolLength(TempNameString);

    if (PadFlag == 1)   /* Does the string contain "PAD"? */
    {
        while(i <= PadIndex)
        {
            ch = GetSymbolChar(i, TempNameString);
            PutSymbolChar(i, ch, PadTypeStr);
            i = i+1;
        }

        PutSymbolChar(i, 0, PadTypeStr);

        j = 0;

        k = i+3;
        while (k <= l)
        {
            ch = GetSymbolChar(k, TempNameString);
            PutSymbolChar(j, ch, PadNumStr);
            j = j+1;
            k = k+1;
        }
        PutSymbolChar(j, 0, PadNumStr);
    }

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

PipePLD()
{
    ok = NextPipe();
    if (ok == 1)
    {
        ok = IsKeyWord();
        ok = 1 - ok;
    }

    while (ok == 1)
    {
        CopySymbol(PipeLine, UserSymbolPipeLine);  /* Place the pipe line in a user symbol to   */
        WriteSymbol(1, UserSymbolPipeLine);        /* avoid being uppercased by WriteSymbol()   */
                                                   /* when the line of pipe text is written.    */
/*        WriteSymbol(1, PipeLine); */

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
    int first;
    int ch;
    int len;
    int temp;
    int l;


    SetCharSet("0123456789");

    /* output pipe PLD text */
    FirstPipe();
    ok = AccessKeyWord(PLD);
    while(ok == 1) {
        PipePLD();
        ok = AccessKeyWord(PLD);
    }
 
    WriteString(1, "|");
    WriteCrLf(1);
    WriteString(1, "|  Netlist:");
    WriteCrLf(1);
    WriteString(1, "|  {");
    WriteCrLf(1);

    /* Build a list of all signals connected to pads. */
    SetFirst(ALL);
    do {
        CopySymbol(ReferenceString, RefStr);
        LoadFieldString(RefStr);

        if (PadSymbol())
        {
            SetFirst(NODES);
            do {
                type = GetStandardSymbol(TypeCode);

                if (type == 'L')  SaveNode();
            } while(SetNext(NODES));
        }
 
    } while(SetNext(ALL));

    /* Write the netlist portion of the source file. */
    SetFirst(ALL);
    do {

        CopySymbol(ReferenceString, RefStr);
        LoadFieldString(RefStr);

        CopySymbol(LookupNameString, TempNameString);

        FindPartName();

        if (PadFlag == 1)
        {
            CopySymbol(PadTypeStr, TempPadTypeStr);
        }
        else
        {
            PutSymbolChar(0, 0, TempPadTypeStr);
        }

        CopySymbol(PartName, TempNameString);

        FindPartName();

        if (PadFlag == 0)
        {
            l = SymbolLength(TempPadTypeStr);
            if (l > 0)
            {
                PadFlag = 1;
                CopySymbol(TempPadTypeStr, PadTypeStr);
                CopySymbol(PartName, PadNumStr);
            }
        }

        WriteString(1, "|   ");

        if (PadFlag == 1)
        {
            CopySymbol(PadStr, Str);
        }
        else
        {
            CopySymbol(PartName, Str);
        }
   
        temp = 0;
        ch = GetSymbolChar(temp, Str);
        /* if 'ch' is a number, then output an underscore before the PartName */
        if (ch >= 48)
        {
            if (ch <= 57)
            {
                WriteString(1, "_");
            }
        }
        WriteSymbol(1, Str);
        WriteString(1, " (");

        len = 0;
        prevpin = 0;
        first = 1;
        SetFirst(NODES);
        do {
            /* write out any pins between the previous one and the current one */
            SetCurrentPinNumber();

            prevpin = prevpin + 1;
            if (currpin > prevpin) {
                if (currpin != 0) {
                    while (currpin > prevpin) {
                        if (first != 1) { WriteString(1, ","); }
                        WriteString(1, "-");

                        first = 0;
                        prevpin = prevpin + 1;
                    }
                }
            }
            prevpin = currpin;

            if (len > 55)
            {
                len = 0;
                first = 1;

                WriteString(1, ",");
                WriteCrLf(1);
                WriteString(1, "|          ");
            }

            if (first != 1) { WriteString(1, ","); }

            type = GetStandardSymbol(TypeCode);
            if (type == 'L')
            {
                if (sw_L == 0)
                {
                    /* If the sig IS attached to a pad don't append _<SheetNumber> */
                    if (PadSignal())
                    {
                        WriteSymbol(1, SignalNameString);
                    }
                    else
                    {
                        SetNumberWidth(0);
                        MakeLocalSignal("_");
                        SetNumberWidth(5);
                        WriteSymbol(1, LocalSignal);
                    }
                }
                else
                {
                    WriteSymbol(1, SignalNameString);
                }
            }
            if (type == 'P')
            {
                WriteSymbol(1, SignalNameString);

                temp = SymbolLength(SignalNameString);
                len = len + temp + 1;
            }
            if (type == 'S')
            {
                WriteSymbol(1, SignalNameString);

                temp = SymbolLength(SignalNameString);
                len = len + temp + 1;
            }
            if (type == 'N')
            {
                WriteString(1, "N");
                WriteSymbol(1, NetNumber);

                len = len + 7;
            }
            if (type == 'U')
            {
                WriteString(1, "-");

                len = len + 2;
            }

            first = 0;
        } while (SetNext(NODES));

        /* Add the type and pin number info if we are writing a pad out. */
        if (PadFlag == 1)
        {
            WriteString(1, ",\"");
            WriteSymbol(1, PadTypeStr);
            WriteString(1, "\"");

            l = SymbolLength(PadNumStr);
            if (l > 0)
            {
                WriteString(1, ",\"");
                WriteSymbol(1, PadNumStr);
                WriteString(1, "\"");
            }
        }

        /* Write the reference */
        WriteString(1, ")  | ");
        WriteSymbol(1, ReferenceString);
        WriteCrLf(1);
    } while (SetNext(ALL));

    WriteString(1, "|  }");
    WriteCrLf(1);

    /* Output pipe VECTORS text */
    FirstPipe();
    ok = AccessKeyWord(VECTORS);
    if (ok == 1) {
        WriteString(1, "|");
        WriteCrLf(1);
        WriteString(1, "|Vectors:");
        WriteCrLf(1);
    }
    while(ok == 1) {
        PipePLD();
        ok = AccessKeyWord(VECTORS);
    }

    WriteString(0, "Done");
    WriteCrLf(0);

    return 0;
}

