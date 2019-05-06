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

#include "binder.h"
#include "debug.h"
#include "livloc.h"

//debug
static void printPicks(std::list<MyPick> picks)
{
  for (std::list<MyPick>::iterator iter = picks.begin(); iter != picks.end(); iter++) {
    std::cerr << *iter << std::endl;
	}
}

#ifdef MY_DEBUG
static void debugStationPositionList(std::list<Position> stationPositionList)
{
  std::list<Position>::iterator piter;

  std::cerr << "*** List of station positions *** " << std::endl;

  FILE *xyzOut = fopen("xyz.txt", "w");
  
  for (piter = stationPositionList.begin(); piter != stationPositionList.end(); piter++) {
    std::cerr << "x = " << piter->x << " y = " << piter->y << " z = " << piter->z << std::endl;
    fprintf(xyzOut, "%f\t%f\t\%f\n", piter->x, piter->y, 1000.0f * piter->z);
  }
  
  fclose(xyzOut);
  
  std::cerr << "*** End of station positions list *** " << std::endl;
  std::cerr.flush();
}
#endif // MY_DEBUG

//--------------------------------------------------------------------------------

static bool eventCmpGT(const Andy::Event& e1, const Andy::Event& e2)
{
  bool val;

  if(e1.pickList.size() > e2.pickList.size()) {
	val = true;
  } else if (e1.pickList.size() == e2.pickList.size()) {
	if(e1.rms<e2.rms) {
	  val = true;
	} else {
	  val = false;
	}
  } else {
	val = false;
  }
	
  return val;
}

//--------------------------------------------------------------------------------

static bool myPickSpecialLT(const MyPick& p1, const MyPick& p2)
{
  // original
  //return p1.pickPtr < p2.pickPtr;

  /* first attempt to remove SC dependance
  if (p1.stationCode < p2.stationCode) { // if this doesn't work, assign a unique index to each pick in addPick
    return true;
  }
  else {
    return p1.pickTime < p2.pickTime;
  }
  */

  return p1.index < p2.index;
}

//--------------------------------------------------------------------------------

bool Binder::dumpEvent(Andy::Event e)
{
#ifdef MY_DEBUG
  debugEventList.push_back(e);
#endif
  
  if (!param.eventFile->is_open()) {
    std::cerr << "Binder::dumpEvent: event output file is not available to write on" << std::endl;
    return false;
  }

  // if there is a registered callback then do it, else just write to file
  e.write(*(param.eventFile), cart, param.ttFunc, param.pToSVelocityRatio, stationMap, stationPositionList);
  
  param.eventFile->flush();

  return true;
}

//--------------------------------------------------------------------------------

// test the events in the list "eventList" against some time to say they have had their day
bool Binder::testForFinishedEvents(const MyPick& recentPick)
{
  std::list<Andy::Event>::iterator eiter;
  std::list<Andy::Event> tempEventList;
  MyPick earliestPick;

  unsigned int nevents = eventList.size();
  
  double recentTime = recentPick.pickTime;

  for (eiter = eventList.begin(); eiter != eventList.end(); eiter++) {
    earliestPick = eiter->pickList.front();
    if (recentTime - earliestPick.pickTime > param.eventCutoff) {
      dumpEvent(*eiter);
    }
    else {
      tempEventList.push_back(*eiter);
    }
  }
  
  eventList = tempEventList;
  
  return eventList.size() != nevents;
}

//--------------------------------------------------------------------------------

// lookup the integer offset of the station code that received this pick
bool Binder::lookupStationPosition(const MyPick& pick, Position& stationPosition)
{
  std::map<std::string, int>::iterator miter;
  std::list<Position>::iterator piter;
  int id, count;

  std::string stationName = pick.stationCode;

  if ((miter = stationMap.find(pick.stationCode)) == stationMap.end()) {
    std::cerr << "Binder::lookupStationPosition: unknown station code '"
		          << stationName
							<< "' found in pick" << std::endl;
    return false;
  }
  else {
    id = miter->second;
  }

  for (piter = stationPositionList.begin(), count = 0; piter != stationPositionList.end(); piter++, count++) {
    if (count == id) {
      stationPosition = *piter;
      break;
    }
  }
  
  return true;
}

//--------------------------------------------------------------------------------

