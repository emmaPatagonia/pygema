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


from obspy.signal.trigger import coincidence_trigger, recursive_sta_lta, trigger_onset, plot_trigger
from pygema.core.parameters import load_stalta_parameters

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def get_triggers_stalta(st):

  len_sta, len_lta, trig_on, trig_off, freqmin, freqmax = load_stalta_parameters()

  stations_list = []
  triggers_list = []
  if len(st)>0:
    for tr in st:
      try:
        tr.detrend('demean')
        tr.detrend('linear')
        tr.taper(max_percentage=0.015,type='hann')
        tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=2)

        cft_rec = recursive_sta_lta(tr.data, int(len_sta*tr.stats.sampling_rate), int(len_lta*tr.stats.sampling_rate) )
        on_off = trigger_onset(cft_rec, trig_on, trig_off)
        triggers = []
        for trig in on_off:
          on = tr.times("utcdatetime")[trig[0]]
          off = tr.times("utcdatetime")[trig[1]]
          triggers.append([on, off])

        stations_list.append(tr.stats.station)
        triggers_list.append(triggers)

      except:
       continue


  return stations_list, triggers_list


