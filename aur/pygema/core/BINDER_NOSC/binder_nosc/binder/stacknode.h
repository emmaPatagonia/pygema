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

#include <list>

#include "mypick.h"
#include "arrayposition.h"
#include "position.h"
#include "cartesian.h"
#include "event.h"

#ifndef _STACK_NODE_H
#define _STACK_NODE_H

class StackNode {
private:
  std::list<MyPick> pickList;
  int nStations;
  float *stationTimes;
  
public:
  Position position;
  ArrayPosition arrayPosition;
  
public:
  StackNode()
    : nStations(0), stationTimes(NULL) {}
  StackNode(Position _position, ArrayPosition _arrayPosition)
    : nStations(0), stationTimes(NULL), position(_position), arrayPosition(_arrayPosition) {}
  virtual ~StackNode() {}
  
  bool create(std::list<Position> stationPositionList, float (*ttFunc)(const Position&, const Position&, float&));
  bool associate(int id, MyPick pick);
  bool identifyEvent(unsigned int eventThreshold, float originThreshold, Andy::Event& event);
  unsigned int cleanOldPicks(float cutoffTime, std::list<MyPick>& oldPickList);
  void cleanPicks(std::list<MyPick>& targetPickList);
  void tidyPicks(std::list<MyPick>& tidyPickList);
  void clean();
};

#endif // !_STACK_NODE_H
