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

#include <stdio.h>          /* C library  */
#include <string.h>
#include <malloc.h>
#include <math.h>

#define IGREG (15+31L*(10+12L*1582))

long julday(mm,id,iyyy)
int mm,id,iyyy;
{
        long jul;
        int ja,jy,jm;
        void nrerror();

        if (iyyy == 0)

        printf("ERROR in JULDAY: there is no year zero.");
        if (iyyy < 0) ++iyyy;
        if (mm > 2) {
                jy=iyyy;
                jm=mm+1;
        } else {
                jy=iyyy-1;
                jm=mm+13;
        }
        jul = (long) (floor(365.25*jy)+floor(30.6001*jm)+id+1720995);
        if (id+31L*(mm+12L*iyyy) >= IGREG) {
                ja=0.01*jy;
                jul += 2-ja+(int) (0.25*ja);
        }
        return jul;
}

#undef IGREG
