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

#include <math.h>

#define IGREG 2299161

void caldat(julian,mm,id,iyyy)
long julian;
int *mm,*id,*iyyy;
{
	long ja,jalpha,jb,jc,jd,je;

	if (julian >= IGREG) {
		jalpha=((float) (julian-1867216)-0.25)/36524.25;
		ja=julian+1+jalpha-(long) (0.25*jalpha);
	} else
		ja=julian;
	jb=ja+1524;
	jc=6680.0+((float) (jb-2439870)-122.1)/365.25;
	jd=365*jc+(0.25*jc);
	je=(jb-jd)/30.6001;
	*id=jb-jd-(int) (30.6001*je);
	*mm=je-1;
	if (*mm > 12) *mm -= 12;
	*iyyy=jc-4715;
	if (*mm > 2) --(*iyyy);
	if (*iyyy <= 0) --(*iyyy);
}

#undef IGREG
