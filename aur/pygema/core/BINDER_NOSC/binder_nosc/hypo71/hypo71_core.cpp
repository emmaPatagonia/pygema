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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory.h>
#include <cmath>
#include <ctime>

#include <unistd.h>

#include <pdas.h>
#include <lsd.h>
#include "loma.h"
#include "utl_time.h"
#include "hypo71_core.h"
#include <hypo71.h>
#include "constants.h"

// external C routine
extern "C" int utl_3d_dist(double, double, double, double, double, double, double *, double *, double *, double *, int);

#define MAX_P_RES        2.0
#define MAX_S_RES        3.0
#define MAX_RES_OFF     40.0
#define MAX_SPICK_DIFF   100
#define MAX_CORR_LENGTH 40.0
#define DEPTH_START     10.0
#define DEPTH_STEP      10.0
#define MAX_DEPTH_TRY   12 

/*----------------------------------------*/

static void io_strip_blanks(char *s)
{
    int i;
    int j;

    i = j = 0;
    while (s[i] != '\0') {
    if (s[i] != ' ') {
        s[j] = s[i];
        j++;
    }
    i++;
    }
    s[j] = '\0';
}

/*----------------------------------------*/

static float do_hypo71_location(AEvent *aev, float search_depth, int final)
{
    int     i;
//    int     j;
    int     deg_lat;
    int     deg_lon;
    float   min_lat;
    float   min_lon;
    char    c_lat;
    char    c_lon;
    float   delta;
    TIME    phase_time;
    TIME    new_time;
    char    line[256];
    char    line_1[256];
    char    line_2[256];
    char    line_3[256];
    char    line_4[256];
    char    temp_str[16];
    char    sta_name[16];
    char    c;
    FILE    *fp_hypo;
    FILE    *fp;
    int     lat1;
    float   lat2;
    int     lon1;
    float   lon2;
    float   new_lon;
    float   new_lat;
    float   new_depth;
    float   new_rms;
    float   new_gap;
    float   new_lat_err;
    float   new_lon_err;
    float   new_depth_err;
    int     num_p;
    int     num_s;
    char    p_id[10];
    char    s_id[10];
    float   p_res;
    float   p_weight;
    float   s_res;
    float   s_weight;
    float   s_sec;
    float   delay;
    float   incident;
    double  hdist;
    double  edist;
    double  az;
    double  baz;
    int	    yr2;

    fp_hypo = fopen("hypo71.inp","w");
    if(fp_hypo == NULL)
    {
        fprintf(stderr,"Could not open temporary file hypo71.inp\n");
        exit(6);
    }
    fprintf(fp_hypo,"HEAD                     HYPO71PC FOR AUTO GIANT\n");

    // Now we echo the reset.hdr file to the input file
    fp = fopen("reset.hdr","r");
    if(fp == NULL)
    {
        fprintf(stderr,"Could not open reset.hdr file for reading\n");
        exit(7);
    }
    while(fgets(line, 256, fp) != NULL)
        fputs(line,fp_hypo);
    fputs("\n",fp_hypo);
    fclose(fp);


    // now we write out the stations
    for(i=0; i < aev->no_traces; i++)
    {
        deg_lat = (int)floor(fabs(aev->tr_array[i].lat));
        min_lat = 60.0 * (fabs(aev->tr_array[i].lat) - (float)deg_lat);
        deg_lon = (int)floor(fabs(aev->tr_array[i].lon));
        min_lon = 60.0 * (fabs(aev->tr_array[i].lon) - (float)deg_lon);
        if(aev->tr_array[i].lat < 0.0) c_lat = 'S'; else c_lat ='N';
        if(aev->tr_array[i].lon < 0.0) c_lon = 'W'; else c_lon ='E';
        // here we can insert the correction level
        delta = 0.0; 

        fprintf(fp_hypo,"  %4s%02d%05.2f%c%03d%05.2f%c%4d %05.2f\n", aev->tr_array[i].stat_code,
                                                                     deg_lat, min_lat, c_lat,
                                                                     deg_lon, min_lon, c_lon,
                                                                     //(int)(aev->tr_array[i].elav*1000.0),
                                                                     (int)(aev->tr_array[i].elav),
                                                                     delta);
    }

    // Now we echo the velmod.hdr file to the input file
    fp = fopen("velmod.hdr","r");
    if(fp == NULL)
    {
        fprintf(stderr,"Could not open velmod.hdr file for reading\n");
        exit(7);
    }
    fputs("\n",fp_hypo);
    while(fgets(line, 256, fp) != NULL)
        fputs(line,fp_hypo);
    fclose(fp);

    // Now we echo the control.hdr file to the input file
    fp = fopen("control.hdr","r");
    if(fp == NULL)
    {
        fprintf(stderr,"Could not open control.hdr file for reading\n");
        exit(7);
    }
    fputs("\n",fp_hypo);
    while(fgets(line, 256, fp) != NULL)
    {
        sprintf(temp_str,"%5.1f", search_depth);
        line[1] = temp_str[0];
        line[2] = temp_str[1];
        line[3] = temp_str[2];
        fputs(line,fp_hypo);
    }
    fclose(fp);

    // now we write out the phases
    for(i=0; i < aev->no_traces; i++)
    {
        if(aev->tr_array[i].use_location)
        {
            phase_time = do2date(aev->tr_array[i].ppick_time);
            if(aev->tr_array[i].spick_time <= 0 || aev->tr_array[i].use_spick == 0)
            {
                s_sec = -1.0;
            }
            else
            {
                s_sec = phase_time.sec + (float)(aev->tr_array[i].spick_time - aev->tr_array[i].ppick_time);
                if(s_sec >= 1000.0)
                    s_sec = -1.;
            }
   
	    if(phase_time.yr >= 2000)
		yr2=phase_time.yr-2000;
	    else
		yr2=phase_time.yr-1900;

            if(s_sec <= 0)
            {
                fprintf(fp_hypo,"%4s%4s %02d%02d%02d%02d%02d%5.2f\n", aev->tr_array[i].stat_code,
                                                                       aev->tr_array[i].pphase_code,
                                                                       yr2,
                                                                       phase_time.mo,
                                                                       phase_time.day,
                                                                       phase_time.hr,
                                                                       phase_time.mn,
                                                                       phase_time.sec);
            }
            else
            {
                fprintf(fp_hypo,"%4s%4s %02d%02d%02d%02d%02d%5.2f      %6.2f%4s\n", aev->tr_array[i].stat_code,
                                                                                     aev->tr_array[i].pphase_code,
                                                                                     yr2,
                                                                                     phase_time.mo,
                                                                                     phase_time.day,
                                                                                     phase_time.hr,
                                                                                     phase_time.mn,
                                                                                     phase_time.sec,
                                                                                     s_sec,
                                                                                     aev->tr_array[i].sphase_code);
            }
        }
    }

    // Now we echo the instruction.hdr file to the input file
    fp = fopen("instruction.hdr","r");
    if(fp == NULL)
    {
        fprintf(stderr,"Could not open instruction.hdr file for reading\n");
        exit(7);
    }
    
    while(fgets(line, 256, fp) != NULL)
        fputs(line,fp_hypo);
    fclose(fp);

    fclose(fp_hypo);

    // Now we can start the program
    fp = fopen("hypo71_input","w");
    if(fp == NULL)
    {
        fprintf(stderr,"Could not open hypo71_input file for writing\n");
        exit(7);
    }
    fprintf(fp,"hypo71.inp\nhypo71.prt\nhypo71.pun\n");
    fclose(fp);
    unlink("hypo71_output");
    system("./hypo71pc < hypo71_input > hypo71_output");

    // hopefully the program perfromed well so that we can read in the locations
    fp_hypo = fopen("hypo71.prt","r");
    if(fp_hypo == NULL)
    {
        fprintf(stderr,"Could not locate this event.\n");
        return -1.0;
    }
    
    while(fgets(line, 256, fp_hypo) != NULL && strncmp(line,"  DATE    ORIGIN    LAT",23))
    {
        strncpy(line_4, line_3, 256);
        strncpy(line_3, line_2, 256);
        strncpy(line_2, line_1, 256);
        strncpy(line_1, line  , 256);
    }
    
    if(feof(fp_hypo))
    {
        //fprintf(stderr,"Could not find location for this event.\n");
        fclose(fp_hypo);
        return -1.0;
    }
    
    c_lat = line[24];
    c_lon = line[34];
    fgets(line, 256, fp_hypo);
    if (line[8] == ' ') line[8] = '0';
    if (line[1] == ' ') line[1] = '0';
    sscanf(line," %2d%2d%2d %2d%2d %f %2d%c%f %3d%c%f %f", &new_time.yr,&new_time.mo,&new_time.day,
                                                           &new_time.hr,&new_time.mn,&new_time.sec,
                                                           &lat1,&c,&lat2,&lon1,&c,&lon2,&new_depth);
    new_lat = (float)lat1 + lat2/60.0;
    new_lon = (float)lon1 + lon2/60.0;
    if (c_lat == 'S') new_lat *= -1.0;
    if (c_lon == 'W') new_lon *= -1.0;
    new_depth *= -1.0;
    strncpy(temp_str,&(line[ 58]),3); temp_str[3] = '\0';  new_gap   = atoi(temp_str);
    strncpy(temp_str,&(line[ 63]),5); temp_str[5] = '\0';  new_rms   = atof(temp_str);
    
    // also read the errors
    strncpy(temp_str,&(line_3[ 95]),6); temp_str[6] = '\0';  new_lat_err   = atof(temp_str);
    strncpy(temp_str,&(line_3[101]),6); temp_str[6] = '\0';  new_lon_err   = atof(temp_str);
    strncpy(temp_str,&(line_3[107]),5); temp_str[6] = '\0';  new_depth_err = atof(temp_str);
    
    if (final) {
        // now we have to fill the complete solution struct and do the boring reading of the
        // phases, so that we can store eveything in the database.
        aev->has_solution = 1;
	      aev->sol.best_solution = 1;
        aev->sol.orig_time     = base_diff(new_time);
        aev->sol.event_lon     = new_lon;
        aev->sol.event_lat     = new_lat;
        aev->sol.event_depth   = new_depth;
        aev->sol.rms_residual  = new_rms;
        aev->sol.gap           = new_gap;
        aev->sol.lon_error     = new_lon_err;
        aev->sol.lat_error     = new_lat_err;
        aev->sol.depth_error   = new_depth_err;
        strcpy(aev->sol.hypo_prog,"HYPO71-AUTO");
        strcpy(aev->sol.vel_model,"velmod.dat");

	      num_p = 0;
	      num_s = 0;

        // skip the header lines

        fgets(line, 256, fp_hypo);
        fgets(line, 256, fp_hypo);
        fgets(line, 256, fp_hypo);

        // now we can store the phases

        while (!feof(fp_hypo) && (strlen(line) > 1) && (line[0] != '1') && (line[1] != '='))
        {
            if(line[1] != ' ')
            {
                strncpy(sta_name,&(line[  1]),4); sta_name[4] = '\0';
            }
            else
            {
                strncpy(sta_name,&(line[  2]),3); sta_name[3] = '\0';
            }
            strncpy(temp_str,&(line[ 16]),3); temp_str[3] = '\0'; incident = atoi(temp_str);
            strncpy(p_id,    &(line[ 20]),4); p_id[4] = '\0'    ; io_strip_blanks(p_id);
            strncpy(temp_str,&(line[ 47]),6); temp_str[6] = '\0'; delay = atof(temp_str);
            strncpy(temp_str,&(line[ 53]),6); temp_str[6] = '\0'; p_res = atof(temp_str);
            strncpy(temp_str,&(line[ 60]),5); temp_str[5] = '\0'; p_weight = atof(temp_str);
            strncpy(s_id,    &(line[ 99]),4); s_id[4] = '\0'    ; io_strip_blanks(s_id);
            strncpy(temp_str,&(line[115]),6); temp_str[6] = '\0'; s_res = atof(temp_str);
            strncpy(temp_str,&(line[122]),5); temp_str[5] = '\0'; s_weight = atof(temp_str);

            //Now we loop over all traces to find the right station again
            
            for(i=0; i < aev->no_traces; i++)
            {
                if(aev->tr_array[i].use_location)
                    if(!strcmp(aev->tr_array[i].stat_code, sta_name))
                    {
			                  num_p++;
                        utl_3d_dist(new_lat, new_lon, new_depth, 
                                    aev->tr_array[i].lat,aev->tr_array[i].lon,
                                    aev->tr_array[i].elav,
                                    &hdist,&edist,&az,&baz,DGDGKM);
                        aev->tr_array[i].pcomp = (Compu_phase *)calloc(1, sizeof(Compu_phase));
                        strcpy(aev->tr_array[i].pcomp->phase_name,p_id);
                        aev->tr_array[i].pcomp->residual      = p_res;
                        aev->tr_array[i].pcomp->used_weight   = p_weight;
                        aev->tr_array[i].pcomp->used_delay    = delay;
                        aev->tr_array[i].pcomp->incident      = incident;
                        aev->tr_array[i].pcomp->azimuth       = (float)az;
                        aev->tr_array[i].pcomp->backazimuth   = (float)baz;
                        aev->tr_array[i].pcomp->epi_distance  = (float)edist;
                        aev->tr_array[i].pcomp->hypo_distance = (float)hdist;
                        if(strlen(s_id) > 0)
                        {
			                      num_s++;
                            aev->tr_array[i].scomp = (Compu_phase *)calloc(1, sizeof(Compu_phase));
                            strcpy(aev->tr_array[i].scomp->phase_name,s_id);
                            aev->tr_array[i].scomp->residual      = s_res;
                            aev->tr_array[i].scomp->used_weight   = s_weight;
                            aev->tr_array[i].scomp->used_delay    = delay;
                            aev->tr_array[i].scomp->incident      = incident;
                            aev->tr_array[i].scomp->azimuth       = az;
                            aev->tr_array[i].scomp->backazimuth   = baz;
                            aev->tr_array[i].scomp->epi_distance  = edist;
                            aev->tr_array[i].scomp->hypo_distance = hdist;
                        }
                        break;
                    }
            }
            fgets(line, 256, fp_hypo);
        }
        aev->sol.num_of_p  = num_p;
        aev->sol.num_of_s  = num_s;
/*
        fprintf(stderr,"Location: %7.4f %7.4f %6.2f %6.3f %5.1f %.2f\n", new_lat,new_lon,new_depth, 
                                                                         new_rms, new_gap, base_diff(new_time));
        fprintf(stderr,"Error: %7.2f %7.2f %7.2f\n", new_lat_err, new_lon_err, new_depth_err);
	      fprintf(stderr,"Used phases: P: %3d  S: %3d\n", num_p, num_s);
*/
    }
    fclose(fp_hypo);
    
    return(new_rms);
}

//----------------------------------------

int do_best_location(AEvent *aev)
{
  int   i;
  float depth, res, new_res, best_depth;

  res = 100000.0;

  for (i=0, depth = constants.depthStart; i < constants.maxDepthTry; i++, depth += constants.depthStep) {
    new_res = do_hypo71_location(aev, depth, 0);
    if (new_res >= 0.0 && new_res < res)
    {
      res = new_res;
      best_depth = depth;
    }
  }
  new_res = do_hypo71_location(aev, best_depth, 1);
  
  return 0;
}
