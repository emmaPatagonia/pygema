//=== File description =========================================================
/*! 
\file uTravelTime.cpp
\brief 1D raytracing ANSI C routines. Module was adopted from hypoDD 
 FORTRAN 77 project written by Felix Waldhauser in 2000
 by using f2c and some language structural changes direct1.f -- translated by f2c (
 version 20020208) and patched into explicit C data types to prevent  -lf2c linking.
 ..Geowissenschaften Uni Potsdam A. Weidauer IV/2002
*/

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

// === applied modules =========================================================
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "uConst.h"
#include "uTravelTime.h"


// === implementation of functions =============================================

// --- function TravelTime1D -----------------------------------------------------
/*!
 Purpose:
  Determine the fastest traveltime between a source 
  at depth [km] and a receiver at distance [km] by a give 
  velocity modell described by a number of layers
  ni, a velocity layer set v[] and the location of the
  layers top[]. 
 
  \param delta tDouble        - epicentral distance in km 
  \param depth tDouble        - focal depth of source in km 
  \param nl    tInteger       - number of layers in velocity model 
  \param v     tDoubleArray   - velocity in each layer 
  \param top   tDoubleArray   - depth to top of velocity layer 
  \param t     tDoubleArray   - <b>return</b> the minimal travelttime from event to reciver in sec
  \param ain   tDouble        - <b>return</b> Angle of incidence 
 
  \sa vmodel(), refract(), direct()
  
  \return value is irrelevant
		 	      
*/
tInteger TravelTime1D(tDouble delta,tDouble depth,tInteger nl,
               tDoubleArray v,tDoubleArray top,
               tDouble& t,tDouble& ain)
{
    /* Local variables */
    tDouble tref, tdir, u, x;
    tInteger jl, kk;
    tDouble xovmax;
    tDouble tkj;
    pDouble thk; 
    pDouble vsq;

/*  allocating memory for the model and 
    call vmodel to set-up model and locate source layer in it 
    output: vsq(l) - v(l) ** 2 , thk(l) - thickness of layer l,
            jl - event layer, tkj - depth of event in event layer  
*/
    vsq = (tDoubleArray) malloc((nl+1)*sizeof(tDouble));
    thk = (tDoubleArray) malloc((nl+1)*sizeof(tDouble));
    vmodel(nl, v, top, depth, vsq, thk, jl, tkj);


/*  call refract to find fastest refracted arrival 
   output:   kk - refracting layer for fastest refracted ray
           tref - travel time of fastest refracted ray 
         xovmax - an upper bound on delta for which the direct ray 
                  can be the first arrival 
  	if delta <= xovmax, them 
 	call direct1 to find the direct ray traveltime 
 	otherwise tref is the minimum traveltime 
 	assume for now refracted path is faster 
*/
    refract(nl, v, vsq, thk, jl, tkj, delta, kk, tref, xovmax);

    t = tref;
/* 	compute the takeoff angle */
    if (kk > 0) {
	u = v[jl] / v[kk];
	ain = asin(u) * 57.2958;
    }

/* handle the direct case 
  output:  tdir - direct ray travel time 
              u - sine of the takeoff angle 
              x - horizontal travel distance in the event layer 
*/	      
    if (delta <= xovmax) {
	direct(nl, v, vsq, thk, jl, tkj, delta, depth, tdir, u, x);

/* 	compare the traveltimes */
	if (tref > tdir) {
/* 	direct time is the minimum traveltime */
	    t = tdir;
	    ain = 180 - asin(u) * 57.2958;
	}
    }
    free(vsq);free(thk);
    return 0;
} 
// --- End of function TravelTime ----------------------------------------------


