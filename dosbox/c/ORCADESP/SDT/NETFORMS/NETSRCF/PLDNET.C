#include "format.h"

static char *swithces = "ORSWITCHES \
/L Do not append sheet number to labels\n \
/W Create a complete PLD source file\n \
/P Only use 'pins' and 'nodes' keywords (do not use in, out, and io)\n";

static char *ver = "VERSION=\"1.20   21-JUN-93\"";


/*  /U Include unspecified module ports as "nodes"
 *
 *
 *      - Added code for outputting missing pins on a part
 *      - Added Version message
 *      - Added switch (/U) to list unspecified module ports as if they were
 *        either inputs or outputs after the netlist keyword (see also /P)
 *      - Added switch option to produce entire PLD source file (pin assignments
 *        and signal names are taken from the module ports)
 *      - Pin assignments are now allowed in module ports: <pin #>:<signal name>
 *      - Added code to output pipe text
 *      - Now unspecified module ports are listed (in the manner they are used)
 *      - Added switch option to output "Pins:" keyword (also outputs "nodes")
 *      - Turned on /U switch permanently (Always outputs unspec ports as
 *        nodes).
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

static int prevpin;
static int currpin;
static int type;
static int ok;
static int newname;
static int flag;
static int porttype;

static int inflag;
static int outflag;

static int unspecport;
static int outwritten;
static int inwritten;

static int OLineIndex;
static int ILineIndex;

static int Comma;

static int outputline;

static int LineLength;

static int ILinePtr;
static int OLinePtr;

static int sw_L;
static int sw_W;
static int sw_P;
static int sw_U;

static int FieldStr, PinNumberStr, PartNameStr, Str, Kwd, NetName, NewNetName;
static int ILine1, ILine2, ILine3, ILine4, ILine5;
static int OLine1, OLine2, OLine3, OLine4, OLine5;
static int SIGNALS, ALL, NODES;
static int PLD, VECTORS;


/*-----------------------*/

Initialize()
{
    WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    Comma = 44;

    FieldStr = AddSymbol("FieldStr");
    PinNumberStr = AddSymbol("PinNumberStr");
    PartNameStr = AddSymbol("PartNameStr");
    Str = AddSymbol("Str");
    Kwd = AddSymbol("Kwd");
    NetName = AddSymbol("NetName");
    NewNetName = AddSymbol("NewNetName");
    ILine1 = AddSymbol("ILine1");
    ILine2 = AddSymbol("ILine2");
    ILine3 = AddSymbol("ILine3");
    ILine4 = AddSymbol("ILine4");
    ILine5 = AddSymbol("ILine5");
    OLine1 = AddSymbol("OLine1");
    OLine2 = AddSymbol("OLine2");
    OLine3 = AddSymbol("OLine3");
    OLine4 = AddSymbol("OLine4");
    OLine5 = AddSymbol("OLine5");

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

    SetCharSet("0123456789");
    SetNumberWidth(5);

    sw_L = SwitchIsSet("L");
    sw_W = SwitchIsSet("W");
    sw_P = SwitchIsSet("P");
    sw_U = 1;                   /* Always output unspec ports as nodes. */

    if (sw_P == 1) { sw_W = 1; } /* If the user requested pins and nodes */
                                 /* make sure he gets them.              */
    inflag     = 0;
    outflag    = 0;

    unspecport = 0;
    outwritten = 0;
    inwritten  = 0;

    OLineIndex = 1;
    ILineIndex = 1;

    LineLength = 55;            /* This can be increased if 'Too many   */
                                /* nodes' error occurs.                 */
    ILinePtr   = 0;
    OLinePtr   = 0;
}

/*-----------------------*/

WriteHeader()
{
    CreatePartDataBase();
}

/*-----------------------*/

