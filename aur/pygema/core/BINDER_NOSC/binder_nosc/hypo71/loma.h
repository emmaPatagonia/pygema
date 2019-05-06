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

/* Raima Data Manager Version 3.21A */

/* database nloma record/key structure declarations */

typedef struct station {
   char  stat_code[6];
   int  chan_code;
   int  stat_type;
   float  stat_lon;
   float  stat_lat;
   float  stat_elav;
   float  fcommon[3];
} Station;
typedef struct calibration {
   double  start_time;
   double  end_time;
   char  cal_name[40];
   float  fcommon[3];
} Calibration;
typedef struct seis_event {
   int  event_no;
   int  cluster_id;
   float  fcommon[3];
} Seis_event;
typedef struct trace_rec {
   int  record_no;
   double  start_time;
   double  end_time;
   float  t_samp;
   int  on_line;
   char  file_name[40];
   int	comp_spec;
   float  fcommon[3];
} Trace_rec;
typedef struct spectrum {
   int  has_spec;
   int  spec_start;
   int  spec_end;
   int  noise_start;
   int  noise_end;
   int  nfft;
   int  type;
   int  taper_type;
   float  taper_frac;
   float  noise_frac;
   float  rad_corr;
   float  rho_corr;
   float  vel_corr;
   float  surf_corr;
   float  hypo_dist;
   float  fcorn;
   float  moment;
   float  src_exp;
   float  qu0;
   float  qalpha;
   float  res_freq;
   float  res_ampl;
   float  delta_t;
   float  smooth_freq;
   float  plateau;
   float  freq1;
   float  freq2;
   int  max_it;
   int  iteration;
   float  residual;
   float  fcommon[3];
} Spectrum;
typedef struct scal_wavelet {
   char  phase_name[20];
   double  onset_time;
   int  onset_delta;
   double  onset_range[2];
   double  first_crossing;
   float  rise_time;
   float  max_ampl;
   double  max_time;
   float  min_ampl;
   double  min_time;
   float  period;
   float  moment;
   double  t_param;
   float  b_param;
   float  c_param;
   int  has_time_end;
   double  time_end;
   float  azimuth;
   float  incident;
   int  rotation_type;
   float  fcommon[3];
} Scal_wavelet;
typedef struct vec_phase {
   char  vec_name[20];
   double  onset_time1;
   double  onset_range1[2];
   double  onset_time2;
   double  onset_range2[2];
   float  max_vector1[3];
   double  max_time1;
   float  min_vector1[3];
   double  min_time1;
   float  max_vector2[3];
   double  max_time2;
   float  min_vector2[3];
   double  min_time2;
   float  fcommon[3];
} Vec_phase;
typedef struct solution {
   int  event_no;
   int  solution_id;
   double  orig_time;
   int  best_solution;
   float  event_lon;
   float  event_lat;
   float  event_depth;
   char  hypo_prog[20];
   char  vel_model[20];
   float  rms_residual;
   float  gap;
   float  lat_error;
   float  lon_error;
   float  depth_error;
   int  num_of_p;
   int  num_of_s;
   float  fcommon[3];
} Solution;
typedef struct focal_mech {
   float  strike_1;
   float  dip_1;
   float  rake_1;
   float  strike_2;
   float  dip_2;
   float  rake_2;
   char  progr_name[40];
   float  fcommon[3];
} Focal_mech;
typedef struct compu_phase {
   char  phase_name[40];
   float  residual;
   float  used_weight;
   float  used_delay;
   float  azimuth;
   float  backazimuth;
   float  incident;
   float  epi_distance;
   float  hypo_distance;
   float  fcommon[3];
} Compu_phase;
typedef struct source_spec {
   float  plateau;
   float  f_corn;
   float  q_value;
   float  src_exp;
   float  seis_moment;
   float  radius;
   float  s_drop;
   float  res_ampl;
   float  res_freq;
   float  res_type;
   float  qual_fac;
   float  fcommon[3];
} Source_spec;
typedef struct station_key {
   char  stat_code[6];
   int  chan_code;
} Station_key;
typedef struct solution_key {
   double  orig_time;
   int  best_solution;
} Solution_key;

/* record, field and set table entry definitions */

/* File Id Constants */

/* Record Name Constants */
#define STATION 10000
#define CALIBRATION 10001
#define SEIS_EVENT 10002
#define TRACE_REC 10003
#define SPECTRUM 10004
#define SCAL_WAVELET 10005
#define VEC_PHASE 10006
#define SOLUTION 10007
#define FOCAL_MECH 10008
#define COMPU_PHASE 10009
#define SOURCE_SPEC 10010

