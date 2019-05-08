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
import matplotlib.pyplot as plt
from matplotlib.transforms import blended_transform_factory
from obspy.core import UTCDateTime, read, Stream
from obspy.geodetics.base import calc_vincenty_inverse
import os, glob, subprocess

from pygema.core.parameters import load_station_metadata
from pygema.db.ask import select_event_from_database, update_event_status, update_event_localization
from pygema.signal.magnitude import get_local_magnitude
from pygema.signal.nonlinloc import build_event_directory_for_nonlinloc, read_nonlinloc_locfile
from pygema.plot.map import plot_map_event
from pygema.plot.waveforms import plot_record_section
from pygema.db.email import send_email_with_attached_files


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# BUILD DATA DIRECTORY

this_event, idxev, events_list = select_event_from_database(UTCDateTime(1970, 1,1), UTCDateTime().now(), table="LOC")

networks, stations, stlons, stlats, stalts = load_station_metadata()
outdir = build_event_directory_for_nonlinloc(this_event[0], networks, stations, freqmin=1, freqmax=10, deconvolve=True, only_vertical_channel=False, time_before=60, time_after=300)

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# CHECK IF SEISMIC EVENT IS CONFIRMED OR REJECTED

sacfiles = glob.glob( outdir + "/*HZ*.sac" )  
st = Stream()
for sacfile in sacfiles:
  st1 = read(sacfile)
  for tr in st1:
    ind = np.where( tr.stats.station == stations )[0][0]
    tr.stats.distance = calc_vincenty_inverse( stlats[ind], stlons[ind], this_event[2], this_event[1] )[0]
    st += tr

fig = plt.figure()
st.plot(type='section', method='full', orientation='vertical', time_down=True, linewidth=.25, grid_linewidth=.25, show=False, fig=fig)
ax = fig.axes[0]
transform = blended_transform_factory(ax.transData, ax.transAxes)
for tr in st:
  ax.text(tr.stats.distance / 1e3, 1.0, tr.stats.station, rotation=270, va="bottom", ha="center", transform=transform, zorder=10)

figManager = plt.get_current_fig_manager()
figManager.window.showMaximized()
plt.show()

flag = input("\n+ Is this a local earthquake? (yes/no): ")
while flag!="yes" and flag !="no":
  flag = input("+ Is this a local earthquake? (yes/no): ")
  if flag=="yes" or flag =="no":
    break

if flag=="yes":
  update_event_status(origin_time=this_event[0], status="confirmed", table="LOC")
elif flag=="no":
  update_event_status(origin_time=this_event[0], status="rejected", table="LOC")
  subprocess.call( "rm -r %s" % (outdir) , shell=True)
  print("+ Event rejected ...\n\nEOF")


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# LOCALIZE SEISMIC EVENT USING NONLINLOC

if flag=="yes":
  flag = input("\n+ Do you want to localize using nonlinloc? (yes/no): ")
  while flag!="yes" and flag !="no":
    flag = input("+ Do you want to localize using nonlinloc? (yes/no): ")
    if flag=="yes" or flag =="no":
      break

  if flag=="yes":
    if len(glob.glob(outdir+'/*') ) > 0:
      nonlinlocpath = "%s/NLL" % (os.getenv("HOME"))
      cmd1 = "python %s/execSG2KMOD2.py" % (nonlinlocpath)
      subprocess.call(cmd1, shell=True)
    else:
      print("+ No sacfiles found in event directory ... ")


  locfile = "%s/%s.loc" % (outdir, outdir)
  if os.path.isfile(locfile):
    try:
      flag = input("\n+ Update status to manual and calculate new magnitude for this event? (yes/no): ")
      while flag!="yes" and flag !="no":
        flag = input("+ Update status to manual and calculate new magnitude for this event? (yes/no): ")
        if flag=="yes" or flag =="no":
          break

      if flag=="yes":
        # read new localization 
        evtime_old = this_event[0]
        evtime, evlon, evlat, evdepth, evrms, everrx, everry, everrz = read_nonlinloc_locfile(locfile)

        # calculate new magnitude
        print("+ Calculating new magnitude ...")
        subprocess.call("rm -r %s" % (outdir) , shell=True)
        outdir_new = build_event_directory_for_nonlinloc(evtime, networks, stations, freqmin=1, freqmax=10, deconvolve=False, only_vertical_channel=True, time_before=60, time_after=300)
        sacfiles = glob.glob( outdir_new + "/*HZ*.sac" )  
        st = Stream()
        for sacfile in sacfiles:
          st1 = read(sacfile)
          for tr in st1:
            ind = np.where( tr.stats.station == stations )[0][0]
            tr.stats.distance = calc_vincenty_inverse( stlats[ind], stlons[ind], this_event[2], this_event[1] )[0]
            st += tr

        evmag = get_local_magnitude(st, stations, stlons, stlats, evtime, evlon, evlat, evdepth, freqmin=1, freqmax=10, max_epicenter_dist=100)

        # update database
        print("+ Updating database ...")
        update_event_localization(evtime_old, evtime, evlon, evlat, evdepth, evrms, everrx, everry, everrz, evmag, table="LOC")
        update_event_status(origin_time=evtime, status="manual", table="LOC")

        # plot localization figures
        plot_map_event(evlon, evlat, evdepth, dlon=0.3, dlat=0.3, res='c', dpi=300, xpixels=800, add_holocene_volcanoes=True, add_seismic_stations=True, add_ralco=True, add_labels=True, add_faults=True, dark_background=False, show_plot=False, savedir=outdir_new)
        plot_record_section(st.select(channel='*HZ'), stations, stlons, stlats, evtime, evlon, evlat, freqmin=1, freqmax=10, dark_background=False, show_plot=False, savedir=outdir_new)
        subprocess.call( "rm %s/*sac" % (outdir_new) , shell=True)

        # send email
        print("+ Sending email ...")
        message = "[ manual ] \n Origin Time: %s   mag = %.1f \n evlon = %.4f deg;  evlat = %.4f deg;  evdep = %.1f km \n errX = %.1f km; errY = %.1f km; errZ = %.1f km" % (evtime.strftime("%Y-%m-%d %H:%M:%S"), evmag, evlon, evlat, evdepth, everrx, everry, everrz)
        subject = "Warning"
        send_email_with_attached_files(message, subject, figsdir=outdir_new)
        subprocess.call("rm -r %s" % (outdir_new) , shell=True)

      elif flag=="no":
        subprocess.call( "rm -r %s" % (outdir) , shell=True)

        flag = input("\n+ Do you want to reject event? (yes/no): ")
        while flag!="yes" and flag !="no":
          flag = input("+ Do you want to reject event? (yes/no): ")
          if flag=="yes" or flag =="no":
            break

        if flag=="yes":
          update_event_status(origin_time=this_event[0], status="rejected", table="LOC")
          print("+ Event rejected ...\n\nEOF")

    except:
      print("+ It seems that the event file is empty (%s) ..." % (locfile) )

  else:
    if os.path.isdir( outdir ):
      subprocess.call( "rm -r %s" % (outdir) , shell=True)



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 




