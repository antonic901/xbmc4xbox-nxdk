/*
 * fstrcmp - fuzzy string compare library
 * Copyright (C) 2009 Peter Miller
 * Written by Peter Miller <pmiller@opensource.org.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * See the comment attached to the symlink_exceptions field o the project
 * config file for how the version stamp is maintained separarely for
 * each development directory and integration directory.
 */

#include <lib/version.h>
#include <lib/patchlevel.h>


const char *
fstrcmp_version(void)
{
    return PATCHLEVEL;
}


const char *
fstrcmp_copyright_years(void)
{
    return COPYRIGHT_YEARS;
}
