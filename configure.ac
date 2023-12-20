dnl              ARTIFICIAL NEURAL NETWORKS SOFTWARE
dnl   Neural Parser (np) :
dnl   environment for training and testing neural networks
dnl
dnl                        by Andreas Hadjiprocopis
dnl                  (andreashad2@gmail.com)
dnl              Copyright Andreas Hadjiprocopis, 1994,2007
dnl AX_CC_MAXOPT macros (and all contents of the m4 directory)
dnl were 'borrowed' from fftw-3.1 by Matteo Frigo and Steven G. Johnson (thanks!)
dnl look at :  http://www.fftw.org/mailman/listinfo/fftw-announce

dnl for these see at the end of file as well
AC_INIT([NeuralParser],[5.0],[andreashad2@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
m4_pattern_allow([AC_CONFIG_MACRO])
m4_pattern_allow([AS_AC_EXPAND])
m4_pattern_allow([AX_CC_MAXOPT])
AC_CANONICAL_HOST
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE()

case "${target_os}" in
	*solaris* | *linux*)
		echo "solaris or linux system"
		CPPFLAGS="-DNP_LINUXSOLARIS_VERSION"
		;;
	*darwin*)
		echo "mac"
		CPPFLAGS="-DNP_LINUXSOLARIS_VERSION"
		;;
	djgpp | *mingw32* | *emx*)
		echo dos
		CPPFLAGS="-DNP_WINDOWS_VERSION"
		;;
	*bsd*)
		echo BSDS
		CPPFLAGS="-DNP_BSD_VERSION"
		;;
esac

echo target os : "${target_os}"
echo target cpu: "${target_cpu}"
echo build cpu : "${build_cpu}"

dnl determine CFLAGS first
dnl AX_CC_MAXOPT

CFLAGS="$CFLAGS -Werror"

cpuid=""
cpuspeed=""
type -P x86info >& /dev/null
if test $? -eq 0 ; then
	x86info -f > cpuid
	cpuid=`grep "CPU Model" cpuid | cut -d':' -f 2`
	echo "$cpuid" | grep -i unknown >& /dev/null
	if test $? -eq 0 ; then cpuid=`grep "Processor name string" cpuid | cut -d':' -f 2 | cut -d'@' -f 1`; fi
	cpuspeed=`grep "Processor name string" cpuid | cut -d':' -f 2 | cut -d'@' -f 2`
else
	# try mac
	type -P /usr/sbin/system_profiler >& /dev/null
	if test $? -eq 0 ; then
		cpuid=`/usr/sbin/system_profiler | grep "Processor Name" | cut -d':' -f 2`
		cpuspeed=`/usr/sbin/system_profiler | grep "Processor Speed" | cut -d':' -f 2`
	else
		echo "can't figure out cpu architecture using -march=native which might not work in some cases (see mac)"
		cpuid='unknown'
		cpuspeed='unknown'
	fi
fi

echo CPU ID: "$cpuid"
echo CPU SPEED: "$cpuspeed"
case "$cpuid" in
	*thlon*64* )
		CFLAGS="$CFLAGS -m64 -march=athlon64-sse3"
		;;
	*Intel*Core*i* )
		CFLAGS="$CFLAGS -march=nocona -O2 -pipe -m64"
		# when Apple updates to > 4.3 then -march=core2 -mtune=generic -O2 -pipe
		;;
	*Intel*Xeon* )
		CFLAGS="$CFLAGS -march=nocona -m64"
		;;
	* )
#		CFLAGS="$CFLAGS"
		;;
esac
echo CFLAGS USED: "$CFLAGS"

dnl Checks for programs. last one is most favourite
AC_PROG_CXX([ cc, gcc, gcc3, gcc-4.1 ])
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_ARG_WITH(gawk, [  --with-gawk=FULLPATH       set full path to gawk (default /usr/bin/gawk)], [GAWKPATH="$withval"])
AC_PATH_PROG(GAWKPATH, gawk,no, $PATH)
if test $GAWKPATH = no; then
        AC_MSG_ERROR([can not find gawk if you know where it is specify it at command line using --with-gawk=])
