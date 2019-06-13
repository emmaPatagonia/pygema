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

from pygema.core.parameters import load_station_metadata
from pygema.streaming.seedlink import connect_slinkserver
import subprocess, time, glob, os

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

# define seedlink server address:port
slink_server_gema = "192.168.1.28:18000" # gema.udec.cl:18000
slink_server_csn = "sl.csn.uchile.cl:18000"

# define path to archive seismic waveforms
archive_path = "%s/archive" % (os.getenv("HOME"))
delay = 5
print("\n+ Archive data dir: %s\n+ Time delay: %i s" % (archive_path, delay) )


# init data adquisition
networks, stations, stlons, stlats, stalts = load_station_metadata()
for network, station in zip(networks, stations):
  if network == "GM": 
    connect_slinkserver(slink_server_gema, network, station, selectors="BH", archive_path=archive_path, delay=5)
  elif network == "C1": 
    connect_slinkserver(slink_server_csn, network, station, selectors="BH", archive_path=archive_path, delay=5)






