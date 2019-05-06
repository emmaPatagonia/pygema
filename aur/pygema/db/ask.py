# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) Diego Gonzalez-Vidal <diegogonzalezvidal@gmail.com>
#
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with This program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

import sys, os, glob, datetime, MySQLdb, time, socket
from obspy.core import UTCDateTime, read, Stream, Trace
from matplotlib.dates import date2num, num2date, DateFormatter
import numpy as np
from pygema.db.credentials import get_credentials_sqldb
from pygema.db.fig2bin import convertToBinaryData

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def select_events_automatic_loc(starttime, endtime, table="LOC"):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()

  status = "automatic"
  if table=="AUTOLOC":
    flag = "SELECT time, longitude, latitude, depth, number_of_stations, gap, rms, status, magnitude FROM AUTOLOC where time BETWEEN '%s' AND '%s' && status='%s'" % ( starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S"), status )
  elif table=="LOC":
    flag = "SELECT time, longitude, latitude, depth, number_of_stations, gap, rms, status, magnitude FROM LOC where time BETWEEN '%s' AND '%s' && status='%s'" % ( starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S"), status )

  cursor.execute(flag)
  res = cursor.fetchall()
  db.close()

  events_list = []
  for row in res:
    time, lon, lat, depth, nstats, gap, rms, status, magnitude = row
    events_list.append( [UTCDateTime(time), lon, lat, depth, magnitude, int(nstats), gap, rms, status] )

  return events_list


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def select_events_confirmed_loc(starttime, endtime, table="LOC"):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()

  status = "confirmed"
  if table=="AUTOLOC":
    flag = "SELECT time, longitude, latitude, depth, number_of_stations, gap, rms, status, magnitude FROM AUTOLOC where time BETWEEN '%s' AND '%s' && status='%s'" % ( starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S"), status )
  elif table=="LOC":
    flag = "SELECT time, longitude, latitude, depth, number_of_stations, gap, rms, status, magnitude FROM LOC where time BETWEEN '%s' AND '%s' && status='%s'" % ( starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S"), status )

  cursor.execute(flag)
  res = cursor.fetchall()
  db.close()

  events_list = []
  for row in res:
    time, lon, lat, depth, nstats, gap, rms, status, magnitude = row
    events_list.append( [UTCDateTime(time), lon, lat, depth, magnitude, int(nstats), gap, rms, status] )

  return events_list


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def select_events_manual_loc(starttime, endtime, table="LOC"):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()

  status = "manual"
  if table=="AUTOLOC":
    flag = "SELECT time, longitude, latitude, depth, number_of_stations, gap, rms, status, magnitude, dx, dy, dz FROM AUTOLOC where time BETWEEN '%s' AND '%s' && status='%s'" % ( starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S"), status )
  elif table=="LOC":
    flag = "SELECT time, longitude, latitude, depth, number_of_stations, gap, rms, status, magnitude, dx, dy, dz FROM LOC where time BETWEEN '%s' AND '%s' && status='%s'" % ( starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S"), status )

  cursor.execute(flag)
  res = cursor.fetchall()
  db.close()

  events_list = []
  for row in res:
    time, lon, lat, depth, nstats, gap, rms, status, magnitude, dx, dy, dz = row
    events_list.append( [UTCDateTime(time), lon, lat, depth, magnitude, dx, dy, dz, status] )

  return events_list


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def select_event_from_database(starttime, endtime, table="LOC"):

  # read automatic, confirmed and manual events
  events_list = []
  events_list_automatic = select_events_automatic_loc( UTCDateTime(1970, 1,1), UTCDateTime().now(), table="LOC")
  events_list_confirmed = select_events_confirmed_loc( UTCDateTime(1970, 1,1), UTCDateTime().now(), table="LOC")
  events_list_manual = select_events_manual_loc( UTCDateTime(1970, 1,1), UTCDateTime().now(), table="LOC")

  if len(events_list_automatic)>0:
    for event in events_list_automatic:
      evtime = event[0]
      evlon = event[1]
      evlat = event[2]
      evdep = event[3]
      evmag = event[4]
      errx = -99
      erry = -99
      errz = -99
      evstatus = event[8]
      events_list.append([evtime, evlon, evlat, evdep, evmag, errx, erry, errz, evstatus])

  if len(events_list_confirmed)>0:
    for event in events_list_confirmed:
      evtime = event[0]
      evlon = event[1]
      evlat = event[2]
      evdep = event[3]
      evmag = event[4]
      errx = -99
      erry = -99
      errz = -99
      evstatus = event[8]
      events_list.append([evtime, evlon, evlat, evdep, evmag, errx, erry, errz, evstatus])

  if len(events_list_manual)>0:
    for event in events_list_manual:
      evtime = event[0]
      evlon = event[1]
      evlat = event[2]
      evdep = event[3]
      evmag = event[4]
      errx = event[5]
      erry = event[6]
      errz = event[7]
      evstatus = event[8]
      events_list.append([evtime, evlon, evlat, evdep, evmag, errx, erry, errz, evstatus])

  events_list = np.array(events_list)
  events_list = events_list[np.argsort(events_list[:, 0])]


  # print events found on the screen
  count = 1; count_list = []
  print("\n\n")
  for event in events_list:
    count_list.append(str(count))
    if event[8] == 'manual':
      pattern = "[%i]   %s   %.4f %.4f   %5.1f km    Ml %3.1f    %4.1f km  %4.1f km  %4.1f km    \x1b[0;32;40m %s \x1b[0m " % (count, event[0].strftime("%Y-%m-%d %H:%M:%S"), event[1], event[2], event[3], event[4], event[5], event[6], event[7], event[8] )
    elif event[8] == 'confirmed':
      pattern = "[%i]   %s   %.4f %.4f   %5.1f km    Ml %3.1f    %4.1f km  %4.1f km  %4.1f km    \x1b[0;33;40m %s \x1b[0m " % (count, event[0].strftime("%Y-%m-%d %H:%M:%S"), event[1], event[2], event[3], event[4], event[5], event[6], event[7], event[8] )
    elif event[8] == 'automatic':
      pattern = "[%i]   %s   %.4f %.4f   %5.1f km    Ml %3.1f    %4.1f km  %4.1f km  %4.1f km    \x1b[0;31;40m %s \x1b[0m " % (count, event[0].strftime("%Y-%m-%d %H:%M:%S"), event[1], event[2], event[3], event[4], event[5], event[6], event[7], event[8] )

    print(pattern)
    count += 1

  print("@pygema  (origin_time, lon, lat, depth, mag, errX, errY, errZ, status)" )

  #print( "[%i] TODOS!!" % (count) )
  flag = input("\n+ Type the seismic event: ")
  while not flag in count_list:
    flag = input("+ Type the seismic event: ")
    if flag in count_list:
      break

  idxev = int(flag)-1
  this_event = events_list[idxev]
  return this_event, idxev, events_list


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def insert_triggers_stalta(stations_list, triggers_list):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  for station, triggers in zip(stations_list, triggers_list):
    for trigger in triggers:
      try:
        flag = "INSERT INTO AUTOTRIGGERS (station, trig_on, trig_off) VALUES ('%s', '%s', '%s')" % ( station, trigger[0].strftime("%Y-%m-%d %H:%M:%S"), trigger[1].strftime("%Y-%m-%d %H:%M:%S") )
        cursor.execute(flag)
        db.commit()
      except:
        continue

  db.close()


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def select_triggers_stalta(station, starttime, endtime):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  flag = "SELECT station, trig_on, trig_on FROM AUTOTRIGGERS  WHERE station='%s' && trig_on BETWEEN '%s' AND '%s'" % ( station, starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S") )
  cursor.execute(flag)
  res = cursor.fetchall()
  db.close()

  triggers_list = []
  for row in res:
    station, on, off = row
    triggers_list.append( [UTCDateTime(on), UTCDateTime(off)] )

  return triggers_list

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def ask_last_triggers_stalta_inserted(N = 10):
  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  flag = "SELECT * FROM AUTOTRIGGERS ORDER BY trig_on DESC LIMIT %i" % ( N )
  cursor.execute(flag)
  res = cursor.fetchall()
  for line in res:
    print( "[%s]  %s    %s " % (line[0],line[1].strftime("%Y-%m-%d %H:%M:%S"),line[2].strftime("%Y-%m-%d %H:%M:%S")) )

  db.close()


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 


def insert_event(evtime, evlon, evlat, evdep, evnstats, evgap, evrms, evmag, status="automatic", table="LOC", figs_dir=None):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  if table=="LOC":
    if figs_dir is not None:
      img_wave = glob.glob(figs_dir+"/*record_section*")[0]
      img_map = glob.glob(figs_dir+"/*map_event*")[0]
      flag = "INSERT INTO LOC (time, longitude, latitude, depth, number_of_stations, gap, rms, magnitude, status, img_wave, img_map) VALUES ('%s', %f, %f, %f, %i, %f, %f, %.1f,'%s','%s','%s')" % (evtime.strftime("%Y-%m-%d %H:%M:%S"), evlon, evlat, evdep, evnstats, evgap, evrms, evmag, status, convertToBinaryData(img_wave), convertToBinaryData(img_map) )
    else:
      flag = "INSERT INTO LOC (time, longitude, latitude, depth, number_of_stations, gap, rms, magnitude, status) VALUES ('%s', %f, %f, %f, %i, %f, %f, %.1f,'%s')" % (evtime.strftime("%Y-%m-%d %H:%M:%S"), evlon, evlat, evdep, evnstats, evgap, evrms, evmag, status)

  cursor.execute(flag)
  db.commit()
  db.close()



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def update_event_status(origin_time, status, table="LOC"):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  if table=="AUTOLOC":
    flag = "UPDATE AUTOLOC SET status='%s' WHERE  time='%s'" % (status, origin_time.strftime("%Y-%m-%d %H:%M:%S") )
  elif table=="LOC":
    flag = "UPDATE LOC SET status='%s' WHERE  time='%s'" % (status, origin_time.strftime("%Y-%m-%d %H:%M:%S") )

  cursor.execute(flag)
  db.commit()
  db.close()


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def update_event_localization(evtime_old, evtime, evlon, evlat, evdepth, evrms, everrx, everry, everrz, evmag, table="LOC"):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  if table=="AUTOLOC":
    flag = "UPDATE  AUTOLOC  SET  time  = '%s',  longitude  = %f,  latitude  = %f,  depth  = %f,  rms  = %.4f,  dx  = %f,  dy  = %f,  dz  = %f,  magnitude  = %.1f WHERE  LOC . time  = '%s'" % (evtime.strftime("%Y-%m-%d %H:%M:%S"), evlon, evlat, evdepth, evrms, everrx, everry, everrz, evmag, evtime_old.strftime("%Y-%m-%d %H:%M:%S") )
  elif table=="LOC":
    flag = "UPDATE  LOC  SET  time  = '%s',  longitude  = %f,  latitude  = %f,  depth  = %f,  rms  = %.4f,  dx  = %f,  dy  = %f,  dz  = %f,  magnitude  = %.1f WHERE  LOC . time  = '%s'" % (evtime.strftime("%Y-%m-%d %H:%M:%S"), evlon, evlat, evdepth, evrms, everrx, everry, everrz, evmag, evtime_old.strftime("%Y-%m-%d %H:%M:%S") )

  cursor.execute(flag)
  db.commit()
  db.close()


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def update_origin_time(evtime_old, evtime_new, table="LOC"):

  host, port, user, password, database = get_credentials_sqldb()
  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  if table=="AUTOLOC":
    flag = "UPDATE  AUTOLOC  SET  time  = '%s' WHERE  LOC . time  = '%s'" % (evtime_new.strftime("%Y-%m-%d %H:%M:%S"), evtime_old.strftime("%Y-%m-%d %H:%M:%S") )
  elif table=="LOC":
    flag = "UPDATE  LOC  SET  time  = '%s'  WHERE  LOC . time  = '%s'" % (evtime_new.strftime("%Y-%m-%d %H:%M:%S"), evtime_old.strftime("%Y-%m-%d %H:%M:%S.%f") )

  cursor.execute(flag)
  db.commit()
  db.close()



"""
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def insert_rsam_warnings(station, rsam_values, rsam_times, rsam_orange_warning, rsam_yellow_warning):

  host, port, user, password, database = get_credentials_sqldb()


  inds_yellow = np.where( (np.array(rsam_values)>=rsam_yellow_warning) & (np.array(rsam_values)<rsam_orange_warning)  )[0]
  inds_orange = np.where(  (np.array(rsam_values)>=rsam_orange_warning)  )[0]
  flags = []

  if len(inds_orange)>0:
    for ind in inds_orange:
      color = "ORANGE"
      comment = "RSAM value of %.1f (RSAM > %.1f)" % ( rsam_values[ind], rsam_orange_warning )
      flag = "INSERT INTO  WARNING (time, color, commentary, station) VALUES ('%s', '%s', '%s', '%s')" % ( (rsam_times[ind]).strftime("%Y-%m-%d %H:%M:%S UTC"), color, comment, station )
      flags.append(flag)


  elif len(inds_yellow)>0:
    for ind in inds_yellow:
      color = "YELLOW"
      comment = "RSAM value of %.1f (%.1f > RSAM > %.1f)" % ( rsam_values[ind], rsam_orange_warning, rsam_yellow_warning )
      flag = "INSERT INTO  WARNING (time, color, commentary, station) VALUES ('%s', '%s', '%s', '%s')" % ( (rsam_times[ind]).strftime("%Y-%m-%d %H:%M:%S UTC"), color, comment, station )
      flags.append(flag)


  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  for flag in flags:
    try:
      cursor.execute(flag)
      db.commit()
    except:
      continue

  db.close()




# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def select_rsam_warnings(station, starttime, endtime):

  host, port, user, password, database = get_credentials_sqldb()


  db = MySQLdb.connect(host=host, port=int(port), user=user, passwd=password, db=database)
  cursor = db.cursor()
  #flag = "SELECT * FROM WARNING WHERE station= '%s' && time BETWEEN '%s' and '%s'" % (station, starttime.strftime("%Y-%m-%d %H:%M:%S.%f"), endtime.strftime("%Y-%m-%d %H:%M:%S.%f") )
  flag = "SELECT * FROM WARNING WHERE station= '%s' && time BETWEEN '%s' and '%s'" % (station, starttime.strftime("%Y-%m-%d %H:%M:%S"), endtime.strftime("%Y-%m-%d %H:%M:%S") )
  cursor.execute(flag)
  res = cursor.fetchall()
  db.close()

  colors = []; times = []; times_matplotlib = []
  for row in res:
    time, color, comment, station = row
    colors.append( color )
    times.append( UTCDateTime(time) )
    times_matplotlib.append( date2num(time) )

  return colors, times, times_matplotlib








"""
