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
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <time.h>
#include <unistd.h>

#include <pdas.h>
#include <lsd.h>
#include "loma.h"
#include "utl_time.h"
#include "hypo71_core.h"
#include <hypo71.h>

// static structure initialised and cleaned as necessary
static AEvent aevent;
static FILE *logFilePtr = NULL;

#define _MY_DEBUG 1

static void makeStationCode(char *code)
{
  if (strlen(code) == 1) {
    code[1] = '_';
    code[2] = '_';
    code[3] = '_';
  }
  else if (strlen(code) == 2) {
    code[2] = '_';
    code[3] = '_';
  }
  else if (strlen(code) == 3) {
    code[3] = '_';
  }
}

static bool lookupAEventTraceResidual(AEvent *aeventPtr, const char *stationID, float& residual, float& weight, bool isSPhase)
{
  Compu_phase *comp = NULL;
  
  Trace *tracePtr = aeventPtr->tr_array;
  for (int i = 0; i < aeventPtr->no_traces; i++) {
    if (tracePtr->use_location == 1 && strcmp(tracePtr->stat_code, stationID) == 0) {
      if (isSPhase) {
        comp = tracePtr->scomp;
      }
      else {
        comp = tracePtr->pcomp;
      }
      if (comp == NULL) {
        std::cerr << "[Hypo71]lookupAEventTraceResidual: ";
        if (isSPhase) {
          std::cerr << "'scomp' member of Trace is NULL";
        }
        else {
          std::cerr << "'pcomp' member of Trace is NULL";
        }
        std::cerr << std::endl;
        return false;
      }
      else {
        residual = comp->residual;
        weight = comp->used_weight;
        return true;
      }
    }
    tracePtr++;
  }
  
  return false;
}

static bool lookupAEventResiduals(std::list<Hypo71::HypoPick>& hypoPickList, AEvent *aeventPtr)
{
  std::list<Hypo71::HypoPick>::iterator hiter;
  float residual, weight;
  char stationID[32];
  
  for (hiter = hypoPickList.begin(); hiter != hypoPickList.end(); hiter++) {
    memset(stationID, 0, 32 * sizeof(char));
    strcpy(stationID, hiter->stationCodeStr.c_str());
    makeStationCode(stationID);
    if (lookupAEventTraceResidual(aeventPtr, stationID, residual, weight, hiter->hasRelatedPPick)) {
      hiter->residual = residual;
      hiter->weight = weight;
    }
    else {
      std::cerr << "[Hypo71]lookupAEventResiduals: cannot find trace record for '"
                << stationID << "'" << std::endl;
      return false;
    }
  }
  
  return true;
}

/*----------------------------------------*/

static bool setAEventTracesNotUsed(AEvent *aeventPtr)
{
  Trace *tracePtr = aeventPtr->tr_array;
  for (int i = 0; i < aeventPtr->no_traces; i++) {
    tracePtr->use_location = 0;
    tracePtr->use_spick = 0;
    tracePtr->ppick_time = -1.0;
    tracePtr->spick_time = -1.0;
    tracePtr->pcomp = NULL;
    tracePtr->scomp = NULL;
    tracePtr++;
  }
  
  return true;
}

/*----------------------------------------*/

static bool setAEventTraceUsed(AEvent *aeventPtr, Hypo71::HypoPick hypoPick)
{
  char stationID[32], phaseCode[16];
  double pickTime;
  
	memset(stationID, 0, 32 * sizeof(char));
  strcpy(stationID, hypoPick.stationCodeStr.c_str());
  makeStationCode(stationID);
  pickTime = hypoPick.pickTime;  
  bool isSPhase = hypoPick.hasRelatedPPick;

  sprintf(phaseCode, "I%s%s%d", hypoPick.phaseHintStr.c_str(), hypoPick.polarityStr.c_str(), hypoPick.importedWeight);
  
  Trace *tracePtr = aeventPtr->tr_array;
  for (int i = 0; i < aeventPtr->no_traces; i++) {
    if (strcmp(tracePtr->stat_code, stationID) == 0) {
      tracePtr->use_location = 1;
      if (isSPhase) {
        tracePtr->use_spick = 1;
        tracePtr->spick_time = pickTime;
        strcpy(tracePtr->sphase_code, phaseCode);
      }
      else {
        tracePtr->ppick_time = pickTime;
        strcpy(tracePtr->pphase_code, phaseCode);
      }
      return true;
    }
    tracePtr++;
  }
  
  return false;
}

