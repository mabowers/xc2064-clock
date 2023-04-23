/*----------------------------------------------------------------------
 * Header file for formatter C files
 *----------------------------------------------------------------------
 */
/*----------------------------
 * standard symbol definitions
 *----------------------------
 */
#define SignalNameString   0
#define SignalType         1
#define TypeCode           2
#define ReferenceString    3
#define PinNumberString    4
#define PinNameString      5
#define PinIndex           6
#define PartName           7
#define ModuleName         8
#define PinType            9
#define NetNumber          10
#define TimeStamp          11
#define FieldString1       12
#define FieldString2       13
#define FieldString3       14
#define FieldString4       15
#define FieldString5       16
#define FieldString6       17
#define FieldString7       18
#define FieldString8       19
#define KeyWord            20
#define PipeLine           21
#define SheetNumber        22
#define TotalSheets        23
#define SheetSize          24
#define DocumentNumber     25
#define DateString         26
#define Revision           27
#define TitleString        28
#define Organization       29
#define AddressLine1       30
#define AddressLine2       31
#define AddressLine3       32
#define AddressLine4       33
#define FileName           34
#define PartIndex          35
#define LibraryNameString  36
#define LookupNameString   37
#define NetNameString      38
#define NetCode            39
#define NetType            40

#define ExitType           0 + 100
#define LocalSignal        1 + 100


extern void  AddSignalName(void);
extern unsigned int  SetIndexByRef(unsigned int symbolId);
extern unsigned int  RecordNode(void);
extern unsigned int  SetNext(unsigned int  symbolId);
extern unsigned int  SetFirst(unsigned int  symbolId);
extern void  CreatePartDataBase(void);
extern unsigned int  SortByNumber(void);
extern unsigned int  StripPath(unsigned int  symbolId);
extern unsigned int  LoadFieldString(unsigned int  symbolId);
extern unsigned char  LastFile(void);
extern unsigned int  LoadPin(void);
extern unsigned int  LoadFirstPin(void);
extern unsigned char  SetSignal(void);
extern unsigned int  LoadInstance(void);
extern unsigned char  NextInstance(void);
extern unsigned int  SetAccessType(char  *stringa);
extern unsigned char  MakeInstanceFile(void);
extern unsigned int  SetTraversal(char  *stringa);
extern unsigned char  RewindInstanceFile(void);
extern unsigned char  NextAccessType(void);
extern unsigned char  PreviousNode(void);
extern unsigned char  NextNode(void);
extern unsigned char  FirstNode(void);
extern unsigned char  NextNet(void);
extern unsigned char  FirstNet(void);
extern unsigned int  WriteInteger(unsigned int  file,int  theInt);
extern unsigned int  PackString(unsigned int  indexa,unsigned int  indexb,unsigned int  symbolId1,unsigned int  symbolId2);
extern unsigned char  EndNode(void);
extern unsigned int  FToUpper(unsigned int  symbolId);
extern unsigned int  ExceptionsFor(char  *stringa,unsigned int  symbolId);
extern unsigned int  ConcatFile(unsigned int  filea,unsigned int  fileb);
extern unsigned char  NextPin(void);
extern unsigned char  FirstPin(void);
extern unsigned int  MakeLocalSignal(char  *stringa);
extern unsigned int  SwitchIsSet(char  *stringa);
extern unsigned int  FindSymbolChar(unsigned int  index,unsigned int  symbolId);
extern unsigned char  NextPipe(void);
extern unsigned char  FirstPipe(void);
extern unsigned char  NextKeyWord(void);
extern unsigned char  IsKeyWord(void);
extern unsigned int  AccessKeyWord(unsigned int  symbolId);
extern unsigned int  CompareSymbol(unsigned int  symbolId1,unsigned int  symbolId2);
extern unsigned char  SetPartIndex(unsigned int  index);
extern unsigned int  AccessPart(unsigned int  symbolId);
extern unsigned char  NextPart(void);
extern unsigned char  FirstPart(void);
extern unsigned int PinCount(void);
extern unsigned int  ChildPinCount(void);
extern unsigned char  NextChildPin(void);
extern unsigned char  FirstChildPin(void);
extern unsigned int  AccessChild(unsigned int  symbolId);
extern unsigned char  NextChild(void);
extern unsigned int  SymbolLength(unsigned int  symbolId);
extern unsigned int  WriteString(unsigned int  file,char  *stringa);
extern unsigned int  SetNumberWidth(unsigned int  width);
extern unsigned int  GetSymbolChar(unsigned int  index,unsigned int  symbolId);
extern unsigned int  PutSymbolChar(unsigned int  index,unsigned int  ch,unsigned int  symbolId);
extern unsigned int  SymbolInCharSet(unsigned int  symbolId);
extern unsigned int  SetCharSet(char  *stringa);
extern unsigned int  WriteMap(unsigned int  file,unsigned int  index);
extern unsigned int  SetPinMap(unsigned int  index,char  *stringa);
extern unsigned int  CopySymbol(unsigned int  sourceId,unsigned int  destId);
extern unsigned int  PadSpaces(unsigned int  symbolId,unsigned int  count);
extern unsigned char  FirstChild(void);
extern unsigned int  GetStandardSymbol(unsigned int  symbolId);
extern unsigned int  WriteCrLf(unsigned int  file);
extern unsigned int  WriteSymbol(unsigned int  file,unsigned int  symbolId);
extern unsigned int  ClearSymbolicStrings(void);
extern unsigned int  SetSymbol(unsigned int  symbolId,char  *value);
extern unsigned int  AddSymbol(char  *symbola);
extern unsigned int  getnum(void);
extern unsigned int  print(char  *stringa);
