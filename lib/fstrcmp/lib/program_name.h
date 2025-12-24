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

#ifndef LIB_PROGRAM_NAME_H
#define LIB_PROGRAM_NAME_H

/**
  * The fstrcmp_program_name_get function may be used to obtain the
  * command name of the process.  Depending on how capable /proc is on
  * your system, this may or may not produce a sensable result.  It
  * works well on Linux.
  *
  * @returns
  *    pointer to string containing the command name (no slashes) of the
  *    calling process.
  */
const char *fstrcmp_program_name_get(void);

/**
  * The fstrcmp_program_name_set function may be used to set the fstrcmp
  * libraries' idea of the command name of the calling process, setting
  * the string to be returned by the fstrcmp_program_name_get function.
  * This overrides the automatic behaviour, which can be quite desirable
  * in commands that can be invoked with more than one name, e.g. if
  * they are a hard link synonym.
  *
  * @param name
  *     The name of the calling process, usually argv[0].
  */
void fstrcmp_program_name_set(const char *name);

#endif /* LIB_PROGRAM_NAME_H */
