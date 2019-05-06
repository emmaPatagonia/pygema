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
#include <fstream>
#include <list>
#include <map>

#include "mypick.h"
#include "position.h"
#include "cartesian.h"
#include "stack.h"
#include "event.h"
#include "mystation.h"

#ifndef _BINDER_H
#define _BINDER_H

class Binder {
public:
  class Param {
  public:
    float (*ttFunc)(const Position&, const Position&, float&);
    float stackSpacing;
    float stackDepth;
    float stackWindow;
    float associationThreshold;
    unsigned int eventThreshold;
    float pEventWindow;
    float sEventWindow;
    float pToSVelocityRatio;
    float eventCutoff;
    float pPickResidualCutoff;
    float sPickResidualCutoff;
    std::string eventFilename;
    std::fstream *eventFile;
    std::string unusedPickFilename;
    FILE *unusedPickFile;
    std::string networkConfigFilename;
    std::string velmodFilename;
    std::string hypo71Filename;
    std::string latencyFilename;
    float latencyValue;

    Param() : ttFunc(NULL),
              stackSpacing(0.0f), stackDepth(0.0f), stackWindow(0.0f),
              associationThreshold(0.0f), eventThreshold(0),
              pEventWindow(0.0f), sEventWindow(0.0f), pToSVelocityRatio(1.0f),
              eventCutoff(0.0f), pPickResidualCutoff(0.0f), sPickResidualCutoff(0.0f),
              latencyValue(0.0f)  {};
    virtual ~Param() {}
  };
  
private:
  Param param;
  Stack stack;
  Cartesian cart;
  std::map<std::string, int> stationMap;
  std::list<Position> stationPositionList;
  std::list<Andy::Event> eventList;
  int nextPickIndex;
#ifdef MY_DEBUG
  std::list<MyPick> debugUnassociatedPickList;
  std::list<Andy::Event> debugEventList;
#endif

  bool lookupStationPosition(const MyPick& pick, Position& stationPosition);
  bool associatePickWithExistingEvent(MyPick pick, std::list<MyPick>& unassociatedPickList);
  void addNewEvents(std::list<Andy::Event> newEventList, std::list<MyPick>& unassociatedPickList);
  bool dumpEvent(Andy::Event e);
  bool testForFinishedEvents(const MyPick& pick);
  void writeUnusedPicks(std::list<MyPick>& unusedPickList);
  void processStack();

public:
  Binder() : nextPickIndex(0) {}
  virtual ~Binder() {}

  bool create(const Param& _param, std::list<MyStation> stationList);
  bool addPick(MyPick pick);
  bool tidy();
  std::string getEventFilename() { return param.eventFilename; }

#ifdef MY_DEBUG
  bool debugCreateSynthetics(std::list<Station> stationList);
#endif

  friend std::ostream& operator <<(std::ostream& os, Binder& binder);
};
  
#endif // !_BINDER_H
