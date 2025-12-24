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

#include <lib/downcase.h>
#include <lib/fstrcmp.h>


double
fstrcasecmp(const char *s1, const char *s2)
{
    char            *lc1;
    char            *lc2;
    double          result;

    lc1 = fstrcmp_downcase(s1);
    if (!lc1)
        return FSTRCMP_ERROR;

    lc2 = fstrcmp_downcase(s2);
    if (!lc2)
    {
        free(lc1);
        return FSTRCMP_ERROR;
    }

    result = fstrcmp(lc1, lc2);
    free(lc1);
    free(lc2);
    return result;
}
