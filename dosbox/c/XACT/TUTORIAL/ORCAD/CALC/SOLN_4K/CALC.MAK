#
# Created by XMAKE Version pre5.0.0n on Wed Mar 16 07:55:15 1994
#
# The following options were used: -V 
#
# The following is the hierarchy of the design 'calc.sch'
#
# calc.sch
#     osc_4k.sch
#     led_inv.sch
#     debounce.sch
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
#         state_4k.sch
#     stack_4k.sch
# 
DEFAULT_TARGET calc.bit

calc.bit : calc.lca 
	makebits calc.lca

calc.lca : calc.xtf 
	ppr calc.xtf cstfile=calc_4k.cst parttype=4003PC84-6
	xdelay -D -W calc.lca

calc.xtf : calc.xff 
	xnfprep calc.xff calc.xtf ignore_xnf_locs=IO parttype=4003PC84-6

calc.xff : xnf\stack_4k.xnf xnf\state_4k.xnf xnf\control.xnf xnf\7segdec.xnf \
xnf\muxblk2.xnf xnf\muxblk5.xnf xnf\xorblk2.xnf xnf\orblk2.xnf \
xnf\andblk2.xnf xnf\alu.xnf xnf\sw7.xnf xnf\7seg_inv.xnf xnf\debounce.xnf \
xnf\led_inv.xnf xnf\osc_4k.xnf xnf\calc.xnf 
	xnfmerge -A -D xnf -D . -P 4003PC84-6 xnf\calc.xnf calc.xff

xnf\calc.xnf : stack_4k.sch state_4k.sch control.sch 7segdec.sch muxblk2.sch \
muxblk5.sch xorblk2.sch orblk2.sch andblk2.sch alu.sch sw7.sch 7seg_inv.sch \
debounce.sch led_inv.sch osc_4k.sch calc.sch 
	annotate calc.sch 
	inet calc.sch 
	sdt2xnf calc.inf calc.xnf -D xnf 

