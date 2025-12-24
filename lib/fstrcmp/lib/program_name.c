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

#include <lib/ac/assert.h>
#include <lib/ac/stdlib.h>
#include <lib/ac/string.h>
#include <lib/ac/unistd.h>

#include <lib/program_name.h>


static char progname[256];


static void
fstrcmp_program_name_set_real(const char *name)
{
    const char      *cp;

    if (!name)
        name = "";
    progname[0] = '\0';
    if (!name)
        return;

    cp = name;
    for (;;)
    {
        char            *pnp;

        if (*cp == '/')
        {
            /*
             * Weird but true: some Unix implementations allow trailing
             * slashes on command names.  Ugh.  Not POSIX conforming, either.
             */
            ++cp;
            continue;
        }
        if (*cp == '\0')
            break;

        /*
         * Libtool produces "temporary" binaries before "make install"
         * that have "lt-" prefixes.  Rip them off when we see them.
         */
        if (0 == memcmp(cp, "lt-", 3))
            cp += 3;

        pnp = progname;
        for (;;)
        {
            if (pnp < progname + sizeof(progname) - 1)
                *pnp++ = *cp;
            ++cp;
            if (*cp == '\0' || *cp == '/')
                break;
        }
        *pnp = '\0';
    }
}


const char *
fstrcmp_program_name_get(void)
{
    if (progname[0])
        return progname;

    /*
     * Does procfs have something useful?
     */
    {
        int             n;
        char            buf[2000];

        n = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (n > 0)
        {
            buf[n] = 0;
            fstrcmp_program_name_set_real(buf);
        }
    }
    if (progname[0])
        return progname;

    /*
     * bash(1) sets the "_" environment variable,
     * use that if available.
     */
    fstrcmp_program_name_set_real(getenv("_"));
    if (progname[0])
        return progname;

    return "";
}


void
fstrcmp_program_name_set(const char *name)
{
    fstrcmp_program_name_set_real(name);
}
