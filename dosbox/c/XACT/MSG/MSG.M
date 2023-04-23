#@(#)msg.m	6.3 9/3/93

L ENGLISH

B MSG -start1011 -end1030 -current1016
: The Message System Messages

#
# Message fragments for the MSGFILE error
M MSGIO_EOLJUNK
: Extra characters at the end of a valid line
M MSGIO_UNKNOWN_LINE
: Unknown message file line type
M MSGIO_MSGSET_NAME
: Message set name is required
M MSGIO_MSGDEF_NAME
: Message definition name is required
M MSGIO_MSGARG_NAME
: Message definition argument name is required
M MSGIO_MSGARG_TYPE
: Message definition argument type is required
M MSGIO_UNKNOWN
: Unknown error number

#
# These messages are used by the message system when something goes
# wrong while reading a message file, or instantiating a message
#

M MSGFILE_ERROR -e1011
> theErrorName  m
> theLineNumber d
> theFileName   s
> theLine       s
> thePosition   z
: [theErrorName][.br]
: The error is on Line [theLineNumber] of "[theFileName]"[.br]
: The line is "[theLine]"[.br]
: -------------[thePosition]
M MSGSET_MISSING -e1012
> theCompName s
: The message set for "[theCompName]" does not exist [.br]
: in any of the standard message files.
M MSGSET_NULL
> theCompNumber d
> theCompName s
: The message set pointer for component [theCompNumber], "[theCompName]"[.br]
: does not point to a valid message set.
M MSGSET_NDEFS -e1013
> theMsgnum d
> theMsgname s
> theNumberOfDefs d
> theCompName s
: An attempt was made to invoke message "[theMsgname]",[.br]
: with a message number of [theMsgnum].  However, the message set for[.br]
: component "[theCompName]" has only [theNumberOfDefs] messages.
M MSGSET_MSGNAMES -e1014
> theMsgnum d
> theMsgname s
> theRealMsgname s
> theCompName s
: Message #[theMsgnum] of the message set for component "[theCompName]"[.br]
: was invoked with a name of "[theMsgname]".  Its name in the message[.br]
: file is "[theRealMsgname]".
M MSGSET_NPARAMS -e1015
> theMsgname s
> theNparams d
> theRealNum d
> theCompName s
: Message "[theMsgname]" of the message set for component "[theCompName]"[.br]
: was invoked with [theNparams] parameters.  It is defined with [theRealNum]
: in the message file.

#
# Message id severity names
# These are used in the creation of errors to describe how severe the
# error really is.  THE ORDER OF THESE NAMES IS VERY, VERY IMPORTANT.
# SEE MSGINST_SEVERITY IN MSGINST.C IF YOU DON'T BELIEVE ME.
#

M MSGSEV_INFO
: Information
M MSGSEV_WARNING
: WARNING
M MSGSEV_ERROR
: ERROR
M MSGSEV_FATAL
: Fatal Error
M MSGSEV_INTERNAL
: Internal Error

E MSG
