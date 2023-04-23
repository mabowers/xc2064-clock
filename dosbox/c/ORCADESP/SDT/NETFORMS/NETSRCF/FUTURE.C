#include "format.h"

static char *switches = "ORSWITCHES \
/N Create a netlist (instead of a pinlist)\n\
/P Output pin numbers (instead of pin names)\n\
/M Assign SIG* attributes to module ports\n\
/K Create CON* symbols for module ports\n\
/V Assign FutureNet power attributes to power objects\n\
/L Do not append sheet number to labels\n";

static char *ver = "VERSION=\"1.20  07-JUL-94\"";
/*
 *  Version 1.20
 *      - Changed ERROR messages to WARNINGS.
 *
 *  Version 1.10a
 *      - Removed checking of legal characters and node name length
 *        (to make compatible with SDT/III)
 *      - Added Version message
 *	     - Added FutureNet power attributes to power objects with /N & /V
 *
 *  Capacities:
 *      - Part Names are limited to 16 characters
 *      - Module Names are not checked for length
 *      - Reference Strings are limited to 6 characters
 *      - Node Names are not checked for length
 *      - Node Numbers are limited to 6 digits (plus the leading '***')
 *      - Pin Numbers are not checked for length
 *
 *  Characters:
 *      - Legal characters are not checked
 */

static int sw_K;
static int sw_L;
static int sw_M;
static int sw_N;
static int sw_P;
static int sw_V;

static int type;
static int len;
static int sym_cnt;

static int Error, PartNameStr, PinNameStr, ReferenceStr, SignalNameStr, Str;
static int SIGNALS, ALL, NODES;

/*-----------------------*/

Initialize()
{
	 WriteString(0, "Creating Netlist...");
    WriteCrLf(0);

    /* OrCAD pin attributes to FutureNet attributes */
    SetPinMap(0, "23");
    SetPinMap(1, "22");
    SetPinMap(2, "21");
    SetPinMap(3, "20");
    SetPinMap(4, "23");
    SetPinMap(5, "20");
    SetPinMap(6, "27");
	 SetPinMap(7, "23");

    sw_K = SwitchIsSet("K");
    sw_M = SwitchIsSet("M");
    sw_N = SwitchIsSet("N");
    sw_P = SwitchIsSet("P");
    sw_V = SwitchIsSet("V");
    sw_L = SwitchIsSet("L");

    if (sw_N == 1)
    {
	     WriteString(1, "NETLIST,2");
        WriteCrLf(1);
        WriteString(1, "(DRAWING,ORCAD.NET,1-1");
        WriteCrLf(1);
		 WriteString(1, "DATA,50,");
		 WriteSymbol(1, TitleString);
		 WriteCrLf(1);
		 WriteString(1, "DATA,51,");
		 WriteSymbol(1, DocumentNumber);
		 WriteCrLf(1);
		 WriteString(1, "DATA,52,");
		 WriteSymbol(1, Revision);
		 WriteCrLf(1);
		 WriteString(1, "DATA,54,");
		 WriteSymbol(1, DateString);
		 WriteCrLf(1);
		 WriteString(1, ")");
	 }
	 else
	 {
		 WriteString(1, "PINLIST,2");
		 WriteCrLf(1);
		 WriteString(1, "(DRAWING,ORCAD.PIN,1-1");
	 }
	 WriteCrLf(1);

	 Error = AddSymbol("Error");
	 SetSymbol(Error, "E");

	 PartNameStr = AddSymbol("PartNameStr");
	 PinNameStr = AddSymbol("PinNameStr");
	 ReferenceStr = AddSymbol("ReferenceStr");
	 SignalNameStr = AddSymbol("SignalNameStr");
	 Str = AddSymbol("Str");

	 SIGNALS = AddSymbol( "SIGNALS" );
	 SetSymbol( SIGNALS, "SIGNALS" );
	 ALL = AddSymbol( "ALL" );
	 SetSymbol( ALL, "ALL" );
	 NODES = AddSymbol( "NODES" );
	 SetSymbol( NODES, "NODES" );

	 SetNumberWidth(6);
    return 0;
}

