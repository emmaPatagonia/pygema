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

#ifndef _MYSTATION_H
#define _MYSTATION_H

#include <string>

class MyStation {
public:
  MyStation() : myLatitude(-9999), myLongitude(-9999), myElevation(-9999) {}
	virtual ~MyStation() {}

	void setCode(const std::string& _myCode) { myCode = _myCode; }
	std::string code() const { return myCode; }
  void setLatitude(double _myLatitude) { myLatitude = _myLatitude; }
	double latitude() const { return myLatitude; }
  void setLongitude(double _myLongitude) { myLongitude = _myLongitude; }
	double longitude() const { return myLongitude; }
  void setElevation(double _myElevation) { myElevation = _myElevation; }
	double elevation() const { return myElevation; }

private:
  std::string myCode;
	double myLatitude;
	double myLongitude;
	double myElevation;
};

#endif //!_MYSTATION_H

