/*
 *  aeh.cpp
 *  NLL wrapper/lib header for the Binder
 *
 *  Created by Andy Heath on 08/07/2008.
 *  Copyright 2008. All rights reserved.
 *
 */

#include <iostream>
#include <cstdio>
#include <cstring>
#include "utl_time.h"
//#include "aehnll.h"
#include <aehnll.h>

extern "C" {
#define EXTERN_MODE 1
#include "NLLocLib.h"
}

static char controlFilename[FILENAME_MAX];

static bool writePickFile(const char *pickFilename, std::list<AehNLL::NLLPick> nllPickList)
{
  FILE *out = fopen(pickFilename, "w");
  if (!out) {
    std::cerr << "[AehNNL]writePickFile: cannot open the pick file" << std::endl;
    return false;
  }
  
  TIME utl;
  char idate[9], ihrmin[5];
  float pickUncertainty;
  std::list<AehNLL::NLLPick>::iterator nllpick;
  
  for (nllpick = nllPickList.begin(); nllpick != nllPickList.end(); nllpick++) {
    utl = do2date(nllpick->pickTime);
    sprintf(idate, "%4.4d%2.2d%2.2d", utl.yr, utl.mo, utl.day);
    sprintf(ihrmin, "%2.2d%2.2d", utl.hr, utl.mn);
    pickUncertainty = 0.05; // map from the "pick imported weight" to time in secs
    fprintf(out,
            "%-6s %-4s %-4s %-1s %-6s %-1s %s %s %9.4lf %-3s %9.2le %9.2le %9.2le %9.2le %9.4lf\n",
            nllpick->stationCodeStr.c_str(),
            ARRIVAL_NULL_STR,
            nllpick->methodIDStr.c_str(),
            ARRIVAL_NULL_STR,
            nllpick->phaseHintStr.c_str(),
            nllpick->polarityStr.c_str(),
            idate,
            ihrmin,
            utl.sec,
            "GAU",
            pickUncertainty,
            0.0,
            0.0,
            0.0,
            1.0);
  }

  fclose(out);
  
  return true;
}

static bool extractLocation(Location *ploc, AehNLL::Sol& sol)
{
  HypoDesc* phypo = ploc->phypo;
  if (phypo == NULL) {
    std::cerr << "[AehNNL]extractLocation: null 'ploc->phypo' passed" << std::endl;
    return false;
  }

  sol.dlat = phypo->dlat;
  sol.dlong = phypo->dlong;
  sol.depth = phypo->depth;
  sol.year = phypo->year;
  sol.month = phypo->month;
  sol.day = phypo->day;
  sol.hour = phypo->hour;
  sol.min = phypo->min;
  sol.sec = phypo->sec;
  sol.nreadings = phypo->nreadings;
  sol.gap = phypo->gap;
  sol.dist = phypo->dist;
  sol.rms = phypo->rms;
  sol.amp_mag = phypo->amp_mag;
  sol.dur_mag = phypo->dur_mag;
  sol.num_amp_mag = phypo->num_amp_mag;
  sol.probmax = phypo->probmax;
  sol.misfit = phypo->misfit;
  sol.grid_misfit_max = phypo->grid_misfit_max;
  sol.expect_dlat = phypo->expect_dlat;
  sol.expect_dlong = phypo->expect_dlong;
  sol.expect_depth = phypo->expect.z;
  sol.ellipsoid[AehNLL::Sol::AZ1] = phypo->ellipsoid.az1;
  sol.ellipsoid[AehNLL::Sol::DIP1] = phypo->ellipsoid.dip1;
  sol.ellipsoid[AehNLL::Sol::LEN1] = phypo->ellipsoid.len1;
  sol.ellipsoid[AehNLL::Sol::AZ2] = phypo->ellipsoid.az2;
  sol.ellipsoid[AehNLL::Sol::DIP2] = phypo->ellipsoid.dip2;
  sol.ellipsoid[AehNLL::Sol::LEN2] = phypo->ellipsoid.len2;
  sol.ellipsoid[AehNLL::Sol::LEN3] = phypo->ellipsoid.len3;
  sol.VpVs = phypo->VpVs;
  sol.nVpVs = phypo->nVpVs;
  sol.tsp_min_max_diff = phypo->tsp_min_max_diff;

  return true;
}

