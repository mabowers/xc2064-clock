#
# Created by XMAKE Version pre5.0.0m on Sun Mar  6 11:28:37 1994
#
# The following options were used: (none)
#
# The following is the hierarchy of the design 'calc.sch'
#
# calc.sch
#     osc_3k.sch
#     led_inv.sch
#     debounce.sch
#     stack.sch
#     7seg_inv.sch
#     sw7.sch
#     alu.sch
#         andblk2.sch
#         orblk2.sch
#         xorblk2.sch
#         muxblk5.sch
#         muxblk2.sch
#     7segdec.sch
#     control.sch
#         statmach.sch
# 
DEFAULT_TARGET calc.bit

calc.bit : calc.lca 
	makebits -S0 -R2 -XB -YA calc.lca

calc.lca : calc.map 
	ppr calc.map parttype=3020APC68-7
	xdelay -D -W calc.lca

calc.map : calc.xtf 
	xnfmap -P 3020APC68-7 calc.xtf calc.map

calc.xtf : calc.xff 
	xnfprep calc.xff calc.xtf parttype=3020APC68-7

calc.xff : xnf\statmach.xnf xnf\control.xnf xnf\7segdec.xnf xnf\muxblk2.xnf \
xnf\muxblk5.xnf xnf\xorblk2.xnf xnf\orblk2.xnf xnf\andblk2.xnf xnf\alu.xnf \
xnf\sw7.xnf xnf\7seg_inv.xnf xnf\stack.xnf xnf\debounce.xnf xnf\led_inv.xnf \
xnf\osc_3k.xnf xnf\calc.xnf 
	xnfmerge -A -D xnf -D . -P 3020APC68-7 xnf\calc.xnf calc.xff

xnf\calc.xnf : statmach.sch control.sch 7segdec.sch muxblk2.sch muxblk5.sch \
xorblk2.sch orblk2.sch andblk2.sch alu.sch sw7.sch 7seg_inv.sch stack.sch \
debounce.sch led_inv.sch osc_3k.sch calc.sch 
	annotate calc.sch 
	inet calc.sch 
	sdt2xnf calc.inf calc.xnf -D xnf

