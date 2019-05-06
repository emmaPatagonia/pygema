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

#ifndef _VELMOD_H
#define _VELMOD_H

#include <string>
#include "uTravelTime.h"

class VelMod
{
public:
  VelMod() : nl(0), v(NULL), top(NULL) {}
  virtual ~VelMod() {}
  
  bool create(std::string velmodFilename);
  double lookupVelocity(double depth) const;
  
  tInteger nl;
  tDouble *v;
  tDouble *top;
};

#endif // !_VELMOD_H
