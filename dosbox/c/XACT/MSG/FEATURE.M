# @(#)feature.m	6.3 9/3/93

L ENGLISH

B FEATURE -start1051 -end1100 -current1082
: Feature License Manager

#
# Nasty Low-Level Error Messages
#
M UNKNOWN_ERROR -e1051
> feature s
: License checkout for feature "[feature]" failed[.br]
: because of some, unknown error condition.  
: Please contact your system administrator for help.
M OS_ERROR -e1052
> feature s
> errtext s
: License checkout for feature "[feature]" failed[.br]
: because of some low-level or operating system error ([errtext]).
: Please contact your system administrator for help.
M BAD_SERVER_DATA_ERROR -e1053
> feature s
: License checkout for feature "[feature]" failed[.br]
: because invalid data was returned from the license server .
: Please contact your system administrator for help.
M READ_SERVER_ERROR -e1054
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] cannot read from the servers listed in the license data 
: file.  Please contact your system adminstrator for help.
M WRITE_SERVER_ERROR -e1055
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] cannot write to the servers listed in the license data 
: file.  Please contact your system adminsitrator for help.
M SELECT_ROUTINE_ERROR -e1056
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] a call to an internal select routine failed.
: Please contact your system adminsitrator for help.
M BUSY_SERVER_ERROR -e1057
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] failed to connect to a majority of the servers listed in the
: license data file.  Please contact your system adminsitrator for help.
M CHECKIN_FEATURE_ERROR -e1058
> feature s
: License checkin for feature "[feature]" failed[.br]
: Don't worry, be happy.

#
# Possible User Errors
#
M NO_LICENSE_FILE_ERROR -e1059
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] could not find the license data file.
: The default location for this file is usually
: /usr/local/data/license.dat.  It may be placed elsewhere at your site, in 
: which case you must define the environmental variable LM_LICENSE_FILE to
: point to it.  Make sure you can access the file from the same shell you are
: trying to run the program.
M NO_FEATURE_ERROR -e1060
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] could not find the feature in the license data
: file.  If you have not yet called customer
: service to obtain the authorization codes for this feature please do so.  If
: you have obtained the codes, please make sure that you have added the codes
: to the license data file.
M LICENSES_EXHAUSTED_ERROR -e1061
> feature s
: License checkout for feature "[feature]" failed[.br]
: because all the valid licenses for the feature are currently in use.  
: Try again later.
M EXPIRED_AUTHCODE_ERROR -e1062
> feature s
: License checkout for feature "[feature]" failed[.br]
: because the required license has expired.  
: Please call customer service to obtain a new license.
M UNSUPPORTED_VERSION_ERROR -e1063
> feature s
: License checkout for feature "[feature]" failed[.br]
: because the license data file does not contain a feature of the required
: version number.  Please call custommer service to obtain a license for 
: the new version of the feature.
M NO_KEY_ERROR -e1064
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because the [program] program could not find a valid Xilinx protection key
: on the parallel port of your computer.  
: Please make sure that the key is attached securely to the parallel port.
M TAMPERED_KEY_ERROR -e1065
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because the [program] program detected an invalid Xilinx protection key
: on the parallel port of your computer.  
: The key's memory has been corrupted.  Please call customer service to
: obtain a new key.
M DEMO_EXPIRED_ERROR -e1066
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because your demo version of the [program] program has expired.
: Please contact customer service
: or your sales representative to purchase the software you have been
: using.
M TAMPERED_DATE_ERROR -e1067
> feature s
: License checkout for feature "[feature]" failed[.br]
: because the current date on your computer is set to a time which is
: before the date when you last ran this program.   
: Please reset your computer's date to the current
: date and rerun the program.
M XACT_VARIABLE_ERROR -e1068
> feature s
: License checkout for feature "[feature]" failed[.br]
: because the XACT environmental variable has not been defined.
: Use the SET XACT=value dos command 
: to define the location of the XACT software.
M INVALID_DATE_ERROR -e1069
> feature s
: License checkout for feature "[feature]" failed[.br]
: because line in the license data file line which describes the feature
: contains an invalid date.  See the file XACT\data\license.dat.  
: All dates must be of the form 
: DD-MMM-YYYY, where DD is an integer 0-31; MMM is the first three 
: letters of the (English) month name: JAN, FEB, MAR, APR, MAY, JUN, JUL,
: AUG, SEP, OCT, NOV, or DEC; and YYYY is an integer 1990-2118.
M LOLEVEL_ERROR -e1070
> feature s
> errtext s
: License checkout for feature "[feature]" failed[.br]
: because of some low-level error ([errtext]).

#
# Possible Installation Errors
#
M MULT_FEATURE_ERROR -e1071
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] detected multiple occurences of FEATURE lines for the 
: feature in the license file.  A feature
: may appear only once in the license file for it to be valid.  Please remove
: one of the feature lines from the license file.
M LICENSE_FILE_SYNTAX_ERROR -e1072
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] detected a syntax error in the license date
: file.  Please make sure that you have 
: correctly entered the authorization codes which you obtained from customer 
: service.
M NO_SERVER_ERROR -e1073
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] could not connect to the network license server.  
: Please make sure that the lmgrd program is 
: currently running on the computers described as servers in the license data 
: file.
M NO_SERVICE_ERROR -e1074
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because the program [program] could not find the TCP "license" service. 
: Make sure that the /etc/services file has
: the license service number defined in it, or add a socket number to the the
: SERVER line of the license data file.
M BAD_AUTHCODE_ERROR -e1075
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] detected an inconsistency in the authorization code on the
: feature line of the license data file.  
: Please make sure that you have correctly entered the authorization codes
: which you obtained from customer service.
M WRONG_HOST_ERROR -e1076
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] could not find the host listed in one of the
: server lines of the license data file.  
: Please make sure that the server information in the license data file is 
: correct.
M BAD_DATE_ERROR -e1077
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] found an invalid date in the license data file.  
: Please make sure that the date information in the
: license data file is correct.  The correct date format is DD-MMM-YY (for
: example 30-JUN-91).  In previous versions of the program, dates of the form
: DD-MMM-YYYY were acceptable, but they are no longer.  To make the correction,
: stop the license server, edit the license.dat file to remove the '19' from
: the year number of all features, restart the license server and rerun the 
: program.
M NO_SERVER_LINES_ERROR -e1078
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] could not find and server lines in the license data file.  
: Please make sure that you have correctly
: entered the all of authorization code information which you obtained from
: customer service.
M BAD_SERVER_NAME_ERROR -e1079
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] encountered an illegal server name in the license 
: data file.  Please make sure that you have correctly
: entered the all of authorization code information which you obtained from
: customer service.
M CONNECT_SERVER_ERROR -e1080
> feature s
> program s
: License checkout for feature "[feature]" failed[.br]
: because [program] could not connect over the network to the license server
: program.  Please make sure that the lmgrd 
: program is currently running on the computers described as servers in the
: license data file.

#
# Error due to PC key
#
M PC_KEY_ERROR -e1081
> feature s
> errtext s
: License checkout for feature "[feature]" failed[.br]
: [errtext]

E FEATURE
