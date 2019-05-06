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

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <memory.h>

#define DGDGKM  0
#define KMKMKM  1
#define MMM     2

#define EPS 1.0e-7

/**
   NAME: utl_hypdis
   SYNOPSIS:
        int res;  1 on successful calculation
        double elat,elon,sdep[km]; latitude N, longitude E, depth (down -)
        double stalat,stalon,elev[km]; latitude N, longitude E, elevation (up +)
        double hdis,edis,az,baz;  hypodist [km], epidist [km],
                                  azimuth [deg], backaz [deg.]
                                  baz = event as seen from station against N
                                  az = station as seen from event against N
        res = utl_hypdis(elat,elon,sdep,stalat,stalon,elev,&hdis,&edis,&az,&baz,ctype)
   DESCRIPTION:
       Determines distance, azimuth and back-azimuth,
       for a single earthquake-station pair.

       Reference: 
       Dieter Ehlert, Die Bessel-Helmertsche Loesung der beiden 
       geodaetischen Hauptaufgaben, Zeitschrift fuer Vermessungswesen,
       (108) 495 - 500, 1983.
**/

int utl_3d_dist(elat,elon,sdep,stalat,stalon,elev,hdis,edis,az,baz,ctype)
double elat,elon,sdep,stalat,stalon,elev;
double *hdis,*edis,*az,*baz;
int ctype;
{
    double c = 6399593.6259;
    double e2 = 6.73949677548e-3;
    double a = 6378137.0;
    double b = 6356752.3141;
    double n;
    double beta0,beta1,beta2,dlambda,dlambda1,dl;
    double z1,z2,n1,n2;
    double sigma,sigma1,sigmaq2,delta1,delta2;
    double k1;
    double rad;
    double p;
    double diff;
    int i = 0;
    float dx,dy,dz;
    
    rad = M_PI/180.0;     
    n = (a - b)/(a + b);
    
    /* angels in radian */
    switch (ctype) {
	case DGDGKM:
    	    elat *= rad;
	    elon *= rad;
	    stalat *= rad;
	    stalon *= rad;
	    
	    beta1 = elat - n*sin(2*elat) + n*n/2 * sin(4*elat) - n*n*n/3 * sin(6*elat);
	    beta2 = stalat - n*sin(2*stalat) + n*n/2 * sin(4*stalat) - n*n*n/3 * sin(6*stalat);
	    
	    dl = stalon - elon;
	    dlambda = dl;
	    
	    do {
		i++;
		z1 = cos(dlambda*0.5) * cos((beta2-beta1)*0.5);
		n1 = sin(dlambda*0.5) * sin((beta2+beta1)*0.5);
		z2 = cos(dlambda*0.5) * sin((beta2-beta1)*0.5);
		n2 = sin(dlambda*0.5) * cos((beta2+beta1)*0.5);
		
		sigma = 2 * atan(sqrt((z2*z2 + n2*n2)/(z1*z1 + n1*n1)));
		
		delta1 = atan2(z1,n1);
		delta2 = -atan2(z2,n2); 
		
		*az = delta1 + delta2;
		
		beta0 = sin(*az) * cos(beta1);
		
		sigma1 = atan2((cos(*az) * cos(beta1)),sin(beta1));
		sigmaq2 = sigma - 2*sigma1;
		
		k1 = (sqrt(1+e2-e2*beta0*beta0) - 1)/
		     (sqrt(1+e2-e2*beta0*beta0) + 1);
		
		dlambda1 = dl + e2/(1+e2) * beta0 * (0.5*sigma*
			   (1 + n - 0.5*k1 - 0.25*k1*k1) - 
			   0.25*k1 * cos(sigmaq2)*sin(sigma)+
			   k1*k1/16.0 * cos(2*sigmaq2) * sin(2*sigma));
		diff = fabs(dlambda - dlambda1);
		dlambda = dlambda1;
	    } while (diff > EPS);
	    
	    *edis = sigma + (k1-0.375*k1*k1*k1) * cos(sigmaq2) * sin(sigma) -
		0.125*k1*k1 * cos(2*sigmaq2) * sin(2*sigma) +
		k1*k1*k1/24.0 * cos(3*sigmaq2) * sin(3*sigma);
		
	    p = (1 + 0.25*k1*k1)/(1 - k1);
	    
	    *edis *= c * p/(1+e2);
	    
	    *baz = delta2 - delta1;
	
	    *az /= rad;
	    *baz /= rad;
	    
	    if (*az < 0) *az += 360;
	    if (*baz < 0) *baz += 360;
	
	    /*
	     let's calculate the hypocentral distance:
	     it is computed simply by Pythagoras:
	    */
	
	    *edis /= 1000.0; /* meter -> kilometer */
	
	    /* add the station height to the depth */
	    sdep = (elev-sdep); 
	
	    *hdis = sqrt((*edis * *edis) + (sdep * sdep));
	    break;
	default:
	    /* in the following the hypocentral distance is simply calculated by Pythagoras */
            dx = stalon - elon;
	    dy = stalat - elat;
  	    dz = elev-sdep;
	    *hdis = sqrt(dx*dx + dy*dy + dz*dz);
	    *edis = sqrt(dx*dx + dy*dy);
	     if((dy != 0) && (dx != 0))
	     {
	        *az = atan2(dx,dy) * 180.0/M_PI;
	     }
	     else
	     {
		if(dy != 0.)
		{
		  if(dy > .0)
		     *az = 0.0;
		  else
		     *az = 180;
		}else
		{
		  if(dx > .0)
		     *az = 90;
		  else
		     *az = 270;
	        }
	     }
	    if (*az < 0.0)
		*az += 360.0;
	    *baz = *az + 180.0;
	    if (*baz > 360.0)
		*baz -= 360.0;
	    if (ctype == MMM) {
	        *hdis *= 0.001;
		*edis *= 0.001;
	    }
	    break;
    } /* end of switch */
    return (1);
}                 