// attempt to find an event for this pick to be part of... if so, relocate the event including the new pick
bool Binder::associatePickWithExistingEvent(MyPick pick, std::list<MyPick>& unassociatedPickList)
{
  std::list<Andy::Event>::iterator eiter;
  std::list<Andy::Event> tempEventList;
  std::list<MyPick>::iterator miter;
  Position stationPosition;
 
  if (!lookupStationPosition(pick, stationPosition)) {
    return false;
  }

  for (eiter = eventList.begin(); eiter != eventList.end(); eiter++) {
    if (eiter->associatePick(stationPosition, param.ttFunc, param.pPickResidualCutoff, param.sPickResidualCutoff, param.pEventWindow, param.sEventWindow, param.pToSVelocityRatio, pick)) {
      if (eiter->relocate(cart, param.pPickResidualCutoff, param.sPickResidualCutoff, param.eventThreshold, unassociatedPickList)) {
        for (miter = unassociatedPickList.begin(); miter != unassociatedPickList.end(); miter++) {
          if (*miter == pick) {
            unassociatedPickList.remove(pick);
            return false;
          }
        }
        return true;
      }
      else {
        eventList.erase(eiter);
        unassociatedPickList.remove(pick);
        return false;
      }
    }
  }
  
  return false;
}

//--------------------------------------------------------------------------------

// find the primary event: this is either the one containing most picks or the first
// event in the list if they all have the same number of picks.
// Compare each pick in the primary event with those in other events: if identical picks
// show up they are removed from the other events. If the number of picks remaining in an
// event is less than 'eventThreshold' then kill the event.
bool filterEventsForCommonPicks(std::list<Andy::Event>& eventList, unsigned int eventThreshold, std::list<MyPick>& unassociatedPickList)
{
  Andy::Event primary, secondary;
  std::list<Andy::Event> editedEventList;
  std::list<MyPick>::iterator miter;
  
  // event with most picks is put first in the list and becomes the 'primary' event
  eventList.sort(eventCmpGT);

  // the first list entry becomes the primary event, and is definitely returned
  primary = eventList.front();
  eventList.pop_front();
  editedEventList.push_back(primary);

  // check thru the rest of the events... remove picks in secondary that are found
  // in primary... if there are still enough picks left in secondary it is returned
  // as an event else the picks left over in secondary are added to unassociatedPickList
  while (eventList.size() > 0) {
    secondary = eventList.front();
    eventList.pop_front();
    if (secondary.filterPicks(eventThreshold, primary, unassociatedPickList)) {
      editedEventList.push_back(secondary);
    }
  }
  
  // return the edited event list
  eventList = editedEventList;

  // remove any duplicates from the unassociatedPickList
  unassociatedPickList.sort(myPickSpecialLT);
  unassociatedPickList.unique();
  
  return !unassociatedPickList.empty();
}

//--------------------------------------------------------------------------------

void Binder::addNewEvents(std::list<Andy::Event> newEventList, std::list<MyPick>& unassociatedPickList)
{
  std::list<Andy::Event>::iterator eiter;

  for (eiter = newEventList.begin(); eiter != newEventList.end(); eiter++) {
    if (eiter->relocate(cart, param.pPickResidualCutoff, param.sPickResidualCutoff, param.eventThreshold, unassociatedPickList)) {
      eventList.push_back(*eiter);
    }
  }
  
  unassociatedPickList.sort(myPickSpecialLT);
  unassociatedPickList.unique();
}

//--------------------------------------------------------------------------------

void Binder::writeUnusedPicks(std::list<MyPick>& unusedPickList)
{
  std::list<MyPick>::iterator miter;
  
  for (miter = unusedPickList.begin(); miter != unusedPickList.end(); miter++) {
    fprintf(param.unusedPickFile, "%s (%s)\n", miter->writeUnusedString(), miter->phaseHint.c_str()); // added phaseHint 23/6/10
  }
}

//--------------------------------------------------------------------------------

