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

from obspy.core import UTCDateTime
from pygema.core.parameters import load_station_metadata
from pygema.db.ask import select_events_automatic_loc, select_events_confirmed_loc, select_events_manual_loc
from pygema.core.kmltools import kmlexport_stations, kmlexport_seismic_events
import subprocess, time, glob, os

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

outdir = "web/PyGema_Web/PyGema_Web/static"

deadtime = 5
while True:
  utc_now = UTCDateTime().now()
  try:

    # seismic stations
    networks, stations, stlons, stlats, stalts = load_station_metadata()
    kmlexport_stations(networks, stations, stlons, stlats, stalts, icon="http://maps.google.com/mapfiles/kml/pal4/icon56.png", kmlfilename='stations.kml', outdir=outdir)

    # last day events
    starttime = utc_now-86400.
    endtime = utc_now

    events_list = select_events_manual_loc(starttime, endtime, table="LOC")
    kmlexport_seismic_events(events_list, icon='http://maps.google.com/mapfiles/kml/paddle/red-stars.png', kmlfilename='seismic_events_manual_last_day.kml', outdir=outdir)

    events_list = select_events_automatic_loc(starttime, endtime, table="LOC")
    kmlexport_seismic_events(events_list, icon='http://maps.google.com/mapfiles/kml/paddle/red-stars-lv.png', kmlfilename='seismic_events_automatic_last_day.kml', outdir=outdir)

    # last month events
    starttime = utc_now-86400.*30
    endtime = utc_now-86400.
    events_list = select_events_manual_loc(starttime, endtime, table="LOC")
    kmlexport_seismic_events(events_list, icon='http://maps.google.com/mapfiles/kml/paddle/ylw-stars.png',  kmlfilename='seismic_events_manual_last_month.kml', outdir=outdir)

    # older events
    starttime = UTCDateTime(1970,1,1)
    endtime = utc_now-86400.*30
    events_list = select_events_manual_loc(starttime, endtime, table="LOC")
    kmlexport_seismic_events(events_list, icon='http://maps.google.com/mapfiles/kml/paddle/grn-stars.png', kmlfilename='seismic_events_manual_older.kml', outdir=outdir)


  except:
    pass


  time.sleep(deadtime)



