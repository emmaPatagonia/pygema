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

this_day = input("\n+ Type the day that you want to sync (yyyy-mm-dd): ")
this_day = UTCDateTime(this_day)

localdir = input("Type local dir (default is $HOME/archive): ")
if localdir == '':
  localdir = '%s/archive' % (os.getenv("HOME"))

if os.path.isdir("tmp"):
  subprocess.call("rm -r tmp" , shell=True)
else:
  os.makedirs("tmp")

remote_path = "/home/gema/archive/%s/*/*/*/*%s" % (this_day.strftime("%Y"), this_day.strftime("%Y.%03j") )

cmd = "rsync -aR gema@192.168.1.28:%s tmp" % (remote_path)
subprocess.call(cmd , shell=True)

cmd = "cp -rv tmp/home/gema/archive/* %s" % (localdir)
subprocess.call(cmd , shell=True)


if os.path.isdir("tmp"):
  subprocess.call("rm -r tmp" , shell=True)


print("EOF\n")