MakeNetName()
{
    int i;
    int j;
    int colon;
    int len;
    int ch;
    int null;

    null  = 0;
    colon = ':';

    CopySymbol(SignalNameString, NetName);

    len = SymbolLength(SignalNameString);

    i = FindSymbolChar(colon, NetName);

    if (i == -1) {
         newname = 0;
         CopySymbol(SignalNameString, NewNetName);
    }
    else { if (i > len) {
         newname = 0;
         CopySymbol(SignalNameString, NewNetName);
    }
    else {
         newname = 1;
         j = 0;
         i = i+1;
         do {
              ch = GetSymbolChar(i, NetName);
              PutSymbolChar(j, ch, NewNetName);
              i = i+1;
              j = j+1;
         }while(i < len);
         PutSymbolChar(j, null, NewNetName);
    }}
}


/*-----------------------*/

AddSig()
{
    int i,j,k,l,ll,ch,written,err;

    if (outputline == 1)
    {
        written = outwritten;
        i = OLinePtr;
        
        if (OLineIndex == 1) { CopySymbol(OLine1, Str); }
        if (OLineIndex == 2) { CopySymbol(OLine2, Str); }
        if (OLineIndex == 3) { CopySymbol(OLine3, Str); }
        if (OLineIndex == 4) { CopySymbol(OLine4, Str); }
        if (OLineIndex == 5) { CopySymbol(OLine5, Str); }
    }
    else
    {
        written = inwritten;
        i = ILinePtr;

        if (ILineIndex == 1) { CopySymbol(ILine1, Str); }
        if (ILineIndex == 2) { CopySymbol(ILine2, Str); }
        if (ILineIndex == 3) { CopySymbol(ILine3, Str); }
        if (ILineIndex == 4) { CopySymbol(ILine4, Str); }
        if (ILineIndex == 5) { CopySymbol(ILine5, Str); }
    }

    j = SymbolLength(NewNetName);

    k = i+j;

    ll = LineLength-12;

    if (k > ll)
    {
        if (outputline == 1) { OLineIndex = OLineIndex + 1; }
        else { ILineIndex = ILineIndex + 1; }

        i = 0;
        k = j;
        PadSpaces(Str, LineLength);
    }

    err = 0;
    if (OLineIndex > 5) { err = 1; }
    else { if (ILineIndex > 5) { err = 1; }}

    if (err == 1)
    {
        WriteString(0, "ERROR:Unable to add signal '");
        WriteSymbol(0, NewNetName);
        WriteString(0, "'");
        WriteCrLf(0);
        WriteString(0, "Try shortening the names of the signals in the unspecified module");
        WriteCrLf(0);
        WriteString(0, "ports, or remove the unspecified module ports which are unneeded.");
        WriteCrLf(0);
        WriteCrLf(0);

        SetSymbol(ExitType, "E");
    }
    else
    {
        if (written == 1)
        {
            l = i+1;

            PutSymbolChar(i, Comma, Str);

            k = k+1;
            i = i+1;
        }

        j = 0;
        for(l=i; l<=k; l=l+1)
        {
            ch = GetSymbolChar(j,NewNetName);
            PutSymbolChar(l, ch, Str);
            j = j+1;
        }
    
        if (outputline == 1)
        {
            outwritten = 1;
            OLinePtr   = k;
            
            if (OLineIndex == 1) { CopySymbol(Str, OLine1); }
            if (OLineIndex == 2) { CopySymbol(Str, OLine2); }
            if (OLineIndex == 3) { CopySymbol(Str, OLine3); }
            if (OLineIndex == 4) { CopySymbol(Str, OLine4); }
            if (OLineIndex == 5) { CopySymbol(Str, OLine5); }
        }
        else
        {
            inwritten = 1;
            ILinePtr  = k;
    
            if (ILineIndex == 1) { CopySymbol(Str, ILine1); }
            if (ILineIndex == 2) { CopySymbol(Str, ILine2); }
            if (ILineIndex == 3) { CopySymbol(Str, ILine3); }
            if (ILineIndex == 4) { CopySymbol(Str, ILine4); }
            if (ILineIndex == 5) { CopySymbol(Str, ILine5); }
        }
    }
}

   
/*-----------------------*/

