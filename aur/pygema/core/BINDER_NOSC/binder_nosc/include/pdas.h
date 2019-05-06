/* 

   PDAS_UTILITIES Include Datei

   Dr.G.Asch	asch@gfz-potsdam.de 		31.Aug.94
   double preamp_gain				19.Jan.95
   leicht ueberarbeitet:
   typedefs und _PDAS_UTILITIES_H eingefuehrt   06.Feb.95
   PDAS_HDR.faktor  fuer Fremdformate 		14.Feb.96
   set_default entfernt, PDAS_EXP gestrafft				    "
   NSAMP und NBYTE: Uebergabe des Datentyps [m]	    "

   Das DOS gebundene Datenformat mit der Absicht verlassen,
   abwaertskompatibel zu bleiben,               04.Aug.99  Mexico 
   was in einem Punkt nicht gelang:
   ph.serial_nr musste von INT auf CHR[5] umgestellt werden.

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

#ifndef  _PDAS_UTILITIES_H
#define  _PDAS_UTILITIES_H

#define MAXHEAD_LEN 20    /* max. Anzahl der Headerzeilen im Datenfile  */

#define NSAMP(n,m)		(((m)==3) ? (n)>>2 : (n)>>1 )
#define NBYTE(n,m)		(((m)==3) ? (n)<<2 : (n)<<1 )

typedef struct PDAS_HDR   /**********************************************/
{                         /* Auswertung des Headers des PDAS Datenfiles */
   char   *dataset;       /*x Name of Dataset				*/
   short  newname;        /*x 0		PDAS file name conventions	*/
   		          /*  1	        long file names allowed	        */
			  /*  2		split into subdirectories       */
   char   win_type;       /*  Window Type       [ P,S,E,C ]            	*/
   short  channel;        /*  Channel Number    [ 0 ... 6 ]		*/
   char   channel_type[4];/*x Channel Type      [ Z,N,E,M,A....]	*/ 
   char   recorder[5];    /*x Recorder Type				
                              PDAS
			      SAM_	(Güralp)
			      QUAN	(Quanterra)
			      ORIO	(Orion Nanometrics)
			      REFT	(Reftek)			*/   
   char   serial_nr[5];   /*x Serial Number, Instrument ID		*/
   short  record_nr;      /*  Number of Recording Window at current day	*/
   short  doy;            /*  Day of the Year				*/
   short  file_typ;       /*  Integer [1], Gainranged [2], Long [3]	*/
   long   num_samples;    /*  Total number of Samples in File		*/
   double dt;             /*  Timeintervall between Samples		*/
   time_t record_time;    /*  Start Time since Jan.1 1980 in Seconds	*/
   short  msec;           /*  plus Milliseconds				*/
   char   *comment;	  /*  Kommentarzeile im PDAS Header           	*/
   double faktor;	  /*  Skalierungsfaktor fuer Fremdformate	*/

} PDASHDR, *PPDASHDR;

int   read_PDASheader      (struct PDAS_HDR *, FILE *);

int   convert_PDAS         (void *, long *, int, int);

int   fconvert_PDAS        (void *, float *, int, float, int);

int   write_PDASheader     (struct PDAS_HDR *, FILE *);

int   set_GMT              (void);

FILE *open_output_file	   (char *, PDASHDR *);
void close_output_file     (FILE *, char *, PDASHDR *);
char *create_dataset_name  (PDASHDR *);

#endif
