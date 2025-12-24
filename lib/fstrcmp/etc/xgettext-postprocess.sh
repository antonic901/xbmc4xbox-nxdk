#!/bin/sh
#
# fstrcmp - fuzzy string compare library
# Copyright (C) 2009 Peter Miller
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
package=fstrcmp
desc="$package - fuzzy string compare library"
author="Peter Miller <pmiller@opensource.org.au>"
year=`date '+%Y'`

set -e
set -x
sed -e "s|SOME DESCRIPTIVE TITLE|$desc|" \
    -e "s|FIRST AUTHOR <EMAIL@ADDRESS>, YEAR|$author, $year|" \
    -e "s|(C) YEAR|(C) $year|" \
    -e "s|PACKAGE package.$|$package package.|" \
    -e 's|#[.] *[*] *$|#.|' \
    -e 's|#[.] *[*] |#. |' \
    -e 's|charset=CHARSET|charset=us-ascii|' \
    -e 's|xgettext: *||' \
    $1 > $1.new
mv $1.new $1
exit 0
