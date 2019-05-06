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

#include <iostream>

#include <values.h>

#ifndef _POSITION_H
#define _POSITION_H

class Position {
public:
  float x;
  float y;
  float z;
  
  Position() : x(FLT_MIN), y(FLT_MIN), z(FLT_MIN) {}
  Position(float _x, float _y) : x(_x), y(_y), z(0.0f) {}
  Position(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
  virtual ~Position() {}
  
  Position operator +(const Position& p) const;
  Position operator -(const Position& p) const;
  bool operator ==(const Position& p) const;
  bool operator !=(const Position& p) const;
  
  double pythag2d(const Position& p) const;
  double pythag3d(const Position& p) const;
    
  friend std::ostream& operator <<(std::ostream& os, Position& p);
};

void checkMinMax(float x, float y, Position& min, Position& max);
void checkMinMax(const Position& p, Position& min, Position& max);

#endif // !_POSITION_H
