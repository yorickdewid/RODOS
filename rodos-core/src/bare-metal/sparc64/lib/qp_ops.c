/**
 * @author Andre Bartke <dev@bartke.cc>
 *
 * @brief Quad word floating point stubs.
 * Currently unsupported, to avoid the unnecessary calling of trap handlers we
 * can specify -msoft-quad-float that invokes the calling of ABI funcions.
 * Since these are not going to be used, the symbols that are provided here
 * are just stubs. Necessary symbols are taken from
 * glibc/sysdeps/sparc/sparc64/soft-fp/qp_***.c
 */

#include <asm.h>

#define qp_op(name)                            \
void name(void) {                                   \
  panic("Error: %s is not implemented.\n", #name);  \
}

qp_op(_Qp_fne)
qp_op(_Qp_feq)
qp_op(_Qp_div)
qp_op(_Qp_flt)
qp_op(_Qp_fgt)
qp_op(_Qp_mul)
qp_op(_Qp_fge)
qp_op(_Qp_qtoux)
qp_op(_Qp_uxtoq)
qp_op(_Qp_sub)
qp_op(_Qp_dtoq)
qp_op(_Qp_qtod)
qp_op(_Qp_qtos)
qp_op(_Qp_stoq)
qp_op(_Qp_itoq)
qp_op(_Qp_add)
qp_op(_Qp_xtoq)
qp_op(_Qp_qtox)