/* Field Name Constants */
#define STATION_STAT_CODE 0L
#define STATION_CHAN_CODE 1L
#define STATION_STAT_TYPE 2L
#define STATION_STAT_LON 3L
#define STATION_STAT_LAT 4L
#define STATION_STAT_ELAV 5L
#define STATION_FCOMMON 6L
#define STATION_STATION_KEY 7L
#define CALIBRATION_START_TIME 1000L
#define CALIBRATION_END_TIME 1001L
#define CALIBRATION_CAL_NAME 1002L
#define CALIBRATION_FCOMMON 1003L
#define SEIS_EVENT_EVENT_NO 2000L
#define SEIS_EVENT_CLUSTER_ID 2001L
#define SEIS_EVENT_FCOMMON 2002L
#define TRACE_REC_RECORD_NO 3000L
#define TRACE_REC_START_TIME 3001L
#define TRACE_REC_END_TIME 3002L
#define TRACE_REC_T_SAMP 3003L
#define TRACE_REC_ON_LINE 3004L
#define TRACE_REC_FILE_NAME 3005L
#define TRACE_REC_COMP_SPEC 3006L
#define TRACE_REC_FCOMMON 3007L
#define SPECTRUM_HAS_SPEC 4000L
#define SPECTRUM_SPEC_START 4001L
#define SPECTRUM_SPEC_END 4002L
#define SPECTRUM_NOISE_START 4003L
#define SPECTRUM_NOISE_END 4004L
#define SPECTRUM_NFFT 4005L
#define SPECTRUM_TYPE 4006L
#define SPECTRUM_TAPER_TYPE 4007L
#define SPECTRUM_TAPER_FRAC 4008L
#define SPECTRUM_NOISE_FRAC 4009L
#define SPECTRUM_RAD_CORR 4010L
#define SPECTRUM_RHO_CORR 4011L
#define SPECTRUM_VEL_CORR 4012L
#define SPECTRUM_SURF_CORR 4013L
#define SPECTRUM_HYPO_DIST 4014L
#define SPECTRUM_FCORN 4015L
#define SPECTRUM_MOMENT 4016L
#define SPECTRUM_SRC_EXP 4017L
#define SPECTRUM_QU0 4018L
#define SPECTRUM_QALPHA 4019L
#define SPECTRUM_RES_FREQ 4020L
#define SPECTRUM_RES_AMPL 4021L
#define SPECTRUM_DELTA_T 4022L
#define SPECTRUM_SMOOTH_FREQ 4023L
#define SPECTRUM_PLATEAU 4024L
#define SPECTRUM_FREQ1 4025L
#define SPECTRUM_FREQ2 4026L
#define SPECTRUM_MAX_IT 4027L
#define SPECTRUM_ITERATION 4028L
#define SPECTRUM_RESIDUAL 4029L
#define SPECTRUM_FCOMMON 4030L
#define SCAL_WAVELET_PHASE_NAME 5000L
#define SCAL_WAVELET_ONSET_TIME 5001L
#define SCAL_WAVELET_ONSET_DELTA 5002L
#define SCAL_WAVELET_ONSET_RANGE 5003L
#define SCAL_WAVELET_FIRST_CROSSING 5004L
#define SCAL_WAVELET_RISE_TIME 5005L
#define SCAL_WAVELET_MAX_AMPL 5006L
#define SCAL_WAVELET_MAX_TIME 5007L
#define SCAL_WAVELET_MIN_AMPL 5008L
#define SCAL_WAVELET_MIN_TIME 5009L
#define SCAL_WAVELET_PERIOD 5010L
#define SCAL_WAVELET_MOMENT 5011L
#define SCAL_WAVELET_T_PARAM 5012L
#define SCAL_WAVELET_B_PARAM 5013L
#define SCAL_WAVELET_C_PARAM 5014L
#define SCAL_WAVELET_HAS_TIME_END 5015L
#define SCAL_WAVELET_TIME_END 5016L
#define SCAL_WAVELET_AZIMUTH 5017L
#define SCAL_WAVELET_INCIDENT 5018L
#define SCAL_WAVELET_ROTATION_TYPE 5019L
#define SCAL_WAVELET_FCOMMON 5020L
#define VEC_PHASE_VEC_NAME 6000L
#define VEC_PHASE_ONSET_TIME1 6001L
#define VEC_PHASE_ONSET_RANGE1 6002L
#define VEC_PHASE_ONSET_TIME2 6003L
#define VEC_PHASE_ONSET_RANGE2 6004L
#define VEC_PHASE_MAX_VECTOR1 6005L
#define VEC_PHASE_MAX_TIME1 6006L
#define VEC_PHASE_MIN_VECTOR1 6007L
#define VEC_PHASE_MIN_TIME1 6008L
#define VEC_PHASE_MAX_VECTOR2 6009L
#define VEC_PHASE_MAX_TIME2 6010L
#define VEC_PHASE_MIN_VECTOR2 6011L
#define VEC_PHASE_MIN_TIME2 6012L
#define VEC_PHASE_FCOMMON 6013L
#define SOLUTION_EVENT_NO 7000L
#define SOLUTION_SOLUTION_ID 7001L
#define SOLUTION_ORIG_TIME 7002L
#define SOLUTION_BEST_SOLUTION 7003L
#define SOLUTION_EVENT_LON 7004L
#define SOLUTION_EVENT_LAT 7005L
#define SOLUTION_EVENT_DEPTH 7006L
#define SOLUTION_HYPO_PROG 7007L
#define SOLUTION_VEL_MODEL 7008L
#define SOLUTION_RMS_RESIDUAL 7009L
#define SOLUTION_GAP 7010L
#define SOLUTION_LAT_ERROR 7011L
#define SOLUTION_LON_ERROR 7012L
#define SOLUTION_DEPTH_ERROR 7013L
#define SOLUTION_NUM_OF_P 7014L
#define SOLUTION_NUM_OF_S 7015L
#define SOLUTION_FCOMMON 7016L
#define SOLUTION_SOLUTION_KEY 7017L
#define FOCAL_MECH_STRIKE_1 8000L
#define FOCAL_MECH_DIP_1 8001L
#define FOCAL_MECH_RAKE_1 8002L
#define FOCAL_MECH_STRIKE_2 8003L
#define FOCAL_MECH_DIP_2 8004L
#define FOCAL_MECH_RAKE_2 8005L
#define FOCAL_MECH_PROGR_NAME 8006L
#define FOCAL_MECH_FCOMMON 8007L
#define COMPU_PHASE_PHASE_NAME 9000L
#define COMPU_PHASE_RESIDUAL 9001L
#define COMPU_PHASE_USED_WEIGHT 9002L
#define COMPU_PHASE_USED_DELAY 9003L
#define COMPU_PHASE_AZIMUTH 9004L
#define COMPU_PHASE_BACKAZIMUTH 9005L
#define COMPU_PHASE_INCIDENT 9006L
#define COMPU_PHASE_EPI_DISTANCE 9007L
#define COMPU_PHASE_HYPO_DISTANCE 9008L
#define COMPU_PHASE_FCOMMON 9009L
#define SOURCE_SPEC_PLATEAU 10000L
#define SOURCE_SPEC_F_CORN 10001L
#define SOURCE_SPEC_Q_VALUE 10002L
#define SOURCE_SPEC_SRC_EXP 10003L
#define SOURCE_SPEC_SEIS_MOMENT 10004L
#define SOURCE_SPEC_RADIUS 10005L
#define SOURCE_SPEC_S_DROP 10006L
#define SOURCE_SPEC_RES_AMPL 10007L
#define SOURCE_SPEC_RES_FREQ 10008L
#define SOURCE_SPEC_RES_TYPE 10009L
#define SOURCE_SPEC_QUAL_FAC 10010L
#define SOURCE_SPEC_FCOMMON 10011L

