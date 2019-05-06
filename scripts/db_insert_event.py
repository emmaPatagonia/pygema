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
from pygema.db.ask import insert_event


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# type event hypocenter

evtime = input("\n+ Origin time (YYYY-MM-DD HH:MM:SS): ")
evlon = input("+ Longitude (xx.xxxx): ")
evlat = input("+ Latitude (yy.yyyy): ")
evdep = input("+ Depth (zz.z): ")
evmag = input("+ Magnitude (default 0.0): ")
evgap    = input("+ GAP (default 0.0): ")
evnstats = input("+ N. records (default 0): ")
evrms    = input("+ RMS (default 0.0): ")
status    = input("+ Status (automatic, confirmed or manual. Default automatic): ")

evtime = UTCDateTime(evtime)
evlon = float(evlon)
evlat = float(evlat)
evdep = float(evdep)

if evmag == '':
  evmag = 0.0
else:
  evmag = float(evmag)

if evgap == '':
  evgap = 0.0
else:
  evgap = float(evgap)

if evnstats == '':
  evnstats = 0
else:
  evnstats = int(evnstats)

if evrms == '':
  evrms = 0.0
else:
  evrms = float(evrms)

if status == '':
  status = 'automatic'
elif status!='confirmed' and status!='manual':
  status = 'automatic'


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

print("[insert]  %s  %.4f %.4f %.1f    %.1f  %i %.1f %.1f  (%s) " % (evtime, evlon, evlat, evdep, evmag, evnstats, evgap, evrms, status  ) )
insert_event(evtime, evlon, evlat, evdep, evnstats, evgap, evrms, evmag, status=status, table="LOC", figs_dir=None)