HandleNodeName()
{
    int in, out, nodesok;

    nodesok = 0;
    if (sw_U == 1) { nodesok = 1; }
    if (sw_P == 1) { nodesok = 1; }

    if (nodesok == 1)
    {
        if (unspecport == 1)
        {
            if        (outflag    == 1) { out = 1; }
            else { if (inflag     == 1) { in  = 1; }}
    
            if (out == 1)
            {
                outputline = 1;
                AddSig();
                outwritten = 1;
            }
            else { if (in == 1)
            {
                outputline = 0;
                AddSig();
                inwritten = 1;
            }}
        }
    
        inflag     = 0;
        outflag    = 0;
    
        unspecport = 0;
    }

    AddSignalName();

}

/*-----------------------*/

WriteNet()
{
    int nodesok;

    nodesok = 0;
    if (sw_U == 1) { nodesok = 1; }
    if (sw_P == 1) { nodesok = 1; }

    if (nodesok == 1)
    {
        type = GetStandardSymbol(TypeCode);
        if (type == 'P')
        {
            type = GetStandardSymbol(SignalType);
            
            if (type == 0)
            {
                unspecport = 1;
                MakeNetName();
    
                type = GetStandardSymbol(PinType); 
            
                if (type == 0) { inflag  = 1; }  /* input pin      */
                if (type == 1) { outflag = 1; }  /* bidirectional  */
                if (type == 2) { outflag = 1; }  /* output pin     */
                if (type == 3) { outflag = 1; }  /* open collector */
                if (type == 4) { outflag = 1; }  /* passive        */
                if (type == 5) { outflag = 1; }  /* hi-z           */
                if (type == 6) { outflag = 1; }  /* open emitter   */
                if (type == 7) { outflag = 1; }  /* power          */
            }
        }
    }

    RecordNode();
}

/*-----------------------*/

WriteNetEnding()
{
}

/*-----------------------*/

ProcessFieldStrings()
{
}

/*-----------------------*/

WritePorts()
{
    int first;

    first = 1;

    SetFirst(SIGNALS);
    do {

         type = GetStandardSymbol(TypeCode);
         if (type == 'P')
         {    
              MakeNetName();
              if (newname == 0) {
                   type = GetStandardSymbol(SignalType);

                   if (type == porttype) {
                        if (first == 0) {
                             WriteString(1, ",");
                             WriteCrLf(1);
                             WriteString(1, "|     ");
                        }
                        else {
                             first = 0;
                             if (flag == 1) { WriteString(1, ","); }
                             WriteCrLf(1);
                        
                             if (porttype == 1) { WriteString(1, "| out:("); }
                             if (porttype == 2) { WriteString(1, "| in:("); }
                             if (porttype == 3) { WriteString(1, "| io:("); }
                             if (porttype == 0) { WriteString(1, "| nodes:("); }
                        }

                        WriteSymbol(1, SignalNameString);
                   }
              }
         }
    }while(SetNext(SIGNALS));

    if (first == 0) { WriteString(1, ")"); }

    if (first == 1) {
         if (flag == 1) { flag = 1; }
         else { flag = 0; }
    }
    if (first == 0) { flag = 1; }
}

/*-----------------------*/

WritePins()
{
    int first;

    first = 1;

    SetFirst(SIGNALS);
    do {

         type = GetStandardSymbol(TypeCode);
         if (type == 'P')
         {    
              MakeNetName();
              if (newname == 0) {
                   type = GetStandardSymbol(SignalType);

                   if (type != 0) {
                        if (first == 0) {
                             WriteString(1, ",");
                             WriteCrLf(1);
                             WriteString(1, "|     ");
                        }
                        if (first == 1) {
                             first = 0;
                             if (flag == 1) { WriteString(1, ","); WriteCrLf(1); }
                        
                             WriteString(1, "| Pins:(");
                        }

                        WriteSymbol(1, SignalNameString);
                   }
              }
         }
    }while(SetNext(SIGNALS));

    if (first == 0) { WriteString(1, ")"); }

    if (first == 1) { flag = 0; }
    if (first == 0) { flag = 1; }

}


