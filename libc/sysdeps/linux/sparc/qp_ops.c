/* XXX add ops from glibc sysdeps/sparc/sparc64/soft-fp */

#define fakedef(name)                                                   \
    void name(void)                                                     \
    {                                                                   \
        printf("Unimplemented %s called, exiting\n", #name);            \
        exit(-1);                                                       \
    }

#ifdef __sparc_v9__
fakedef(_Qp_fne)
fakedef(_Qp_feq)
fakedef(_Qp_div)
fakedef(_Qp_flt)
fakedef(_Qp_mul)
fakedef(_Qp_fge)
fakedef(_Qp_qtoux)
fakedef(_Qp_uxtoq)
fakedef(_Qp_sub)
fakedef(_Qp_dtoq)
fakedef(_Qp_qtod)
fakedef(_Qp_qtos)
fakedef(_Qp_stoq)
fakedef(_Qp_itoq)
fakedef(_Qp_add)
#else
fakedef(_Q_fne)
fakedef(_Q_feq)
fakedef(_Q_div)
fakedef(_Q_flt)
fakedef(_Q_mul)
fakedef(_Q_fge)
fakedef(_Q_qtoux)
fakedef(_Q_uxtoq)
fakedef(_Q_qtou)
fakedef(_Q_utoq)
fakedef(_Q_sub)
fakedef(_Q_dtoq)
fakedef(_Q_qtod)
fakedef(_Q_qtos)
fakedef(_Q_stoq)
fakedef(_Q_itoq)
fakedef(_Q_add)
#endif

#undef fakedef