// --- function vmodel ---------------------------------------------------------
/*!
 Purpose: Extract needed information from the layered velocity model. 
  \param nl    tInteger      -  number of velocity layers  max see cmax_layers ttconst.h
  \param top   tDoubleArray  -  depth to top of each layer km^2 
  \param v     tDoubleArray  -  velocities of each layer in km/s
  \param depth tDouble       -  event depth in km
  \param vsq   tDoubleArray  -  <b>return</b> square of velocities of layers (km/s)^2 
  \param thk   tDoubleArray  -  <b>return</b> thickness of each layer in km
  \param jl    tInteger      -  <b>return</b> layer number containing the event
  \param tkj   tDouble       -  <b>return</b> depth/disctance event <-> event layer 
  
  \return value is irrelevant
*/	     
tInteger vmodel (tInteger nl ,
                 tDoubleArray  v  , tDoubleArray top, tDouble depth, 
                 tDoubleArray& vsq, tDoubleArray& thk , tInteger& jl,
		 tDouble&  tkj)  
{
    tInteger i;  
    tInteger ajl;
    tDouble  athk;
    
/* compute square of layer velocity */
    for (i = 1; i <= nl; ++i) vsq[i] = v[i] * v[i];

/* determine layer thickness and  *
  find layer containing event, 
  Important note:  depth.<top(i) will
	lead to incorrect results for traveltime */
    ajl = nl-1;
    for (i=1; i<=nl; ++i) 
     if (depth <= top[i]) {
	ajl = i - 1;
	break;
      }
    
    for (i=1; i<=nl-1; ++i) thk[i] = top[i+1] - top[i];
/* 	compute depth from top of layer to source  */
    athk = depth - top[ajl];
    tkj = athk;
    jl  = ajl;
return 0;
} 
// --- EndOf function vmodel --------------------------------------------------



