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
#include <cmath>
#include <cstdio>
#include <cfloat>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "livloc.h"
#include "TNT/tnt.h"
#include "TNT/jama_svd.h"

using namespace TNT;


// this is the one and only LivLov instance, declared extern in livlov.h
LivLoc *livLocObj = NULL;

// for debugging 
// #define LIV_LOC_DEBUG 1

/*
#define DEF_START_DEPTH	10.0
#define ITER_MAX	10
#define ITER_RUN_MAX	10
#define STD_DEV_CUT_P	5.0
#define STD_DEV_CUT_S	5.0
#define MIN_WEIGHT	0.2
#define S_WEIGHT	1.0
#define LOC_ACCURACY	0.5
#define RESIDUAL_CUT	1.0
#define RES_CUT_RATIO   2
*/
#define SVD_CUT_OFF 200
//#define ADJ_PERC 1.0
//#define MAX_DEPTH_SHIFT 20.0
#define ADJ_PERC 0.75
//#define MAX_SHIFT 30.0
#define MAX_SHIFT 30.0
#define MAX_ORIG_SHIFT 5.0
/*
// this class probably will morpth into relocation class
class RelocationParams
{
public:
  RelocationParams() 
  : startDepth(-10.0), iterMax(10), iterRunMax(10), stdDevCutP(4.0), stdDevCutS(6.0),
    minWeight(0.1), sWeight(0.5), locAccuracy(0.05), residualCut(2.0), resCutRatio(2)
  {}
  virtual ~RelocationParams() {}
  
public:
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

// these parameters probably will be member variables of relocation class
static RelocationParams rp; // used defaults
*/

// forward declarations
static double find_median(double *, int);
static int res_comp(const void *, const void *);

//--------------------------------------------------

/*
 we might need to add classes for the following: Arrival, OriginQuality and maybe improve our Origin and Pick definitions
 */
