//=== File description =========================================================
/*! \file uTravelTime.h
    \brief 1D raytracing ANSI C routines. Module was adopted from hypoDD 
 FORTRAN 77 project written by Felix Waldhauser in 2000
 by using f2c and some language structural changes direct1.f -- translated by f2c
 (version 20020208) and patched into explicit C data types to prevent  -lf2c linking.
 .. Geowissenschaften Uni-Potsdam A. Weidauer IV/2002
*/
//================================================================================

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

#ifndef __UTRAVELTIME_H__
#define __UTRAVELTIME_H__

#include "uConst.h"

// === Definition of constants =================================================
//! Maximal number of layers in the velocity model
#define cmax_layers 35       
//! ctracer_eps  Break condition for shooting routine 
#define ctracer_eps 0.001    
//! A lagre number marker (very dirty programming) 
#define ctracer_largenum 1e5 
//! Number of iterations in the shooting routine 
#define ctracer_numloops 35  

// === external Function interface =============================================


// --- function TravelTime1D -----------------------------------------------------
//! Determine the fastest traveltime between a source and a receiver by a given velocity modell. 
/*!
 Purpose:
  Determine the fastest traveltime between a source 
  at depth [km] and a receiver at distance [km] by a give 
  velocity modell described by a number of layers
  ni, a velocity layer set v[] and the location of the
  layers top[]. 
 
 
 Parameter Input:
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
               tDouble& t,tDouble& ain);
// === internal function interfaces =============================================

// --- function vmodel ---------------------------------------------------------
//! Extract needed information from the layered velocity model. 
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
		 tDouble&  tkj);  

// --- function refract ---------------------------------------------------------
//! Purpose: Find "refracted" ray with smallest travel time 
/*!
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
                 tInteger& kk, tDouble& tref, tDouble& xovmax);

// --- function tiddid ---------------------------------------------------------
//!Purpose:  Compute intercept times and critical distances for refracted rays 
/*!

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
                tDoubleArray tid, tDoubleArray did);

// --- function direct ---------------------------------------------------------
//!  Purpose:  Compute travel time, etc., for direct (upward-departing) ray 
/*! 
  \param   nl    tInteger     -  number of velocity layers  max see cmax_layers
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
            tDouble &tdir, tDouble& u, tDouble& x);


// --- function test TestTravelTimes -------------------------------------------
//! Testing the travel time routine whith a synthetic, a P-wave and a S-wave set.
/*!
Purpose:
 Testing the travel time routine whith a  synthetic, a P-wave and a S-wave set.
 Use it as ANSI C sample code ! For C++ usage see object cVeloModel1D  
 it contains dynamic array allocation and  index access error prevetion !  
*/
tInteger TestTravelTimes1D (void);


#endif // __UTRAVELTIME_H__
//==============================================================================
// EndOf
//==============================================================================
