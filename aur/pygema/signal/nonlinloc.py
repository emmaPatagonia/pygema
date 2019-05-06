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

import numpy as np
from obspy.core import UTCDateTime
import os, subprocess
from pygema.signal.read import get_streams_gema
from pygema.signal.preprocessing import remove_instrument_response

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  

def build_event_directory_for_nonlinloc(event_time, networks, stations, freqmin=None, freqmax=None, deconvolve=False, only_vertical_channel=False, time_before=60, time_after=300):

  starttime = event_time - time_before
  endtime = event_time + time_after
  print("+ Reading seismic data from server ...")
  st, gaps = get_streams_gema(networks, stations, starttime, endtime, only_vertical_channel=only_vertical_channel, local_dir_name=None)

  print("+ Pre-processing seismic data ...")
  if len(st)>0:
    for tr in st:
      try:
        if deconvolve:
          tr = remove_instrument_response(tr, pre_filt=(0.01, 0.02, 50, 100), detrend=True, taper=True, dataless_file=None)
        else:
          tr.detrend('demean')
          tr.detrend('linear')
          tr.taper(max_percentage=0.005,type='hann')

        if freqmin is not None and freqmax is not None:
          tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=3)

      except:
        st.remove(tr)
        continue

    outdir = "%s" % (event_time.strftime("%Y-%m-%d-%H%M%S") )
    if len(st)>0:
      print("+ Exporting waveforms ...")
      if not os.path.isdir(outdir):
        os.makedirs(outdir) 

      for tr in st:
        outfile = "%s/%s.%s.%s.%s.%s-%s.sac" % (outdir, tr.stats.network, tr.stats.station, tr.stats.location, tr.stats.channel, tr.stats.starttime.strftime("%Y%m%d%H%M%S"),tr.stats.endtime.strftime("%Y%m%d%H%M%S") )
        #print(outfile)
        tr.write(outfile, "SAC")

    else:
      print("+ No data found ...")

  return outdir


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def read_nonlinloc_locfile(locfile):
  evtime, evlon, evlat, evdepth, evrms, everrx, everry, everrz = np.loadtxt(locfile, dtype='str')

  return UTCDateTime(evtime), float(evlon), float(evlat), float(evdepth), float(evrms), float(everrx), float(everry), float(everrz)




