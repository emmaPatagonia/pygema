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
import matplotlib as mpl
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.dates import date2num, num2date, DateFormatter
from obspy.core import UTCDateTime, read, Stream, Trace
from obspy.signal.filter import envelope, bandpass, lowpass, highpass
from obspy.signal.trigger import coincidence_trigger, recursive_sta_lta, trigger_onset, plot_trigger
from obspy.signal.invsim import estimate_magnitude
from obspy.geodetics.base import calc_vincenty_inverse
from obspy.taup import TauPyModel
from obspy import read_inventory
from obspy.io.xseed import Parser
from obspy.imaging.cm import pqlx
from scipy.fftpack import fft, ifft
from scipy.interpolate import interp1d
import scipy.ndimage as ndimage
from pprint import pprint
import sys, os, glob, datetime, MySQLdb, imp, time, socket, io, subprocess

 

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def compute_rsam(st, twin, freqmin, freqmax, overlap=0):

  rsam_utcs = []; rsam_values = []
  t1 = st[0].stats.starttime
  while t1+twin <= st[-1].stats.endtime:
    t2 = t1 + twin
    st1 = st.copy().trim(t1,t2)

    """
    if len(st1)>0:
      envelopes_stack = []
      for tr in st1:
        tr.detrend('demean')
        tr.detrend('linear')
        #tr.taper(max_percentage=0.015,type='hann')
        tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=2)
        envelopes_stack.append( envelope(tr.data) )

      envelopes = np.concatenate(envelopes_stack)
      rsam = np.nanmean(envelopes)
      rsam_values.append(rsam)
    else:
      rsam_values.append(np.nan)
    """

    envelopes_stack = []
    for tr in st1:
      tr.detrend('demean')
      tr.detrend('linear')
      #tr.taper(max_percentage=0.015,type='hann')
      tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=2)
      envelopes_stack.append( envelope(tr.data) )

    envelopes = np.concatenate(envelopes_stack)
    rsam = np.nanmean(envelopes)
    rsam_values.append(rsam)

    rsam_utcs.append(t1+twin/2.)

    t1 += twin*(1-overlap)

  return np.array(rsam_utcs), np.array(rsam_values)


