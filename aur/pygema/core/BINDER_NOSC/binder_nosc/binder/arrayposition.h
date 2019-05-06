/*
 * This file is part of binder.
 *
 * Copyright (C) 2009 Andy Heath, Stuart Nippress & Andreas Rietbrock,
 *                    University of Liverpool
 *
 * This work was funded as part of the NERIES (JRA5) project.
 * Additional funding for Nippress from NERC research grant NE/C000315/1
 *
 * binder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * binder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with binder.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _ARRAY_POSITION_H
#define _ARRAY_POSITION_H

class ArrayPosition {
public:
  int i;
  int j;
  int k;
  
  ArrayPosition() : i(-1), j(-1), k(-1) {}
  ArrayPosition(int _i, int _j, int _k) : i(_i), j(_j), k(_k) {}
  virtual ~ArrayPosition() {}
  
  bool operator ==(const ArrayPosition& ap) const;
  bool operator !=(const ArrayPosition& ap) const;
};

#endif // !_ARRAY_POSITION_H
