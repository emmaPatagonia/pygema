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

#include "arrayposition.h"

#ifndef _ARRAY_INDEX_H
#define _ARRAY_INDEX_H

class ArrayIndex {
public:
  int nx;
  int ny;
  int nz;
  
  ArrayIndex() : nx(-1), ny(-1), nz(-1) {}
  ArrayIndex(int _nx, int _ny) : nx(_nx), ny(_ny), nz(1) {}
  ArrayIndex(int _nx, int _ny, int _nz) : nx(_nx), ny(_ny), nz(_nz) {}
  
  int operator()(int i, int j) const;
  int operator()(int i, int j, int k) const;
  int operator()(const ArrayPosition& ap) const;
  
  bool ok(int i, int j) const;
  bool ok(int i, int j, int k) const;
  int number() const;
};

#endif // !_ARRAY_INDEX_H