/*-----------------------*/

WriteSignals()
{
    int first, allassigned;

    allassigned = 1;    /* initially assume all the signals are assigned */
    first = 1;
    flag = 0;

    SetFirst(SIGNALS);
    do {
         MakeNetName();

         if (newname == 0) { allassigned = 0; }
         if (newname == 1) {
              flag = 1;
              if (first == 0) {
                   WriteString(1, ",");
                   WriteCrLf(1);
              }
              if (first == 1) { first = 0; }
              WriteString(1, "| ");
              WriteSymbol(1, SignalNameString);
          }

     }while(SetNext(SIGNALS));

     if (sw_P == 1) {
          WritePins();
          porttype = 0; WritePorts();   /* unspecified ports   */
     }
     else {
          porttype = 1; WritePorts();   /* output ports        */
          porttype = 2; WritePorts();   /* input ports         */
          porttype = 3; WritePorts();   /* bidirectional ports */
          if (sw_U == 1) {
              porttype = 0;             /* unspecified ports   */
              WritePorts();
          }
     }

     WriteCrLf(1);
     WriteString(1, "|");
     WriteCrLf(1);
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
        WriteString(1, "|");
        WriteSymbol(1, PipeLine);
        WriteCrLf(1);

        ok = NextPipe();
        if (ok == 1)
        {
            ok = IsKeyWord();
            ok = 1 - ok;
        }
    }
}

/*-----------------------*/

WriteComments()
{
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
}

/*-----------------------*/

WriteINBISignals()
{
    int first, len, temp, i;

    len = 0;

    WriteString(1, "| Netlist:  ");

    first = 1;
    SetFirst(SIGNALS);
    do {
        type = GetStandardSymbol(TypeCode);

        MakeNetName();

        if (type == 'P')
        {
            type = GetStandardSymbol(SignalType);
            if (type == 2)
            {
                if (len > LineLength)
                {
                    len = 0;
                    first = 1;

                    WriteString(1, ",");
                    WriteCrLf(1);
                    WriteString(1, "|           ");
                }

                if (first != 1) { WriteString(1, ","); }
                WriteSymbol(1, NewNetName);
                first = 0;

                temp = SymbolLength(NewNetName);
                len = len + temp + 1;
            }

            if (type == 3)
            {
                if (len > LineLength)
                {
                    len = 0;
                    first = 1;

                    WriteString(1, ",");
                    WriteCrLf(1);
                    WriteString(1, "|           ");
                }

                if (first != 1) { WriteString(1, ","); }
                WriteSymbol(1, NewNetName);
                first = 0;

                temp = SymbolLength(NewNetName);
                len = len + temp + 1;
            }
        }
    } while (SetNext(SIGNALS));

    if (inwritten == 1)
    {
        if (first == 0)
        {
            WriteString(1, ",");
            WriteCrLf(1);
        }

        if (ILineIndex > 5) { ILineIndex = 5; }

        for (i=1; i<=ILineIndex; i=i+1)
        {
            if (first == 0) { WriteString(1, "|           "); }

            first = 0;

            if (i==1) { WriteSymbol(1, ILine1); }
            if (i==2) { WriteSymbol(1, ILine2); }
            if (i==3) { WriteSymbol(1, ILine3); }
            if (i==4) { WriteSymbol(1, ILine4); }
            if (i==5) { WriteSymbol(1, ILine5); }

            WriteCrLf(1);
        }
    }
}

/*-----------------------*/

