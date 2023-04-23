#@(#)mxpart.m	6.2 9/3/93
L ENGLISH

B MXPART -start1131 -end1160 -current1148
: File reader for partlist.xct file and <package>.pkg files

M CANT_OPEN_FILE -e1131 -w1132
> filename s
: Cannot open file '[filename]' for reading.

M NO_PKG -e1133 -w1134
> part s
> filename s
: The part '[part]' could not be found in the file '[filename]'.

M BAD_ATTRIB_VALUE -e1135 -w1136
> partname s
> filename s
: The attribute list (name=value entries) for the part '[partname]' in the
: file '[filename]' has the wrong format.

M ILLEGAL_PKG_KEYWORD -e1137 -w1138
> value s
: Illegal keyword '[value]' exists in the .pkg file.
 
M DUP_PAD_DEF_IN_PKG -e1139
> pad s
: Duplicate definition of PAD '[pad]'.

M DUP_PIN_DEF_IN_PKG -e1140
> pin s
: Duplicate definition of PIN '[pin]'.

M PAD_NUM_OUT_OF_RANGE -e1141
> pad s
: Pad number '[pad]' is out of range.

M BAD_PAD_NUM -e1142
> pad s
: Bad pad number '[pad]'. Should be in the form: PAD<number>.

M WRONG_ATTR_VALUE -e1143
> value s
> chk_value s
: Attribute value '[value]' does not match check value '[chk_value]'.

M ILLEGAL_FAMILY -e1144
> family d
: Not a legal family '[family]'.

M ILLEGAL_FAMILY_ATTR -e1145
> attrib s
> family d 
: Not a legal attribute '[attrib]' for this family '[family]'.

M BAD_ROWCOL -e1146
: Not and integer value for clb row|cols number.

M CLK_BUF -e1147
> pad s
> padn d
: Clock Buffer using '[pad]' should use 'PAD[padn]'.

E MXPART
