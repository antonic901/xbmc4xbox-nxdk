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

#ifndef LIB_DOWNCASE_H
#define LIB_DOWNCASE_H

/**
  * The fstrcmp_downcase function may be used to create a new copy of a
  * string, except that all the upper case letters have been translated
  * to lower case.
  *
  * @param s
  *    The string to copy and translate.
  * @returns
  *    a new string.  Use free() when you are done with it.
  */
char *fstrcmp_downcase(const char *s);

#endif /* LIB_DOWNCASE_H */