/*----------------------------------------*/

static bool setupTheAEventPicks(std::list<Hypo71::HypoPick> hypoPickList, AEvent *aeventPtr)
{
  std::list<Hypo71::HypoPick>::iterator hiter;

  for (hiter = hypoPickList.begin(); hiter != hypoPickList.end(); hiter++) {
    if (!setAEventTraceUsed(aeventPtr, *hiter)) {
      std::cerr << "cannot find station '" << hiter->stationCodeStr.c_str() << "' in AEvent struct" << std::endl;
      return false;
    }
  }
  
  return true;
}

/*----------------------------------------*/

static const char *makeTimeStr(double t)
{
  static char buf[64];
  
  TIME tt = do2date(t);
  
  memset(buf, 0, 64*sizeof(char));
  sprintf(buf,
          "%02d/%02d/%04d %02d:%02d:%05.2f",
          tt.day, tt.mo, tt.yr, tt.hr, tt.mn, tt.sec);
  
  return (const char *)&buf[0];
}

/*----------------------------------------*/

static bool logTheAEvent(std::list<Hypo71::HypoPick> hypoPickList, AEvent *aeventPtr)
{
  std::list<Hypo71::HypoPick>::iterator hiter;
  std::string rtnStr;
  char buf[256];
  
  if (logFilePtr == NULL) return false;

  fprintf(logFilePtr,
          "Event at %s\n",
          makeTimeStr(aeventPtr->sol.orig_time));

  fprintf(logFilePtr,
          "Location: %7.4f %7.4f %6.2f %6.3f %5.1f\n",
          aeventPtr->sol.event_lat, aeventPtr->sol.event_lon, aeventPtr->sol.event_depth,
          aeventPtr->sol.rms_residual, aeventPtr->sol.gap);
  fprintf(logFilePtr,
          "Error: %7.2f %7.2f %7.2f\n",
          aeventPtr->sol.lat_error, aeventPtr->sol.lon_error, aeventPtr->sol.depth_error);
    
  for (hiter = hypoPickList.begin(); hiter != hypoPickList.end(); hiter++) {
    memset(buf, 0, 256 * sizeof(char));
    sprintf(buf, "station '%s' @ %.2lf\n", hiter->stationCodeStr.c_str(), hiter->pickTime);
    rtnStr += std::string(buf);
  }
  if (rtnStr.empty()) {
     rtnStr = std::string("*** no stations found! ***");
  }
  else {
    memset(buf, 0, 256 * sizeof(char));
    sprintf(buf, "%d picks in window size of %.2lf\n", hypoPickList.size(), hypoPickList.front().pickTime - hypoPickList.back().pickTime);
    rtnStr += std::string(buf) + std::string("***---***\n");
  }
  fprintf(logFilePtr, "%s\n", rtnStr.c_str());
  
  fprintf(logFilePtr, "\n");
  fflush(logFilePtr);
 
  return true;
}

/*----------------------------------------*/

static bool findLocation(std::list<Hypo71::HypoPick> hypoPickList)
{
  // remove the last prt file (if it exists)
  unlink("hypo71.prt");
  
  // set all the traces to 'not used'
  if (!setAEventTracesNotUsed(&aevent)) {
    std::cerr << "cannot initialise the trace usage in the AEvent structure" << std::endl;
    return false;
  }
  
  // the 'traces' are used as an interface to the algorithm
  if (!setupTheAEventPicks(hypoPickList, &aevent)) {
    std::cerr << "cannot setup the AEvent structure" << std::endl;
    return false;
  }
  
  // locate and log the event
  if (do_best_location(&aevent) == 0) {
    if (!logTheAEvent(hypoPickList, &aevent)) {
      std::cerr << "unable to log the event" << std::endl;
      return false;
    }
  }
  
  return true;
}

/*----------------------------------------*/