bool LivLoc::relocate(std::list<LivLoc::LLPick> pickList, LivLoc::LLOrigin& origin, bool useOriginAsInitialLocation)
{
  /*
   CreationInfo ci;
   OriginQuality oq; // ~sysop/seiscomp3/trunk/include/seiscomp3/datamodel/originquality.h
   
   OriginPtr org = Origin::Create(); // Origin is a heavyweight object; "Create" generates a publicID 
   org->setTime(...); // time_t style
   org->setLatitude(...);
   org->setLongitude(...);
   org->setDepth(...)
   org->setEvaluationMode(AUTOMATIC); // not required, SC app can add later
   // fill ci - this could be put in by SC App - not v. important
   org->setCreationInfo(ci);
   // fill oq - this info is needed by SC, we should define a class that carries the necessary info
   org->setOriginQuality(oq);
   */
  
  // for argument types see ~sysop/seiscomp3/trunk/include/seiscomp3/datamodel/origin.h
  // maybe we should create a list of "Arrivals" here as a member of Origin?
	// SeisComP: sing Arrival would be better because it is a light-weight object (no publicID required)
	// so can just use 'new' (c++ version of malloc) to create each Arrival,
	// then when Origin is passed back to SC application can just copy Arrivals into SC Origin
	// object and send out message
  
  /*
   // for argument types see ~sysop/seiscomp3/trunk/include/seiscomp3/datamodel/arrival.h
   ArrivalPtr arrivalPtr = new Arrival; // Arrival is a lightweight object, no publicID required
   arrivalPtr->setPickID(...); // ID of pick that caused this Arrival
   arrivalPtr->setPhase(...);
   arrivalPtr->setAzimuth(...);
   arrivalPtr->setDistance(...); // epicentre distance in degrees (not kilometres)
   arrivalPtr->setTimeResidual(...);
   arrivalPtr->setWeight(...); // if associated then set to non zero (give =1)
   arrivalPtr->setCreationInfo(...); // data-centre ID, ref to relocatation routine/binder? not really necessary
   origin.push_back(arrivalPtr);
   */

  // find the station with the earliest pick and use this as the starting epicenter
  double origin_time=DBL_MAX;
  Position e_pos, s_pos;
  int n_obs=pickList.size();
  int n_obs_used;
  float ain;
  int   i,j;
  int   ip,is;
  int   iter, run_iter;
  double dist, epi_dist;
  double rms_all, rms_mean;
  double rms_all_old;
  double all_rms_weighted, sum_of_weights, old_all_rms_weighted;
  double rms_all_p, rms_mean_p;
  double rms_all_s, rms_mean_s;
  std::list<LLPick>::iterator pickIter;
  double *res_vp;
  double *res_vs;
  std::string Sphase ="S";
  Array1D< double > Res_save;
  Array1D< double > weight;
  //debug
  double largestCond = -DBL_MAX;
  double lastCond = -9999.0;
  bool   did_break;
  // just debugging
  char	station[400][6];
#ifdef LIV_LOC_DEBUG
#endif
  FILE	*fp_debug;
  fp_debug=fopen("loc.debug","a");
  
  // starting lon and lat could be taken from binder grid node or previous location
  if (useOriginAsInitialLocation) {
    // assume origin (starting) location is in lat/lon
    origin_time = origin.originTime;
    cart.toXYZ(origin.location.y, origin.location.x, &e_pos.x, &e_pos.y);
    e_pos.z = origin.location.z;    
  }
  else {
    for (pickIter = pickList.begin(); pickIter != pickList.end(); pickIter++) {
      if(pickIter->pickTime < origin_time) {
        origin_time = pickIter->pickTime;
        e_pos = stationMap[pickIter->stationCode];
        e_pos.z = rp.startDepth;
      }
    }
  }
  
  Array1D< int > used(n_obs, 1);
	// this is just an array indicating at the end of the location if the phase has been used or not 1:yes, 0:no
  Array1D< int > isP(n_obs, 1);
	// this is just an array indicating if the phase is P 1 or S 0
  
  for(pickIter = pickList.begin(),i=0; pickIter != pickList.end();pickIter++,i++) {
    if(pickIter->phaseHint.compare(Sphase)==0)
	    isP[i]=0;
#ifdef LIV_LOC_DEBUG
    fprintf(stdout,"Phase: %s %d\n", pickIter->phaseHint.c_str(), isP[i]);
#endif 
  }
  int start_residual_cut=rp.iterRunMax/rp.resCutRatio;
  
  rms_all_old=1.0e6;
  old_all_rms_weighted=1.0e6;
  for(run_iter=0;run_iter<rp.iterRunMax;run_iter++){
    // start depth should ideally be set according to the stacking node, saqme for lon and lat
    /*
    if(run_iter==0)
      e_pos.z=rp.startDepth;
    */
    
    did_break=false;
    int n_obs_used_p=0;
    int n_obs_used_s=0;
    for(i=0,n_obs_used=0;i<n_obs;i++) {
      if(used[i]==1) {
        n_obs_used++;
        if(isP[i])
          n_obs_used_p++;
        else
          n_obs_used_s++;
      }
      // fprintf(stdout,"Obs[%02d]: Used: %1d \n", i, used[i]);
    }
#ifdef LIV_LOC_DEBUG
    fprintf(stdout,"Using %02d = %02d + %02d observations\n\n", n_obs_used,n_obs_used_p,n_obs_used_s);
#endif    
    // stop if we have less than 4 observations
    if (n_obs_used < 4) {
      fclose(fp_debug);
      return false;
    }
    
    // now we set up the arrays for the LSQR location
    Array2D< double > A(n_obs_used, 4, 0.0);
    Array1D< double > Res(n_obs_used, 0.0);
    //Array1D< double > Res_save(n_obs_used, 0.0);
    Res_save = Array1D< double > (n_obs_used, 0.0);
    Array1D< double > Res_p(n_obs_used_p, 0.0);
    Array1D< double > Res_s(n_obs_used_s, 0.0);
    //Array1D< double > weight(n_obs_used, 0.0); // residual weighting; have to include later also a distance weighting
    weight = Array1D< double > (n_obs_used, 0.0); // residual weighting; have to include later also a distance weighting
    Array1D< double > res_std(n_obs_used, 0.0);
    res_vp = (double *)calloc(n_obs_used_p,sizeof(double)); 
    res_vs = (double *)calloc(n_obs_used_s,sizeof(double)); 
    
    for(iter=0;iter<rp.iterMax;iter++) {
        double vp = velmod.lookupVelocity(-e_pos.z);
        double vs = vp/pToSVelocityRatio;
        double v;
#ifdef LIV_LOC_DEBUG
        fprintf(stdout,"Depth: %g %g\n", e_pos.z, vp);
        fprintf(stdout,"S-Weight: %g\n", rp.sWeight);
#endif
        //future: check if e_pos.z is on an interface. If yes, move it down a bit 20m?
      
        rms_all=rms_all_p=rms_all_s=0.0;
        rms_mean=rms_mean_p=rms_mean_s=0.0;
        for(pickIter = pickList.begin(),ip=0,is=0,i=0,j=0; pickIter != pickList.end();pickIter++,j++) {
	    strcpy(station[j],pickIter->stationCode.c_str());
#ifdef LIV_LOC_DEBUG
#endif
            if(used[j]==1) {
                ain = -1.0;
                s_pos = stationMap[pickIter->stationCode];
          
                double synTime;
                if(isP[j]) {
                  synTime=origin_time + tt1d(e_pos, s_pos, ain);
                  v=vp;
                } else {
                  synTime=origin_time + tt1d(e_pos, s_pos, ain)*pToSVelocityRatio;
                  v=vs;
                }
          
                Res[i]=pickIter->pickTime - synTime;
                ain *= M_PI/180.0;
                if(isP[j]) {
                  res_vp[ip]=Res[i];
                  Res_p[ip]=Res[i];
                  rms_all_p += (Res_p[ip]*Res_p[ip]);
                  rms_mean_p += Res_p[ip];
                  ip++;
                } else {
                  res_vs[is]=Res[i];
                  Res_s[is]=Res[i];
                  rms_all_s += (Res_s[is]*Res_s[is]);
                  rms_mean_s += Res_s[is];
                  is++;
                }
                rms_all+=(Res[i]*Res[i]);
                rms_mean+=Res[i];
                epi_dist = sqrt((s_pos.x-e_pos.x)*(s_pos.x-e_pos.x)+(s_pos.y-e_pos.y)*(s_pos.y-e_pos.y));
                
                if(ain<M_PI/2) {
                  dist = epi_dist/sin(ain);
	          A[i][3] =  1.0/v * cos(ain);
                } else {
                  dist = epi_dist/sin(M_PI-ain);
	          A[i][3] = -1.0/v * cos(M_PI-ain);
                }
                
                A[i][0] = 1.0;
                if(dist!=0.0) {
                  A[i][1] = -1.0/v * (s_pos.x-e_pos.x)/fabs(dist);
                  A[i][2] = -1.0/v * (s_pos.y-e_pos.y)/fabs(dist);
                } else {
                  A[i][1]=0.0;
                  A[i][2]=0.0;
                }
                //A[i][3] = -1.0/v * cos(ain);
                
                // assigning a weight
                if(isP[j]) {
                  weight[i]=1.0;
                } else {
                  weight[i]=rp.sWeight;
                }
                
                //fprintf(stdout,"Obs(%03d):%lf Res: %lf %f => %lf %lf %lf %lf\n", i, 
                //		pickIter->pickTime-origin_time, Res[i], ain, A[i][0], A[i][1], A[i][2],A[i][3]);
                // fprintf(stdout,"Obs(%d):%lf Res: %lf %f => %lf %lf %lf %lf\n", i,
                // 		pickIter->pickTime-origin_time, Res[i], ain/M_PI*180.0, dist, epi_dist,cos(ain),
                // 		ain);
                i++;
            }
        }
      
        // find the median for P
        double md_res=find_median(res_vp, n_obs_used_p); 
#ifdef LIV_LOC_DEBUG
        fprintf(fp_debug,"  Median: %lf\n", md_res);
#endif
        for(i=0;i<n_obs_used_p;i++) {
#ifdef LIV_LOC_DEBUG
	    fprintf(fp_debug,"%lf = %lf - %lf ", fabs(res_vp[i]-md_res), res_vp[i], md_res);
#endif
            res_vp[i]=fabs(res_vp[i]-md_res);
#ifdef LIV_LOC_DEBUG
	    fprintf(fp_debug,"%lf\n", res_vp[i]);
#endif
        }
        double mad_res_p=find_median(res_vp, n_obs_used_p)/0.67449*rp.stdDevCutP;
#ifdef LIV_LOC_DEBUG
        fprintf(fp_debug,"  Median Interval: %lf (%lf)\n", mad_res_p, find_median(res_vp, n_obs_used_p));
#endif
        rms_all_p /= (double)n_obs_used_p;
        rms_mean_p /= (double)n_obs_used_p;
      
        // find the median for S
        md_res=find_median(res_vs, n_obs_used_s); 
        for(i=0;i<n_obs_used_s;i++) {
            res_vs[i]=fabs(res_vs[i]-md_res);
        }
        double mad_res_s=find_median(res_vs, n_obs_used_s)/0.67449*rp.stdDevCutS;
        rms_all_s /= (double)n_obs_used_s;
        rms_mean_s /= (double)n_obs_used_s;
      
        rms_all /= (double)n_obs_used;
        rms_mean /= (double)n_obs_used;
#ifdef LIV_LOC_DEBUG 
        fprintf(stdout,"RMS: %lf %lf  P: %lf %lf   S: %lf %lf\n", rms_all, rms_mean, rms_all_p, rms_mean_p, rms_all_s, rms_mean_s);
#endif 
        rms_all_old = rms_all;

        double val;
#ifdef LIV_LOC_DEBUG
        fprintf(fp_debug,"Before:=============\n");
#endif
	// now we only apply the reweighting aftert the first iteration in order to not downweight new associated phases
	// if(iter!=0) {
          for(j=0,i=0,is=0,ip=0;j<n_obs;j++)
          {
            if(used[j]==1) {
                if(isP[j]) {
                    res_std[i]=sqrt((Res_p[ip]-rms_mean_p)*(Res_p[ip]-rms_mean_p)/(n_obs_used_p-1));
                    val=1.0-pow(fabs(Res_p[ip]/mad_res_p),3.0);
                    ip++;
                } else {
                    res_std[i]=sqrt((Res_s[is]-rms_mean_s)*(Res_s[is]-rms_mean_s)/(n_obs_used_s-1));
                    val=1.0-pow(fabs(Res_s[is]/mad_res_s),3.0);
                    is++;
                }
	        // only applying statistics if we have at least 4 S-picks
	        if(isP[j]==1)
	        {
	            if(n_obs_used_p>=4) {
                        weight[i] *= pow(val,3.0);
                        if(weight[i]<0.00001)
                            weight[i]=0.00001;
	            }
	        } else {
	            //if(n_obs_used_s>=4) {
		    // switching off s-wave weighting just for now....
	            if(n_obs_used_s>=4) {
                        weight[i] *= pow(val,3.0);
                        if(weight[i]<0.00001)
                                weight[i]=0.00001;
	            }
                }
	        fprintf(fp_debug,"%s %1d %lf %lf %lf \n", station[j], used[j], Res[i], val, weight[i]);
                fprintf(stdout,"(%02d)[%1d]: %lf %lf %lf %lf %lf %lf %lf\n", i, isP[j], rms_mean, res_std[i],Res[i], mad_res_p, mad_res_s, val, weight[i]);
#ifdef LIV_LOC_DEBUG 

                fprintf(stdout,"(%02d)[%1d]: %lf %lf %lf %lf %lf %lf %lf\n", i, isP[j], rms_mean, res_std[i],Res[i], mad_res_p, mad_res_s, val, weight[i]);
#endif
                i++;
            }
	  }
        // }
	// end from iter !=0

        // normalize the weights
        double max_weight=-1.0;
        for(i=0;i<n_obs_used;i++) {
	    if(weight[i]>max_weight)
	        max_weight=weight[i];
        }
        for(i=0;i<n_obs_used;i++) {
	    weight[i]/=max_weight;
        }
      
	// calculating the weighted misfit
	all_rms_weighted=0.0;
	sum_of_weights=0.0;
        // weighting the matrix according to the pick weights
        for(i=0;i<n_obs_used;i++) {
            Res_save[i] = Res[i];
            Res[i] *= weight[i];
	    all_rms_weighted+=(Res[i]*Res[i]);
	    sum_of_weights+=weight[i];
            for(j=0;j<4;j++) {
	          A[i][j] *= weight[i];
            }
        }

	all_rms_weighted /= sum_of_weights;
      
	old_all_rms_weighted = all_rms_weighted;
      
        // now setting up inversion problem;
        // Matrix< double > A_t = transpose(A);
        // Matrix< double > B = matmult(A_t, A);
        // Array2D< double > T(A.dim1(),A.dim2());
      
#ifdef LIV_LOC_DEBUG 
        fprintf(stdout,"Iter: %02d RMS: %lf\n", iter, rms_all);
#endif
        //fprintf(stdout,"row: %d  col: %d\n", A.dim1(), A.dim2());
      
        // now calculate the inverse
        JAMA::SVD< double > svd(A);
        Array2D< double > U,V;
        Array1D< double > s;
      
        svd.getU(U);
        svd.getV(V);
        svd.getSingularValues(s);
      
        Array1D< double > tv(s.dim());
        Array1D< double > x(s.dim());

	// debug
	lastCond = svd.cond();
	if (lastCond > largestCond) {
	    largestCond = lastCond;
	}
	// end debug
#ifdef LIV_LOC_DEBUG
        fprintf(stdout,"Condition number %lf ==> rank: %d\n",svd.cond(),svd.rank());
#endif
      
        // now we solve the sytem of linear equations in a least square sense
        for(i=0;i<U.dim2();i++)
        {
            double sum=0.0;
            if(s[i]!=0.0) {
                for(j=0;j<U.dim1();j++)
                    sum += U[j][i]*Res[j];
                sum /= s[i];
            }
            tv[i]=sum;
        }
        for(i=0;i<V.dim1();i++)
        {
            double sum=0.0;
            for(j=0;j<V.dim2();j++) {
              sum += V[i][j]*tv[j];
            }
            x[i]=sum;
        }
      
      
        // now we  have to check if the hypocentre has moved...
        double sum_loc=0;
        int    sum_count=x.dim();
        if(svd.cond()>SVD_CUT_OFF)
	  sum_count--;

        for(i=0;i<sum_count;i++) {
          sum_loc+=fabs(x[i]);
          //fprintf(stdout,"M[%1d]: %lf\n", i+1, x[i]);
        }
        sum_loc/=sum_count;

#ifdef LIV_LOC_DEBUG
        fprintf(stdout,"Shifts: %g %g %g %g\n", x[0],x[1],x[2],x[3]);
        fprintf(stdout,"Average location shift: %lf\n", sum_loc);
#endif

        fprintf(fp_debug,"B: %lf %lf %lf %lf %lf %lf\n", x[0],x[1],x[2],x[3],sum_loc, rp.locAccuracy);
        if(sum_loc<rp.locAccuracy)
        {
	    did_break=true;
            break;
        }

      // now we also reduce the maximum allowed shift...
      if(fabs(x[0])>MAX_ORIG_SHIFT) {
	if(x[0]<0.0) {
	    x[0]=-MAX_ORIG_SHIFT;
	} else {
	    x[0]=MAX_ORIG_SHIFT;
	}
      }
      if(fabs(x[1])>MAX_SHIFT) {
	if(x[1]<0.0) {
	    x[1]=-MAX_SHIFT;
	} else {
	    x[1]=MAX_SHIFT;
	}
      }
      if(fabs(x[2])>MAX_SHIFT) {
	if(x[2]<0.0) {
	    x[2]=-MAX_SHIFT;
	} else {
	    x[2]=MAX_SHIFT;
	}
      }
      if(fabs(x[3])>MAX_SHIFT) {
	if(x[3]<0.0) {
	    x[3]=-MAX_SHIFT;
	} else {
	    x[3]=MAX_SHIFT;
	}
      }

      fprintf(fp_debug,"A: %lf %lf %lf %lf \n\n", x[0],x[1],x[2],x[3]);
      
      origin_time+=x[0];
      e_pos.x+=x[1];
      e_pos.y+=x[2];
      if(svd.cond()>SVD_CUT_OFF) {
#ifdef LIV_LOC_DEBUG
	 fprintf(stdout,"depth fixed!!\n");
#endif
      } else {
	 // no air quakes
	 if(e_pos.z+(x[3]*ADJ_PERC)>0.0) {
            x[3]=-e_pos.z/ADJ_PERC;
	 }
         e_pos.z+=(x[3]*ADJ_PERC);
      }

      // now do the whole rms and weight calculation after the location step
      all_rms_weighted=0.0;
      rms_all=rms_all_p=rms_all_s=0.0;
      rms_mean=rms_mean_p=rms_mean_s=0.0;
      for(pickIter = pickList.begin(),ip=0,is=0,i=0,j=0; pickIter != pickList.end();pickIter++,j++) {
        if(used[j]==1) {
          ain = -1.0;
          s_pos = stationMap[pickIter->stationCode];
          
          double synTime;
          if(isP[j]) {
            synTime=origin_time + tt1d(e_pos, s_pos, ain);
            v=vp;
          } else {
            synTime=origin_time + tt1d(e_pos, s_pos, ain)*pToSVelocityRatio;
            v=vs;
          }
          
          Res[i]=pickIter->pickTime - synTime;
          ain *= M_PI/180.0;
          if(isP[j]) {
            res_vp[ip]=Res[i];
            Res_p[ip]=Res[i];
            rms_all_p += (Res_p[ip]*Res_p[ip]);
            rms_mean_p += Res_p[ip];
            ip++;
          } else {
            res_vs[is]=Res[i];
            Res_s[is]=Res[i];
            rms_all_s += (Res_s[is]*Res_s[is]);
            rms_mean_s += Res_s[is];
            is++;
          }
          rms_all+=(Res[i]*Res[i]);
	  all_rms_weighted+=((Res[i]*weight[i])*(Res[i]*weight[i]));
          rms_mean+=Res[i];

#if 0
          epi_dist = sqrt((s_pos.x-e_pos.x)*(s_pos.x-e_pos.x)+(s_pos.y-e_pos.y)*(s_pos.y-e_pos.y));
          
          if(ain<M_PI/2) {
            dist = epi_dist/sin(ain);
	    A[i][3] =  1.0/v * cos(ain);
          } else {
            dist = epi_dist/sin(M_PI-ain);
	    A[i][3] = -1.0/v * cos(M_PI-ain);
          }
          
          A[i][0] = 1.0;
          if(dist!=0.0) {
            A[i][1] = -1.0/v * (s_pos.x-e_pos.x)/dist;
            A[i][2] = -1.0/v * (s_pos.y-e_pos.y)/dist;
          } else {
            A[i][1]=0.0;
            A[i][2]=0.0;
          }
          //A[i][3] = -1.0/v * cos(ain);
          
          // assigning a weight
          if(isP[j]) {
            weight[i]=1.0;
          } else {
            weight[i]=rp.sWeight;
          }
          
          //fprintf(stdout,"Obs(%03d):%lf Res: %lf %f => %lf %lf %lf %lf\n", i, 
          //		pickIter->pickTime-origin_time, Res[i], ain, A[i][0], A[i][1], A[i][2],A[i][3]);
          // fprintf(stdout,"Obs(%d):%lf Res: %lf %f => %lf %lf %lf %lf\n", i,
          // 		pickIter->pickTime-origin_time, Res[i], ain/M_PI*180.0, dist, epi_dist,cos(ain),
          // 		ain);
#endif
          i++;
        }
      }

      all_rms_weighted /= sum_of_weights;
      
      // find the median for P
      md_res=find_median(res_vp, n_obs_used_p); 
      for(i=0;i<n_obs_used_p;i++) {
        res_vp[i]=fabs(res_vp[i]-md_res);
      }
      mad_res_p=find_median(res_vp, n_obs_used_p)/0.67449*rp.stdDevCutP;
      rms_all_p /= (double)n_obs_used_p;
      rms_mean_p /= (double)n_obs_used_p;
      
      // find the median for S
      md_res=find_median(res_vs, n_obs_used_s); 
      for(i=0;i<n_obs_used_s;i++) {
        res_vs[i]=fabs(res_vs[i]-md_res);
      }
      mad_res_s=find_median(res_vs, n_obs_used_s)/0.67449*rp.stdDevCutS;
      rms_all_s /= (double)n_obs_used_s;
      rms_mean_s /= (double)n_obs_used_s;
      
      rms_all /= (double)n_obs_used;
      rms_mean /= (double)n_obs_used;
#ifdef LIV_LOC_DEBUG      
      fprintf(stdout,"==> AFTER:: RMS: %lf %lf  P: %lf %lf   S: %lf %lf\n", rms_all, rms_mean, rms_all_p, rms_mean_p, rms_all_s, rms_mean_s);
#endif

      fprintf(fp_debug,"Old: %lf New: %lf || Weighted: Old: %lf  New: %lf\n", sqrt(rms_all_old), sqrt(rms_all), sqrt(old_all_rms_weighted), sqrt(all_rms_weighted));
      // now we are changing for the weighted sum to not include outliers in the RMS calculation
      // if(rms_all_old<rms_all) {
      if(old_all_rms_weighted<all_rms_weighted) {
#ifdef LIV_LOC_DEBUG
	   fprintf(stdout,"RMS values increasing, stopping iteration OLD: %lf NEW: %lf\n", rms_all_old, rms_all);
#endif
           origin_time-=x[0];
           e_pos.x-=x[1];
           e_pos.y-=x[2];
           if(svd.cond()>SVD_CUT_OFF) {
#ifdef LIV_LOC_DEBUG
	      fprintf(stdout,"depth fixed!!\n");
#endif
           } else {
              e_pos.z-=(x[3]*ADJ_PERC);
           }

      // now do the whole rms and weight calculation after the location step
      rms_all=rms_all_p=rms_all_s=0.0;
      rms_mean=rms_mean_p=rms_mean_s=0.0;
      for(pickIter = pickList.begin(),ip=0,is=0,i=0,j=0; pickIter != pickList.end();pickIter++,j++) {
        if(used[j]==1) {
          ain = -1.0;
          s_pos = stationMap[pickIter->stationCode];
          
          double synTime;
          if(isP[j]) {
            synTime=origin_time + tt1d(e_pos, s_pos, ain);
            v=vp;
          } else {
            synTime=origin_time + tt1d(e_pos, s_pos, ain)*pToSVelocityRatio;
            v=vs;
          }
          
          Res[i]=pickIter->pickTime - synTime;
          ain *= M_PI/180.0;
          if(isP[j]) {
            res_vp[ip]=Res[i];
            Res_p[ip]=Res[i];
            rms_all_p += (Res_p[ip]*Res_p[ip]);
            rms_mean_p += Res_p[ip];
            ip++;
          } else {
            res_vs[is]=Res[i];
            Res_s[is]=Res[i];
            rms_all_s += (Res_s[is]*Res_s[is]);
            rms_mean_s += Res_s[is];
            is++;
          }
          rms_all+=(Res[i]*Res[i]);
          rms_mean+=Res[i];
#if 0
          epi_dist = sqrt((s_pos.x-e_pos.x)*(s_pos.x-e_pos.x)+(s_pos.y-e_pos.y)*(s_pos.y-e_pos.y));
          
          if(ain<M_PI/2) {
            dist = epi_dist/sin(ain);
	    A[i][3] =  1.0/v * cos(ain);
          } else {
            dist = epi_dist/cos(M_PI-ain);
	    A[i][3] = -1.0/v * cos(M_PI-ain);
          }
          
          A[i][0] = 1.0;
          if(dist!=0.0) {
            A[i][1] = -1.0/v * (s_pos.x-e_pos.x)/dist;
            A[i][2] = -1.0/v * (s_pos.y-e_pos.y)/dist;
          } else {
            A[i][1]=0.0;
            A[i][2]=0.0;
          }
          // A[i][3] = -1.0/v * cos(ain);
          
          // assigning a weight
          if(isP[j]) {
            weight[i]=1.0;
          } else {
            weight[i]=rp.sWeight;
          }
          
          //fprintf(stdout,"Obs(%03d):%lf Res: %lf %f => %lf %lf %lf %lf\n", i, 
          //		pickIter->pickTime-origin_time, Res[i], ain, A[i][0], A[i][1], A[i][2],A[i][3]);
          // fprintf(stdout,"Obs(%d):%lf Res: %lf %f => %lf %lf %lf %lf\n", i,
          // 		pickIter->pickTime-origin_time, Res[i], ain/M_PI*180.0, dist, epi_dist,cos(ain),
          // 		ain);
#endif
          i++;
        }
      }
      
      // find the median for P
      md_res=find_median(res_vp, n_obs_used_p); 
      for(i=0;i<n_obs_used_p;i++) {
        res_vp[i]=fabs(res_vp[i]-md_res);
      }
      mad_res_p=find_median(res_vp, n_obs_used_p)/0.67449*rp.stdDevCutP;
      rms_all_p /= (double)n_obs_used_p;
      rms_mean_p /= (double)n_obs_used_p;
      
      // find the median for S
      md_res=find_median(res_vs, n_obs_used_s); 
      for(i=0;i<n_obs_used_s;i++) {
        res_vs[i]=fabs(res_vs[i]-md_res);
      }
      mad_res_s=find_median(res_vs, n_obs_used_s)/0.67449*rp.stdDevCutS;
      rms_all_s /= (double)n_obs_used_s;
      rms_mean_s /= (double)n_obs_used_s;
      
      rms_all /= (double)n_obs_used;
      rms_mean /= (double)n_obs_used;
      
#ifdef LIV_LOC_DEBUG
      fprintf(stdout,"==> BACK:: RMS: %lf %lf  P: %lf %lf   S: %lf %lf\n", rms_all, rms_mean, rms_all_p, rms_mean_p, rms_all_s, rms_mean_s);
#endif
	   break;
      }
    }
    
#if 0
    if(run_iter<=start_residual_cut) {
      // now let's see if we can  kick out some of the outliers
      for(i=0,j=0;i<n_obs;i++) {
        if(used[i]==1) {
          if(weight[j]<rp.minWeight)
            used[i]=0;
          j++;
        }
      }
    } else {
      // now also apply residual cut
#ifdef LIV_LOC_DEBUG
      fprintf(stdout,"Iter: %2d Cut: %2d\n", run_iter, start_residual_cut);
#endif
      for(i=0,j=0;i<n_obs;i++) {
	  if(used[i]==1) {
#ifdef LIV_LOC_DEBUG
             fprintf(stdout,"(%02d) [%1d]: %1d %lf\n",i,isP[i],used[i], Res_save[j]);
#endif
             if(fabs(Res_save[j])>rp.residualCut)
               used[i]=0;
             j++;
	  }
      }
    }
#endif
    free(res_vp);
    free(res_vs);
  }
  
  // results
  
  origin.rms = sqrt(rms_all);
  origin.originTime = origin_time;
  cart.toLonLat(e_pos.x, e_pos.y, &origin.location.y, &origin.location.x);
  origin.location.z = e_pos.z;

  fprintf(fp_debug, "%lf %8.4f %8.4f %6.3f %03d %03d\n", origin_time, origin.location.x, origin.location.y, origin.location.z, run_iter, iter);

  // debug
  origin.largestCond = largestCond;
  origin.lastCond = lastCond;
  
  for (pickIter = pickList.begin(), i=0, j=0; pickIter != pickList.end(); pickIter++, i++) {
    LLPick pick = *pickIter;
    pick.timeResidual = Res_save[i];
    // convert back to lat/lon for distance calc
    Position stationPos;
    stationPos.z = stationMap[pick.stationCode].z;
    cart.toLonLat(stationMap[pick.stationCode].x, stationMap[pick.stationCode].y, &stationPos.y, &stationPos.x);
    pick.distance = origin.location.pythag2d(stationPos);
    pick.azimuth = cart.azimuthXYZ(e_pos, stationMap[pick.stationCode]);
    if (used[i] == 1) {
      pick.isArrival = true;
      pick.relocatedWeight = weight[j];
      j++;
    }
    origin.pickList.push_back(pick);
  }

  origin.setGap(); // calculate the gap

  fclose(fp_debug);
  return true;
}

