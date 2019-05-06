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
#include <cstdlib>
#include <time.h>

#include "stack.h"

static bool myPickSpecialLT(const MyPick& p1, const MyPick& p2)
{
  //return p1.pickPtr < p2.pickPtr;

  /* first attempt to remove SC dependance
  if (p1.stationCode < p2.stationCode) {
    return true;
  }
  else {
    return p1.pickTime < p2.pickTime;
  }
  */

  return p1.index < p2.index;
}

//--------------------------------------------------------------------------------

void Stack::deduceLatticeParameters(float ss, float sd, int& nx, int& ny, int& nz)
{
  // ss is the required cubic cell parameter (i.e. side length)
  float temp;
  
  nx = int((max.x - min.x) / ss) + 1;
  temp = (float(nx) * ss) - (max.x - min.x);
  min.x -= 0.5f * temp;
  max.x += 0.5f * temp;
  
  ny = int((max.y - min.y) / ss) + 1;
  temp = (float(ny) * ss) - (max.y - min.y);
  min.y -= 0.5f * temp;
  max.y += 0.5f * temp;

  min.z = -sd;
  nz = int((max.z - min.z) / ss) + 1;
  temp = (float(nz) * ss) - (max.z - min.z);
  min.z -= 0.5f * temp;
  max.z += 0.5f * temp;
}

//--------------------------------------------------------------------------------

Stack::~Stack()
{
  if (nodes != NULL) {
    StackNode *ptr = nodes;
    for (int i = 0; i < idx.number(); i++) {
      ptr->clean();
      ptr++;
    }
    delete[] nodes;
    nodes = NULL;
  }
}

bool Stack::create(float ss, float sd, float (*ttFunc)(const Position&, const Position&, float&), std::list<Position> stationPositionList)
{
  int nx, ny, nz;
  std::list<Position>::iterator piter;
  
  // find bounding box containing stations
  min = Position(FLT_MAX, FLT_MAX, FLT_MAX);
  max = Position(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  for (piter = stationPositionList.begin(); piter != stationPositionList.end(); piter++) {
    checkMinMax(*piter, min, max);
  }
  
  // find parameters to suit the cell length cubic lattice parameter 'ss'
  deduceLatticeParameters(ss, sd, nx, ny, nz);

  // allocate nodes
  idx = ArrayIndex(nx, ny, nz);
  if ((nodes = new StackNode[idx.number()]) == NULL) {
    std::cerr << "Stack::create: memory allocation error" << std::endl;
    return false;
  }
  
  // computer synthetic tt at cell centres to all stations
  float halfSs = 0.5f * ss;
  for (int k = 0; k < idx.nz; k++) {
    for (int j = 0; j < idx.ny; j++) {
      for (int i = 0; i < idx.nx; i++) {
        StackNode sn = StackNode(min + Position(i*ss + halfSs, j*ss + halfSs, k*ss + halfSs), ArrayPosition(i, j, k));
        if (!sn.create(stationPositionList, ttFunc)) {
          std::cerr << "Stack::create: could not create a stacknode" << std::endl;
          return false;
        }
        nodes[idx(i, j, k)] = sn;
      }
    }
  }
  
  return true;
}

void Stack::cleanPicks(std::list<MyPick>& cleanPickList)
{
  StackNode *snPtr = nodes;
  
  for (int i = 0; i < idx.number(); i++) {
    snPtr->cleanPicks(cleanPickList);
    snPtr++;
  }
}

bool Stack::cleanOldPicks(const MyPick& pick, float stackWindow, std::list<MyPick>& unusedPickList)
{
  unsigned int nstackPicks = 0;

  float cutoffTime = pick.pickTime - stackWindow;
  
  StackNode *snPtr = nodes;
  for (int i = 0; i < idx.number(); i++) {
    nstackPicks += snPtr->cleanOldPicks(cutoffTime, unusedPickList);
    snPtr++;
  }

  if (!unusedPickList.empty()) {
    unusedPickList.sort(myPickSpecialLT);
    unusedPickList.unique();
    return true;
  }
  
  return false;
}

bool Stack::tidyPicks(std::list<MyPick>& tidyPickList)
{
  StackNode *snPtr = nodes;
  
  for (int i = 0; i < idx.number(); i++) {
    snPtr->tidyPicks(tidyPickList);
    snPtr++;
  }
  
  if (!tidyPickList.empty()) {
    tidyPickList.sort(myPickSpecialLT);
    tidyPickList.unique();
    return true;
  }
  
  return false;
}

bool Stack::addPick(std::map<std::string, int>& stationMap, MyPick pick)
{
  std::map<std::string, int>::iterator iter;
  int id;
  
  // lookup the integer offset of the station to use in the sythetic arrays held in the StackNodes
  if ((iter = stationMap.find(pick.stationCode)) == stationMap.end()) {
    std::cerr << "Stack::addPick: unknown station code '"
              << pick.stationCode
							<< "' found in test pick" << std::endl;
    return false;
  }
  else {
    id = iter->second;
  }
  
  // add the new pick into each node and compute it's origin time at that node
  StackNode *snPtr = nodes;
  for (int i = 0; i < idx.number(); i++) {
    snPtr->associate(id, pick);
    snPtr++;
  }

  return true;
}

bool Stack::addPick(std::map<std::string, int>& stationMap, std::list<MyPick> pickList)
{
  std::list<MyPick>::iterator miter;

  for (miter = pickList.begin(); miter != pickList.end(); miter++) {
    if (!addPick(stationMap, *miter)) {
      return false;
    }
  }

  return true;
}

/* Original version
bool Stack::identifyNewEvents(int eventThreshold, float originThreshold, std::list<Event>& newEventList)
{
  std::list<MyPick> redundantPickList;
  bool newEventExistFlag = false;
  StackNode *snPtr = NULL;
  Event event;
  int i;

  // find any nodes containing 'eventThreshold' picks, and declare a new event
  // (and collect the picks that make up this event)
  snPtr = nodes;
  for (i = 0; i < idx.number(); i++) {
    if (snPtr->identifyEvent(eventThreshold, originThreshold, event)) {
      newEventList.push_back(event);
      redundantPickList.merge(event.pickList);
      newEventExistFlag = true;
    }
    snPtr++;
  }

  if (newEventExistFlag) {
    // process the 'redundantPickList' to remove any duplicates
    redundantPickList.sort(myPickSpecialLT);
    redundantPickList.unique();
    // remove the picks associated with the new events from the stack nodes
    snPtr = nodes;
    for (i = 0; i < idx.number(); i++) {
      snPtr->cleanPicks(redundantPickList);
      snPtr++;
    }
  }

  return newEventExistFlag;
}
*/

bool Stack::identifyNewEvents(int eventThreshold, float originThreshold, std::list<Andy::Event>& newEventList)
{
  StackNode *snPtr = NULL;
  Andy::Event event;
  bool newEventExistFlag = false;

  // find any nodes containing 'eventThreshold' picks, and declare new events
  snPtr = nodes;
  for (int i = 0; i < idx.number(); i++) {
    if (snPtr->identifyEvent(eventThreshold, originThreshold, event)) {
      // remove any similar phases
      //event.removeSimilarPicks();
      //if (event.pickList.size() >= eventThreshold) {
        newEventList.push_back(event);
        newEventExistFlag = true;
      //}
    }
    snPtr++;
  }
  
  return newEventExistFlag;
}
