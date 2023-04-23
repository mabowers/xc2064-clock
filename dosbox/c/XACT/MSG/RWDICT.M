#TMF 1.0
# @(#)rwdict.m	6.2  9/3/93

L ENGLISH

B RWDICT -start1031 -end1050 -current1035
: Dictionary File Reader/Writer

M MISSING_EQUALSIGN -e1031 -w1032
> linenumber d
> filename   s
> dicttype   s
> line       s
: Missing equal sign in the name-value assignment[.br]
: in the [dicttype] "[filename]" at line [linenumber][.br]
: "[line]"

M ILLEGAL_NAME -e1033 -w1034
> linenumber d
> filename   s
> dicttype   s
> name       s
: Illegal name used in the name-value assigment[.br]
: in the [dicttype] "[filename]" at line [linenumber][.br]
: "[name]"


E RWDICT