//--------------------------------------------------

double find_median(double *r, int n)
{
  double med;
  // sort the list
  qsort(r,n,sizeof(double),res_comp);
  int n2=n/2;
  if(2*n2 == n)
    med = 0.5*(r[n2-1]+r[n2]);
  else
    med = r[n2];
  
  return(med);
}

//--------------------------------------------------

static int res_comp(const void *p1, const void *p2)
{
  if(*(double *)p1>*(double *)p2)
    return(-1);
  else if (*(double *)p1<*(double *)p2)
    return(1);
  else
    return(0);
}

//--------------------------------------------------

float LivLoc::LLOrigin::setGap()
{
  std::vector<float> azimuthVector;

  for (std::list<LivLoc::LLPick>::iterator pickIter = pickList.begin(); pickIter != pickList.end(); pickIter++) {
    if (pickIter->isArrival) {
      azimuthVector.push_back(pickIter->azimuth);
    }
  }

  std::sort(azimuthVector.begin(), azimuthVector.end());

  float tmpGap = -FLT_MAX;

  for (unsigned int i=0; i < azimuthVector.size() - 1; i++) {
    float tmp = azimuthVector[i+1] - azimuthVector[i];
    tmpGap = tmp > tmpGap ? tmp : tmpGap;
  }

  // complete the loop between largest azi and smallest azi
  float tmp = 360.0f - azimuthVector[azimuthVector.size() - 1] + azimuthVector[0];
  tmpGap = tmp > tmpGap ? tmp : tmpGap;

  gap = tmpGap;

  return gap;
}
