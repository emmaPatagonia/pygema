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
#include <iomanip>

#include "debug.h"

void debugPicks(std::list<MyPick>& theList)
{
  std::list<MyPick>::iterator miter;
  MyPick mp;
  
  if (theList.empty()) {
    std::cerr << "list is empty" << std::endl;
  }
  else {
    std::cerr << theList.size() << " entries" << std::endl;
    for (miter = theList.begin(); miter != theList.end(); miter++) {
      mp = *miter;
      std::cerr << miter->stationCode << " "
                << std::setprecision(3) << std::fixed << miter->pickTime << " ("
                << miter->originTime << ")" << std::endl;
  
    }
  }
}
