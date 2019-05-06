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
import time, subprocess
from pygema.core.parameters import load_station_metadata
from pygema.db.ask import select_triggers_stalta, insert_event
from pygema.signal.autoloc import run_autoloc_binder
from pygema.signal.read import get_streams_gema
from pygema.signal.preprocessing import remove_instrument_response
from pygema.signal.magnitude import get_local_magnitude
from pygema.plot.map import plot_map_event
from pygema.plot.waveforms import plot_record_section
from pygema.db.email import send_email_with_attached_files

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

# read stations info
networks, stations, stlons, stlats, stalts = load_station_metadata()

this_utc = UTCDateTime(2017,8,1)
last_utc = UTCDateTime(2018,1,1)

twin = 86400
overlap = 0.0

while this_utc <= last_utc:
  starttime = this_utc
  endtime = this_utc + twin 

  try:
    # read trigger list
    triggers_list = []
    for station in stations:
      triggers = select_triggers_stalta(station, starttime, endtime)
      triggers_list.append(triggers)

    # calculate some stuf in order to check that the triggers_list is not empty
    count2 = 0; count1 = 0
    for line1, line2 in zip(stations, triggers_list):
      count2 += len(line2)
      if len(line2)>0:
        count1 += 1

    # run binder-nosc
    if count1>=3 and count2>=3:
      events_list = run_autoloc_binder(stations, triggers_list)
      if len(events_list)>0:
        for event in events_list:
          evtime = event[0]
          evlon = event[1]
          evlat = event[2]
          evdep = event[3]
          evnstats = event[4]
          evgap = event[5]
          evrms = event[6]
          # calculate local magnitude
          t1 = evtime - 60
          t2 = evtime + 300
          st, gaps = get_streams_gema(networks, stations, t1, t2, only_vertical_channel=True, local_dir_name=None)
          evmag = get_local_magnitude(st, stations, stlons, stlats, evtime, evlon, evlat, evdep, freqmin=1, freqmax=10, max_epicenter_dist=100)

          # insert event to pygema db
          insert_event(evtime, evlon, evlat, evdep, evnstats, evgap, evrms, evmag, status="automatic", table="LOC")
          print("[insert] automatic  %s  %.4f %.4f %.1f    %.1f  %i %.1f %.1f " % (evtime, evlon, evlat, evdep, evmag, evnstats, evgap, evrms) )

  except:
    pass

  this_utc += twin*(1-overlap)





