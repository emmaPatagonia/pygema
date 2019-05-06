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
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "input.h"

#ifdef MY_DEBUG
static void debugStationList(std::list<Station> stationList)
{
  std::list<Station>::iterator iter;
  
  std::cerr << "*** List of stations *** " << std::endl;
  
  for (iter = stationList.begin(); iter != stationList.end(); iter++) {
    std::cerr << "station name = " << iter->code() << std::endl;
  }
  
  std::cerr << "*** End of station list ***" << std::endl;
  std::cerr.flush();
}
#endif // MY_DEBUG

//--------------------------------------------------------------------------------

static bool stationCmpLT(MyStation s1, MyStation s2)
{
  return s1.code() < s2.code();
}

static bool stationCmpEQ(MyStation s1, MyStation s2)
{
  return s1.code() == s2.code();
}

//--------------------------------------------------------------------------------

bool inputStations(std::string filename, std::list<MyStation>& stationList)
{ 
  // open the PDAS configuration file
  FILE *in = NULL;
  if ((in = fopen(filename.c_str(), "r")) == NULL) {
    std::cerr << "[Binder] inputStations: cannot open '" << filename << "'" << std::endl;
    return false;
  }
 
  char *items[16];
  char *tok = NULL;
  int count = 0;
  
  // read in station locations
  char buf[256];
 
  while (!feof(in)) {
    memset(buf, 0, 256 * sizeof(char));
    if (fgets(buf, 256, in) != NULL) {
      count = 0;
      tok = strtok(buf, " \t\n");
      while (tok) {
        items[count++] = tok;
        tok = strtok(NULL, " \t\n");
      }
      if (count != 16) {
        std::cerr << "[Binder] inputStations: incorrect number of tokens found when reading PDAS info line" << std::endl;
        return false;
      }
			MyStation station;
      station.setCode(std::string(items[7]));
      station.setLatitude(atof(items[8]));
      station.setLongitude(atof(items[9]));
      station.setElevation(atof(items[10]));
      stationList.push_back(station);
    }
  }

  // remove station duplicates 
  stationList.sort(stationCmpLT);
  stationList.unique(stationCmpEQ);

#ifdef MY_DEBUG
  //debugStationList(stationList);
#endif // MY_DEBUG
  
  fclose(in);
  
  return true;
}
