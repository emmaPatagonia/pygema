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

import numpy as np
from obspy.core import UTCDateTime
import os, subprocess, site


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def run_autoloc_binder(stations, triggers_list):

  # sort trigger list by time
  all_triggers = []
  for station, triggers in zip(stations, triggers_list):
    for trigger in triggers:
      all_triggers.append( [station, trigger[0].timestamp] )

  all_triggers = np.array(all_triggers)
  all_triggers = all_triggers[np.argsort(all_triggers[:, 1])]

  # define current directory and binder nosc source directory
  localdir = os.getcwd()
  binder_path = "%s/BINDER_NOSC" % (os.getenv("HOME"))

  # export trigger input file to run later binder nosc
  picker_input = []
  for trigger in all_triggers:
    pattern = "%.2f %s Z P 0 _" % ( float(trigger[1]), trigger[0] )
    picker_input.append(pattern)

  picker_input_filename = "%s/picker_input.txt" % (binder_path)
  np.savetxt(picker_input_filename, np.array(picker_input), fmt='%s') #delimiter=','

  # remove events file (from binder nosc) if exists
  if os.path.isfile("%s/events_binder_output.txt" %(binder_path) ):
    subprocess.call("rm %s/events_binder_output.txt" %(binder_path) , shell=True) 

  if os.path.isfile("%s/events.txt" % (binder_path) ):
    subprocess.call("rm %s/events.txt" % (binder_path) , shell=True)

  # move python terminal to binder nosc main directory
  os.chdir("%s" % (binder_path) )

  # excecute binder nosc
  cmd1 = "./binder_nosc_AR picker_input.txt param.txt"
  subprocess.call(cmd1, shell=True)

  # check if we found any event
  if os.path.isfile("events_binder_output.txt") and sum(1 for line in open('events_binder_output.txt')) > 1:
    cmd2 = "more events_binder_output.txt | awk '{if ( NF > 10 ) print $0}' > events.txt"
    subprocess.call(cmd2, shell=True)

  os.chdir(localdir)
  event_file = "%s/events.txt" % (binder_path) 
  if os.path.isfile(event_file) and sum(1 for line in open(event_file)) > 0:
    events = np.loadtxt(event_file)
    events_list = []
    if events.ndim == 1:
      event = events
      event_time = UTCDateTime("%04i-%02i-%02iT%02i:%02i:%02.4f" % ( int(event[1]), int(event[2]), int(event[3]), int(event[4]), int(event[5]), float(event[6]) ) ) 
      event_lon = float(event[8])
      event_lat = float(event[7])
      event_dep = -float(event[9])
      event_nstats = int(event[10])
      event_gap = float(event[11])
      event_rms = float(event[12])
      events_list.append([event_time, event_lon, event_lat, event_dep, event_nstats, event_gap, event_rms])

    elif events.ndim > 1:
      for event in events:
        event_time = UTCDateTime("%04i-%02i-%02iT%02i:%02i:%02.4f" % ( int(event[1]), int(event[2]), int(event[3]), int(event[4]), int(event[5]), float(event[6]) ) ) 
        event_lon = float(event[8])
        event_lat = float(event[7])
        event_dep = -float(event[9])
        event_nstats = int(event[10])
        event_gap = float(event[11])
        event_rms = float(event[12])
        events_list.append([event_time, event_lon, event_lat, event_dep, event_nstats, event_gap, event_rms])

  else:
    events_list = []


  return events_list