WriteOUTBISignals()
{
    int first, len, temp, i;

    len = 0;

    WriteString(1, "|           ");

    first = 1;
    SetFirst(SIGNALS);
    do {
        type = GetStandardSymbol(TypeCode);

        MakeNetName();

        if (type == 'P')
        {
            type = GetStandardSymbol(SignalType);
            if (type == 1)
            {
                if (len > LineLength)
                {
                    len = 0;
                    first = 1;

                    WriteString(1, ",");
                    WriteCrLf(1);
                    WriteString(1, "|           ");
                }

                if (first != 1) { WriteString(1, ","); }
                WriteSymbol(1, NewNetName);
                first = 0;

                temp = SymbolLength(NewNetName);
                len = len + temp + 1;
            }

            if (type == 3)
            {
                if (len > LineLength)
                {
                    len = 0;
                    first = 1;

                    WriteString(1, ",");
                    WriteCrLf(1);
                    WriteString(1, "|           ");
                }

                if (first != 1) { WriteString(1, ","); }
                WriteSymbol(1, NewNetName);
                first = 0;

                temp = SymbolLength(NewNetName);
                len = len + temp + 1;
            }
        }
    } while (SetNext(SIGNALS));

    if (outwritten == 1)
    {
        if (first == 0)
        {
            WriteString(1, ",");
            WriteCrLf(1);
        }

        if (OLineIndex > 5) { OLineIndex = 5; }

        for (i=1; i<=OLineIndex; i=i+1)
        {
            if (first == 0) { WriteString(1, "|           "); }

            first = 0;

            if (i==1) { WriteSymbol(1, OLine1); }
            if (i==2) { WriteSymbol(1, OLine2); }
            if (i==3) { WriteSymbol(1, OLine3); }
            if (i==4) { WriteSymbol(1, OLine4); }
            if (i==5) { WriteSymbol(1, OLine5); }

            WriteCrLf(1);
        }
    }
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
}

/*-----------------------*/

WriteNetListEnd()
{
    int first, ch, len, temp, in, out;

    if (unspecport == 1)
    {
        if        (outflag    == 1) { out = 1; }
        else { if (inflag     == 1) { in  = 1; }}

        if (out == 1)
        {
            outputline = 1;
            AddSig();
            outwritten = 1;
        }
        else { if (in == 1)
        {
            outputline = 0;
            AddSig();
            inwritten = 1;
        }}
    }

    if (sw_W == 1) {
        /* list the signal assignments */
        WriteSignals();

        /* output pipe PLD text */
        FirstPipe();
        ok = AccessKeyWord(PLD);
        while(ok == 1) {
            PipePLD();
            ok = AccessKeyWord(PLD);
        }
    }

    /* write revision, date and title comments */
    WriteComments();

    /* write the input and bidirectional signals */
    WriteINBISignals();

    if (inwritten != 1) { WriteCrLf(1); }
    WriteString(1, "|           -> ");
    WriteCrLf(1);

    /* write the output and bidirectional signals */
    WriteOUTBISignals();
    if (outwritten != 1) { WriteCrLf(1); }

    WriteString(1, "|  {");
    WriteCrLf(1);

    SetFirst(ALL);
    do {
        WriteString(1, "|   ");
        CopySymbol(PartName, Str);
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
        WriteSymbol(1, PartName);
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

            if (len > LineLength)
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
            if (type == 'P')
            {
                MakeNetName(); WriteSymbol(1, NewNetName);

                temp = SymbolLength(NewNetName);
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

        /* write the reference */
        WriteString(1, ")  | ");
        WriteSymbol(1, ReferenceString);
        WriteCrLf(1);
    } while (SetNext(ALL));

    WriteString(1, "|  }");
    WriteCrLf(1);

    if (sw_W == 1) {
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
    }

    WriteCrLf(1);

    WriteString(0, "Done");
    WriteCrLf(0);
}
