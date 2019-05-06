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
from pygema.plot.waveforms import plot_triggers_stalta
import subprocess, time, glob, os


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

networks, stations, stlons, stlats, stalts = load_station_metadata()

savedir = "web/PyGema_Web/PyGema_Web/static/figs_html"

deadtime = 0.1
while True:
  utc_now = UTCDateTime().now()
  starttime = utc_now - 60*30
  endtime = utc_now
  try:
    plot_triggers_stalta(networks, stations, starttime, endtime, freqmin=1, freqmax=10, include_trig_off=False, dark_background=True, show_plot=False, save_plot=True, savedir=savedir, format='jpg', dpi=300)

  except:
    pass

  time.sleep(deadtime)



