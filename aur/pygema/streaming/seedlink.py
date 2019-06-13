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

import subprocess, time, glob, os

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def connect_slinkserver_gema(slink_address, archive_path=None, delay=5):
  if not archive_path:
    archive_path = "%s/archive" % (os.getenv("HOME"))

  if not os.path.isdir(archive_path):
    os.mkdir(archive_path)

  print("+ Connecting to seedlink server --> %s" % (slink_address) )
  cmd = "slinktool -nd %i %s -SDS %s &" % (delay, slink_address, archive_path)
  subprocess.call(cmd, shell=True)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def connect_slinkserver(slinkserver, network, station, selectors, archive_path=None, delay=5):
  if not archive_path:
    archive_path = "%s/archive" % (os.getenv("HOME"))

  if not os.path.isdir(archive_path):
    os.mkdir(archive_path)

  print("+ Connecting to seedlink server --> %s  (%s %s %s)" % (slinkserver, network, station, selectors) )
  cmd = "slinktool -nd %i -s '%s?' -S '%s_%s' %s -SDS %s &" % (delay, selectors, network, station, slinkserver, archive_path)
  subprocess.call(cmd, shell=True)


