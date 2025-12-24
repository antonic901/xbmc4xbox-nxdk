/*
 * fstrcmp - fuzzy string compare library
 * Copyright (C) 2009 Peter Miller
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <lib/ac/string.h>

#include <lib/fstrcmp.h>


double
fstrcmp(const char *string1, const char *string2)
{
    return fmemcmp(string1, strlen(string1), string2, strlen(string2));
}
