#!/bin/sh
#
# fstrcmp - fuzzy string compare library
# Copyright (C) 2009, 2011 Peter Miller
# Written by Peter Miller <pmiller@opensource.org.au>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.
#
set -x
key=
while [ $# -gt 0 ]
do
    case "$1" in
    -*)
        key="$key $1"
        shift
        continue
        ;;
    *)
        ;;
    esac
    break
done
intarball=$1
outtarball=$2

project_minus=`basename $intarball .tar.gz`

unset GPG_AGENT_INFO
rm -rf web-site/debian
mkdir -p web-site/debian
echo Options Indexes > web-site/debian/.htaccess
gunzip < $intarball |
    tardy -exclude "${project_minus}/debian/*" |
    gzip -9 > web-site/debian/$outtarball

gunzip < $intarball | ( cd web-site/debian ; tar xf - )

set -e
cd web-site/debian
cd $project_minus
debuild $key -sa
cd ..
rm -rf $project_minus
lintian -iI --pedantic *.changes
