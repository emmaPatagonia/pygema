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

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include "velmod.h"

class VelModItem
{
public:
  VelModItem() : v(-1.0), top(-1.0) {}
  VelModItem(tDouble _v, tDouble _top) : v(_v) , top(_top) {}
  virtual ~VelModItem() {}
      
  tDouble v;
  tDouble top;
};

static bool loadVelMod(FILE *in, std::list<VelModItem>& items)
{
  char buf[255];
  VelModItem item;
  
  while (!feof(in)) {
    memset(buf, 0, 255 * sizeof(char));
    if (fgets(buf, 255, in) != NULL) {
      item.v = atof(strtok(buf, "\t \n"));
      item.top = atof(strtok(NULL, "\t \n"));
      items.push_back(item);
    }
  }
  
  return true;
}

bool VelMod::create(std::string velmodFilename)
{
  FILE *in = NULL;
  if ((in = fopen(velmodFilename.c_str(), "r")) == NULL) {
    std::cerr << "[Binder] VelMod::create: cannot open '" << velmodFilename << "'" << std::endl;
    return false;
  }

  std::list<VelModItem> items;
  if (!loadVelMod(in, items)) {
    fclose(in);
    return false;
  }
  
  if (items.empty()) {
    std::cerr << "[Binder] VelMod::create: velocity model has too few entries" << std::endl;
    fclose(in);
    return false;
  }
  
  fclose(in);

  nl = items.size() + 1;
  v = new tDouble[nl];
  top = new tDouble[nl];

  if (!(top && v)) {
    std::cerr << "[Binder] VelMod::create: memory allocation error" << std::endl;
    return false;
  }

  // duplicate first entry (just how the algorithm works to calculate TT)
  int n = 1;
  std::list<VelModItem>::iterator iter;
  for (iter = items.begin(); iter != items.end(); iter++) {
    v[n] = iter->v;
    top[n] = iter->top;
    n++;
  }
  v[0] = v[1];
  top[0] = top[1];

  return true;
}
