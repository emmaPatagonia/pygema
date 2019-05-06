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

#ifndef _HYPO71_CORE_H
#define _HYPO71_CORE_H

#define ISAM_FORMAT    0
#define PDAS_FORMAT  100
#define GSE_FORMAT   200

#define DGDGKM  0
#define KMKMKM  1
#define MMM     2

#define ALL_MODE  1
#define RES_MODE  2

#define DB_ADDR long

typedef struct {
               char    hypo_name[128];
               char    data_path[128];
               float   low_fc;
               float   high_fc;
               int     sections;
               float   tdownmax;
               float   tupevent;
               float   thrshl1;
               float   thrshl2;
               float   preset_len;
               float   pdur;
} Acl;

typedef struct {
               DB_ADDR dba[3];
               int     format;
               int     use_location;
               int     use_spick;
               double  start_time;
               char    stat_code[6];
               int     chan_code;
               float   lat;
               float   lon;
               float   elav;
               double  ppick_time;
               double  spick_time;
               char    pphase_code[10];
               char    sphase_code[10];
               float   dt;
               int     ndat;
               float   *trace;
               float   *x_hor;
               float   *y_hor;
               Scal_wavelet    *pwave;
               Scal_wavelet    *swave;
               Compu_phase     *pcomp;
               Compu_phase     *scomp;
} Trace;

typedef struct {
               int       no_traces;
               Trace     *tr_array;
               int       res_cut;
               int       has_solution;
               Solution  sol;
} AEvent;

extern int do_best_location(AEvent *aev);

#endif /* !_HYPO71_CORE_H */
