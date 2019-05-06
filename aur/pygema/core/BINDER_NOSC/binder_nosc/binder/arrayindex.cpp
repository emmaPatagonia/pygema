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

#include "arrayindex.h"

// operators

int ArrayIndex::operator()(int i, int j) const
{
  return j*nx + i;
}

int ArrayIndex::operator()(int i, int j, int k) const
{
  return k*ny*nx + j*nx + i;
}

int ArrayIndex::operator()(const ArrayPosition& ap) const
{
  return ap.k*ny*nx + ap.j*nx + ap.i;
}

// methods

bool ArrayIndex::ok(int i, int j) const
{
  return i >= 0 && i < nx && j >= 0 && j < ny;
}

bool ArrayIndex::ok(int i, int j, int k) const
{
  return i >= 0 && i < nx && j >= 0 && j < ny && k >= 0 && k < nz;
}

int ArrayIndex::number() const
{
  return nx * ny * nz;
}
