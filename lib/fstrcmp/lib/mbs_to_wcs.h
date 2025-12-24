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

#ifndef LIB_MBS_TO_WCS_H
#define LIB_MBS_TO_WCS_H

#include <lib/ac/stddef.h>

/**
  * The fstrcmp_mbs_to_wcs function may be used to create a new
  * wide-character string from a multi-byte character string.
  *
  * @param s
  *    The multi-byte character string to convert.
  * @returns
  *    pointer to a wide-character string.
  *    Use free() when you are done with it.
  */
wchar_t *fstrcmp_mbs_to_wcs(const char *s);

#endif /* LIB_MBS_TO_WCS_H */
