#!/usr/bin/perl
# Silly script to fixup the uClibc config file
# (c) Erik Andersen <andersee@codepoet.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

use strict;
use Getopt::Long;

# User Specified variables (commandline)
my($arch)	    = "";
my($cross)	    = "";
my($xcc)	    = "";
my($native_cc)	    = "";
my($devel_prefix)   = "/usr/$arch-linux-uclibc";
my($kernel_dir)	    = "/usr/src/linux";
my($ldso_path)	    = "$devel_prefix/lib";
my($mmu)	    = "true";
my($large_file)	    = "false";
my($rpc_support)    = "false";
my($c99_math)	    = "false";
my($shared_support) = "true";
my($shadow)	    = "false";
my($pic)	    = "true";
my($filename)	    = "";
my($line);
my($got_arch);

# Get commandline parameters
Getopt::Long::Configure("no_ignore_case", "bundling");
&GetOptions(	"arch=s" => \$arch,
		"cross=s" => \$cross,
		"cc=s" => \$xcc,
		"native_cc=s" => \$native_cc,
		"devel_prefix=s" => \$devel_prefix,
		"kernel_dir=s" => \$kernel_dir,
		"mmu=s" => \$mmu,
		"large_file=s" => \$large_file,
		"rpc_support=s" => \$rpc_support,
		"c99_math=s" => \$c99_math,
		"shadow=s" => \$shadow,
		"shared_support=s" => \$shared_support,
		"ldso_path=s" => \$ldso_path,
		"pic=s" => \$pic,
		"file=s" => \$filename,
		);
chomp($arch);
chomp($cross);
chomp($xcc);
chomp($native_cc);
chomp($devel_prefix);
chomp($kernel_dir);
chomp($mmu);
chomp($large_file);
chomp($rpc_support);
chomp($c99_math);
chomp($shadow);
chomp($shared_support);
chomp($ldso_path);
chomp($pic);
chomp($filename);

if ($filename) {
	open(FILE,"<$filename") or
		    die "(fatal) Can't open $filename:$!";
} else {
    die "(fatal) Please give me a --file argument$!";
}


while($line = <FILE>) {
    if ($line =~ /^TARGET_ARCH.*/) {
	print "TARGET_ARCH=\"$arch\"\n";
	$got_arch=1;
	next;
    } 
    if ($cross && $line =~ /^CROSS.*/) {
	print "CROSS=\"$cross\"\n";
	next;
    }
    if ($xcc && $line =~ /^CC.*/) {
	print "CC=\"$xcc\"\n";
	next;
    }
    if ($native_cc && $line =~ /^NATIVE_CC.*/) {
	print "NATIVE_CC=\"$native_cc\"\n";
	next;
    }
    if ($line =~ /^DEVEL_PREFIX.*/) {
	print "DEVEL_PREFIX=\"$devel_prefix\"\n";
	next;
    }
    if ($line =~ /^KERNEL_SOURCE.*/) {
	print "KERNEL_SOURCE=\"$kernel_dir\"\n";
	next;
    }
    if ($line =~ /^HAS_MMU.*/) {
	print "HAS_MMU=$mmu\n";
	next;
    }
    if ($line =~ /^DOLFS.*/) {
	print "DOLFS=$large_file\n";
	next;
    }
    if ($line =~ /^INCLUDE_RPC.*/) {
	print "INCLUDE_RPC=$rpc_support\n";
	next;
    }
    if ($line =~ /^HAS_SHADOW.*/) {
	print "HAS_SHADOW=$shadow\n";
	next;
    }
    if ($line =~ /^DO_C99_MATH.*/) {
	print "DO_C99_MATH=$c99_math\n";
	next;
    }
    if ($shared_support == "true") {
	if ($line =~ /^BUILD_UCLIBC_LDSO.*/) {
	    print "BUILD_UCLIBC_LDSO=true\n";
	    next;
	}
	if ($line =~ /^HAVE_SHARED.*/) {
	    print "HAVE_SHARED=true\n";
	    next;
	}
	# Force PIC to be true when HAVE_SHARED is true
	if ($line =~ /^DOPIC.*/) {
	    print "DOPIC=true\n";
	    next;
	}
	if ($line =~ /^SHARED_LIB_LOADER_PATH.*/) {
	    print "SHARED_LIB_LOADER_PATH=\"$ldso_path\"\n";
	    next;
	}
    } else {
	if ($line =~ /^BUILD_UCLIBC_LDSO.*/) {
	    print "BUILD_UCLIBC_LDSO=false\n";
	    next;
	}
	if ($line =~ /^HAVE_SHARED.*/) {
	    print "HAVE_SHARED=false\n";
	    next;
	}
	if ($line =~ /^DOPIC.*/) {
	    print "DOPIC=false\n";
	    next;
	}
    }
    print "$line";
}

if (! $got_arch) {
    print "TARGET_ARCH=$arch\n";
}

