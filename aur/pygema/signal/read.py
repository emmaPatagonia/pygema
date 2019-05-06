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

import sys, os, glob, datetime, MySQLdb, time, socket, subprocess, requests
from obspy.core import UTCDateTime, read, Stream

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def get_streams_gema(networks, stations, starttime, endtime, only_vertical_channel=False, local_dir_name=None):
  if not local_dir_name:
    local_dir_name = "%s/mount" % (os.getenv("HOME"))

  r = requests.get(r'http://jsonip.com')
  public_ip= r.json()['ip']
  if (socket.gethostname()=='maniedba') and (public_ip=="152.74.135.51"):
    local_buffer = "/home/gema/seiscomp3/var/lib/seedlink/buffer" 
    local_archive = "/home/gema/seiscomp3/var/lib/archive " 
  else:
    local_buffer = "%s/seiscomp_data_buffer" % (local_dir_name)
    local_archive = "%s/seiscomp_data_archive" % (local_dir_name)


  st = Stream()
  if only_vertical_channel:
    channels = "*Z"
  else:
    channels = "*"

  # read archive directory
  for network, station in zip(networks, stations):
    this_day = starttime
    while this_day <= endtime:
      pattern = '%s/%s/%s/%s/%s.D' % (local_archive, this_day.strftime("%Y"), network, station, channels )
      paths_ch = sorted(glob.glob(pattern))
      for path in paths_ch:
        pattern = "%s/*%s" % (path, this_day.strftime("%Y.%03j"))
        msfile_list = glob.glob(pattern)
        if len(msfile_list)>0:
          for msfile in msfile_list:
            st += read(msfile, starttime=starttime, endtime=endtime)

      this_day += 86400

  gaps = st.get_gaps()

  # read buffer directory
  if UTCDateTime().now() - endtime <= 3600:
    for network, station in zip(networks, stations):
      path = '%s/%s/segments' % (local_buffer, station)
      name_list = os.listdir(path)
      full_list = [os.path.join(path,i) for i in name_list]
      time_sorted_list = sorted(full_list, key=os.path.getmtime)
      msfiles = time_sorted_list[-3::]
      for msfile in msfiles:
        st += read(msfile, starttime=starttime, endtime=endtime).select(channel=channels)

    if len(st)>1:
      st.merge(method=1, interpolation_samples=-1, fill_value='interpolate')
    
  return st, gaps