// --- function refract ---------------------------------------------------------
/*!
 Purpose:  
  Find "refracted" ray with smallest travel time 
  
 \param  nl tInteger       -  number of velocity layers  max see cmax_layers ttconst.h
 \param  v tDoubleArray    -  velocities of each layer in km/s
 \param  vsq tDoubleArray  -  square of velocities of layers (km/s)^2 
 \param  thk tDoubleArray  -  thickness of each layer in km
 \param  jl tInteger       -  layer number containing the event
 \param  tkj tDouble       -  depth/disctance event <-> event layer 
 \param  delta tDouble      -  epicentral dictance in km
 \param  kk     tInteger   -  <b>return</b> refracting layer for fastest refracted ray 
 \param  tref   tDouble    -  <b>return</b> travel time of fastest refracted ray 
 \param  xovmax tDouble    -  <b>return</b> an upper bound on delta for which the direct ray can 
                              be the first arrival 
 
  ..was reported but not in interface
     didjkk - critical distance for refraction in layer kk
 
 Internal arrays: .. see limits cmax_layer 
    tDouble tr  [cmax_layer] - travel time for refraction in layer m 
    tDouble didj[cmax_layer] - critical distance 
    tDouble tinj[cmax_layer] - traveltime intercept 
    tDouble did [cmax_layer] - terms in critical distance which are
                      independent of tkj 
    tDouble tid [cmax_layer] - terms in travel time intercept which are 
                               independent of tkj 

 Call subroutine:  tiddid to evaluate tid[] and did(m), 
  the terms in the travel time intercept and critical 
  distance for a ray refracted in the m-th layer that 
  are independent of tkj. 
 
 Description:    
   For refracted rays in a layered earth model, refract 
   determines the fastest travel time, tref, the layer 
   in which the fastest ray is refracted, kk, the 
   critical distance for refraction in that layer, 
   didjkk, and an upper bound on delta for which a 
   direct ray can be a first arrival, xovmax.  Refract 
   allows for the possibility of low velocity layers. 
 
   Note that there may not be a refracted ray, either because 
   all layers below the event layer are low velocity layers or 
   because for all layers below the event layer which are not low 
   velocity layers the critical distance exceeds delta.  In such 
   cases tref, didjkk, and xovmax are set very large, kk is set to 
   zero, and refract returns to the calling program. 
 
  \return value is irrelevant
*/
tInteger refract(tInteger nl,
                 tDoubleArray v,  tDoubleArray vsq, tDoubleArray thk,
                 tInteger jl,  tDouble tkj , tDouble delta,
                 tInteger& kk, tDouble& tref, tDouble& xovmax)
{

    /* Local variables */
    tInteger jx,j1, l, m, m1, lx;
    tDouble didj[cmax_layers+1], 
           tinj[cmax_layers+1], 
	   tr  [cmax_layers+1],
           did [cmax_layers+1], 
	   tid [cmax_layers+1];
    tDouble  tim, sqt;

/*  determine tref, kk, didjkk */
    tref=0.0; kk=0; 

    for (m = 1; m <= cmax_layers; ++m) {
     didj[m]=0.0; tinj[m]=0.0; tr  [m]=0.0;
     did [m]=0.0; tid [m]=0.0;    } 

    tiddid(jl, nl,v,vsq,thk,tid,did);
      
    tref = ctracer_largenum;
    j1 = jl + 1;
    for (m = j1; m <=nl; ++m) {
      if (! (tid[m] == ctracer_largenum)) goto L23153;
      tr[m] = ctracer_largenum;
      goto L23154; 
L23153:       
      sqt = sqrt(vsq[m] - vsq[jl]);
      tinj[m] = tid[m] - tkj * sqt / (v[m] * v[jl]);
      didj[m] = did[m] - tkj * v[jl] / sqt;
      tr[m] = tinj[m] + delta / v[m];
      if (!(didj[m] > delta)) goto L23155;
      tr[m]=ctracer_largenum;
L23155:
L23154:
     if (! (tr[m] < tref)) goto L23157;
     tref = tr[m];
     kk = m;
L23157:  ;   
    } /* L23151 */
    
/*   if there is no refracted ray: */
    if  (tref == ctracer_largenum) {
     xovmax = ctracer_largenum;
     kk = 0;
     return 0;
    }

/*  to control */
/*   if there is a refracted ray, determine xovmax: */
/*   find lx, the 1st layer below the event layer which */
/*   is not a low velocity layer */
    m = jl + 1;
L23161:    
    if (!(tid[m] == ctracer_largenum)) goto L23162;
     m++;
     goto L23161;
L23162:     
    lx = m;

/*   check whether the event is in the 1st layer */
    if (!(jl == 1)) goto L23163;
    xovmax = tinj[lx ] * v[lx] * v[1] / (v[lx] - v[1]);
     return 0;
    
L23163:
    m = jl;
    
/*   find jx, the 1st layer above and including the event */
/*   layer which is not a low velocity layer */
L23165:
      tid[m] = 0.0;
      m1 = m - 1;
      for (l = 1; l <= m1; ++l) {
 	 if (!(vsq[m] <= vsq[l])) goto L23170;
	 tid[m] = ctracer_largenum;
	 goto L23171;
L23170:	 
	 sqt = sqrt(vsq[m] - vsq[l]);
 	 tim = thk[l] * sqt / (v[l] * v[m]);
 	 tid[m] += tim;
L23171: ;
      } 
       --m;
/*  decide whether or not jx=1 and calculate xovmax */
    if (! ((tid[m+1] < ctracer_largenum)||(m==1 ))) goto L23165;
    if (! (tid[m+1] < ctracer_largenum)) goto L23172; 

    jx = (m + 1);
    xovmax = (tinj[lx]-tid[jx])*v[lx]*v[jx]
              /(v[lx]-v[jx]);
    goto L23173;
L23172:    

     xovmax = tinj[lx] * v[lx] * v[1] / (v[lx] - v[1]);
L23173:
    return 0;
} 

// --- EndOf function refract --------------------------------------------------

// --- function tiddid ---------------------------------------------------------
/*!
Purpose: 
 Compute intercept times and critical distances for refracted rays 

 \param  nl  tInteger        -  number of velocity layers  max see cmax_layers ttconst.h
 \param  jl  tInteger        -  layer number containing the event
 \param  v   tDoubleArray    -  velocities of each layer in km/s
 \param  vsq tDoubleArray    -  square of velocities of layers (km/s)^2 
 \param  thk tDoubleArray    -  thickness of each layer in km
 \param  tid tDoubleArray   - <b>return</b> travel time intercept for refraction  in the m-th layer  
 \param  did tDoubleArray   - <b>return</b> critical distance 

 \return value is irrelevant
*/
tInteger tiddid(tInteger jl,  tInteger nl,
                tDoubleArray v,  tDoubleArray vsq, tDoubleArray thk, 
                tDoubleArray tid, tDoubleArray did)
	
