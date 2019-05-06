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

#include "latencycheck.h"

static double theArrivalTime = 0.0;
static double theCutoff = 0.0;

static bool tooOld(const LatencyItem& li)
{
  return theArrivalTime - li.arrivalTime >= theCutoff;
}

LatencyCheck::LatencyCheck()
: latency(-999999.0)
{
}

bool LatencyCheck::create(std::string filename, float _latency)
{
  latency = _latency;
  theCutoff = 2.0 * latency;

  file.open(filename.c_str());
  
  if (!file) {
    return false;
  }
  else {
    return true;
  }
}

bool LatencyCheck::testPick(MyPick pick)
{
  std::list<LatencyItem>::reverse_iterator riter;
  bool rtnVal = true;
  
  theArrivalTime = pick.pickTime;
  std::string stationID = pick.stationCode;
  
  // iterate thru the list from back (recent time) to front (older time)
  for (riter = items.rbegin(); riter != items.rend(); riter++) {
    if (theArrivalTime - riter->arrivalTime <= latency) {
      if (stationID == riter->stationID) {
        file << stationID << " " << std::setprecision(3) << std::fixed << theArrivalTime << std::endl;
        rtnVal = false;
        break;
      }
    }
    else {
      break;
    }
  }
  
  items.remove_if(tooOld);
  
  if (rtnVal) items.push_back(LatencyItem(theArrivalTime, stationID));

  return rtnVal;
}

