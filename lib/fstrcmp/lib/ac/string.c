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
 */

#include <lib/ac/stddef.h>
#include <lib/ac/stdlib.h>
#include <lib/ac/string.h>
#include <lib/ac/stdio.h>

#include <lib/gcc_attributes.h>


/*
 *  NAME
 *      strerror - string for error number
 *
 *  SYNOPSIS
 *      char *strerror(int errnum);
 *
 *  DESCRIPTION
 *      The strerror function maps the error number in errnum to an error
 *      message string.
 *
 *  RETURNS
 *      The strerror function returns a pointer to the string, the contents of
 *      which are implementation-defined.  The array pointed to shall not be
 *      modified by the program, but may be overwritten by a subsequent call to
 *      the strerror function.
 *
 *  CAVEAT
 *      Unknown errors will be rendered in the form "Error %d", where %d will
 *      be replaced by a decimal representation of the error number.
 */

#ifndef HAVE_STRERROR

LINKAGE_HIDDEN
char *
strerror(int n)
{
    extern int      sys_nerr;
    extern char     *sys_errlist[];
    static char     buffer[16];

    if (n < 1 || n > sys_nerr)
    {
            sprintf(buffer, "Error %d", n);
            return buffer;
    }
    return sys_errlist[n];
}

#endif /* !HAVE_STRERROR */
