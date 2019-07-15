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
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.dates import date2num, num2date, DateFormatter
from obspy.core import UTCDateTime
from pygema.db.ask import select_events_manual_loc
import os

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def plot_seismicity_time_serie(starttime, endtime, dark_background=True, show_plot=False, save_plot=True, savedir=None, format='jpg', dpi=150):

  if dark_background:
    plt.style.use(['dark_background'])
  else:
    plt.style.use(['default'])

  fig = plt.figure(dpi=dpi)
  gs = mpl.gridspec.GridSpec(2, 1, hspace=0.25) 
  ax1 = fig.add_subplot(gs[0])
  ax2 = fig.add_subplot(gs[1])
  date_format = DateFormatter('%b %d')

  ax1.set_title("last update: %s UTC" % (UTCDateTime().strftime("%Y/%m/%d %H:%M:%S")), fontsize=6, loc='right',  y=1.03)
  for ax in [ax1,ax2]:
    ax.minorticks_on()
    ax.tick_params(axis='y', which='major', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in', color='y')
    ax.tick_params(axis='y', which='minor', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in', color='y')
    ax.tick_params(axis='x', which='major', labelsize=8, bottom='on', top='off', left='on', right='on', direction='in', rotation=90)
    ax.tick_params(axis='x', which='minor', labelsize=8, bottom='on', top='off', left='on', right='on', direction='in', rotation=90)
    ax.xaxis.set_major_formatter(date_format)
    ax.set_xlim([ date2num(starttime.datetime), date2num(endtime.datetime) ])
    ax.grid(axis='x', lw=0.7, ls=':', color='0.5')
    ax.get_xaxis().set_minor_formatter(date_format)
    ax.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
    ax.yaxis.offsetText.set_fontsize(6)
    ax.spines['right'].set_visible(True)
    ax.spines['top'].set_visible(False)
    ax.spines['left'].set_visible(True)
    ax.spines['bottom'].set_visible(False)

  ax1.set_ylabel(r"N$^\circ$ events")
  ax2.set_ylabel(r"Released energy (dyn cm$^2$)")
  plt.setp( ax1.get_xticklabels(), visible=False)
  plt.setp( ax1.get_xticklabels("minor"), visible=False)

  ax11 = ax1.twinx()
  ax22 = ax2.twinx()
  for axx in [ax11,ax22]:
    axx.minorticks_on()
    axx.xaxis.set_major_formatter(date_format)
    axx.set_xlim([ date2num(starttime.datetime), date2num(endtime.datetime) ])
    axx.grid(axis='x', lw=0.7, ls=':', color='0.5')
    axx.get_xaxis().set_minor_formatter(date_format)
    axx.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
    axx.tick_params(axis='y', which='major', labelsize=8, bottom='off', top='off', left='off', right='on', direction='out', color='r')
    axx.tick_params(axis='y', which='minor', labelsize=8, bottom='off', top='off', left='off', right='on', direction='out', color='r')
    axx.tick_params(axis='x', which='major', labelsize=8, bottom='off', top='off', left='off', right='on', direction='out', rotation=90)
    axx.tick_params(axis='x', which='minor', labelsize=8, bottom='off', top='off', left='off', right='on', direction='out', rotation=90)
    axx.yaxis.offsetText.set_fontsize(6)
    axx.spines['right'].set_visible(True)
    axx.spines['top'].set_visible(False)
    axx.spines['left'].set_visible(True)
    axx.spines['bottom'].set_visible(False)

  ax11.set_ylabel(r"Cumulative events")
  ax22.set_ylabel(r"Cumulative energy")
  plt.setp( ax11.get_xticklabels(), visible=False)
  plt.setp( ax11.get_xticklabels("minor"), visible=False)



  # plot seismicity
  events_list = select_events_manual_loc(starttime, endtime, table="LOC")
  origin_times = sorted(np.array(events_list).T[0])
  evmags = np.array(events_list).T[4]
  ergs = np.power(11.8 + 1.5*evmags, 10)

  # daily seismicity
  nevents = []; utcs = []; ergs_day = []
  this_day = UTCDateTime(starttime.strftime("%Y-%m-%d"))
  while this_day <= endtime:
    inds = np.where( (np.array(origin_times)>=this_day) & (np.array(origin_times)<(this_day+86400.) ) )[0]
    num = len(inds)
    ergs_this_day = np.sum( ergs[inds] )
    utcs.append( date2num(this_day.datetime) )
    nevents.append(num)
    ergs_day.append(ergs_this_day)
    this_day+=86400

  markerline, stemlines, baseline = ax1.stem(utcs, nevents, markerfmt='None', basefmt='None', linefmt='y-')
  plt.setp(stemlines, 'linewidth', 1.5)
  markerline, stemlines, baseline = ax2.stem(utcs, ergs_day, markerfmt='None', basefmt='None', linefmt='y-')
  plt.setp(stemlines, 'linewidth', 1.5)

  # cumulative seismicity
  cumsum = np.cumsum( nevents )
  ax11.step(utcs, cumsum, color='r', where='pre', zorder=2, lw=0.8)

  cumsum = np.cumsum( ergs_day )
  ax22.step(utcs, cumsum, color='r', where='pre', zorder=2, lw=0.8)
  #cumsum_times = []
  #for time in np.array(events_list).T[0]:
  #  cumsum_times.append( date2num(time.datetime) )
  #ax22.step(cumsum_times, cumsum, color='r', where='pre', zorder=2, lw=0.8)



  # export figure
  if save_plot:
    if savedir is None:
      outdir = "figs" 
    else:
      outdir = "%s" % (savedir)

    if not os.path.isdir(outdir):
      os.makedirs(outdir)

    figname = "%s/seismicity_time_serie.%s" % (outdir, format)
    plt.savefig(figname, dpi=dpi, bbox_inches='tight', transparent=False)
    if not show_plot:
      plt.close('all')

  if show_plot:
    plt.show()
    plt.close('all')




