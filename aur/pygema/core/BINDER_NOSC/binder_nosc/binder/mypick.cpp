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

#include <iomanip>
#include <cstdio>

#include "mypick.h"

// constructor

MyPick::MyPick(std::string _stationCode, double _pickTime, std::string _phaseHint, std::string _methodID, Polarity _polarity, int _weight)
: stationCode(_stationCode), pickTime(_pickTime), phaseHint(_phaseHint), methodID(_methodID), polarity(_polarity),
  originTime(-9999), weight(_weight), timeResidual(-9999), relocatedWeight(-9999), distance(-9999), azimuth(-9999)
{
  originalPhaseHint = phaseHint;
  phaseHint = std::string("P");
}

// operators

bool MyPick::operator ==(const MyPick& mp)
{
  // first attempt to remove SC dependance
  //return stationCode == mp.stationCode && pickTime == mp.pickTime;

  return index == mp.index;
}

bool MyPick::operator !=(const MyPick& mp)
{
  // first attempt to remove SC dependance
  //return !(stationCode == mp.stationCode && pickTime == mp.pickTime);

  return index != mp.index;
}

bool MyPick::operator <(const MyPick& mp)
{
  return pickTime < mp.pickTime;
}

// methods

void MyPick::setOriginTime(float stationTime)
{
  originTime = pickTime - stationTime;
}

bool MyPick::isSPhase() const
{
  return phaseHint == std::string("S");
}

// non-class

std::ostream& operator <<(std::ostream& os, MyPick& myPick)
{
  os << myPick.stationCode
     << " "
     << std::setprecision(3) << std::fixed << myPick.pickTime
     << " (" << myPick.phaseHint << ")";
  
  return os;
}

const char *MyPick::writeUnusedString()
{
  static char cstr[1024];
  
  sprintf(cstr,
          "%s %.3lf",
          stationCode.c_str(),
          pickTime);

  return cstr;
}

const char *MyPick::writeString()
{
  static char cstr[1024];  
  std::string polarityStr;
  
  if (polarity == POLARITY_POSITIVE) {
    polarityStr = std::string("U");
  }
  else if (polarity == POLARITY_NEGATIVE) {
    polarityStr = std::string("D");
  }
  else {
    polarityStr = std::string("_");
  }
  
  sprintf(cstr,
          "%s %.3lf %s %s",
          stationCode.c_str(),
          pickTime,
          phaseHint.c_str(),
          polarityStr.c_str());
  
  // write out the history (temporary)
  /*
  sprintf(cstr, "%s < ", cstr);
  std::list<int>::iterator hiter;
  for (hiter = historyList.begin(); hiter != historyList.end(); hiter++) {
    sprintf(cstr, "%s%d ", cstr, *hiter);
  }
  sprintf(cstr, "%s>", cstr);  
  */
  
  return cstr;
}

void MyPick::writeAndreas(std::ostream& os, float tt)
{
  std::string polarityStr;
  
  if (polarity == POLARITY_POSITIVE) {
    polarityStr = std::string("U");
  }
  else if (polarity == POLARITY_NEGATIVE) {
    polarityStr = std::string("D");
  }
  else {
    polarityStr = std::string("_");
  }

  os << stationCode << " "
     << std::setprecision(3) << std::fixed << pickTime << " "
     << timeResidual << " "
     << tt << " "
     << relocatedWeight << " "
     << phaseHint << " "
     << polarityStr;
  
  // write out the history (temporary)
  /*
  os << " < ";
  std::list<int>::iterator hiter;
  for (hiter = historyList.begin(); hiter != historyList.end(); hiter++) {
    os << *hiter << " ";
  }
  os << ">";
  */
}

#ifdef MY_DEBUG
void MyPick::writeDebug(std::ostream& os) const
{
  os << pickTime << " [" << stationCode << "] ";
}
#endif

#ifdef MY_DEBUG
void MyPick::writeDebugExtra(std::ostream& os) const
{
  os << "pick '" << stationCode << "' [" << pickTime << "]";
}
#endif

void MyPick::writeInfo()
{
  std::cerr << stationCode
            << " "
            << std::fixed << std::setprecision(2) << pickTime
            << std::endl;
}

// external

bool comparePickOriginLT(const MyPick& mp1, const MyPick& mp2)
{
  return mp1.originTime < mp2.originTime;
}
