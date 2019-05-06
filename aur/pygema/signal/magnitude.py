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
from obspy.geodetics.base import calc_vincenty_inverse
from obspy.io.xseed import Parser
from obspy.signal.invsim import estimate_magnitude
import site, glob

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
def get_local_magnitude(st, stations, stlons, stlats, evtime, evlon, evlat, evdep, freqmin=1, freqmax=10, max_epicenter_dist=100):

  allmags = []
  for station in stations:
    st1 = st.copy().select(station=station)
    for tr in st1:
      try:
        tr.detrend('demean')
        tr.detrend('linear')
        tr.taper(max_percentage=0.005,type='hann')
        tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=2)

        amp_min = tr.data.min()
        amp_max = tr.data.max()
        ind1 = np.where(amp_min == tr.data)[0][0]
        ind2 = np.where(amp_max == tr.data)[0][0]
        amplitude = abs(amp_max) + abs(amp_min)
        timespan = tr.times("utcdatetime")[ind2] - tr.times("utcdatetime")[ind1] 

        ind = np.where(station==stations)[0][0]
        h_dist = calc_vincenty_inverse(evlat, evlon, stlats[ind], stlons[ind])[0]/1000.

        PYGEMA_PATH = "%s/pygema" % (site.getsitepackages()[0])
        dataless_file = glob.glob( "%s/src/dataless/%s_%s.dataless" % (PYGEMA_PATH, tr.stats.network, tr.stats.station) )[0]
        parser = Parser(dataless_file)
        paz = parser.get_paz(tr.id)

        mag = estimate_magnitude(paz, amplitude, timespan, h_dist)
        if h_dist<=max_epicenter_dist:
          allmags.append(mag)

      except:
        continue

  # calculate mean of magnitudes
  if len(allmags)>0:
    evmag = np.nanmean(allmags)
  else:
    evmag = 0.0

  return evmag


