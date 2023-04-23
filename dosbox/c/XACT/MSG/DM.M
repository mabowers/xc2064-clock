# @(#)dm.m	6.5  2/2/94

L ENGLISH

B DM -start1161 -end1200 -current1187
: Data Manager 

M CANNOT_WRITE_TO_FILE -e1161 -w1162
> filename s
: Cannot write file '[filename]'.  Some possible causes of this problem are
: the disk being full or problems with writing over a network.

M ADDHISTORY_FAILED -e1163 -w1164
> cellname z
> celltype s
: A history description could not be appended to the [celltype] cell
: "[cellname]" because it is a READONLY cell

M CLRHISTORY_FAILED -e1165 -w1166
> cellname z
> celltype s
: The history descriptions of the [celltype] cell "[cellname]" could not be
: cleared because it is a READONLY cell

M CLEAR_FAILED_RDONLY -e1167 -w1168
> cellname z
> celltype s
: The [celltype] cell "[cellname]" could not be cleared because it is a
: READONLY cell

#M CLEAR_FAILED -e -w
#> cellname z
#> celltype s
#: The [celltype] cell "[cellname]" could not be cleared because it is
#: locked in memory

M DELETE_FAILED -e1169 -w1170
> cellname z
> celltype s
: The [celltype] cell "[cellname]" could not be deleted because it is
: locked in memory

M FILE_ACCESS -e1171 -w1172
> cellname z
> filename z
> celltype s
: The file "[filename]" associated with the [celltype] cell "[cellname]"
: exists, but could not be opened

M FILE_MISSING -e1173 -w1174
> cellname z
> filename z
> celltype s
: The file "[filename]" associated with the [celltype] cell "[cellname]"
: could not be found.

M FILE_OPEN_FAILED -e1175 -w1176
> cellname z
> filename z
> celltype s
: The file "[filename]" associated with the [celltype] cell "[cellname]"
: could not be opened

M HEADER_CELLTYPENAME -e1177 -w1178
> cellname z
> filename z
> celltype s
: The type of the cell "[cellname]" associated with the file "[filename]"
: does not correspond to the expected "[celltype]" type.

M HEADER_ERROR -e1179 -w1180
> cellname z
> filename z
> celltype s
: The file "[filename]" for the [celltype] cell "[cellname]" contains an
: illegal header

M NAME_FAILED -e1181 -w1182
> cellname z
> newname  s
> celltype s
: The [celltype] cell "[cellname]" could not be renamed to "[newname]"
: because a [celltype] cell named "[newname]" already exists

M TYPE_FAILED -e1183 -w1184
> typename s
> cellname z
: The type of cell "[cellname]" could not be changed to "[typename]"
: because a cell of that name/type already exists

M NEWCELL_FAILED -e1185 -w1186
> cellname s
> celltype s
: The [celltype] cell "[cellname]" could not be created because a cell of
: that type and name already exists

#
# Data Manager Messages
#
M SAVECELL_SECTION
: Data Manager Auto Save

M SAVECELL
> cellname z
> typename s
: Saving [typename] cell "[cellname]"

M LOCKCOUNT_SECTION
: Data Manager Cell Lock Counts

M LOCKCOUNT
> cellname z-10
> typename s-10
> lockcount d2
: [cellname] [typename] [lockcount]

E DM




