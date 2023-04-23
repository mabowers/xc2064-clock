# @(#)xnf.m	6.13  1/14/94

L ENGLISH

B XNF -start1301 -end1350 -current1315
: Xnf Data Handler

#
# These are message phrases used for syntax errors
#
M SYNTAX_UNK
: Unknown error found 
M SYNTAX_PRN
: A program name is required
M SYNTAX_PAT
: A parttype is required
M SYNTAX_SYN
: A symbol name is required
M SYNTAX_STN
: A symbol type name is required
M SYNTAX_PIN
: A pin name is required
M SYNTAX_PID
: A pin direction character is required
M SYNTAX_SIN
: A signal name is required
M SYNTAX_IPN
: An input pin name is required
M SYNTAX_CPN
: A clock pin name is required
M SYNTAX_CLE
: A clock edge character is required
M SYNTAX_CLP
: A clock polarity character is required
M SYNTAX_PUW
: A pulse width value is required
M SYNTAX_BUN
: A bus name is required
M SYNTAX_PWP
: A power polarity character is required
M SYNTAX_ESD
: An external record signal direction character is required
M SYNTAX_PPN
: Package pin name is required
M SYNTAX_SET
: A setup up time value is required
M SYNTAX_HLD
: A hold time value is required
M SYNTAX_MLR
: The LCANET record is required
M SYNTAX_BRT
: Unrecognized record type found
M SYNTAX_V4R
: Lcanet version 4 is required (Run xnfupd to convert your design)
M SYNTAX_ILV
: Illegal LOC value found
M SYNTAX_LSV
: No value is specified for LOC parameter
M SYNTAX_INT
: An integer is required
M SYNTAX_CHR
: Character is required
M SYNTAX_STR
: A character string is required
M SYNTAX_FLT
: A floating point number is required
M SYNTAX_DLY
: Delay numbers are required
M SYNTAX_BDN
: Illegal delay number specification found
M SYNTAX_COM
: A comma is required
M SYNTAX_EOL
: End of line is required
M SYNTAX_EOF
: End of File is required
M SYNTAX_ERROR 
> errorname m
> linenumber d
> filename s
: [errorname]
: on line [linenumber] of file [filename]:

# 
# These are message phrases used for semantic errors
#
M BADPARTTYPE
> parttype s
: Unrecognized parttype '[parttype]'
M MULTIPLESYM
> symname s
: Multiple symbols exist with the name '[symname]'
M MULTIPLEPIN
> symname s
> pinname s
: The symbol '[symname]' contains multiple pins with the name '[pinname]'
M MULTIPLEBUS
> busname s
: Multiple busses exist with the name '[busname]'
M MULTIPLEEXT
> pinnum s
> signame s
: Multiple Ext-records exist with the same signal '[signame]' or package
: pin name '[pinnum]'
M MULTIPLEEXT2
> signame s
: Multiple Ext-records exist with the same signal '[signame]'
M MULTIPLEHIERG
> hier_id d
> sym_name s
: Multiple Hierg-records exist for hier_id '[hier_id]', sym_name '[sym_name]'
M INSYMBOL
> rectype s
> recname s
: The '[rectype], [recname]' record is illegal between SYM and END records
M RECORD_OUT_OF_PLACE
> record s
: The '[record]' record is illegal outside of SYM and END records
M NOSYMBOL
> rectype s
> recname s
: The '[rectype], [recname]' record is illegal outside of SYM and END records
M BADSETUPEDGE
> inname s
> clkname s
> edge s
: The 'SETUP, [inname], [clkname]' record has an illegal clock edge value '[edge]'
M MULTIPLEPOLARITY
> signame s
: The signal '[signame]' is both GND and VCC
M BADPOWERPOLARITY
> signame s
> polarity s
: The PWR record for signal '[signame]' has an illegal polarity value '[polarity]'
M BADPULSEPOLARITY
> symname s
> pinname s
> polarity s
: The 'PULSE,[pinname]' record in symbol [symname] has an illegal polarity
: value '[polarity]'
M BADEXTDIR
> signame s
> dir s
: The 'EXT, [signame]' record has an illegal direction value '[dir]'
M BADPINDIR
> symname s
> pinname s
> dir s
: The pin [pinname] in symbol '[symname]' has an illegal direction value '[dir]'
M NO_PARAM_VALUE
> rectype s
> recname s
> namevalue s
: The [rectype] '[recname]' contains a parameter '[namevalue]' with no value
: specified
M MULTIPLEPARAM
> rectype s
> recname s
> name s
: The [rectype] '[recname]' contains multiple '[name]' parameters 
M INVALID_SYMBOL_PIN
> symbol  s
> pinname s
: The '[symbol]' symbol pin '[pinname]' is not a legal pin for this type of
: symbol
M INVALID_PIN_INV
> symbol  s
> pinname s
: The '[symbol]' symbol pin '[pinname]' cannot be legally inverted
M INVALID_EXT_PARAM
> ext  s
> param s
: The PAD '[ext]' has an illegal parameter with the name '[param]'
M CONFLICT_EXT_PARAM
> ext  s
> param s
> conflict_param s
: PAD '[ext]' parameters '[param]' and '[conflict_param]' are
: mutually exclusive; they cannot be used together
M INVALID_SIG_PARAM
> sig  s
> param s
: The signal '[sig]' has an illegal parameter with the name '[param]'
M CONFLICT_SYM_PARAM
> symbol  s
> param s
> conflict_param s
: Symbol '[symbol]' parameters '[param]' and '[conflict_param]' are
: mutually exclusive; they cannot be used together
M INVALID_SYM_PARAM
> symbol  s
> param s
: The symbol '[symbol]' has an illegal parameter with the name '[param]'
M INVALID_PIN_PARAM
> pinname s
> symbol  s
> param s
: The '[pinname]' pin on symbol '[symbol]' has an illegal parameter with
: the name '[param]'
M MULTPARTTYPES
> part1 s
> part2 s
: The design network contains multiple parttypes - [part1] and [part2]
M UNKNOWNPIN
> rectype s
> pinname s
: The [rectype] record refers to an undefined pin '[pinname]'
M ILLEGAL_BOOLEAN
> sym s
> numpins d
> numarchpins d
: The symbol '[sym]' has [numpins] input pins specified; only
: [numarchpins] input pins are allowed on this type Boolean gate. 
M MODEL
> rectype s
: The [rectype] record is illegal record outside of MODEL and ENDMOD records
M CANNOT_WRITE_TO_FILE
> filename s
: Cannot write to file '[filename]'.  Some possible causes of this problem are
: the disk being full or problems with writing over a network.
M MISSINGPARAM
> symname s
> symtype s
> paramname s
: The [symtype] symbol '[symname]' is missing the required [paramname] parameter

