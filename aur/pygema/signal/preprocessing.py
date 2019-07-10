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

import matplotlib.pyplot as plt
from obspy.core import UTCDateTime
from obspy.io.xseed import Parser
from obspy.signal import PPSD
from obspy.imaging.cm import pqlx
import glob, site

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def remove_instrument_response(tr, pre_filt=(0.01, 0.02, 50, 100), detrend=True, taper=True, dataless_file=None):

  if not dataless_file:
    PYGEMA_PATH = "%s/pygema" % (site.getsitepackages()[0])
    dataless_file = glob.glob( "%s/src/dataless/%s_%s.dataless" % (PYGEMA_PATH, tr.stats.network, tr.stats.station) )[0]

  if detrend:
    tr.detrend('demean')
    tr.detrend('linear')

  if taper:
    tr.taper(max_percentage=0.005,type='hann')

  parser = Parser(dataless_file)
  paz = parser.get_paz(tr.id)
  tr.simulate(paz_remove=paz, pre_filt=pre_filt, paz_simulate=None, remove_sensitivity=True)

  return tr


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def init_ppsd_dataless(tr, dataless_file):
  parser = Parser(dataless_file)
  paz = parser.get_paz(tr.id)
  ppsd = PPSD(tr.stats, metadata=paz)
  return ppsd


