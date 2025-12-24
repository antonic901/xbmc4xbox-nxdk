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

#ifndef LIB_GCC_ATTRIBUTES_H
#define LIB_GCC_ATTRIBUTES_H

#ifdef __GNUC__
#define LIB_FORMAT_PRINTF(x, y) __attribute__((format(printf, x, y)))
#define NORETURN __attribute__((noreturn))
#define LINKAGE_HIDDEN __attribute__((visibility("hidden")))
#else
#define LIB_FORMAT_PRINTF(x, y)
#define NORETURN
#define LINKAGE_HIDDEN
#endif

#endif /* LIB_GCC_ATTRIBUTES_H */
