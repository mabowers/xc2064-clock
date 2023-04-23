# @(#)mxcomm.m	6.2  9/3/93

L ENGLISH

B MXCOMM -start1001 -end1010 -current1003
: Mx Program Communications

# Boolean values
M TRUE_VALUES
: Y YES T TRUE ON
M FALSE_VALUES
: N NO F FALSE OFF

# These two messages are used in rwdict error messages
M STARTUP_NAME
: startup file

# These error messages are used by mxaccs directly
M STARTUP_OPEN_FAILED -e1001
> filename z
: The Xilinx Automatic CAE Tools initialization file "[filename]"[.br]
: exists, but could not be opened for some reason

M NO_STARTUP_FILE -e1002
> filename z
: The Xilinx Automatic CAE Tools initialization file "[filename]"[.br]
: does not exist, so it could not be opened

E MXCOMM
