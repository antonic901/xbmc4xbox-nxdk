/*
 * fstrcmp - fuzzy string compare library
 * Copyright (C) 2009 Peter Miller
 * Written by Peter Miller <pmiller@opensource.org.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <lib/ac/stdlib.h>
#include <lib/ac/string.h>

#include <lib/fstrcmp.h>
#include <lib/mbs_to_wcs.h>


double
fstrcoll(const char *mbs1, const char *mbs2)
{
    wchar_t         *wcs1;
    wchar_t         *wcs2;
    double          result;

    wcs1 = fstrcmp_mbs_to_wcs(mbs1);
    if (!wcs1)
        return FSTRCMP_ERROR;
    wcs2 = fstrcmp_mbs_to_wcs(mbs2);
    if (!wcs2)
    {
        free(wcs1);
        return FSTRCMP_ERROR;
    }

    result = fwcscmp(wcs1, wcs2);
    free(wcs1);
    free(wcs2);
    return result;
}
