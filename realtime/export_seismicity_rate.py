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
from pygema.plot.catalogue import plot_seismicity_time_serie
import subprocess, time, glob, os

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

savedir = "web/PyGema_Web/PyGema_Web/static/figs_html"

deadtime = 5
while True:
  try:
    starttime = UTCDateTime() - 86400*30*6
    endtime = UTCDateTime()
    plot_seismicity_time_serie(starttime, endtime, dark_background=True, show_plot=False, save_plot=True, savedir=savedir, format='jpg', dpi=300)

  except:
    pass

  time.sleep(deadtime)



