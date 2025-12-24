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

#ifndef LIB_MINMAX_H
#define LIB_MINMAX_H

/* MAX(a,b) returns the maximum of A and B.  */
#ifndef MAX
# define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/* MIN(a,b) returns the minimum of A and B.  */
#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#endif /* LIB_MINMAX_H */
