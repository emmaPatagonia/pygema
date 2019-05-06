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

#include <map>
#include <list>
#include <string>

#include "position.h"
#include "velmod.h"
#include "cartesian.h"

#ifndef _RELOCATE_H
#define _RELOCATE_H

class LivLoc {
	public:
		class LLPick {
			public:
				enum Polarity {
					POSITIVE,
					NEGATIVE,
					UNDECIDABLE
				};

				LLPick()
					: pickTime(-9999), polarity(UNDECIDABLE), weight(-9999),
				isArrival(false), timeResidual(-9999), relocatedWeight(-9999), distance(-9999), azimuth(-9999) {}

				LLPick(std::string _stationCode, double _pickTime, std::string _phaseHint, Polarity _polarity, int _weight)
					: stationCode(_stationCode), pickTime(_pickTime), phaseHint(_phaseHint), polarity(_polarity), weight(_weight),
				isArrival(false), timeResidual(-9999), relocatedWeight(-9999), distance(-9999), azimuth(-9999) {}

				std::string stationCode;
				double pickTime;
				std::string phaseHint; // 'P' or 'S'
				Polarity polarity; // enum set to one of POSITIVE, NEGATIVE OR UNDECIDABLE
				int weight; // read from the pick file
				bool isArrival;
				float timeResidual;
				float relocatedWeight;
				float distance; // in degrees (?)!
				float azimuth;
				int index; // hack
		};

		class LLOrigin {
			public:
				LLOrigin()
					: originTime(-9999), rms(-9999), gap(-9999), largestCond(-9999), lastCond(-9999) {}
				LLOrigin(double _originTime, const Position& _location, const Position& _error, float _rms, float _gap, const std::list<LLPick>& _pickList)
					: originTime(_originTime), location(_location), error(_error), rms(_rms), gap(_gap), pickList(_pickList),
				    largestCond(-9999), lastCond(-9999) {}

				float setGap();

				double originTime;
				Position location;
				Position error;
				float rms;
				float gap;
				std::list<LLPick> pickList;
				// debug
				double largestCond;
				double lastCond;
				// end debug
		};

		class LLRelocationParams {
			public:
				LLRelocationParams() 
					: startDepth(-10.0), iterMax(10), iterRunMax(10), stdDevCutP(10.0), stdDevCutS(10.0),
				    minWeight(0.1), sWeight(0.5), locAccuracy(0.05), residualCut(2.0), resCutRatio(2) {}
					//: startDepth(-10.0), iterMax(10), iterRunMax(10), stdDevCutP(10.0), stdDevCutS(10.0),
				    //minWeight(0.1), sWeight(1.0), locAccuracy(0.005), residualCut(2.0), resCutRatio(2) {}
				virtual ~LLRelocationParams() {}

				double startDepth;
				int iterMax;
				int iterRunMax;
				double stdDevCutP;
				double stdDevCutS;
				double minWeight;
				double sWeight;
				double locAccuracy;
				double residualCut;
				int resCutRatio;
		};
		
	public:
		LivLoc() : pToSVelocityRatio(-9999.0f), tt1d(NULL) {}
		virtual ~LivLoc() {}

	public:
		bool relocate(std::list<LLPick> pickList, LLOrigin& origin, bool useOriginAsInitialLocation =false);

	public:
		LLRelocationParams rp;
		std::map<std::string, Position> stationMap;
		float pToSVelocityRatio;
		float (*tt1d)(const Position& src, const Position& rec, float& ain);
		VelMod velmod;
		Cartesian cart;
};

extern LivLoc *livLocObj;

#endif //!_RELOCATE_H