static bool lookupArrivalInfo(ArrivalDesc* parrivals, int narrivals,
                              const char *label, const char *phase,
                              float& residual, float& weight)
{
  for (int i = 0; i < narrivals; i++) {
    if (strcmp(parrivals->label, label) == 0 && (strcmp(parrivals->phase, phase) == 0)) {
      residual = float(parrivals->residual);
      weight = float(parrivals->weight);
      return true;
    }
    parrivals++;
  }
  
  return false;
}

static bool updatePickList(Location *ploc, std::list<AehNLL::NLLPick>& nllPickList)
{
  ArrivalDesc* parrivals = ploc->parrivals;
  if (parrivals == NULL) {
    std::cerr << "[AehNNL]updatePickList: null 'ploc->parrivals' passed" << std::endl;
    return false;
  }
  int narrivals = ploc->narrivals;
  
  std::list<AehNLL::NLLPick>::iterator niter;
  float residual, weight;
  char stationID[ARRIVAL_LABEL_LEN], phase[PHASE_LABEL_LEN];
  
  for (niter = nllPickList.begin(); niter != nllPickList.end(); niter++) {
    memset(stationID, 0, ARRIVAL_LABEL_LEN * sizeof(char));
    strcpy(stationID, niter->stationCodeStr.c_str());
    memset(phase, 0, PHASE_LABEL_LEN * sizeof(char));
    strcpy(phase, niter->phaseHintStr.c_str());
    if (lookupArrivalInfo(parrivals, narrivals, stationID, phase, residual, weight)) {
      niter->residual = residual;
      niter->weight = weight;
    }
    else {
      std::cerr << "[AehNLL]updatePickList: cannot find arrival record for '"
                << stationID << "' "
                << " and phase '"
                << phase << "'" << std::endl;
      return false;
    }
  }

  return true;
}

bool AehNLL::locate(const char *pickFilename, std::list<AehNLL::NLLPick>& nllPickList, AehNLL::Sol& sol)
{
  if (!writePickFile(pickFilename, nllPickList)) {
    std::cerr << "AehNNL::locate: cannot write the pick file" << std::endl;
    return false;
  }

  int nllRtnCode = EXIT_NORMAL;
  LocNode *ploc_list_head = NULL;
  Location *ploc = NULL;
  
  if ((nllRtnCode = NLLoc(0,
                          controlFilename,
                          NULL, 0,
                          1, 0, 0,
                          &ploc_list_head)) != EXIT_NORMAL) {
    std::cerr << "AehNNL::locate: NLLoc abnormal exit with code " << nllRtnCode << std::endl;
    return false;
  }

  if (ploc_list_head == NULL) {
    std::cerr << "[AehNNL]extractLocation: null 'ploc_list_head' passed" << std::endl;
    return false;
  }
  if ((ploc = ploc_list_head->plocation) == NULL) {
    std::cerr << "[AehNNL]extractLocation: null 'ploc_list_head->plocation' passed" << std::endl;
    return false;
  }

  if (!extractLocation(ploc, sol)) {
    std::cerr << "AehNNL::locate: problem with location extraction" << std::endl;
    return false;
  }
  
  if (!updatePickList(ploc, nllPickList)) {
    std::cerr << "AehNNL::locate: problem when updating pick list" << std::endl;
    return false;
  }

  freeLocList(ploc_list_head, ~0);

  return true;
}

bool AehNLL::initialise(const char *_controlFilename)
{
  memset(controlFilename, 0, FILENAME_MAX * sizeof(char));
  strcpy(controlFilename, _controlFilename);
  
  return true;
}

bool AehNLL::clean()
{
  return true;
}