fi

AC_ARG_WITH(cp, [  --with-cp=FULLPATH       set full path to cp (default /usr/bin/cp)], [CPPATH="$withval"])
AC_PATH_PROG(CPPATH, cp,no, $PATH)
if test $CPPATH = no; then
        AC_MSG_ERROR([can not find cp if you know where it is specify it at command line using --with-cp=])
fi

AC_ARG_WITH(perl, [  --with-perl=FULLPATH       set full path to perl (default /usr/bin/perl)], [PERLPATH="$withval"])
AC_PATH_PROG(PERLPATH, perl,no, $PATH)
if test $PERLPATH = no; then
        AC_MSG_ERROR([can not find perl if you know where it is specify it at command line using --with-perl=])
fi

AC_ARG_WITH(hostname, [  --with-hostname=FULLPATH       set full path to hostname (default /usr/bin/hostname)], [HOSTNAMEPATH="$withval"])
AC_PATH_PROG(HOSTNAMEPATH, hostname,no, $PATH)
if test $HOSTNAMEPATH = no; then
        AC_MSG_ERROR([can not find hostname if you know where it is specify it at command line using --with-hostname=])
fi

AC_ARG_WITH(mkdir, [  --with-mkdir=FULLPATH       set full path to mkdir (default /usr/bin/mkdir)], [MKDIRPATH="$withval"])
AC_PATH_PROG(MKDIRPATH, mkdir,no, $PATH)
if test $MKDIRPATH = no; then
        AC_MSG_ERROR([can not find mkdir if you know where it is specify it at command line using --with-mkdir=])
fi

AC_ARG_WITH(mail, [  --with-mail=FULLPATH       set full path to mail (default /usr/bin/mail)], [MAILPATH="$withval"])
AC_PATH_PROG(MAILPATH, mail,no, $PATH)
if test $MAILPATH = no; then
        AC_MSG_WARN([can not find mail if you know where it is specify it at command line using --with-mail= . For the time being I am using bin/fake_mail.bash which does absolutely nothing.])
	MAILPATH=bin/fake_mailer.bash
fi

AC_ARG_WITH(cat, [  --with-cat=FULLPATH       set full path to cat (default /usr/bin/cat)], [CATPATH="$withval"])
AC_PATH_PROG(CATPATH, cat,no, $PATH)
if test $CATPATH = no; then
        AC_MSG_ERROR([can not find cat if you know where it is specify it at command line using --with-cat=])
fi

AC_ARG_WITH(mv, [  --with-mv=FULLPATH       set full path to mv (default /usr/bin/mv)], [MVPATH="$withval"])
AC_PATH_PROG(MVPATH, mv,no, $PATH)
if test $MVPATH = no; then
        AC_MSG_ERROR([can not find mv if you know where it is specify it at command line using --with-mv=])
fi

AC_ARG_WITH(rcp, [  --with-rcp=FULLPATH       set full path to rcp (default /usr/bin/rcp  - you need to install rsh services if you are in linux, if you can't or don't want to then just run configure with --with-rcp=/bin/false but you wont be able to parallelise scripts and spawn processes onto other computers - only local processing will be supported.)], [RCPPATH="$withval"])
AC_PATH_PROG(RCPPATH, rcp,no, $PATH)
if test $RCPPATH = no; then
        AC_MSG_WARN([can not find rcp if you know where it is specify it at command line using --with-rcp= . For the time being I am using bin/fake_rcp.bash which does absolutely nothing.])
	RCPPATH=bin/fake_rcp.bash
fi

