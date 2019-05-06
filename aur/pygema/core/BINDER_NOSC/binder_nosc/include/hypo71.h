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
#include <string>

#ifndef _HYPO71_H
#define _HYPO71_H

class Hypo71 {
public:
  class Sol {
  public:
   int event_no;
   int solution_id;
   double orig_time;
   int best_solution;
   float event_lon;
   float event_lat;
   float event_depth;
   char hypo_prog[20];
   char vel_model[20];
   float rms_residual;
   float gap;
   float lat_error;
   float lon_error;
   float depth_error;
   int num_of_p;
   int num_of_s;
   float fcommon[3];
  
   Sol() {}
   virtual ~Sol() {}
  };

  class HypoPick {
  public:
		std::string stationCodeStr;
		double pickTime;
		std::string phaseHintStr;
		std::string polarityStr;
    int importedWeight;
    float residual;
    float weight;
		bool hasRelatedPPick;
    
    HypoPick()
      : importedWeight(-9999), residual(-9999), weight(-9999), hasRelatedPPick(false) {}
		/*
    HypoPick(PickPtr _pickPtr)
      : pickPtr(_pickPtr), importedWeight(0), residual(0.0f), weight(0.0f), pPickPtr(NULL) {}
    HypoPick(PickPtr _pickPtr, int _importedWeight)
      : pickPtr(_pickPtr), importedWeight(_importedWeight), residual(0.0f), weight(0.0f), pPickPtr(NULL) {}
    HypoPick(PickPtr _pickPtr, int _importedWeight, PickPtr _pPickPtr)
      : pickPtr(_pickPtr), importedWeight(_importedWeight), residual(0.0f), weight(0.0f), pPickPtr(_pPickPtr) {}
		*/
    virtual ~HypoPick() {}
  };
    
  Hypo71() {}
  virtual ~Hypo71() {}

  bool locate(std::list<HypoPick>& hypoPickList, Sol& sol);

  static bool initialise(const char *logFilename);
  static bool clean();
};

#endif /* !_HYPO71_H */
