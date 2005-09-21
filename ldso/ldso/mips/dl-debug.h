/* vi: set sw=4 ts=4: */
/* mips/mipsel ELF shared library loader suppport
 *
   Copyright (C) 2002, 2005 Steven J. Hill (sjhill@realitydiluted.com)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. The name of the above contributors may not be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

static const char *_dl_reltypes_tab[R_MIPS_NUM];

void _dl_populate_reltypes_tab(void)
{
	_dl_reltypes_tab [0] = "R_MIPS_NONE";
	_dl_reltypes_tab [1] = "R_MIPS_16";
	_dl_reltypes_tab [2] = "R_MIPS_32";
	_dl_reltypes_tab [3] = "R_MIPS_REL32";
	_dl_reltypes_tab [4] = "R_MIPS_26";
	_dl_reltypes_tab [5] = "R_MIPS_HI16";
	_dl_reltypes_tab [6] = "R_MIPS_LO16";
	_dl_reltypes_tab [7] = "R_MIPS_GPREL16";
	_dl_reltypes_tab [8] = "R_MIPS_LITERAL";
	_dl_reltypes_tab [9] = "R_MIPS_GOT16";
	_dl_reltypes_tab[10] = "R_MIPS_PC16";
	_dl_reltypes_tab[11] = "R_MIPS_CALL16";
	_dl_reltypes_tab[12] = "R_MIPS_GPREL32";
	_dl_reltypes_tab[13] = "";
	_dl_reltypes_tab[14] = "";
	_dl_reltypes_tab[15] = "";
	_dl_reltypes_tab[16] = "R_MIPS_SHIFT5";
	_dl_reltypes_tab[17] = "R_MIPS_SHIFT6";
	_dl_reltypes_tab[18] = "R_MIPS_64";
	_dl_reltypes_tab[19] = "R_MIPS_GOT_DISP";
	_dl_reltypes_tab[20] = "R_MIPS_GOT_PAGE";
	_dl_reltypes_tab[21] = "R_MIPS_GOT_OFST";
	_dl_reltypes_tab[22] = "R_MIPS_GOT_HI16";
	_dl_reltypes_tab[23] = "R_MIPS_GOT_LO16";
	_dl_reltypes_tab[24] = "R_MIPS_SUB";
	_dl_reltypes_tab[25] = "R_MIPS_INSERT_A";
	_dl_reltypes_tab[26] = "R_MIPS_INSERT_B";
	_dl_reltypes_tab[27] = "R_MIPS_DELETE";
	_dl_reltypes_tab[28] = "R_MIPS_HIGHER";
	_dl_reltypes_tab[29] = "R_MIPS_HIGHEST";
	_dl_reltypes_tab[30] = "R_MIPS_CALL_HI16";
	_dl_reltypes_tab[31] = "R_MIPS_CALL_LO16";
	_dl_reltypes_tab[32] = "R_MIPS_SCN_DISP";
	_dl_reltypes_tab[33] = "R_MIPS_REL16";
	_dl_reltypes_tab[34] = "R_MIPS_ADD_IMMEDIATE";
	_dl_reltypes_tab[35] = "R_MIPS_PJUMP";
	_dl_reltypes_tab[36] = "R_MIPS_RELGOT";
	_dl_reltypes_tab[37] = "R_MIPS_JALR";
	_dl_reltypes_tab[38] = "R_MIPS_TLS_DTPMOD32";
	_dl_reltypes_tab[39] = "R_MIPS_TLS_DTPREL32";
	_dl_reltypes_tab[40] = "R_MIPS_TLS_DTPMOD64";
	_dl_reltypes_tab[41] = "R_MIPS_TLS_DTPREL64";
	_dl_reltypes_tab[42] = "R_MIPS_TLS_GD";
	_dl_reltypes_tab[43] = "R_MIPS_TLS_LDM";
	_dl_reltypes_tab[44] = "R_MIPS_TLS_DTPREL_HI16";
	_dl_reltypes_tab[45] = "R_MIPS_TLS_DTPREL_LO16";
	_dl_reltypes_tab[46] = "R_MIPS_TLS_GOTTPREL";
	_dl_reltypes_tab[47] = "R_MIPS_TLS_TPREL32";
	_dl_reltypes_tab[48] = "R_MIPS_TLS_TPREL64";
	_dl_reltypes_tab[49] = "R_MIPS_TLS_TPREL_HI16";
	_dl_reltypes_tab[50] = "R_MIPS_TLS_TPREL_LO16";
}