bool Binder::create(const Binder::Param& _param, std::list<MyStation> stationList)
{
  std::list<MyStation>::iterator iter;
  int count;
  Position p, minDeg, maxDeg;

  if (_param.ttFunc == NULL) {
    std::cerr << "Binder::create: null passed as TT function" << std::endl;
    return false;
  }

  param = _param;
  
  minDeg = Position(FLT_MAX, FLT_MAX);
  maxDeg = Position(-FLT_MAX, -FLT_MAX);
  
  // open output files
  if (param.eventFilename.empty()) {
    std::cerr << "Binder::create: no event file name specified" << std::endl;
    return false;
  }
  else {
    param.eventFile = new std::fstream(param.eventFilename.c_str(), std::ios_base::out);
    if (!param.eventFile->is_open()) {
      std::cerr << "Binder::create: cannot open event file '" << param.eventFilename << "'" << std::endl;
      return false;
    }
  }
  
  if (param.unusedPickFilename.empty()) {
    param.unusedPickFile = NULL;
  }
  else {
    if ((param.unusedPickFile = fopen(param.unusedPickFilename.c_str(), "w")) == NULL) {
      std::cerr << "Binder::create: cannot open event file '" << param.unusedPickFilename << "'" << std::endl;
      return false;
    }
  }

  // map the station code names to their index in the original list...
  // the indices will be looked up when a new pick arrives and used to reference the appropriate station
  for (iter = stationList.begin(), count = 0; iter != stationList.end(); iter++, count++) {
    stationMap[iter->code()] = count;
    checkMinMax(iter->longitude(), iter->latitude(), minDeg, maxDeg);
  }

  // use the min/max Area parameters from constants here?

  cart = Cartesian((minDeg.x + maxDeg.x)/2.0f, (minDeg.y + maxDeg.y)/2.0f, 0.0f);

	// station map is for livLocObj use
	std::map<std::string, Position> livLocStationMap;
	
  for (iter = stationList.begin(); iter != stationList.end(); iter++) {
    if (!cart.toXYZ(iter->longitude(), iter->latitude(), &p.x, &p.y)) {
      std::cerr << "Binder::create: cannot convert lat/lon/elev to XYZ" << std::endl;
      return false;
    }
    p.z = 0.001f * iter->elevation(); // convert elevation to km
    stationPositionList.push_back(p);
		livLocStationMap[iter->code()] = p;
  }

  if (!stack.create(param.stackSpacing, param.stackDepth, param.ttFunc, stationPositionList)) {
    std::cerr << "Binder::create: cannot create the stack object" << std::endl;
    return false;
  }

  // initialise the pick index counter... this variable is used to assign a unique
  // ID (index) to each pick as it is added to the binder. The index is used in the
  // less-than and equal-to operators of the MyPick for sorting and comparison
  nextPickIndex = 0;

  // *************
	// initialise the LivLoc relocation object;
  // *************

	/*
	 parameters (and default values in parenthesis) for livLocObj... of the form "livLocObj->rp.<param_name>"
	 
	startDepth(1.0), iterMax(10), iterRunMax(10), stdDevCutP(4.0), stdDevCutS(6.0),
	minWeight(0.1), sWeight(0.5), locAccuracy(0.05), residualCut(2.0), resCutRatio(2) {}
  */
  // reloc params 
  /* these parameters are working for Maule and are used in v4
	livLocObj->rp.startDepth  =  -10.0;
        livLocObj->rp.stdDevCutP  =  3.0;
        livLocObj->rp.stdDevCutS  =  3.0;
	livLocObj->rp.locAccuracy = 0.05;
	livLocObj->rp.minWeight = 0.0;
	livLocObj->rp.iterMax = 50.0;
	livLocObj->rp.iterRunMax = 1.0;
	livLocObj->rp.resCutRatio = 100.0;
  */
	livLocObj->rp.startDepth  =  -10.0;
        livLocObj->rp.stdDevCutP  =  4.0;
        livLocObj->rp.stdDevCutS  =  4.0;
	livLocObj->rp.locAccuracy = 0.05;
	livLocObj->rp.minWeight = 0.1;
	livLocObj->rp.iterMax = 50.0;
	livLocObj->rp.iterRunMax = 1.0;
	livLocObj->rp.resCutRatio = 100.0;
  // other stuff
	livLocObj->stationMap = livLocStationMap;
  livLocObj->pToSVelocityRatio = param.pToSVelocityRatio;
	livLocObj->tt1d = param.ttFunc;
	livLocObj->cart = cart;
	
  return true;
}

//--------------------------------------------------------------------------------

// called from the main application
bool Binder::addPick(MyPick pick)
{
  //std::cout << "pick added to binder: " << pick << std::endl; // debug

  static int stackPickTotal = 0; //debug

  // lists are created empty on each call of this function
  //std::list<Andy::Event> newEventList;
  std::list<MyPick> unassociatedPickList, unusedPickList;

  // assign a unique index to the pick (used for comparison between picks)
  pick.index = nextPickIndex++;

  // check for finished events
  testForFinishedEvents(pick);
  
  // attempt to associate this pick with an existing event (if pick not associated as a "P" then attempt as an "S")
  bool associatedFlag = associatePickWithExistingEvent(pick, unassociatedPickList);
  
  // picks may have unassociated from events during a relocation done just above (i.e. if residuals too large)
  if (!unassociatedPickList.empty()) {
    stack.addPick(stationMap, unassociatedPickList);
    unassociatedPickList.clear();
  }
  
  // we have associated the new pick - so get out and wait for the next one
  if (associatedFlag) {
    return true;
  }

  // clean out any old picks that have not been associated by now
  if (stack.cleanOldPicks(pick, param.stackWindow, unusedPickList)) {
    if (param.unusedPickFile != NULL) {
      writeUnusedPicks(unusedPickList);
    }
    unusedPickList.clear();
  }
  
  // add the new pick to the stack
  //pick.historyList.push_back(2); // history
  stack.addPick(stationMap, pick);

  // attempt to find new events inside the stack
  processStack();
  
  // the new pick did not make anything happen
  return false;
}

