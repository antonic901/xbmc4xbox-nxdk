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

#ifndef LIB_WARN_UNUSED_RESULT_H
#define LIB_WARN_UNUSED_RESULT_H

/*
 * Convenience macros to test the versions of glibc and gcc.
 * Use them like this:
 *    #if __GNUC_PREREQ (2,8)
 *    ... code requiring gcc 2.8 or later ...
 *    #endif
 * Note - they won't work for gcc1 or glibc1, since the _MINOR macros
 *                   were not defined then.  */
#if defined __GNUC__ && defined __GNUC_MINOR__
# define LIB_GNUC_PREREQ(maj, min) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define LIB_GNUC_PREREQ(maj, min) 0
#endif

/*
 * We attach this attribute to functions that should never have their
 * return value ignored.  Amongst other things, this can detect the case
 * where the client has called explain_fubar when they meant to call
 * explain_fubar_or_die instead.
 */
#if LIB_GNUC_PREREQ(3, 4)
#define LIB_WARN_UNUSED_RESULT __attribute__ ((__warn_unused_result__))
#else
#define LIB_WARN_UNUSED_RESULT
#endif

#endif /* LIB_WARN_UNUSED_RESULT_H */
