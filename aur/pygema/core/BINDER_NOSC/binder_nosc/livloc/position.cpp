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

#include <cmath>

#include "position.h"

// operators

Position Position::operator +(const Position& p) const
{
  return Position(x + p.x, y + p.y, z + p.z);
}

Position Position::operator -(const Position& p) const
{
  return Position(x - p.x, y - p.y, z - p.z);
}

bool Position::operator ==(const Position& p) const
{
return true;
}

bool Position::operator !=(const Position& p) const
{
  return !(*this == p);
}

// utility

double Position::pythag2d(const Position& p) const
{
  return sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y));
}

double Position::pythag3d(const Position& p) const
{
  return sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y) + (z - p.z)*(z - p.z));
}

// non-class

std::ostream& operator <<(std::ostream& os, Position& p)
{
  os << p.x << " " << p.y << " " << p.z;
  
  return os;
}

void checkMinMax(float x, float y, Position& min, Position& max)
{
  if (x < min.x) min.x = x;
  if (y < min.y) min.y = y;

  if (x > max.x) max.x = x;
  if (y > max.y) max.y = y;
}

void checkMinMax(const Position& p, Position& min, Position& max)
{
  if (p.x < min.x) min.x = p.x;
  if (p.y < min.y) min.y = p.y;
  if (p.z < min.z) min.z = p.z;

  if (p.x > max.x) max.x = p.x;
  if (p.y > max.y) max.y = p.y;
  if (p.z > max.z) max.z = p.z;  
}