//--------------------------------------------------------------------------------

// find unique contents of stack and write to the unused pick file
bool Binder::tidy()
{
  std::list<MyPick> unusedPickList;
  std::list<MyPick>::iterator miter;

  if (stack.tidyPicks(unusedPickList)) {
    for (miter = unusedPickList.begin(); miter != unusedPickList.end(); miter++) {
      //miter->historyList.push_back(-4); // history
      fprintf(param.unusedPickFile, "%s (%s)\n", miter->writeUnusedString(), miter->phaseHint.c_str());
    }
  }

  return true;
}

//--------------------------------------------------------------------------------

#ifdef MY_DEBUG
bool Binder::debugCreateSynthetics(std::list<Station> stationList)
{
  Position min, max;
  const int nSynthEvents = 1;
  const int eventSeparation = 1000;
  Position synthEventLoc, synthEventLatLon;
  std::list<Station>::iterator siter;
  std::list<Position>::iterator piter;
  FILE *outFile = NULL, *locFile = NULL;
  char outFilename[256];
  float minT, maxT, t;

  stack.getMinMax(min, max);
  /*
  float xdiff = max.x - min.x;
  float ydiff = max.y - min.y;
  float zdiff = max.z - min.z;
  */
  
  float basetime = 0.0f;

  if ((locFile = fopen("synthloc.txt", "w")) == NULL) {
    std::cerr << "debugCreateSynthetics: cannot open file 'synthloc.txt'" << std::endl;
    return false;
  }

  srand((unsigned int)time(NULL));
  
  for (int i = 0; i < nSynthEvents; i++) {
    /*
    synthEventLoc.x = min.x + ((float)rand()/float(RAND_MAX))*xdiff;
    synthEventLoc.y = min.y + ((float)rand()/float(RAND_MAX))*ydiff;
    synthEventLoc.z = min.z + ((float)rand()/float(RAND_MAX))*zdiff;
    */
    synthEventLoc = Position(0.0f, 0.0f, -13.0f);
    sprintf(outFilename, "event%d.txt", i);
    if ((outFile = fopen(outFilename, "w")) == NULL) {
      std::cerr << "debugCreateSynthetics: cannot open file '" << outFilename << "'" << std::endl;
      continue;
    }
    minT = FLT_MAX;
    maxT = -minT;
    for (piter = stationPositionList.begin(), siter = stationList.begin();
         piter != stationPositionList.end();
         piter++, siter++) {
      t = basetime + param.ttFunc(synthEventLoc, *piter);
      if (t < minT) minT = t;
      if (t > maxT) maxT = t;
      fprintf(outFile, "%.2f %s synthetic\n", t, siter->code().c_str());
    }
    fclose(outFile);
    basetime += eventSeparation;
    cart.toLonLat(synthEventLoc.x, synthEventLoc.y, &(synthEventLatLon.y), &(synthEventLatLon.x));
    synthEventLatLon.z = synthEventLoc.z;
    fprintf(locFile,
            "Event %d (range = %.2fs):\t[%.4f, %.4f, %.4f]\tor\t[%.2f, %.2f, %.2f]\n",
            i, maxT - minT,
            synthEventLatLon.x, synthEventLatLon.y, synthEventLatLon.z,
            synthEventLoc.x, synthEventLoc.y, synthEventLoc.z);
  }
  
  fclose(locFile);
  
  return true; 
}
#endif // MY_DEBUG

//--------------------------------------------------------------------------------

// non-class

std::ostream& operator <<(std::ostream& os, Binder& binder)
{
  std::list<Andy::Event>::iterator eiter;

  if (binder.eventList.empty()) {
    os << "No events found" << std::endl;
  }
  else {
    for (eiter = binder.eventList.begin(); eiter != binder.eventList.end(); eiter++) {
      eiter->write(os, binder.cart, binder.param.ttFunc, binder.param.pToSVelocityRatio,
                   binder.stationMap, binder.stationPositionList);
    }
#ifdef MY_DEBUG
    binder.eventList.merge(binder.debugEventList);
    for (eiter = binder.eventList.begin(); eiter != binder.eventList.end(); eiter++) {
      eiter->writeDebug(std::cerr, binder.cart);
      std::cerr << std::endl;
    }
#endif
  }
  
  return os;
}

