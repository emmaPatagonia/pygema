#include <iostream>
#include <fstream>
#include <list>
#include <cstring>

#include <hypo71.h>
#include <aehnll.h>

#include "binder/input.h"
#include "binder/uTravelTime.h"
#include "binder/binder.h"
#include "binder/velmod.h"
#include "binder/constants.h"
#include "binder/latencycheck.h"
#include "binder/mystation.h"
#include "binder/livloc.h"

static Binder binder;
static VelMod velmod;
Constants constants;
LatencyCheck latencyCheck;

//--------------------------------------------------

// called whenever we need to compute a travel-time between 2 3D locations
static float tt1d(const Position& src, const Position& rec, float& _ain)
{
  tDouble t, ain;
  
  //tDouble delta = sqrt((src.x - rec.x)*(src.x - rec.x) + (src.y - rec.y)*(src.y - rec.y));
  tDouble delta = src.pythag2d(rec);
  tDouble depth = 0.0 - src.z; // depth measured from sea-level (ignore station elevation)
  
  TravelTime1D(delta, depth, velmod.nl, velmod.v, velmod.top, t, ain);

	_ain = ain;
	
  return float(t);
}

//--------------------------------------------------

// opens and loops picks in a file
static bool readPicks(const char *pickFilename)
{
  MyPick myPick;
  Polarity polarity;
  
  FILE *in = NULL;
  if ( ( in = fopen(pickFilename, "r" )) == NULL ) {
	std::cerr << "cannot open pick file '" << pickFilename << "'" << std::endl;
	return false;
  }
  
  char buf[2048];
  while ( !feof( in ) ) {
	memset( buf, 0, 2048*sizeof( char ) );
    if ( fgets( buf, 2048, in ) != NULL ) {
      int ntok = 0;
      char *tok = strtok(buf, " \t\n");
      while (tok != NULL) {
        switch (ntok) {
          case 0: myPick.pickTime = atof(tok); break;
          case 1: myPick.stationCode = std::string(tok); break;
          case 2: myPick.methodID = std::string(tok); break;
          case 3: myPick.phaseHint = std::string(tok); break;
          case 4: myPick.weight = atoi(tok); break;
          case 5:
            if (std::string(tok) == std::string("U")) {
              polarity = POLARITY_POSITIVE;
            }
            else if (std::string(tok) == std::string("D")) {
              polarity = POLARITY_NEGATIVE;
            }
            else {
              polarity = POLARITY_UNDECIDABLE;
            }
            myPick.polarity = polarity;
            break;
          default: std::cerr << "unrecognised format in pick file record..." << std::endl; return false;
        }
        ntok++;
        tok = strtok(NULL, " \t\n");
      }
	}
    if ( latencyCheck.testPick(myPick) ) {
	  binder.addPick( myPick ); // feed to binder
	}
  }

  fclose( in );

  return true;
}

//--------------------------------------------------

int main(int argc, char *argv[])
{
  if (argc < 3) {
	std::cerr << "usage: " << argv[0] << ": <pickfile> <paramfile>" << std::endl;
	return 1;
  }
  
  // get params
  Binder::Param param;

  if (!constants.import(argv[0], argv[2])) {
    std::cerr << ": problem with parameter file - exiting" << std::endl;
    return false;
  }
  
  // parameters for use inside the Binder
  param.ttFunc = tt1d;
  param.stackSpacing = constants.stackSpacing;
  param.stackDepth = constants.stackDepth;
  param.stackWindow = constants.stackWindow;
  param.associationThreshold = constants.associationThreshold;
  param.eventThreshold = constants.eventThreshold;
  param.pEventWindow = constants.pEventWindow;
  param.sEventWindow = constants.sEventWindow;
  param.pToSVelocityRatio = constants.pToSVelocityRatio;
  param.eventCutoff = constants.eventCutoff;
  param.pPickResidualCutoff = constants.pPickResidualCutoff;
  param.sPickResidualCutoff = constants.sPickResidualCutoff;
  param.eventFilename = constants.eventFilename;
  param.unusedPickFilename = constants.unusedPickFilename;
  param.networkConfigFilename = constants.networkConfigFilename;
  param.velmodFilename = constants.velmodFilename;
  param.hypo71Filename = constants.hypo71Filename;
  param.latencyFilename = constants.latencyFilename;
  param.latencyValue = constants.latencyValue;

  if ( !velmod.create(param.velmodFilename) ) {
    std::cerr << ": could not import velocity model - exiting" << std::endl;
    return 1;
  }
  
  if ( !latencyCheck.create(param.latencyFilename, param.latencyValue) ) {
    std::cerr << ": could not open latency error file - exiting" << std::endl;
    return 1;
  }

	// testing the LivLoc relocation - create obj here.. rest of initialisation done in 'binder::create'
	if ( ( livLocObj = new LivLoc ) == NULL ) {
    std::cerr << ": cannot create the LivLoc object - exiting" << std::endl;
    return 1;
  }
	else {
		livLocObj->velmod = velmod;
	}
	
  // Replace this with init for new relocation
  if ( !Hypo71::initialise("hypo71.log") ) {
    std::cerr << ": cannot setup the relocation algorithm (hypo71) - exiting" << std::endl;
    return 1;
  }

  // setup of non-lin loc
  if ( !AehNLL::initialise("nllcontrol.txt") ) {
    std::cerr << ": cannot setup the relocation algorithm (NLL) - exiting" << std::endl;
    return 1;
  }

  // read in the stations from param.networkConfigFilename
  std::list<MyStation> myStationList;
  if ( !inputStations( param.networkConfigFilename, myStationList) ) {
    std::cerr << ": cannot input stations - exiting" << std::endl;
    return 1;
  }
  
  // init binder
  if ( !binder.create(param, myStationList) ) {
    std::cerr << ": could not initialise the binder library - exiting" << std::endl;
    return 1;
  }

  // loop over pick file input and feed the binder
  if ( !readPicks(argv[1]) ) {
    std::cerr << ": could not process picks - exiting" << std::endl;
    return 1;
  }

  // write out any remaining picks still in the stack
  binder.tidy();
  
  // close down hypo71
  Hypo71::clean();
  
  // close down NLL
  AehNLL::clean();

  // output any events still remaining in the binder
  std::ofstream out;
  out.open(binder.getEventFilename().c_str(), std::ios::app);
  if (!out) {
    std::cerr << ": cannot open output file to append remaining events" << std::endl;
  }
  else {
    out << binder;
    out.close();
  }
  
  return 0;
}
