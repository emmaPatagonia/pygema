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

#ifndef _AEHNLL_H
#define _AEHNLL_H

class AehNLL {
public:
  class Sol {
  public:
    enum EllipsoidID {
      AZ1, DIP1, LEN1, AZ2, DIP2, LEN2, LEN3, NELLIPSOID_ID
    };
    double dlat, dlong, depth; 	/* loc (geographic) */
    int year, month, day; 		/* date */
    int hour, min; 			/* hour/min */
    double sec; 			/* origin time (s) */
    int nreadings; 			/* number arrivals used in solution */
    int gap; 			/* largest azimuth separation between stations as seen from epicenter (deg) */
    double dist; 			/* epi distance to closest sta (km or deg (MODE_GLOBAL)) */
    double rms; 			/* rms error of arrival times */
    double amp_mag;			/* amplitude magnitude */
    int num_amp_mag;		/* number of readings */
    double dur_mag;			/* duration magnitude */
    int num_dur_mag;		/* number of readings */
    double probmax; 		/* probability density at hypo (maximum in grid) */
    double misfit; 			/* misfitat hypo (minimum in grid) */
    double grid_misfit_max;		/* max misfit in loc grid */
    double expect_dlat, expect_dlong;	/* lat / long trans of expectation */
    double expect_depth; /* derived from Vect3D 'expect' */
    double ellipsoid[NELLIPSOID_ID]; /* parameters describing the ellipsoid */
    double VpVs;			/* Vp / Vs ratio */
    int nVpVs;			/* num P-S pairs used to calc VpVs */
    double tsp_min_max_diff;	/* max P-S - min P-S (sec) */
  };
  /* above is subset of what NLL offsers: see "GridLib.h" (HypoDesc) for details */
	class NLLPick {
  public:
    std::string stationCodeStr;
    double pickTime;
    std::string phaseHintStr;
    std::string polarityStr;
		std::string methodIDStr;
    float residual;
    float weight;

    NLLPick()
    : pickTime(-9999), residual(-9999), weight(-9999) {}
    virtual ~NLLPick() {}
  };
  
  AehNLL() {}
  virtual ~AehNLL() {}
  
  bool locate(const char *pickFilename, std::list<NLLPick>& nllPickList, AehNLL::Sol& sol);
  static bool initialise(const char *controlFilename);
  static bool clean();
};
    
#endif // !_AEHNLL_H
