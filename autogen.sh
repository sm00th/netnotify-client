#!/bin/sh

die() {
    echo "! $@" >&2
    exit 1
}

run() {
    echo "+ $@"
    $@
}

run "aclocal" || die "aclocal failed"
run "autoconf" || die "autoconf failed"
run "autoheader" || die "autoheader failed"
run "automake --add-missing --copy" || die "automake failed"

#echo ">>> aclocal"
#aclocal || die "aclocal failed"

#echo ">>> autoheader"
#autoheader || die "autoheader failed"

#echo ">>> autoconf"
#autoconf || die "autoconf failed"

#echo ">>> automake --add-missing --copy"
#automake --add-missing --copy || die "automake failed"