static bool setupTheAEvent(AEvent *aeventPtr)
{
  char buf[256];
  
  // open the PDAS configuration file
  FILE *in = NULL;
  if ((in = fopen("info.dat", "r")) == NULL) {
    std::cerr << "[Hypo71] setupTheAEvent: cannot open 'info.dat'" << std::endl;
    return false;
  }
  
  // count the stations
  int nstation = 0;
  while (!feof(in)) {
   memset(buf, 0, 256 * sizeof(char));
   if (fgets(buf, 256, in) != NULL) {
      nstation++;
    }
  }
  rewind(in);
  
  // allocate memory
  aeventPtr->no_traces = nstation;
  aeventPtr->tr_array = (Trace *)calloc(nstation, sizeof(Trace));
  if (aeventPtr->tr_array == NULL) {
    std::cerr << "[Hypo71] setupTheAEvent: out of memory assigning the AEvent struct trace array" << std::endl;
    return false;
  }
  
  char *items[16];
  char *tok = NULL;
  int count = 0;
  
  // read in station locations
  Trace *tracePtr = aeventPtr->tr_array;
  while (!feof(in)) {
    memset(buf, 0, 256 * sizeof(char));
    if (fgets(buf, 256, in) != NULL) {
      count = 0;
      tok = strtok(buf, " \t\n");
      while (tok) {
        items[count++] = tok;
        tok = strtok(NULL, " \t\n");
      }
      if (count != 16) {
        std::cerr << "[Hypo71] setupTheAEvent: incorrect number of tokens found when reading PDAS info line" << std::endl;
        return false;
      }
      tracePtr->use_location = 0;
      tracePtr->use_spick = 0;
      memset(tracePtr->stat_code, 0, 6 * sizeof(char));
      strcpy(tracePtr->stat_code, items[7]);
      makeStationCode(tracePtr->stat_code);
      tracePtr->lat = (float)atof(items[8]);
      tracePtr->lon = (float)atof(items[9]);
      tracePtr->elav = (float)atof(items[10]);
      tracePtr++;
    }
  }

  fclose(in);
  
  return true;
}

/*----------------------------------------*/

static bool cleanTheAEvent(AEvent *aeventPtr)
{
  if (aeventPtr->tr_array == NULL) {
    std::cerr << "[Hypo71] cleanTheAEvent: WARNING: AEvent struct already clean" << std::endl;
    return true;
  }

  free(aeventPtr->tr_array);
  aeventPtr->tr_array = NULL;
  aeventPtr->no_traces = 0;

  return true;
}

/*----------------------------------------*/
/*----------------------------------------*/

// static function that initialises the AEvent structure
bool Hypo71::initialise(const char *logFilename)
{
  // need files and paths
  
  if (!setupTheAEvent(&aevent)) {
    return false;
  }
  
  // temp
  if ((logFilePtr = fopen(logFilename, "w")) == NULL) {
    std::cerr << "Hypo71::initialise: cannot open log file for writing" << std::endl;
    return false;
  }
  else {
    time_t t = time(NULL);
    fprintf(logFilePtr, "*** Time stamp: %s\n", ctime(&t));
  }
  
  return true;
}

// static function that cleans up the AEvent structure
bool Hypo71::clean()
{
  if (logFilePtr != NULL) {
    fclose(logFilePtr);
    logFilePtr = NULL;
  }
  
  return cleanTheAEvent(&aevent);
}

/*----------------------------------------*/

bool Hypo71::locate(std::list<Hypo71::HypoPick>& hypoPickList, Hypo71::Sol& sol)
{
  if (aevent.tr_array == NULL || aevent.no_traces < 1) {
    std::cerr << "Hypo71::locate: internal station list is NULL or no stations registered" << std::endl;
    return false;
  }

  if (!findLocation(hypoPickList)) {
    std::cerr << "Hypo71::doLocation: problem when trying to compute event location\n" << std::endl;
    return false;
  }
  
  sol.orig_time = aevent.sol.orig_time;
  sol.event_lon = aevent.sol.event_lon;
  sol.event_lat = aevent.sol.event_lat;
  sol.event_depth = aevent.sol.event_depth;
  sol.rms_residual = aevent.sol.rms_residual;
  sol.gap = aevent.sol.gap;
  sol.lat_error = aevent.sol.lat_error;
  sol.lon_error = aevent.sol.lon_error;
  sol.depth_error = aevent.sol.depth_error;
  sol.num_of_p = aevent.sol.num_of_p;
  sol.num_of_s = aevent.sol.num_of_s;
  
  if (!lookupAEventResiduals(hypoPickList, &aevent)) {
    return false;
  }
  
  return true;
}
