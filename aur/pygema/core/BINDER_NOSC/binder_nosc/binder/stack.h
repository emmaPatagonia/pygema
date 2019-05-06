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
#include "position.h"
#include "cartesian.h"
#include "arrayindex.h"
#include "stacknode.h"
#include "event.h"

#ifndef _STACK_H
#define _STACK_H

class Stack {
private:
  Position min, max;
  ArrayIndex idx;
  StackNode *nodes;

  void deduceLatticeParameters(float ss, float sd, int& nx, int& ny, int& nz);

public:
  Stack() : nodes(NULL) {}
  virtual ~Stack();

  bool create(float stackSpacing, float stackDepth, float (*ttFunc)(const Position&, const Position&, float&), std::list<Position> stationPositionList);
  void cleanPicks(std::list<MyPick>& cleanPickList);
  bool cleanOldPicks(const MyPick& pick, float stackWindow, std::list<MyPick>& unusedPickList);
  bool tidyPicks(std::list<MyPick>& tidyPickList);
  bool addPick(std::map<std::string, int>& stationMap, MyPick pick);
  bool addPick(std::map<std::string, int>& stationMap, std::list<MyPick> pickList);
  bool identifyNewEvents(int eventThreshold, float originThreshold, std::list<Andy::Event>& newEventList);

#ifdef MY_DEBUG
  void getMinMax(Position& _min, Position& _max) { _min = min; _max = max; }
#endif
};

#endif // !_STACK_H
