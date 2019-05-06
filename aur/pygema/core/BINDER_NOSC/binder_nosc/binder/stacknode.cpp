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
#include <cmath>

#include "stacknode.h"

// utility class for stacknode association method
class Interval : public std::list<MyPick>
{
public:
  Interval() {}
  
  bool operator <(const Interval& i)
  {
    return size() < i.size();
  }
  
  double averageOriginTime()
  {
    double sum = 0.0;
    std::list<MyPick>::iterator iter;
    
    if (empty()) return 0.0;
    
    for (iter = begin(); iter != end(); iter++) {
      sum += iter->originTime;
    }
    
    return sum / size();
  }

  double averageRMS()
  {
    double sum = 0.0;
    float rms = 0.0;
    std::list<MyPick>::iterator iter;

    if (empty()) return 999.0;

    for (iter = begin(); iter != end(); iter++) {
      sum += iter->originTime;
    }
    sum /= size();

    for (iter = begin(); iter != end(); iter++) {
      rms += (sum-iter->originTime)*(sum-iter->originTime);
    }
    return sqrt(sum/size());
  }

};

// methods  
bool StackNode::create(std::list<Position> stationPositionList, float (*tt1d)(const Position&, const Position&, float&))
{
  std::list<Position>::iterator iter;
  int n;
  float dummy;
	
  if ((nStations = stationPositionList.size()) == 0) {
    std::cerr << "StackNode::create: no station positions given" << std::endl;
    return false;
  }
  
  if ((stationTimes = new float[nStations]) == NULL) {
    std::cerr << "StackNode::create: memory allocation error" << std::endl;
    return false;
  }
  
  for (iter = stationPositionList.begin(), n = 0; iter != stationPositionList.end(); iter++, n++) {
    stationTimes[n] = tt1d(position, *iter, dummy);
  }
  
  return true;
}

bool StackNode::associate(int id, MyPick pick)
{
  //if(stationTimes[id]<35.0) {   // try to avoid flooding the stack with far distance travel times
  if(stationTimes[id]<20.0) {   // try to avoid flooding the stack with far distance travel times
      pick.setOriginTime(stationTimes[id]);
      pickList.push_back(pick);
  }
   
  return true;
}

/*
static bool duplicateStation(const MyPick& mp, std::list<MyPick>& mpicks)
{
  std::list<MyPick>::iterator mpIter;

  std::string stationName = mp.pickPtr->waveformID().stationCode();
  
  for (mpIter = mpicks.begin(); mpIter != mpicks.end(); mpIter++) {
    if (stationName == mpIter->pickPtr->waveformID().stationCode()) {
      return true;
    }
  }
  
  return false;
}
*/

bool StackNode::identifyEvent(unsigned int eventThreshold, float originThreshold, Andy::Event& event)
{
  std::list<MyPick>::iterator iter1, iter2;
  Interval interval;
  std::list<Interval> intervals;

  if (pickList.size() >= eventThreshold) {
    pickList.sort(comparePickOriginLT);
    for (iter1 = pickList.begin(); iter1 != pickList.end(); iter1++) {
      double stopTime = iter1->originTime + originThreshold;
      iter2 = iter1;
      interval.clear();
      while (iter2 != pickList.end() && iter2->originTime <= stopTime) {
        interval.push_back(*iter2);
        iter2++;
      }
      intervals.push_back(interval);
    }
    
    intervals.sort();
    interval = intervals.back();
    
    if (interval.size() >= eventThreshold) {
      //event = Andy::Event(interval.averageOriginTime(), position, interval);
      event = Andy::Event(interval.averageOriginTime(),interval.averageRMS(), position, interval);
      return true;
    }
  }
  
  return false;
}

/*
bool StackNode::identifyEvent(unsigned int eventThreshold, float originThreshold, Event& event)
{
  std::list<MyPick>::iterator miter;
  std::list<MyPick> eventPickList;
  double sum = 0.0;
  unsigned int count;

  if (pickList.size() >= eventThreshold) {
    pickList.sort(comparePickOriginLT);
    double originMax = pickList.back().originTime;
    for (miter = pickList.begin(), count = pickList.size(); miter != pickList.end() && count >= eventThreshold; miter++, count--) {
      if (originMax - miter->originTime <= originThreshold) {
        while (miter != pickList.end()) {
          if (!duplicateStation(*miter, eventPickList)) {
            eventPickList.push_back(*miter);
            sum += miter->originTime;
          }
          miter++;
        }
        if (eventPickList.size() >= eventThreshold) {
          event = Event(sum/double(eventPickList.size()), position, eventPickList);
          return true;
        }
        else {
          return false;
        }
      }
    }
  }

  return false;
}
*/

unsigned int StackNode::cleanOldPicks(float cutoffTime, std::list<MyPick>& oldPickList)
{
  std::list<MyPick> currentPickList;
  std::list<MyPick>::iterator miter;
  
  for (miter = pickList.begin(); miter != pickList.end(); miter++) {
    if (miter->pickTime < cutoffTime) {
      //miter->historyList.push_back(-3); // history
      oldPickList.push_back(*miter);
    }
    else {
      currentPickList.push_back(*miter);
    }
  }

  pickList = currentPickList;
  
  return pickList.size();
}

void StackNode::cleanPicks(std::list<MyPick>& cleanPickList)
{
  std::list<MyPick>::iterator miter;
  
  for (miter = cleanPickList.begin(); miter != cleanPickList.end(); miter++) {
    pickList.remove(*miter);
  }
}

void StackNode::tidyPicks(std::list<MyPick>& tidyPickList)
{
  // add this nodes picks to the returned list
  tidyPickList.merge(pickList);
  
  // delete all picks from this node
  pickList.clear();
}

void StackNode::clean()
{
  if (stationTimes != NULL) {
    delete[] stationTimes;
    stationTimes = NULL;
  }
}
