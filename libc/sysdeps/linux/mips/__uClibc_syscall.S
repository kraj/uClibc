/*
 * $Id: __uClibc_syscall.S,v 1.1 2002/08/09 13:43:21 andersen Exp $
 *
 * __uClibc_syscall.S - MIPS version.
 *
 * Derived from x86 implementation to reduce memory footprint
 * for syscalls.
 *
 * Mark Rustad, 03/21/2002
 * Copyright 2002 BRECIS Communications Corp.
 *
 * uClibc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with uClibc; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

 #include <sys/regdef.h>

	.text
	.align  4
	.globl  __uClibc_syscall
	.ent    __uClibc_syscall,0
	.type   __uClibc_syscall,@function
__uClibc_syscall:
	syscall
	.set    noreorder
	beqz    a3, 1f
	lui     t3, %hi(errno)
	.set    reorder
	negu    a1, v0
	sw      a1, %lo(errno)(t3)
	li      v0, -1
1:
	j       ra
	.end    __uClibc_syscall

.Lsize:
	.size   __uClibc_syscall,.Lsize-__uClibc_syscall

