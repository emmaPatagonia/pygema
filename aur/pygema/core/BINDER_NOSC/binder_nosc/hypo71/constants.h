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

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include <string>

class Constants
{
public:
  Constants();
  virtual ~Constants() {}
  
  bool import(const char *progname, const char *filename);
  
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
  std::string unusedPickFilename;
  std::string networkConfigFilename;
  std::string velmodFilename;
  std::string hypo71Filename;
  std::string latencyFilename;
  float latencyValue;
  float maxPRes;
  float maxSRes;
  float maxResOff;
  float maxSpickDiff;
  float maxCorrLength;
  float depthStart;
  float depthStep;
  int maxDepthTry;
  float minAreaLong;
  float maxAreaLong;
  float minAreaLat;
  float maxAreaLat;

private:
  bool parseRecord(std::string line, std::string& arg, std::string& val);
  bool assignConstant(std::string arg, std::string val);
};

extern Constants constants;

#endif //!_CONSTANTS_H