AC_ARG_WITH(rsh, [  --with-rsh=FULLPATH       set full path to rsh (default /usr/bin/rsh  - you need to install rsh services if you are in linux, if you can't or don't want to then just run configure with --with-rsh=/bin/false but you wont be able to parallelise scripts and spawn processes onto other computers - only local processing will be supported.)], [rshPATH="$withval"])
AC_PATH_PROG(RSHPATH, rsh,no, $PATH)
if test $RSHPATH = no; then
        AC_MSG_WARN([can not find rsh if you know where it is specify it at command line using --with-rsh= . For the time being I am using bin/fake_rsh.bash which does absolutely nothing.])
	RCHPATH=bin/fake_rcp.bash
fi

AC_ARG_WITH(gzip, [  --with-gzip=FULLPATH       set full path to gzip (default /usr/bin/gzip)], [GZIPPATH="$withval"])
AC_PATH_PROG(GZIPPATH, gzip,no, $PATH)
if test $GZIPPATH = no; then
        AC_MSG_ERROR([can not find gzip if you know where it is specify it at command line using --with-gzip=])
fi

AC_ARG_WITH(gunzip, [  --with-gunzip=FULLPATH       set full path to gunzip (default /usr/bin/gunzip)], [GUNZIPPATH="$withval"])
AC_PATH_PROG(GUNZIPPATH, gunzip,no, $PATH)
if test $GUNZIPPATH = no; then
        AC_MSG_ERROR([can not find gunzip if you know where it is specify it at command line using --with-gunzip=])
fi

AC_ARG_WITH(tar, [  --with-tar=FULLPATH       set full path to tar (default /usr/bin/tar)], [TARPATH="$withval"])
AC_PATH_PROG(TARPATH, tar,no, $PATH)
if test $TARPATH = no; then
        AC_MSG_ERROR([can not find tar if you know where it is specify it at command line using --with-tar=])
fi

AC_ARG_WITH(sleep, [  --with-sleep=FULLPATH       set full path to sleep (default /usr/bin/sleep)], [SLEEPPATH="$withval"])
AC_PATH_PROG(SLEEPPATH, sleep,no, $PATH)
if test $SLEEPPATH = no; then
        AC_MSG_ERROR([can not find sleep if you know where it is specify it at command line using --with-sleep=])
fi

AC_ARG_WITH(date, [  --with-date=FULLPATH       set full path to date (default /usr/bin/date)], [DATEPATH="$withval"])
AC_PATH_PROG(DATEPATH, date,no, $PATH)
if test $DATEPATH = no; then
        AC_MSG_ERROR([can not find date if you know where it is specify it at command line using --with-date=])
fi


LT_INIT

AC_SUBST(AUTHOR, "Andreas Hadjiprocopis (andreashad2@gmail.com)")
AC_SUBST(BUILTDATE, `date`)
AC_SUBST(TITLE, "A lexical analyser and parser for Neural Network Scripts")
AC_SUBST(DATESTARTED, "Tue Mar 25 1997")
AC_SUBST(SEARCHPATH, $PATH)


dnl because we get irritating ${..} in perl
AS_AC_EXPAND(bindir, "${bindir}")
AS_AC_EXPAND(libdir, "${libdir}")

AC_CONFIG_FILES([src/perl/np/Constants.ph \
	src/perl/np/GeneralLib.pl \
	src/perl/np/IOLib.pl \
	src/perl/np/NeuralLib.pl \
	src/perl/np/ReferenceGuide.ph \
	src/perl/np/np \
\
	src/perl/convert_data/convert_column_data_to_libsvm.pl \
	src/perl/convert_data/convert_libsvm_to_column_data.pl \
\
	src/perl/svm-select-parameters/svm-select-parameters.pl \
	src/perl/svm-select-parameters/Makefile \
	src/perl/svm-select-parameters/GridSearch/Makefile.PL
])
AC_CONFIG_FILES([Makefile
	src/Makefile
	src/C/Makefile
	src/perl/Makefile
	src/perl/convert_data/Makefile
	src/perl/np/Makefile
	src/3rdparty/Makefile
	src/3rdparty/libsvm-3.1/Makefile
])
AC_OUTPUT