{
    static tDouble dimm;
    static tInteger l, m, j1, m1;
    static tDouble tim, sqt, did1, did2, tid1, tid2;

    j1 =jl+1;
    for (m = j1; m<=nl; ++m) {
	tid[m] = 0.0; did[m] = 0.0;
	tid1 = 0.0; tid2 = 0.0;
	did1 = 0.0; did2 = 0.0;
	m1 = m - 1;
	
	for (l = 1; l<=m1; ++l) {
	  if (!(vsq[m] <= vsq[l])) goto L23178;
	  	  
/*   if m is a low velocity layer, set tid and did to */
/*   very large values */

	   tid[m] = ctracer_largenum;
	   did[m] = ctracer_largenum;
	   goto L23179;
L23178:
  	   sqt = sqrt(vsq[m] - vsq[l]);
	   tim = thk[l] * sqt / (v[l] * v[m]);
	   dimm = thk[l] * v[l] / sqt;
	   if (!(l < jl)) goto L23180;
/*   sum for layers above event layer */
           tid1 += tim;
	   did1 += dimm;

	   goto L23181;
L23180:	   
/*   sum for layers below and including the event layer */
           tid2 += tim;
	   did2 += dimm;

L23181: ;
L23179: ;
	 }  
      
/*   calculate tid and did if m is not a low velocity layer */

	if (!(tid[m]!= ctracer_largenum)) goto L23182;

  	 tid[m] = tid1 + tid2 * 2;
	 did[m] = did1 + did2 * 2;
L23182: ;
    }
    return 0;
}
// --- Endof function tiddid ---------------------------------------------------


