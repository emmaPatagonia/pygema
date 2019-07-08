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
from pygema.plot.map import plot_map


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

elon = -71.0
wlon = -71.75
nlat = -37.7
slat = -38.6

plot_map(elon, wlon, nlat, slat, res='f', dpi=300, xpixels=1920, 
	      add_holocene_volcanoes=True, add_seismic_stations=True, add_ralco=True, 
	      add_labels=True, add_faults=True, add_events_pygemadb=True, 
	      evstarttime=UTCDateTime(2019,6,1), evendtime=UTCDateTime(2019,7,1), 
	      dark_background=False, show_plot=False, savedir='figs')







