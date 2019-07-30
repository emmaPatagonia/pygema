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
from obspy.core import UTCDateTime

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

first_day = input("\n+ starttime (yyyy,mm,dd,hh,mm,ss): ")
last_day  = input("\n+ endtime   (yyyy,mm,dd,hh,mm,ss): ")

localdir = input("Type the path (default is $HOME/archive): ")
if localdir == '':
  localdir = '%s/archive' % (os.getenv("HOME"))

cmd = "slinktool -v 192.168.1.28 -SDS %s -tw %s:%s" % (localdir, first_day, last_day)
subprocess.call(cmd , shell=True)

print("EOF\n")