// --- function direct ---------------------------------------------------------
/*! 
 Purpose: 
  Compute travel time, etc., for direct (upward-departing) ray 

  \param   nl    tInteger     -  number of velocity layers  max see cmax_layers ttconst.h
  \param   v     tDoubleArray -  velocities of each layer in km/s
  \param   vsq   tDoubleArray -  square of velocities of layers (km/s)^2 
  \param   thk   tDoubleArray -  thickness of each layer in km
  \param   jl    tInteger     -  layer number containing the event
  \param   tkj   tDouble      -  depth/disctance event <-> event layer 
  \param   delta  tDouble      -  epicentral dictance in km
  \param   depth tDouble      -  event depth in km
  \param   tdir  tDouble      -  <b>return</b> direct ray traveltime in sec.
  \param   u     tDouble      -  <b>return</b> sine of the take of angle 
  \param   x     tDouble      -  <b>return</b> horizontal travel distance in the event layer  

 Description: 
  For the direct seismic ray from an event to a receiver in 
  a layered velocity structure, direct predicts the travel time, the 
  sine of the takeoff angle, and the horizontal distance of travel in 
  the event layer.  The receiver must be located at the top of layer 
  1 and the event must be located below layer 1.  Low velocity 
  layers are permitted. 
  
  To find the takeoff angle of the ray, a numerical approach 
  is required.  The basic scheme adopted here is the method of false 
  position.  (see acton, 1970, 'numerical methods that work,' for 
  example.)  First, the characteristics of the fastest layer 
  between the event and the surface are determined.  These permit 
  placing definite lower and upper bounds, ua and ub, on the 
  sine of the takeoff angle.  In turn, lower and upper bounds, xa 
  and xb, on the horizontal travel distance in the event layer are
  determined.  
  
  The total horizontal travel distance for a ray with
  with horizontal travel distance x in the event layer is denoted 
  by del, and the zero of del - delta is obtained by using xa and 
  xb as initial guesses for x in the method of false position 
  from x and tkj, the depth of the event below the top of the event 
  layer, the sine of the takeoff angle, u , is calculated. 

  From u and x, tdir is found by summing the travel time in 
  each layer.  finally, a slight correction to tdir is made, based
  on the misfit between the final del and delta. 

 \return value is irrelevant
*/
tInteger direct(tInteger nl,tDoubleArray v,tDoubleArray vsq, tDoubleArray thk, 
            tInteger jl,tDouble tkj,tDouble delta,tDouble depth,
            tDouble &tdir, tDouble& u, tDouble& x)
{
    /* Local variables */
    tDouble dela, delb;
    tInteger lmax,l,j1,kount;
    tDouble uasq, ubsq;
    tDouble  r;
    tDouble ubdiv, vlmax;
    tDouble xtest,ua,ub,xa,xb,tklmax,del,usq;
    
/*  Focus in surface layer */

    if (jl == 1) {
        r = sqrt(depth*depth+delta*delta);
        tdir = r / v[1];
        u = delta / r;
        x = delta;
        return 0;
    }
/*  Find the fastest layer, lmax, above and including jl */
    lmax = jl;
    tklmax = tkj;
    vlmax = v[jl];
    j1 = jl - 1;
    for (l = 1; l <= j1; ++l) {
      if (v[l] > vlmax) { 
       lmax = l;
       tklmax = thk[l];
       vlmax = v[l];
      } 
    }
    
/* CHANGE BY E.KISSLING MARCH 1984 */
    if (tklmax <= 0.05) tklmax = 0.05;
    
/*  Find initial bounds on the sine of the takeoff angle */
    ua = (v[jl]/vlmax)*delta/sqrt(delta*delta+depth*depth);
    ub = (v[jl]/vlmax)*delta/sqrt(delta*delta+tklmax*tklmax);
    
/*  Calculate horizontal travel distances */
    uasq = ua*ua;
    ubsq = ub*ub;
    
/* CHANGE BY E.KISSLING MARCH 1984 */
    if (ubsq >= 1.0) ubsq = 0.99999;
    if (uasq >= 1.0) uasq = 0.99999;
    
    xa = tkj * ua / sqrt(1.0 - uasq);
    if (!(lmax == jl)) 
     xb = tkj * ub / sqrt(1. - ubsq);
    else 
     xb = delta;
    dela = xa;
    delb = xb;
    for (l=1; l<=j1; ++l) {
        dela += thk[l] * ua / sqrt(vsq[jl] / vsq[l] - uasq);
        ubdiv = sqrt(vsq[jl] / vsq[l] - ubsq);
        if (ubdiv < 1e-20) ubdiv=1e-20;
        delb += thk[l] * ub / sqrt(vsq[jl] / vsq[l] - ubsq);
    }
/*  Loop to find the zero of del-delta by the method of false position */
    for (kount = 1; kount <= ctracer_numloops; ++kount) {
        if (!(delb - dela < 0.02))  goto L23194;
         x = (xa + xb) * 0.5;
         u = (x)/sqrt((x)*(x)+tkj*tkj);
         usq = u*u;
         break;
L23194:
        x = xa + (delta - dela) * (xb - xa) / (delb - dela);
        u = x/sqrt(x*x+tkj*tkj);
        usq = u*u;
        del = x;
        for (l = 1; l <= j1; ++l) {
           del += thk[l] * u / sqrt(vsq[jl] / vsq[l] - usq);
        }

        xtest = del - delta;
        if (fabs(xtest) < ctracer_eps) break; 
        if (!(xtest < 0.0)) goto L23200;
         xa = x;
         dela = del;
	goto L23201;
L23200:	
          xb = x;
          delb = del;
L23201:            

;   }
L23193:
/*  Calculate direct-ray travel time */
    tdir = sqrt(x*x +tkj*tkj) / v[jl];
    for (l = 1; l <= j1; ++l) {
        tdir += thk[l] * v[jl] / (vsq[l] * sqrt(vsq[jl] / vsq[l] - usq));
    }
    tdir -= u / v[jl] * (del - delta);
    return 0;
}
// --- EndOf function direct ---------------------------------------------------


