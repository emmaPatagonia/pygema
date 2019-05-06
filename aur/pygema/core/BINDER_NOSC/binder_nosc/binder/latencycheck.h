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

#include <fstream>
#include <iomanip>
#include <string>
#include <list>

#include "mypick.h"

#ifndef _LATENCY_CHECK_H
#define _LATENCY_CHECK_H

class LatencyItem {
public:
  LatencyItem() {}
  LatencyItem(double _arrivalTime, std::string _stationID) : arrivalTime(_arrivalTime), stationID(_stationID) {}
  virtual ~LatencyItem() {}
  
  bool operator <(const LatencyItem& li) { return arrivalTime < li.arrivalTime; }
  
  double arrivalTime;
  std::string stationID;
};

class LatencyCheck {
public:
  LatencyCheck();
  virtual ~LatencyCheck() {}
  
  bool create(std::string filename, float _latency);
  bool testPick(MyPick pick);

private:  
  float latency;
  std::ofstream file;
  std::list<LatencyItem> items;
};

#endif // !_LATENCY_CHECK_H

