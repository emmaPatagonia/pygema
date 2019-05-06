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

/*
 * cartesian.h: utility to convert lat/lon to cartesian XY (needs a reference point)
 *
 */

#include "position.h"

#ifndef _CARTESIAN_H
#define _CARTESIAN_H

class Cartesian {
private:
  float	 orig_lon;		/* origin longitude */
  float	 orig_lat;		/* origin latitude  */
  double lat_fac;		  /* conversion factor for latitude in km */
  double lon_fac;		  /* conversion factor for longitude in km */
  double snr;			    /* sin of rotation angle */
  double csr;			    /* cos of rotation angle */

public:
  Cartesian() : orig_lon(0.0f), orig_lat(0.0f), lat_fac(0.0f), lon_fac(0.0f), snr(0.0f), csr(0.0f) {}
  Cartesian(float lon, float lat, float rota);
  virtual ~Cartesian() {}
    
  bool toXYZ(float lon, float lat, float *x, float *y) const;
  bool toLonLat(float x, float y, float *lon, float *lat) const;
  float azimuthXYZ(const Position& p1, const Position& p2) const;
  float azimuthLatLon(const Position& p1, const Position& p2) const;
};

#endif // !_CARTESIAN_H
