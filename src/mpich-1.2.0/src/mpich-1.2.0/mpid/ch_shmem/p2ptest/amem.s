; $CHeader: amem.s 1.1 94/02/21 08:45:17 $
; Copyright 1992 Convex Computer Corp.
;	int __ldcws(void *addr)
;	int __read32(void *addr)
;
;	Note: addr must be on a 16 byte boundary.

        .space  $TEXT$,sort=8
        .subspa $CODE$,quad=0,align=8,access=44,code_only,sort=24
        .export __ldcws32,entry,priv_lev=3,argw0=gr,rtnval=gr
__ldcws32
        .proc
        .callinfo caller,frame=0
        .entry
	ldcws	0(0,%arg0),%ret0
	nop
	bv	%r0(%rp)
	.exit
	nop
        .procend

        .space  $TEXT$
        .subspa $CODE$
        .export __read32,entry,priv_lev=3,argw0=gr,rtnval=gr
__read32
        .proc
        .callinfo caller,frame=0
        .entry
	bv	%r0(%rp)
	.exit
	ldws	0(0,%arg0),%ret0
        .procend
        .end
