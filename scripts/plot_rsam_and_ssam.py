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
from pygema.signal.read import get_streams_gema
from pygema.core.parameters import load_rsam_parameters, load_ssam_parameters #, load_warnings_rsam
from pygema.signal.rsam import compute_rsam
from pygema.signal.ssam import compute_ssam
from pygema.plot.rsam_ssam import plot_rsam_and_ssam
from pygema.signal.preprocessing import remove_instrument_response

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  

starttime = UTCDateTime(2019,4,22,17,0,0) 
endtime   = UTCDateTime(2019,4,22,23,0,0) 

network = "GM"
station = "MAYA"

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #  

# read waveform vertical component
st, gaps = get_streams_gema([network], [station], starttime, endtime, only_vertical_channel=True, local_dir_name=None)

# compute rsam
twin, freqmin, freqmax = load_rsam_parameters()
rsam_utcs, rsam_values = compute_rsam(st, twin, freqmin, freqmax, overlap=0.7)
#rsam_utcs, rsam_values = compute_rsam(st, 10, freqmin, freqmax, overlap=0.95)

# compute ssam
twin, freqmin, freqmax, nfreqs = load_ssam_parameters()
ssam_utcs, ssam_values, ssam_freqs = compute_ssam(st, twin, freqmin, freqmax, nfreqs, overlap=0.7)
#ssam_utcs, ssam_values, ssam_freqs = compute_ssam(st, 5, freqmin, freqmax, nfreqs, overlap=0.95)

# plot
for tr in st:
  try:
    tr.detrend('demean')
    tr.detrend('linear')
    tr.taper(max_percentage=0.005,type='hann')
    #tr = remove_instrument_response(tr, pre_filt=(0.01, 0.02, 50, 100), detrend=True, taper=True, dataless_file=None)
    tr.filter("bandpass", freqmin=1, freqmax=10, corners=2)
  except:
  	st.remove(tr)
  	continue


plot_rsam_and_ssam(st, gaps, rsam_utcs, rsam_values, ssam_utcs, ssam_values, ssam_freqs, include_waveform=True, include_warnings_rsam=False, include_last_update_title=True, dark_background=True, show_plot=False, savedir='figs')


