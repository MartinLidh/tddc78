dnl
dnl record top-level directory (this one)
dnl A problem.  Some systems use an NFS automounter.  This can generate
dnl paths of the form /tmp_mnt/... . On SOME systems, that path is
dnl not recognized, and you need to strip off the /tmp_mnt. On others, 
dnl it IS recognized, so you need to leave it in.  Grumble.
dnl The real problem is that OTHER nodes on the same NFS system may not
dnl be able to find a directory based on a /tmp_mnt/... name.
dnl
dnl It is WRONG to use $PWD, since that is maintained only by the C shell,
dnl and if we use it, we may find the 'wrong' directory.  To test this, we
dnl try writing a file to the directory and then looking for it in the 
dnl current directory.  Life would be so much easier if the NFS automounter
dnl worked correctly.
dnl
dnl PAC_GETWD(varname [, filename ] )
dnl 
dnl Set varname to current directory.  Use filename (relative to current
dnl directory) if provided to double check.
dnl
dnl Need a way to use "automounter fix" for this.
dnl
define(PAC_GETWD,[
AC_MSG_CHECKING(for current directory name)
$1=$PWD
if test "${$1}" != "" -a -d "${$1}" ; then 
    if test -r ${$1}/.foo$$ ; then
        /bin/rm -f ${$1}/.foo$$
	/bin/rm -f .foo$$
    fi
    if test -r ${$1}/.foo$$ -o -r .foo$$ ; then
	$1=
    else
	echo "test" > ${$1}/.foo$$
	if test ! -r .foo$$ ; then
            /bin/rm -f ${$1}/.foo$$
	    $1=
        else
 	    /bin/rm -f ${$1}/.foo$$
	fi
    fi
fi
if test "${$1}" = "" ; then
    $1=`pwd | sed -e 's%/tmp_mnt/%/%g'`
fi
dnl
dnl First, test the PWD is sensible
ifelse($2,,,
if test ! -r ${$1}/$2 ; then
    dnl PWD must be messed up
    $1=`pwd`
    if test ! -r ${$1}/$2 ; then
	print_error "Cannot determine the root directory!" 
        exit 1
    fi
    $1=`pwd | sed -e 's%/tmp_mnt/%/%g'`
    if test ! -d ${$1} ; then 
        print_error "Warning: your default path uses the automounter; this may"
        print_error "cause some problems if you use other NFS-connected systems."
        $1=`pwd`
    fi
fi)
if test -z "${$1}" ; then
    $1=`pwd | sed -e 's%/tmp_mnt/%/%g'`
    if test ! -d ${$1} ; then 
        print_error "Warning: your default path uses the automounter; this may"
        print_error "cause some problems if you use other NFS-connected systems."
        $1=`pwd`
    fi
fi
AC_MSG_RESULT(${$1})
])dnl
dnl
dnl This version compiles an entire function; used to check for
dnl things like varargs done correctly
dnl
dnl PAC_COMPILE_CHECK_FUNC(msg,function,if_true,if_false)
dnl
define(PAC_COMPILE_CHECK_FUNC,
[AC_PROVIDE([$0])dnl
ifelse([$1], , , [AC_MSG_CHECKING(for $1)]
)dnl
if test ! -f confdefs.h ; then
    AC_MSG_RESULT("!! SHELL ERROR !!")
    echo "The file confdefs.h created by configure has been removed"
    echo "This may be a problem with your shell; some versions of LINUX"
    echo "have this problem.  See the Installation guide for more"
    echo "information."
    exit 1
fi
cat > conftest.c <<EOF
#include "confdefs.h"
[$2]
EOF
dnl Don't try to run the program, which would prevent cross-configuring.
if eval $compile; then
  ifelse([$1], , , [AC_MSG_RESULT(yes)])
  ifelse([$3], , :, [rm -rf conftest*
  $3
])
ifelse([$4], , , [else
  rm -rf conftest*
  $4
])dnl
   ifelse([$1], , , ifelse([$4], ,else) [AC_MSG_RESULT(no)])
fi
rm -f conftest*]
)dnl
dnl
dnl We can use wish to find tcl and tk libraries with
dnl puts stdout $tk_library
dnl tclsh can be used with 
dnl puts stdout $tcl_library
dnl
dnl
define(PAC_FIND_TCL,[
# Look for Tcl
if test -z "$TCL_DIR" ; then
# See if tclsh is in the path
# If there is a tclsh, it MAY provide tk.
AC_PATH_PROG(TCLSH,tclsh,1,,tclshloc)
AC_MSG_CHECKING([for Tcl])
if test -n "$tclshloc" ; then
    cat >conftest <<EOF
puts stdout [\$]tcl_library
EOF
    tcllibloc=`$tclshloc conftest 2>/dev/null`
    # The tcllibloc is the directory containing the .tcl files.  
    # The .a files may be one directory up
    if test -n "$tcllibloc" ; then
        tcllibloc=`dirname $tcllibloc`
        # and the lib directory one above that
        tcllibs="$tcllibloc `dirname $tcllibloc`"
    fi
    /bin/rm -f conftest   
fi
#
if test "$host" = "irix" ; then
    subdir="irix-6"
elif test "$host" = "linux" ; then
    subdir="linux"
elif test "$host" = "solaris" ; then
    subdir="solaris-2" 
elif test "$host" = "sun4" ; then
    subdir="sun4"
fi
#
for dir in $tcllibs \
    /usr \
    /usr/local \
    /usr/local/tcl7.5 \
    /usr/local/tcl7.3 \
    /usr/local/tcl7.3-tk3.6 \
    /usr/local/tcl7.0 \
    /usr/local/tcl7.0-tk3.3 \
    /usr/local/tcl7.* \
    /usr/contrib \
    /usr/contrib/tk3.6 \
    /usr/contrib/tcl7.3-tk3.6 \
    /usr/contrib/tk3.3 \
    /usr/contrib/tcl7.0-tk3.3 \
    $HOME/tcl \
    $HOME/tcl7.3 \
    $HOME/tcl7.5 \
    /opt/Tcl \
    /opt/local \
    /opt/local/tcl7.5 \
    /opt/local/tcl7.* \
    /usr/bin \
    /Tools/tcl \
    /usr/sgitcl \
    /software/$subdir/apps/packages/tcl-7* \
    /local/encap/tcl-7.1 ; do
    if test -r $dir/include/tcl.h ; then 
	# Check for correct version
	changequote(,)
	tclversion=`grep 'TCL_MAJOR_VERSION' $dir/include/tcl.h | \
		sed -e 's/^.*TCL_MAJOR_VERSION[^0-9]*\([0-9]*\).*$/\1/'`
	changequote([,])
	if test "$tclversion" != "7" ; then
	    # Skip if it is the wrong version
	    continue
	fi
        if test -r $dir/lib/libtcl.a -o -r $dir/lib/libtcl.so ; then
 	    TCL_DIR=$dir
	    break
        fi
	for file in $dir/lib/libtcl*.a ; do
	    if test -r $file ; then 
                TCL_DIR_W="$TCL_DIR_W $file"
	    fi
	done
    fi
done
fi
if test -n "$TCL_DIR" ; then 
  AC_MSG_RESULT(found $TCL_DIR/include/tcl.h and $TCL_DIR/lib/libtcl)
else
  if test -n "$TCL_DIR_W" ; then
    AC_MSG_RESULT(found $TCL_DIR_W but need libtcl.a)
  else
    AC_MSG_RESULT(no)
  fi
fi
# Look for Tk (look in tcl dir if the code is nowhere else)
if test -z "$TK_DIR" ; then
AC_MSG_CHECKING([for Tk])
if test -n "$wishloc" ; then
    tklibloc=`strings $wishloc | grep 'lib/tk'`
    # The tklibloc is the directory containing the .tclk files.  
    # The .a files may be one directory up
    # There may be multiple lines in tklibloc now.  Make sure that we only
    # test actual directories
    if test -n "$tklibloc" ; then
	for tkdirname in $tklibloc ; do
    	    if test -d $tkdirname ; then
                tkdirname=`dirname $tkdirname`
                # and the lib directory one above that
                tklibs="$tkdirname `dirname $tkdirname`"
	    fi
	done
    fi
    /bin/rm -f conftest   
fi
TK_UPDIR=""
TK_UPVERSION=""
for dir in $tklibs \
    /usr \
    /usr/local \
    /usr/local/tk3.6 \
    /usr/local/tcl7.3-tk3.6 \
    /usr/local/tk3.3 \
    /usr/local/tcl7.0-tk3.3 \
    /usr/contrib \
    /usr/contrib/tk3.6 \
    /usr/contrib/tcl7.3-tk3.6 \
    /usr/contrib/tk3.3 \
    /usr/contrib/tcl7.0-tk3.3 \
    $HOME/tcl \
    $HOME/tcl7.3 \
    /opt/Tcl \
    /opt/local \
    /opt/local/tk3.6 \
    /usr/bin \
    /Tools/tk \
    /usr/sgitcl \
    /software/$subdir/apps/packages/tcl* \
    /local/encap/tk-3.4 $TCL_DIR ; do
    if test -r $dir/include/tk.h ; then 
	# Check for correct version"
	changequote(,)
	tkversion=`grep 'TK_MAJOR_VERSION' $dir/include/tk.h | \
		sed -e 's/^.*TK_MAJOR_VERSION[^0-9]*\([0-9]*\).*$/\1/'`
	tk2version=`grep 'TK_MINOR_VERSION' $dir/include/tk.h | \
		sed -e 's/^.*TK_MINOR_VERSION[^0-9]*\([0-9]*\).*$/\1/'`
	changequote([,])
	tkupversion="$tkversion.$tk2version"
        if test "$tkversion" != "3" ; then
            if test -z "$TK_UPDIR" -a $tkversion -ge 3 ; then
                if test $tkversion -eq 3 -a $tk2version -lt 6 ; then
	            continue
	        fi
                TK_UPDIR=$dir
	        TK_UPVERSION=$tkversion
                if test -r $TK_UPDIR/lib/libtk.a -o \
		        -r $TK_UPDIR/lib/libtk.so ; then
	            continue
	        elif test -r $TK_UPDIR/lib/libtk$tkupversion.a -o \
			  -r $TK_UPDIR/lib/libtk$tkupversion.so ; then
                    continue
	        else
	            TK_UPDIR=""
	            TK_UPVERSION=""
                fi
            fi
	    # Skip if it is the wrong version
	    continue
        fi
        if test -r $dir/lib/libtk.a -o -r $dir/lib/libtk.so ; then
	    TK_DIR=$dir
	    break
	fi
	for file in $dir/lib/libtk*.a ; do
            if test -r $file ; then 
                TK_DIR_W="$TK_DIR_W $file"
	    fi
	done
   fi
done
fi
if test -n "$TK_DIR" ; then 
    AC_MSG_RESULT(found $TK_DIR/include/tk.h and $TK_DIR/lib/libtk)
    TK_VERSION=$tkversion
elif test -n "$TK_DIR_W" ; then
    AC_MSG_RESULT(found $TK_DIR_W but need libtk.a (and version 3.6) )
    BUILD_NUPSHOT=0
    if test -n "$TK_UPDIR" ; then
        TK_DIR=$TK_UPDIR
        TK_VERSION=$TK_UPVERSION
    else
	BUILD_UPSHOT=0
    fi
else
    AC_MSG_RESULT(no)
    BUILD_NUPSHOT=0
    if test -n "$TK_UPDIR" ; then
	TK_DIR=$TK_UPDIR
	TK_VERSION=$TK_UPVERSION
    else
	BUILD_UPSHOT=0
    fi
fi
])dnl
dnl
dnl PAC_OUTPUT_EXEC(files[,mode]) - takes files (as shell script or others),
dnl and applies configure to the them.  Basically, this is what AC_OUTPUT
dnl should do, but without adding a comment line at the top.
dnl Must be used ONLY after AC_OUTPUT (it needs config.status, which 
dnl AC_OUTPUT creates).
dnl Optionally, set the mode (+x, a+x, etc)
dnl
define(PAC_OUTPUT_EXEC,[
CONFIG_FILES="$1"
export CONFIG_FILES
./config.status
CONFIG_FILES=""
for pac_file in $1 ; do 
    /bin/rm -f .pactmp
    sed -e '1d' $pac_file > .pactmp
    /bin/rm -f $pac_file
    mv .pactmp $pac_file
    ifelse($2,,,chmod $2 $pac_file)
done
])dnl
dnl
dnl We need routines to check that make works.  Possible problems with
dnl make include
dnl
dnl It is really gnumake, and contrary to the documentation on gnumake,
dnl it insists on screaming everytime a directory is changed.  The fix
dnl is to add the argument --no-print-directory to the make
dnl
dnl It is really BSD 4.4 make, and can't handle 'include'.  For some
dnl systems, this can be fatal; there is no fix (other than removing this
dnl aleged make).
dnl
dnl It is the OSF V3 make, and can't handle a comment in a block of targe
dnl code.  There is no acceptable fix.
dnl
dnl This assumes that "MAKE" holds the name of the make program.  If it
dnl determines that it is an improperly built gnumake, it adds
dnl --no-print-directorytries to the symbol MAKE.
define(PAC_MAKE_IS_GNUMAKE,[
AC_MSG_CHECKING(gnumake)
/bin/rm -f conftest
cat > conftest <<.
SHELL=/bin/sh
ALL:
	@(dir=`pwd` ; cd .. ; \$(MAKE) -f \$\$dir/conftest SUB)
SUB:
	@echo "success"
.
str=`$MAKE -f conftest 2>&1`
if test "$str" != "success" ; then
    str=`$MAKE --no-print-directory -f conftest 2>&1`
    if test "$str" = "success" ; then
        MAKE="$MAKE --no-print-directory"
	AC_MSG_RESULT(yes using --no-print-directory)
    else
	AC_MSG_RESULT(no)
    fi
else
    AC_MSG_RESULT(no)
fi
/bin/rm -f conftest
str=""
])dnl
dnl
dnl PAC_MAKE_IS_BSD44([true text])
dnl
define(PAC_MAKE_IS_BSD44,[
AC_MSG_CHECKING(BSD 4.4 make)
/bin/rm -f conftest
cat > conftest <<.
ALL:
	@echo "success"
.
cat > conftest1 <<.
include conftest
.
str=`$MAKE -f conftest1 2>&1`
/bin/rm -f conftest conftest1
if test "$str" != "success" ; then
    AC_MSG_RESULT(Found BSD 4.4 so-called make)
    echo "The BSD 4.4 make is INCOMPATIBLE with all other makes."
    echo "Using this so-called make may cause problems when building programs."
    echo "You should consider using gnumake instead."
    ifelse([$1],,[$1])
else
    AC_MSG_RESULT(no - whew)
fi
str=""
])dnl
dnl
dnl PAC_MAKE_IS_OSF([true text])
dnl
define(PAC_MAKE_IS_OSF,[
AC_MSG_CHECKING(OSF V3 make)
/bin/rm -f conftest
cat > conftest <<.
SHELL=/bin/sh
ALL:
	@# This is a valid comment!
	@echo "success"
.
str=`$MAKE -f conftest 2>&1`
/bin/rm -f conftest 
if test "$str" != "success" ; then
    AC_MSG_RESULT(Found OSF V3 make)
    echo "The OSF V3 make does not allow comments in target code."
    echo "Using this make may cause problems when building programs."
    echo "You should consider using gnumake instead."
    ifelse([$1],,[$1])
else
    AC_MSG_RESULT(no)
fi
str=""
])dnl
dnl
dnl Look for a style of VPATH.  Known forms are
dnl VPATH = .:dir
dnl .PATH: . dir
dnl
dnl Defines VPATH or .PATH with . $(srcdir)
dnl Requires that vpath work with implicit targets
dnl NEED TO DO: Check that $< works on explicit targets.
dnl
define(PAC_MAKE_VPATH,[
AC_SUBST(VPATH)
AC_MSG_CHECKING(for virtual path format)
rm -rf conftest*
mkdir conftestdir
cat >conftestdir/a.c <<EOF
A sample file
EOF
cat > conftest <<EOF
all: a.o
VPATH=.:conftestdir
.c.o:
	@echo \$<
EOF
ac_out=`$MAKE -f conftest 2>&1 | grep 'conftestdir/a.c'`
if test -n "$ac_out" ; then 
    AC_MSG_RESULT(VPATH)
    VPATH='VPATH=.:$(srcdir)'
else
    rm -f conftest
    cat > conftest <<EOF
all: a.o
.PATH: . conftestdir
.c.o:
	@echo \$<
EOF
    ac_out=`$MAKE -f conftest 2>&1 | grep 'conftestdir/a.c'`
    if test -n "$ac_out" ; then 
        AC_MSG_RESULT(.PATH)
        VPATH='.PATH: . $(srcdir)'
    else
	AC_MSG_RESULT(neither VPATH nor .PATH works)
    fi
fi
rm -rf conftest*
])dnl