/* Field Sizes */
#define SIZEOF_STATION_STAT_CODE 6
#define SIZEOF_STATION_CHAN_CODE 4
#define SIZEOF_STATION_STAT_TYPE 4
#define SIZEOF_STATION_STAT_LON 4
#define SIZEOF_STATION_STAT_LAT 4
#define SIZEOF_STATION_STAT_ELAV 4
#define SIZEOF_STATION_FCOMMON 12
#define SIZEOF_CALIBRATION_START_TIME 8
#define SIZEOF_CALIBRATION_END_TIME 8
#define SIZEOF_CALIBRATION_CAL_NAME 40
#define SIZEOF_CALIBRATION_FCOMMON 12
#define SIZEOF_SEIS_EVENT_CL_ID 4
#define SIZEOF_SEIS_EVENT_CLUSTER_ID 4
#define SIZEOF_SEIS_EVENT_FCOMMON 12
#define SIZEOF_TRACE_REC_RECORD_NO 4
#define SIZEOF_TRACE_REC_START_TIME 8
#define SIZEOF_TRACE_REC_END_TIME 8
#define SIZEOF_TRACE_REC_T_SAMP 4
#define SIZEOF_TRACE_REC_ON_LINE 4
#define SIZEOF_TRACE_REC_FILE_NAME 40
#define SIZEOF_TRACE_REC_COMP_SPEC 4
#define SIZEOF_TRACE_REC_FCOMMON 12
#define SIZEOF_SPECTRUM_HAS_SPEC 4
#define SIZEOF_SPECTRUM_SPEC_START 4
#define SIZEOF_SPECTRUM_SPEC_END 4
#define SIZEOF_SPECTRUM_NOISE_START 4
#define SIZEOF_SPECTRUM_NOISE_END 4
#define SIZEOF_SPECTRUM_NFFT 4
#define SIZEOF_SPECTRUM_TYPE 4
#define SIZEOF_SPECTRUM_TAPER_TYPE 4
#define SIZEOF_SPECTRUM_TAPER_FRAC 4
#define SIZEOF_SPECTRUM_NOISE_FRAC 4
#define SIZEOF_SPECTRUM_RAD_CORR 4
#define SIZEOF_SPECTRUM_RHO_CORR 4
#define SIZEOF_SPECTRUM_VEL_CORR 4
#define SIZEOF_SPECTRUM_SURF_CORR 4
#define SIZEOF_SPECTRUM_HYPO_DIST 4
#define SIZEOF_SPECTRUM_FCORN 4
#define SIZEOF_SPECTRUM_MOMENT 4
#define SIZEOF_SPECTRUM_SRC_EXP 4
#define SIZEOF_SPECTRUM_QU0 4
#define SIZEOF_SPECTRUM_QALPHA 4
#define SIZEOF_SPECTRUM_RES_FREQ 4
#define SIZEOF_SPECTRUM_RES_AMPL 4
#define SIZEOF_SPECTRUM_DELTA_T 4
#define SIZEOF_SPECTRUM_SMOOTH_FREQ 4
#define SIZEOF_SPECTRUM_PLATEAU 4
#define SIZEOF_SPECTRUM_FREQ1 4
#define SIZEOF_SPECTRUM_FREQ2 4
#define SIZEOF_SPECTRUM_MAX_IT 4
#define SIZEOF_SPECTRUM_ITERATION 4
#define SIZEOF_SPECTRUM_RESIDUAL 4
#define SIZEOF_SPECTRUM_FCOMMON 12
#define SIZEOF_SCAL_WAVELET_PHASE_NAME 20
#define SIZEOF_SCAL_WAVELET_ONSET_TIME 8
#define SIZEOF_SCAL_WAVELET_ONSET_DELTA 4
#define SIZEOF_SCAL_WAVELET_ONSET_RANGE 16
#define SIZEOF_SCAL_WAVELET_FIRST_CROSSING 8
#define SIZEOF_SCAL_WAVELET_RISE_TIME 4
#define SIZEOF_SCAL_WAVELET_MAX_AMPL 4
#define SIZEOF_SCAL_WAVELET_MAX_TIME 8
#define SIZEOF_SCAL_WAVELET_MIN_AMPL 4
#define SIZEOF_SCAL_WAVELET_MIN_TIME 8
#define SIZEOF_SCAL_WAVELET_PERIOD 4
#define SIZEOF_SCAL_WAVELET_MOMENT 4
#define SIZEOF_SCAL_WAVELET_T_PARAM 8
#define SIZEOF_SCAL_WAVELET_B_PARAM 4
#define SIZEOF_SCAL_WAVELET_C_PARAM 4
#define SIZEOF_SCAL_WAVELET_HAS_TIME_END 4
#define SIZEOF_SCAL_WAVELET_TIME_END 8
#define SIZEOF_SCAL_WAVELET_AZIMUTH 4
#define SIZEOF_SCAL_WAVELET_INCIDENT 4
#define SIZEOF_SCAL_WAVELET_ROTATION_TYPE 4
#define SIZEOF_SCAL_WAVELET_FCOMMON 12
#define SIZEOF_VEC_PHASE_VEC_NAME 20
#define SIZEOF_VEC_PHASE_ONSET_TIME1 8
#define SIZEOF_VEC_PHASE_ONSET_RANGE1 16
#define SIZEOF_VEC_PHASE_ONSET_TIME2 8
#define SIZEOF_VEC_PHASE_ONSET_RANGE2 16
#define SIZEOF_VEC_PHASE_MAX_VECTOR1 12
#define SIZEOF_VEC_PHASE_MAX_TIME1 8
#define SIZEOF_VEC_PHASE_MIN_VECTOR1 12
#define SIZEOF_VEC_PHASE_MIN_TIME1 8
#define SIZEOF_VEC_PHASE_MAX_VECTOR2 12
#define SIZEOF_VEC_PHASE_MAX_TIME2 8
#define SIZEOF_VEC_PHASE_MIN_VECTOR2 12
#define SIZEOF_VEC_PHASE_MIN_TIME2 8
#define SIZEOF_VEC_PHASE_FCOMMON 12
#define SIZEOF_SOLUTION_EVENT_NO 4
#define SIZEOF_SOLUTION_SOLUTION_ID 4
#define SIZEOF_SOLUTION_ORIG_TIME 8
#define SIZEOF_SOLUTION_BEST_SOLUTION 4
#define SIZEOF_SOLUTION_EVENT_LON 4
#define SIZEOF_SOLUTION_EVENT_LAT 4
#define SIZEOF_SOLUTION_EVENT_DEPTH 4
#define SIZEOF_SOLUTION_HYPO_PROG 20
#define SIZEOF_SOLUTION_VEL_MODEL 20
#define SIZEOF_SOLUTION_RMS_RESIDUAL 4
#define SIZEOF_SOLUTION_GAP 4
#define SIZEOF_SOLUTION_LAT_ERROR 4
#define SIZEOF_SOLUTION_LON_ERROR 4
#define SIZEOF_SOLUTION_DEPTH_ERROR 4
#define SIZEOF_SOLUTION_NUM_OF_P 4
#define SIZEOF_SOLUTION_NUM_OF_S 4
#define SIZEOF_SOLUTION_FCOMMON 12
#define SIZEOF_FOCAL_MECH_STRIKE_1 4
#define SIZEOF_FOCAL_MECH_DIP_1 4
#define SIZEOF_FOCAL_MECH_RAKE_1 4
#define SIZEOF_FOCAL_MECH_STRIKE_2 4
#define SIZEOF_FOCAL_MECH_DIP_2 4
#define SIZEOF_FOCAL_MECH_RAKE_2 4
#define SIZEOF_FOCAL_MECH_PROGR_NAME 40
#define SIZEOF_FOCAL_MECH_FCOMMON 12
#define SIZEOF_COMPU_PHASE_PHASE_NAME 40
#define SIZEOF_COMPU_PHASE_RESIDUAL 4
#define SIZEOF_COMPU_PHASE_USED_WEIGHT 4
#define SIZEOF_COMPU_PHASE_USED_DELAY 4
#define SIZEOF_COMPU_PHASE_AZIMUTH 4
#define SIZEOF_COMPU_PHASE_BACKAZIMUTH 4
#define SIZEOF_COMPU_PHASE_INCIDENT 4
#define SIZEOF_COMPU_PHASE_EPI_DISTANCE 4
#define SIZEOF_COMPU_PHASE_HYPO_DISTANCE 4
#define SIZEOF_COMPU_PHASE_FCOMMON 12
#define SIZEOF_SOURCE_SPEC_PLATEAU 4
#define SIZEOF_SOURCE_SPEC_F_CORN 4
#define SIZEOF_SOURCE_SPEC_Q_VALUE 4
#define SIZEOF_SOURCE_SPEC_SRC_EXP 4
#define SIZEOF_SOURCE_SPEC_SEIS_MOMENT 4
#define SIZEOF_SOURCE_SPEC_RADIUS 4
#define SIZEOF_SOURCE_SPEC_S_DROP 4
#define SIZEOF_SOURCE_SPEC_RES_AMPL 4
#define SIZEOF_SOURCE_SPEC_RES_FREQ 4
#define SIZEOF_SOURCE_SPEC_RES_TYPE 4
#define SIZEOF_SOURCE_SPEC_QUAL_FAC 4
#define SIZEOF_SOURCE_SPEC_FCOMMON 12

/* Set Name Constants */
#define STATION_SET 20000
#define CALIBRATION_SET 20001
#define WAVELET_SET 20002
#define SPECTRUM_SET 20003
#define EVENT_TRACE_SET 20004
#define EVENT_HYPO_SET 20005
#define FOCAL_SET 20006
#define COMPU_PHASE_SET 20007
#define WAVE_COMPU_SET 20008
#define HYPO_SPECTRUM_SET 20009
#define VEC_PHASE_SET 20010
#define SOURCE_SPEC_SET 20011
#define FALSE_POL_SET 20012
#define TRUE_POL_SET 20013
/*
 *
*/
#define HAS_MAGNITUDE	100
