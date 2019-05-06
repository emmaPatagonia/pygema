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

#include <map>
#include <iostream>
#include <list>
#include <time.h>

#include "mypick.h"
#include "position.h"
#include "cartesian.h"

#ifndef _EVENT_H
#define _EVENT_H

namespace Andy {
	
class Event {
public:
  double originTime;
  Position location;
  Position error;
  float rms;
  float gap;
  int numberP;
  int numberS;
  std::list<MyPick> pickList;

  Event() {}
  Event(double _originTime, const Position& _location, std::list<MyPick> _pickList);
  Event(double _originTime, float _rms, const Position& _location, std::list<MyPick> _pickList);
  virtual ~Event() {}
  
  bool operator <(const Event& event);
  bool operator ==(const Event& event);
  
  void addPick(const MyPick& pick);
  bool associatePick(const Position& stationPosition, float (*ttFunc)(const Position&, const Position&, float&), float pCutoff, float sCutoff, float pEventWindow, float sEventWindow, float pToSVelocityRatio, MyPick pick);
  bool filterPicks(unsigned int eventThreshold, Event& primary, std::list<MyPick>& unassociatedPickList);
  bool removeSimilarPicks();
  bool relocate(const Cartesian& cart, float pResidualCutoff, float sResidualCutoff, unsigned int eventThreshold, std::list<MyPick>& unassociatedPickList);
  void write(std::ostream& os, const Cartesian& cart, float (*ttFunc)(const Position&, const Position&, float&), float pToSVelocityRatio, std::map<std::string, int>& stationMap, std::list<Position> stationPositionList);
  bool relocateNLL(const Cartesian& cart, const char *pickFilename);
  bool relocateHypo71(const Cartesian& cart);
  //bool relocateLivLoc(float largeResidual, const Cartesian& cart);
	bool relocateLivLoc( const Cartesian& cart, std::list<MyPick>& unassociatedPickList );

#ifdef MY_DEBUG
  void writeDebug(std::ostream& os, const Cartesian& cart);
#endif
};

} // end namespace

#endif // !_EVENT_H
