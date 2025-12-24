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

#include <lib/mbs_to_wcs.h>


wchar_t *
fstrcmp_mbs_to_wcs(const char *mbs)
{
    size_t          mbs_len;
    size_t          wcs_len;
    wchar_t         *wcs;

    mbs_len = strlen(mbs) + 1;
    wcs_len = mbstowcs(NULL, mbs, mbs_len);
    if (wcs_len == (size_t)-1)
        return NULL;
    wcs = malloc(sizeof(wchar_t) * (wcs_len + 1));
    if (!wcs)
        return NULL;
    wcs_len = mbstowcs(wcs, mbs, wcs_len + 1);
    if (wcs_len == (size_t)-1)
    {
        free(wcs);
        return NULL;
    }
    return wcs;
}