// --- function test TestTravelTimes ----------------------------------------------------
/*!
Purpose:
 Testing the travel time routine whith a
 synthetic, a P-wave and a S-wave set.
 Use it as ANSI C sample code !
 For C++ usage see object cVeloModel1D  
 it contains dynamic array allocation and 
 index access error prevetion !  
*/
tInteger TestTravelTimes1D (void){

tInteger i,j;  		// loop variable depth and width 
tInteger mod_nl = 9; 	// number of real layers
tInteger mod_nlc= 3; 	// number of synth layers
tDouble dx,dy;           // width an depth
tDouble tt,ain;          // traveltime and Angel of incidence          
tDouble mod_ratio;	// Velocity ratio p/s
tDoubleArray mod_t;          // layer location real modell
tDoubleArray mod_tc;         // layer location synth modell
tDoubleArray mod_vp;         // P wave speed real modell
tDoubleArray mod_vs;         // S wave speed real modell
tDoubleArray mod_vc;         // wavespeed synth modell 

mod_ratio = 1.75;
// --- layers for the real structure ----------------	 
mod_t   = (tDoubleArray) malloc((mod_nl+1)*sizeof(tDouble)); 
mod_vs  = (tDoubleArray) malloc((mod_nl+1)*sizeof(tDouble)); 
mod_vp  = (tDoubleArray) malloc((mod_nl+1)*sizeof(tDouble)); 

// --- layers for the synthetic structure ------------	 
mod_tc  = (tDouble*) malloc((mod_nlc+1)*sizeof(tDouble)); 
mod_vc  = (tDouble*) malloc((mod_nlc+1)*sizeof(tDouble)); 

// --- filling synthetic structure -------------------
mod_tc[0]=   0.0;
mod_tc[1]=   0.0;
mod_tc[2]= 100.0;
mod_tc[3]= 300.0;

mod_vc[0]= 1.0;
mod_vc[1]= 1.0;
mod_vc[2]= 1.0;
mod_vc[3]= 1.0;

// --- filling real structure -------------------
mod_t[1]=   0.0;
mod_t[2]=   5.0;
mod_t[3]=  10.0;
mod_t[4]=  20.0;
mod_t[5]=  50.0;
mod_t[6]=  80.0;
mod_t[7]=  90.0;
mod_t[8]= 170.0;
mod_t[9]= 280.0;

mod_vp[1]= 4.59;
mod_vp[2]= 5.83;
mod_vp[3]= 6.19;
mod_vp[4]= 6.44;
mod_vp[5]= 6.89;
mod_vp[6]= 7.59;
mod_vp[7]= 7.94;
mod_vp[8]= 8.34;
mod_vp[9]= 8.51;

// --- filling real s wave speeds ---------------
for (i = 0; i < mod_nl; ++i) 
   mod_vs[i] = mod_vp[i]/mod_ratio;
     
// --- test ---------------
for (i=1;i<=10;i++) 
 for (j=1;j<=10;j++) { 
  dx=(tDouble)i*10;
  dy=(tDouble)j*10;
  TravelTime1D(dx,dy,mod_nlc,mod_vc,mod_tc,tt,ain);
  printf("c> dx=%2.0f dy=%2.0f tt=%f ain=%f   %f\n",dx,dy,tt,ain,180-ain);
  TravelTime1D(dx,dy,mod_nl ,mod_vp ,mod_t,tt,ain);  
  printf("p> dx=%2.0f dy=%2.0f tt=%f ain=%f %f\n",dx,dy,tt,ain,180-ain); 
  TravelTime1D(dx,dy,mod_nl ,mod_vs ,mod_t,tt,ain);  
  printf("s> dx=%2.0f dy=%2.0f tt=%f ain=%f %f\n\n",dx,dy,tt,ain,180-ain); 
}      

// --- free memory --------------------------------------------------      
 free(mod_t);
 free(mod_tc);
 free(mod_vs);
 free(mod_vp);
 free(mod_vc);	
 return 0;
}