//--------------------------------------------------------------------------------

/******************** N E W  S T A C K  S T U F F *************************/

static bool listsEqual(std::list<MyPick>& a, std::list<MyPick>& b)
{
  // same length?
  if (a.size() != b.size()) return false;

  // sort lists
  a.sort();
  b.sort();
  
  // lexi comparison
  std::list<MyPick>::iterator aiter, biter;
  for (aiter = a.begin(), biter = b.begin(); aiter != a.end(); aiter++, biter++) {
    if (*aiter != *biter) return false;
  }
  
  return true;
}

void Binder::processStack()
{
	Andy::Event event;
	std::list<Andy::Event> newEventList;
	std::list<MyPick> stackPicks, disassociatedPicks, remainingPicks;
	std::list<MyPick>::iterator miter;
	bool firstEventRelocated = false;
	Position sp;
	int nevent = 0; // debug
	int niter = 0; // debug

	if (!stack.identifyNewEvents(param.eventThreshold, param.associationThreshold, newEventList)) {
		//std::cout << "no new events found" << std::endl; // debug
		return;
	}

	// process new events found in the stack
	while (true) {
		if (firstEventRelocated) {
			// delete any previous events before checking the stack again
			newEventList.clear();
			if (stack.identifyNewEvents(param.eventThreshold, param.associationThreshold, newEventList)) {
				firstEventRelocated = false;
			}
			else {
				break;
			}
		}
		else {
			if (newEventList.empty()) {
				break;
			}
		}
		// std::cout << "new event (" << ++nevent << ") found in stack" << std::endl; // debug
		// event with most picks is put first in the list
		newEventList.sort(eventCmpGT);
		// process this event - next iteration might find another, etc
		event = newEventList.front();
		newEventList.pop_front();
		// remove any similar phases
		event.removeSimilarPicks();
		if (event.pickList.size() < param.eventThreshold) {
			continue;
		}
		// initial relocate
		disassociatedPicks.clear();
		if (event.relocate(cart, param.pPickResidualCutoff, param.sPickResidualCutoff, param.eventThreshold, disassociatedPicks)) {
			// std::cout << "initial relocate successful" << std::endl;  // debug
			firstEventRelocated = true;
			// remove the picks which make up 'event' from the stack
			stack.cleanPicks(event.pickList);
			// extract & remove the remaining picks from the stack
			stackPicks.clear();
			stack.tidyPicks(stackPicks);
			// sort by arrival time so that P comes before S
			stackPicks.sort();
			// process the stack picks repeatedly until they cease to change the event
			niter = 0; // debug
			while (true) {
				// std::cout << "looping over stack picks (iteration " << niter << ")" << std::endl; niter++; // debug
				// loop over the stack picks attempting to incorporate them into the event
				remainingPicks.clear();
				for (miter = stackPicks.begin(); miter != stackPicks.end(); miter++) {
					lookupStationPosition(*miter, sp);
					// attempt to associate pick with this event as a P or S
					if (event.associatePick(sp, param.ttFunc, param.pPickResidualCutoff, param.sPickResidualCutoff, param.pEventWindow, param.sEventWindow, param.pToSVelocityRatio, *miter)) {
						// relocate including the extra pick
						disassociatedPicks.clear();
						if (event.relocate(cart, param.pPickResidualCutoff, param.sPickResidualCutoff, param.eventThreshold, disassociatedPicks)) {
							if (!disassociatedPicks.empty()) {
								remainingPicks.merge(disassociatedPicks);
							}
						}
						else {
							remainingPicks.push_back(*miter);
						}
					}
					else {
						remainingPicks.push_back(*miter);
					}
				}
				// all stack picks are now processed, if nothing has changed since the last iteration then stop
				if (listsEqual(stackPicks, remainingPicks)) {
					break;
				}
				// something changed in the last iteration so repeat to try and associte more of the stack picks
				else {
					stackPicks = remainingPicks;
				}
			}
			eventList.push_back(event);
			if (!stackPicks.empty()) {
				stack.addPick(stationMap, stackPicks);
			}
		}
	}
	// debug
	std::cerr << std::endl << "*** return from ProcessStack" << std::endl << std::endl;
}