/*-----------------------*/

WriteHeader()
{
	 /* needed for Netlists and Pinlists */
	 CreatePartDataBase();

	 /* review the Part Names and Reference strings for errors */
	 while (LoadInstance())
    {
        len = SymbolLength(PartName);
        if (len > 16)
        {
            CopySymbol(PartName, PartNameStr);
			 PadSpaces(PartNameStr, 16);

            WriteString(0, "WARNING: Name is too long '");
            WriteSymbol(0, PartName);
            WriteString(0, "', truncated to ");
            WriteSymbol(0, PartNameStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
        }

        len = SymbolLength(ReferenceString);
        if (len > 6)
        {
            CopySymbol(ReferenceString, ReferenceStr);
			 PadSpaces(ReferenceStr, 6);

            WriteString(0, "WARNING: Reference is too long '");
            WriteSymbol(0, ReferenceString);
			 WriteString(0, "', truncated to ");
            WriteSymbol(0, ReferenceStr);
            WriteCrLf(0);

            SetSymbol(ExitType, "W");
		 }
    }
    return 0;
}

/*-----------------------*/

ValidateNodeInfo()
{
    /* globals type, legal, and len are set */
    type = GetStandardSymbol(TypeCode);

	 CopySymbol(SignalNameString, SignalNameStr);

    if (type == 'L')
    {
        if (sw_L == 0)
        {
            SetNumberWidth(1);
            MakeLocalSignal("_");
            SetNumberWidth(6);
            CopySymbol(LocalSignal, SignalNameStr);
        }
    }
    return 0;
}

/*-----------------------*/

WriteNodeInfo()
{
    if (type == 'L')
	 {
        WriteSymbol(3, SignalNameStr);
    }
    if (type == 'P')
    {
		 WriteSymbol(3, SignalNameStr);
    }
    if (type == 'S')
    {
        WriteSymbol(3, SignalNameStr);
    }
    if (type == 'N')
    {
        WriteString(3, "***");
        WriteSymbol(3, NetNumber);
    }
	 if (type == 'U')
    {
        WriteString(3, "***");
        WriteSymbol(3, NetNumber);
    }
    return 0;
}

/*-----------------------*/

SwitchM()
{
    int i;

    if (sw_M == 1)
    {
		 if (type == 'P')
        {
            i = GetStandardSymbol(SignalType);
            if (i == 0) { WriteString(3, "5"); }
			 if (i == 1) { WriteString(3, "11"); }
            if (i == 2) { WriteString(3, "10"); }
            if (i == 3) { WriteString(3, "12"); }
        }
        else
		 {
            WriteString(3, "5");
        }
    }
    else
    {
        WriteString(3, "5");
    }
    return 0;
}

/*-----------------------*/

HandleNodeName()
{
    /* check the node name */
    ValidateNodeInfo();

    if (sw_N == 1)
    {

        WriteString(3, "(SIG,,");
        WriteNodeInfo();

        WriteString(3, ",1-1,");
		 SwitchM();
        WriteString(3, ",");
        WriteNodeInfo();

		 WriteCrLf(3);
    }
    else
    {
        /* write out any net that originally had a label.  Illegal  */
		 /* characters or length may alter its type.                 */
        if (type == 'L')
        {
            WriteString(3, "SIG,");
            WriteNodeInfo();
            WriteString(3, ",1-1,5,");
            WriteNodeInfo();
            WriteCrLf(3);
        }
        if (type == 'P')
        {
			 WriteString(3, "SIG,");
            WriteNodeInfo();
            WriteString(3, ",1-1,");
            SwitchM();
            WriteString(3, ",");
            WriteNodeInfo();
            WriteCrLf(3);
        }
        if (type == 'S')
        {
            WriteString(3, "SIG,");
            WriteNodeInfo();
            WriteString(3, ",1-1,5,");
            WriteNodeInfo();
            WriteCrLf(3);
		 }
    }

    /* needed for Netlists and Pinlists */
	 AddSignalName();
    return 0;
}

/*-----------------------*/

WriteNet()
{
	 int i, j, ok;

    RecordNode();

    if (sw_N == 1)
    {
        WriteString(3, "PIN,1-1,");

		 /* sets PartIndex to be the array index of this Reference */
		 CopySymbol(ReferenceString, ReferenceStr);
		 SetIndexByRef(ReferenceStr);
		 SetNumberWidth(1);
		 WriteSymbol(3, PartIndex);
		 SetNumberWidth(6);
		 WriteString(3, ",");

		 WriteSymbol(3, ReferenceStr);
		 WriteString(3, ",");

		 i = GetStandardSymbol(PinType);


        /* Changed 7/26/93 to include FutureNet power attributes 
	      * to power objects on a "power net".
         */
		 if( sw_V == 1 && type == 'S' )
		 {

		 	j = 0;
		 	ok = 0;

			SetSymbol(PinNameStr, "GND");
			j = CompareSymbol(PinNameString, PinNameStr);
			if (j == 0) {
               WriteString(3, "100");
               ok = 1;
               }
			SetSymbol(PinNameStr, "+5V");
			j = CompareSymbol(PinNameString, PinNameStr);
			if (j == 0) {
               WriteString(3, "101");
               ok = 1;
               }
			SetSymbol(PinNameStr, "+12V");
			j = CompareSymbol(PinNameString, PinNameStr);
			if (j == 0) {
               WriteString(3, "105");
               ok = 1;
               }
			SetSymbol(PinNameStr, "-12V");
			j = CompareSymbol(PinNameString, PinNameStr);
			if (j == 0) {
               WriteString(3, "106");
               ok = 1;
               }
			SetSymbol(PinNameStr, "VEE");
			j = CompareSymbol(PinNameString, PinNameStr);
			if (j == 0) {
               WriteString(3, "107");
               ok = 1;
               }


			if (ok == 0) {
               WriteMap(3, i);
               }

		  }
		  else
			WriteMap(3, i);


		  WriteString(3, ",");

		  if (sw_P == 1)
			{ WriteSymbol(3, PinNumberString); }
		  else
			{ WriteSymbol(3, PinNameString); }

		  WriteCrLf(3);
	 }
    return 0;
}

/*-----------------------*/

WriteNetEnding()
{
	 if (sw_N == 1)
	 {
		 WriteString(3, ")");
		 WriteCrLf(3);
	 }
    return 0;
}

/*-----------------------*/

ProcessFieldStrings()
{
    return 0;
}

/*-----------------------*/

SwitchK()
{
    int i;

    if (sw_K == 1)
    {
        SetFirst(SIGNALS);
        do {
            i = GetStandardSymbol(SignalType);
            if (i != 0)
            {
                ValidateNodeInfo();

                if (type == 'P')
                {
                    sym_cnt = sym_cnt + 1;
                    WriteString(1, "(SYM,");
                    SetNumberWidth(1);
                    WriteInteger(1, sym_cnt); 
                    SetNumberWidth(6);
                    WriteCrLf(1);

                    WriteString(1, "DATA,2,");
                    if (i == 1) { WriteString(1, "CONO"); }
                    if (i == 2) { WriteString(1, "CONI"); }
                    if (i == 3) { WriteString(1, "CONB"); }
                    WriteCrLf(1);
            
                    WriteString(1, "PIN,,");
                    WriteSymbol(1, SignalNameString);
                    WriteString(1, ",1-1,");

                    /* for writing the correct module port attributes */
                    if (sw_M == 1)
                    {
                        if (i == 0) { WriteString(1, "5"); }
                        if (i == 1) { WriteString(1, "11"); }
                        if (i == 2) { WriteString(1, "10"); }
                        if (i == 3) { WriteString(1, "12"); }
                    }
                    else
                    {
                        WriteString(1, "5");
                    }
            
                    if (i == 1) { WriteString(1, ",25"); }
                    if (i == 2) { WriteString(1, ",24"); }
                    if (i == 3) { WriteString(1, ",26"); }

                    if (i == 1) { WriteString(1, ",OUT"); }
                    if (i == 2) { WriteString(1, ",IN"); }
                    if (i == 3) { WriteString(1, ",IO"); }
                    WriteCrLf(1);
                    WriteString(1, ")");
                    WriteCrLf(1);
                }
            }
        } while (SetNext(SIGNALS));
    }
    return 0;
}

/*-----------------------*/

WriteNetListEnd()
{
	 int i;
	 int j;
    int ok;
    int net;

	 net = 0;

    if (sw_N == 0)
    {
        WriteString(3, ")");
        WriteCrLf(3);
    }

    sym_cnt = 0;
	 SetFirst(ALL);
    do {
        CopySymbol(ReferenceString, Str);
        LoadFieldString(Str);

        SetNumberWidth(1);
        if (sw_N == 1)
        {
            WriteString(1, "(SYM,1-1,");
            WriteSymbol(1, PartIndex);
        }
        else
		 {
		  	 WriteString(1, "(SYM,");
		 	 WriteSymbol(1, PartIndex);
		 }
		 SetNumberWidth(6);
		 WriteCrLf(1);

		 /* for numbering CON* symbols if sw_K is set */
		 i = GetStandardSymbol(PartIndex);
		 if (i > sym_cnt) { sym_cnt = i; }

		 WriteString(1, "DATA,2,");
		 CopySymbol(ReferenceString, ReferenceStr);
		 len = SymbolLength(ReferenceString);
		 if (len > 6)
		 {
		 	 PadSpaces(ReferenceStr, 6);
		 }
		 WriteSymbol(1, ReferenceStr);
		 WriteCrLf(1);

		 WriteString(1, "DATA,3,");
		 CopySymbol(PartName, PartNameStr);
		 len = SymbolLength(PartNameStr);
		 if (len > 16)
		 {
		 	 PadSpaces(PartNameStr, 16);
		 }
		 WriteSymbol(1, PartNameStr);
		 WriteCrLf(1);

		 WriteString(1, "DATA,4,");
		 len = SymbolLength(ModuleName);
		 if (len > 0)
		 	 { WriteSymbol(1, ModuleName); }
		 else
		 	 { WriteSymbol(1, PartName); }
		 WriteCrLf(1);

		 do {
		 	if (sw_N == 1)
		 	{
		 		WriteString(1, "DATA,");

		 		ValidateNodeInfo();

		 		i = GetStandardSymbol(PinType);

               /* Changed 7/26/93 to include FutureNet power
                * attributes to power objects in netlist.
                */

				if( sw_V == 1 && type == 'S' )
				{

				    j = 0;
				    ok = 0;

				    SetSymbol(PinNameStr, "GND");
				    j = CompareSymbol(PinNameString, PinNameStr);
				    if (j == 0) {
                       WriteString(1, "100");
                       ok = 1;
                       }
				    SetSymbol(PinNameStr, "+5V");
				    j = CompareSymbol(PinNameString, PinNameStr);
				    if (j == 0) {
                       WriteString(1, "101");
                       ok = 1;
                       }
				    SetSymbol(PinNameStr, "+12V");
				    j = CompareSymbol(PinNameString, PinNameStr);
				    if (j == 0) {
                       WriteString(1, "105");
                       ok = 1;
                       }
				    SetSymbol(PinNameStr, "-12V");
				    j = CompareSymbol(PinNameString, PinNameStr);
				    if (j == 0) {
                       WriteString(1, "106");
                       ok = 1;
                       }
				    SetSymbol(PinNameStr, "VEE");
				    j = CompareSymbol(PinNameString, PinNameStr);
				    if (j == 0) {
                       WriteString(1, "107");
                       ok = 1;
                       }


				    if (ok == 0) {
                       WriteMap(1, i);
                       }

				}
				else
				{
				    WriteMap(1, i);
				}

				WriteString(1, ",");
			}
			else
			{
				 WriteString(1, "PIN,,");

				 ValidateNodeInfo();
				 if (type == 'L')
				 {
					  WriteSymbol(1, SignalNameStr);
				 }
				 if (type == 'P')
				 {
					  WriteSymbol(1, SignalNameStr);
				 }
				 if (type == 'S')
				 {
					  WriteSymbol(1, SignalNameStr);
				 }
				 if (type == 'N')
				 {
					  WriteString(1, "***");
					  WriteSymbol(1, NetNumber);
				 }
				 if (type == 'U')
				 {
					  WriteString(1, "UN");
					  net = net + 1;
					  WriteInteger(1, net);
				 }

				 WriteString(1, ",1-1,");
				 if (sw_M == 1)
				 {
					  if (type == 'L') {
                       WriteString(1, "5");
                       }
					  if (type == 'P')
					  {
							i = GetStandardSymbol(SignalType);
							if (i == 0) { WriteString(1, "5"); }
							if (i == 1) { WriteString(1, "11"); }
							if (i == 2) { WriteString(1, "10"); }
							if (i == 3) { WriteString(1, "12"); }
					  }
					  if (type == 'S') { WriteString(1, "5"); }
					  if (type == 'N') { WriteString(1, "5"); }
					  if (type == 'U') { WriteString(1, "5"); }
				 }
				 else
				 {
					  WriteString(1, "5");
				 }
				 WriteString(1, ",");

				 i = GetStandardSymbol(PinType);
				 if (sw_V == 1)
				 {
					  if (type == 'S')
					  {
							j = 0;
							ok = 0;

							SetSymbol(PinNameStr, "GND");
							j = CompareSymbol(PinNameString, PinNameStr);
							if (j == 0) {
                               WriteString(1, "100");
                               ok = 1;
                               }
							SetSymbol(PinNameStr, "+5V");
							j = CompareSymbol(PinNameString, PinNameStr);
							if (j == 0) {
                               WriteString(1, "101");
                               ok = 1;
                               }
							SetSymbol(PinNameStr, "+12V");
							j = CompareSymbol(PinNameString, PinNameStr);
							if (j == 0) {
                               WriteString(1, "105");
                               ok = 1;
                               }
							SetSymbol(PinNameStr, "-12V");
							j = CompareSymbol(PinNameString, PinNameStr);
							if (j == 0) {
                               WriteString(1, "106");
                               ok = 1;
                               }
							SetSymbol(PinNameStr, "VEE");
							j = CompareSymbol(PinNameString, PinNameStr);
							if (j == 0) {
                               WriteString(1, "107");
                               ok = 1;
                               }

							if (ok == 0) {
                               WriteMap(1, i);
                               }
					  }
					  else
					  {
							WriteMap(1, i);
					  }
				 }
				 else
				 {
					  WriteMap(1, i);
				 }

				 WriteString(1, ",");
			}

			if (sw_P == 1)
				 { WriteSymbol(1, PinNumberString); }
			else
				 { WriteSymbol(1, PinNameString); }

			WriteCrLf(1);
		 } while (SetNext(NODES));

		  WriteString(1, ")");
		  WriteCrLf(1);

	 } while (SetNext(ALL));

	 /* for creating the CON* objects -- if necessary */
	 SwitchK();

	 /* add the net oriented info to the end of the part  */
	 /* oriented info for Netlist formats.                */
	 ConcatFile(1, 3);

	 WriteString(0, "Done");
	 WriteCrLf(0);
    return 0;
}

