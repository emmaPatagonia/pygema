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

def compute_ssam(st, twin, freqmin, freqmax, nfreqs, overlap=0):

  ssam_utcs = []; ssam_values = []
  ssam_freqs = np.linspace(freqmin, freqmax, nfreqs)
  t1 = st[0].stats.starttime
  while t1+twin <= st[-1].stats.endtime:
    t2 = t1 + twin
    tr = st.copy().trim(t1,t2).merge(method=1, fill_value='interpolate', interpolation_samples=-1)[0]

    tr.detrend('demean')
    tr.detrend('linear')
    tr.taper(max_percentage=0.015,type='hann')
    tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=2)

    Fs = tr.stats.sampling_rate       # sampling rate
    Ts = 1.0/Fs                       # sampling interval
    n = tr.stats.npts                 # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T                         # two sides frequency range
    frq = frq[range(int(n/2))]        # one side frequency range
    Y = np.fft.fft(tr.data)           # fft computing
    #Y = Y/n                           # normalization
    fft_amp = abs(Y[range(int(n/2))]) 
    power_spectrum_db = 10*np.log10(2*(fft_amp**2)/n)

    ssam = []
    for i in range(nfreqs-1):
      f1 = ssam_freqs[i]
      f2 = ssam_freqs[i+1]
      inds = np.where( (frq>=f1) & (frq<=f2) )[0]
      ssam.append( np.nanmean( power_spectrum_db[inds] ) )

    ind = np.where( (frq>=f2) )[0]
    ssam.append( np.nanmean( power_spectrum_db[ind] ) )
    ssam_values.append(ssam)

    ssam_utcs.append(t1+twin/2.)
    t1 += twin*(1-overlap)

  return np.array(ssam_utcs), np.array(ssam_values), np.array(ssam_freqs)



