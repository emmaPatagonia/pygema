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

#include <cstdio>
#include <cstring>
#include <cmath>
using namespace std;

#include "cartesian.h"

#define	E_RAD	 6378.163
#define E_FLAT 298.26
#define DRAD	1.7453292e-2
#define DRLT	9.9330647e-1

Cartesian::Cartesian(float lon, float lat, float rota)
{
   double	dlt1, dlt2, del, radius;

   /* convert everything to minutes */
   lat = 60.0f * lat;
   lon = 60.0f * lon;
   orig_lon = lon;
   orig_lat = lat;

   /* latitude */
   dlt1 = atan(DRLT * tan((double)lat * DRAD/60.0));
   dlt2 = atan(DRLT * tan(((double)lat +1.0) * DRAD/60.0));
   del  = dlt2 - dlt1;
   radius = E_RAD * (1.0 - (sin(dlt1)*sin(dlt1) / E_FLAT));
   lat_fac = radius * del;

   /* longitude */
   del = acos(1.0 - (1.0 - cos(DRAD/60.0)) * cos(dlt1) * cos(dlt1));
   dlt2 = radius * del;
   lon_fac = dlt2 / cos(dlt1);

   /* rotation */
   snr = sin((double)rota * DRAD);
   csr = cos((double)rota * DRAD);
}

bool Cartesian::toXYZ(float lon, float lat, float *x, float *y) const
{
  float	tmp, tmp_x, tmp_y;

  lat = 60.0f * lat;
  lon = 60.0f * lon;

  tmp_x = lon - orig_lon;
  tmp_y = lat - orig_lat;

  tmp   = atan(DRLT * tan(DRAD * (lat+orig_lat)/120.0f));
  tmp_x = (double)tmp_x * lon_fac * cos(tmp);    
  tmp_y = (double)tmp_y * lat_fac;
  
  /* rotation */
  *x = csr*tmp_x - snr*tmp_y;
  *y = csr*tmp_y + snr*tmp_x;

  return true;
}


bool Cartesian::toLonLat(float x, float y, float *lon, float *lat) const
{
  float	tmp_x, tmp_y,	tmp;

  tmp_x = snr*y + csr*x;
  tmp_y = csr*y - snr*x;

  tmp_y = tmp_y/lat_fac;
  tmp_y += orig_lat;

  tmp = atan(DRLT * tan(DRAD * (tmp_y+orig_lat)/120.0));
  tmp_x = tmp_x / (lon_fac * cos(tmp));
  tmp_x += orig_lon;

  *lon = tmp_x/60.0f;
  *lat = tmp_y/60.0f;

  return true;
}

// returns azimuth calculated in rectangular trig between p1 & p2 in degrees
float Cartesian::azimuthXYZ(const Position& originPos, const Position& stationPos) const
{
  float x = stationPos.x - originPos.x;
  float y = stationPos.y - originPos.y;
  float xOverY = fabsf(x) / fabsf(y);
  float yOverX = fabsf(y) / fabsf(x);
  float offset = 0.0f;
  float ratio = 0.0f;
  
  if (x >= 0.0f) {
    if (y >= 0.0f) {
      ratio = xOverY;
      offset = 0.0f;
    }
    else {
      ratio = yOverX;
      offset = 90.0f;
    }
  }
  else {
    if (y < 0.0f) {
      ratio = xOverY;
      offset = 180.0f;
    }
    else {
      ratio = yOverX;
      offset = 270.0f;
    }
  }
  
  return offset + atanf(ratio) * 180.0/M_PI;
}

// returns azimuth calculated in spherical trig between p1 & p2 in degrees
float Cartesian::azimuthLatLon(const Position& originPos, const Position& stationPos) const
{
  double dlon = stationPos.y - originPos.y;
  double dlat = stationPos.x - originPos.x;

  // more of this at http://tchester.org/sgm/analysis/peaks/how_to_get_view_params.html
  
  return 0.0f;
}
