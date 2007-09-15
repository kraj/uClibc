// XXX add ops from glibc sysdeps/sparc/sparc64/soft-fp

#ifdef __sparc_v9__

#define fakedef(name)                                                   \
    void name(void)                                                     \
    {                                                                   \
        printf("Unimplemented %s called, exiting\n", #name);            \
        exit(-1);                                                       \
    }

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
#undef fakedef

#endif