M INVALID_SIG_NAMES
> siglist s
: The following signals have illegal names:[.br]
: [siglist]
M INVALID_SYM_NAMES
> symlist s
: The following symbols have illegal names:[.br]
: [symlist]
M TOOMANY
> errlimit d
: The number of errors exceeds [errlimit], bailing out prematurely
M WOOPS
> namevalue s
> rectype s
: [namevalue] specified without an active [rectype]
M SEMANTIC_ERROR
> error z
: [error][.br]
M SEMANTIC_ERROR_LOCATION
> error z
> linenum d
> filename s
: [error][.br]
: near line [linenum] of file [filename].

M VALIDITY_NO_ARCH
: The Design Validity Check cannot be implemented because no archfile or
: parttype can be found.

M ERROR_LIST -e1303 -w1304
> filename s
> details z
: The following problem(s) were found in the [filename] file; 
: the file could not be processed successfully.[.br]
: [.br][details]

M VALIDITY_ERRORS -e1305 -w1306
> details z
: [details]

#
# These errors are used for loading xnf archtype data files
#
M CHECKS_PARTTYPE -e1307 -w1308
> designname s
: The parttype for the [designname] design is missing so we cannot perform
: the checks related to legal symbols, pins, parameters, etc.

M CHECKS_UNKNOWN_DATAFILE -e1309 -w1310
> designname s
> parttype s
: The [designname] design is for a parttype called "[parttype]".  We do not
: know which archtype data file we should read for that parttype.  We have
: to read an archtype data file so that we can perform the checks 
: related to legal symbols, pins, parameters, etc.  We are unable, therefore,
: to perform these types of checks.

M CHECKS_NO_DATAFILE -e1311 -w1312
> designname s
> datafilename s
: The [datafilename] data file (which we need to read so that we can perform the
: checks related to legal symbols, pins, parameters, etc. on the [designname]
: design) does not exist in the $XACT/data directory.  We therefore cannot
: perform these types of checks.

M CHECKS_DATAFILE_ERROR -e1313 -w1314
> designname s
> datafilename s
: We encountered problems while reading the [datafilename] data file.  We
: have to read this file so that we can perform the checks related to legal 
: symbols, pins, parameters, etc. in the [designname] design.  We therefore
: cannot perform these types of checks.

E XNF
