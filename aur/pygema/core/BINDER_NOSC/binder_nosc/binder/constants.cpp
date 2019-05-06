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
#include <cctype>
#include <cstdlib>
#include <values.h>

#include "constants.h"

#define DELIM ":"

Constants::Constants()
: stackSpacing(40.0f),
  stackDepth(300.0f),
  stackWindow(90.0f),
  associationThreshold(2.0f),
  eventThreshold(8),
  pEventWindow(60.0f),
  sEventWindow(90.0f),
  pToSVelocityRatio(1.78f),
  eventCutoff(120.0f),
  pPickResidualCutoff(2.0f),
  sPickResidualCutoff(3.0f),
  eventFilename("events.txt"),
  unusedPickFilename("unusedpicks.txt"),
  networkConfigFilename("info.dat"),
  velmodFilename("velmod.hdr"),
  hypo71Filename("hypo71pc"),
  latencyFilename("latency_error.txt"),
  latencyValue(0.1f),
  maxPRes(2.0f),
  maxSRes(3.0f),
  maxResOff(40.0f),
  maxSpickDiff(100.0f),
  maxCorrLength(40.0f),
  depthStart(10.0f),
  depthStep(10.0f),
  maxDepthTry(12),
  minAreaLong(FLT_MAX),
  maxAreaLong(-FLT_MAX),
  minAreaLat(FLT_MAX),
  maxAreaLat(-FLT_MAX)
{
}

static std::string stripWhite(std::string sin)
{
  std::string sout;
  std::string::iterator siter;
  
  for (siter = sin.begin(); siter != sin.end(); siter++) {
    if (!isspace(*siter)) {
      sout.push_back(*siter);
    }
  }
  
  return sout;
}

static std::string makeUpper(std::string sin)
{
  std::string sout;
  std::string::iterator siter;
  
  for (siter = sin.begin(); siter != sin.end(); siter++) {
    sout.push_back(toupper(*siter));
  }
  
  return sout;
}

bool Constants::parseRecord(std::string line, std::string& arg, std::string& val)
{
  std::string::size_type pos;
  
  line = stripWhite(line);
  if ((pos = line.find(DELIM, 0)) == std::string::npos) {
    return false;
  }
  
  arg = line.substr(0, pos);
  val = line.substr(pos+1);
  
  return true;
} 

bool Constants::assignConstant(std::string arg, std::string val)
{
  arg = makeUpper(arg);

  if (arg == makeUpper("stackSpacing")) {
    stackSpacing = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("stackDepth")) {
    stackDepth = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("stackWindow")) {
    stackWindow = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("associationThreshold")) {
    associationThreshold = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("eventThreshold")) {
    eventThreshold = (unsigned int)atoi(val.c_str());
  }
  else if (arg == makeUpper("pEventWindow")) {
    pEventWindow = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("sEventWindow")) {
    sEventWindow = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("pToSVelocityRatio")) {
    pToSVelocityRatio = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("eventCutoff")) {
    eventCutoff = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("pPickResidualCutoff")) {
    pPickResidualCutoff = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("sPickResidualCutoff")) {
    sPickResidualCutoff = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("eventFilename")) {
    eventFilename = val;
  }
  else if (arg == makeUpper("unusedPickFilename")) {
    unusedPickFilename = val;
  }
  else if (arg == makeUpper("networkConfigFilename")) {
    networkConfigFilename = val;
  }
  else if (arg == makeUpper("velmodFilename")) {
    velmodFilename = val;
  }
  else if (arg == makeUpper("hypo71Filename")) {
    hypo71Filename = val;
  }
  else if (arg == makeUpper("latencyFilename")) {
    latencyFilename = val;
  }
  else if (arg == makeUpper("latencyValue")) {
    latencyValue = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxPRes")) {
    maxPRes = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxSRes")) {
    maxSRes = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxResOff")) {
    maxResOff = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxSpickDiff")) {
    maxSpickDiff = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxCorrLength")) {
    maxCorrLength = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("depthStart")) {
    depthStart = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("depthStep")) {
    depthStep = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxDepthTry")) {
    maxDepthTry = atoi(val.c_str());
  }
  else if (arg == makeUpper("minAreaLong")) {
    minAreaLong = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxAreaLong")) {
    maxAreaLong = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("minAreaLat")) {
    minAreaLat = (float)atof(val.c_str());
  }
  else if (arg == makeUpper("maxAreaLat")) {
    maxAreaLong = (float)atof(val.c_str());
  }
  else {
    return false;
  }
  
  return true;
}

bool Constants::import(const char *progname, const char *filename)
{
  FILE *in = fopen(filename, "r");
  if (!in) {
    std::cerr << progname << ": cannot open parameter file '" << filename << "'" << std::endl;
    return false;
  }
  
  char line[255];
  std::string arg, val, record;
  
  while (!feof(in)) {
    memset(line, 0, 255 * sizeof(char));
    if (fgets(line, 255, in) != NULL) {
      if (line[0] != '#') {
        record = stripWhite(std::string(line));
        if (!record.empty()) {
          if (parseRecord(record, arg, val)) {
            if (!assignConstant(arg, val)) {
              std::cerr << progname << ": problem assigning parameter '" << arg << "'" << std::endl;
              return false;
            }
          }
          else {
            std::cerr << progname << ": unable to parse record '" << line << "'" << std::endl;
            return false;
          }
        }
      }
    }
  }
  
  fclose(in);
  
  return true;
}
