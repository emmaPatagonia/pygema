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
import time
from pygema.core.parameters import load_station_metadata
from pygema.signal.read import get_streams_gema
from pygema.signal.autopick import get_triggers_stalta
from pygema.db.ask import insert_triggers_stalta


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

networks, stations, stlons, stlats, stalts = load_station_metadata()

this_utc = UTCDateTime(2019,5,17,1)
last_utc = UTCDateTime(2019,5,17,1,15)

overlap_stalta = 0.1
stream_length = 15*60

while this_utc <= last_utc:
  starttime = this_utc
  endtime = this_utc + stream_length 

  try:
    print("\n Current time: %s" % (UTCDateTime()))
    # read data
    st, gaps = get_streams_gema(networks, stations, starttime, endtime, only_vertical_channel=True, local_dir_name=None)
    # compute sta/lta
    stations_list, triggers_list = get_triggers_stalta(st)
    # insert triggers to pygema database
    if len(triggers_list)>0:
      insert_triggers_stalta(stations_list, triggers_list)
      # print results
      for station,triggers in zip(stations_list, triggers_list):
        for trigger in triggers:
          print("[%s] trig_on = %s   trig_off = %s" % (station, trigger[0], trigger[1]) )

  except:
    pass

  this_utc += stream_length*(1-overlap_stalta)













