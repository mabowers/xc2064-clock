#@(#)speed.m	6.4  9/3/93
L ENGLISH

B SPEED -start 1101 -end1130 -current14
: File reader for speeds.xct file 

M CANT_OPEN_FILE -e0 -w1
> filename s
: Cannot open file '[filename]' for reading.

M FILE_CORRUPTED -e2 -w3
> filename s
: The speeds file '[filename]' has been corrupted and cannot be read.

M CANT_READ_FILE -e4 -w5
> filename s
: The unencrypted speeds file '[filename]' cannot be read.

M BAD_PARTLIST_FILE -e6 -w7
: The partlist.xct file does not contain the correct information.

M UNKNOWN_SPEED_AND_PART -e8 -w9
> speed s
> part s
: The speed '[speed]' is unavailable for part '[part]'.

M BAD_VERSION -e10 -w11
> filename s
> badversion s
> version s
: The '[filename]' file has a bad version, '[badversion]'.
: The version of file must be '[version]'.

M SYNTAX_ERROR -e12 -w13
> filename s
: There is a syntax error in the '[filename]' speeds file.

E SPEED
