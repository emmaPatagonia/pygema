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
#include <string>
#include <list>

#ifndef _MYPICK_H
#define _MYPICK_H

enum Polarity {
  POLARITY_POSITIVE,
  POLARITY_NEGATIVE,
  POLARITY_UNDECIDABLE
};

class MyPick {
public:
  std::string stationCode;
  double pickTime;
  std::string phaseHint;
  std::string methodID;
  Polarity polarity;
  double originTime;
  int weight;
  float timeResidual;
  float relocatedWeight;
  float distance;
  float azimuth;
  std::string originalPhaseHint;
  int index;

  //std::list<int> historyList;  // history

  MyPick()
  : pickTime(-9999), phaseHint("P"), polarity(POLARITY_UNDECIDABLE), originTime(-9999), weight(-9999),
  timeResidual(-9999), relocatedWeight(-9999), distance(-9999), azimuth(-9999), index(-9999) {}
  MyPick(std::string _stationCode, double _pickTime, std::string _phaseHint, std::string _methodID, Polarity _polarity, int _weight);
  virtual ~MyPick() {}
  
  bool operator ==(const MyPick& mp);
  bool operator !=(const MyPick& mp);
  bool operator <(const MyPick& mp);
  
  void setOriginTime(float stationTime);
  bool isSPhase() const;

  friend std::ostream& operator <<(std::ostream& os, MyPick& myPick);
  const char *writeUnusedString();
  const char *writeString();
  void writeAndreas(std::ostream& os, float tt);
    
  void writeInfo();
#ifdef MY_DEBUG
  void writeDebug(std::ostream& os) const;
  void writeDebugExtra(std::ostream& os) const;
#endif
};

bool comparePickOriginLT(const MyPick& mp1, const MyPick& mp2);

#endif // !_MYPICK_H
