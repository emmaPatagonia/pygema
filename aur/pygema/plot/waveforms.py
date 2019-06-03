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
from matplotlib.transforms import blended_transform_factory
from matplotlib.patches import Rectangle
from matplotlib.dates import date2num, num2date, DateFormatter
from obspy.core import UTCDateTime, read, Stream
from obspy.geodetics.base import calc_vincenty_inverse
import os
from pygema.plot.artist import adjustFigAspect
from pygema.db.ask import select_events_manual_loc, select_triggers_stalta
from pygema.signal.preprocessing import remove_instrument_response
from pygema.signal.read import get_streams_gema

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def plot_helicorder(st, include_events_pygemadb=False, include_last_update_title=False, dark_background=False, show_plot=True, savedir=None):

  if dark_background:
    plt.style.use(['dark_background'])
  else:
    plt.style.use(['default'])

  starttime = UTCDateTime(st[0].stats.starttime.strftime("%Y-%m-%d"))
  endtime = starttime + 86400. - st[0].stats.delta

  if include_events_pygemadb:
    events_list = select_events_manual_loc( starttime, endtime, table="LOC")
    events = [ ]
    for event in events_list:
      dic = {"time":event[0], "text": "Ml %.1f" % (event[4]) }
      events.append(dic)
  else:
    events = []


  fig = plt.figure()
  st.plot(type='dayplot', 
        interval=60, 
        one_tick_per_line=True, 
        show_y_UTC_label=False, tick_format="%H:%M",
        events = events,
        linewidth=0.3, method='full', starttime=starttime, endtime=endtime, fig=fig, title=None)

  adjustFigAspect(fig,aspect=0.75)

  ax = fig.axes[0]
  if include_last_update_title:
    ax.set_title("last update: %s UTC" % (UTCDateTime().now().strftime("%Y/%m/%d %H:%M:%S")), fontsize=6, loc='right' )
    fig.suptitle("%s" % (st[0].id), y=0.97, fontsize=10)
  else:
    fig.suptitle("%s" % (st[0].id), y=0.94, fontsize=10)


  ax.tick_params(axis='y', which='major', labelsize=8, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
  ax.tick_params(axis='y', which='minor', labelsize=8, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
  ax.tick_params(axis='x', which='major', labelsize=8, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
  ax.tick_params(axis='x', which='minor', labelsize=8, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
  ax.spines['right'].set_visible(True)
  ax.spines['top'].set_visible(True)
  ax.spines['left'].set_visible(True)
  ax.spines['bottom'].set_visible(True)
  ax.grid(axis='x', lw=0.7, ls=':', color='0.75')
  ax.set_ylabel("UTC time (local time = UTC - 03:00)", fontsize=8 )
  ax.set_xlabel("time in minutes", fontsize=8 )

  if dark_background:
    ax.patch.set_facecolor('k')
  else:
    ax.patch.set_facecolor('w')

  if savedir:
    outdir = "%s/%s" % (savedir, st[0].stats.station)
    if not os.path.isdir(outdir):
      os.makedirs(outdir)

    figname = "%s/%s_helicorder.jpg" % (outdir, st[0].stats.station)
    plt.savefig(figname, dpi=300, bbox_inches='tight', transparent=False)
    if not show_plot:
      plt.close('all')

  if show_plot:
    plt.show()
    plt.close('all')


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def plot_three_component_waveforms(st, gaps, vel_units=True, include_triggers=False, include_last_update_title=False, dark_background=False, show_plot=True, savedir=None):

  if dark_background:
    plt.style.use(['dark_background'])
  else:
    plt.style.use(['default'])

  tr_z = st.select(channel='*Z').merge(method=1, interpolation_samples=-1, fill_value='interpolate')[0]
  tr_n = st.select(channel='*N').merge(method=1, interpolation_samples=-1, fill_value='interpolate')[0]
  tr_e = st.select(channel='*E').merge(method=1, interpolation_samples=-1, fill_value='interpolate')[0]

  # init figure
  fig = plt.figure()
  #gs = mpl.gridspec.GridSpec(3, 1, height_ratios=[1,1,1], hspace=0.15) 
  #ax = fig.add_subplot(gs[0])
  #ax1 = fig.add_subplot(gs[1], sharex=ax)
  #ax2 = fig.add_subplot(gs[2], sharex=ax1)
  ax = plt.subplot(3,3,(1,2) )
  ax1 = plt.subplot(3,3,(4,5) )
  ax2 = plt.subplot(3,3,(7,8) )
  axx = plt.subplot(3,3,3 )
  axx1 = plt.subplot(3,3,6 )
  axx2 = plt.subplot(3,3,9 )
  plt.subplots_adjust(wspace=0.3, hspace=0.15)
  if include_last_update_title:
    ax.set_title("last update: %s UTC" % (UTCDateTime().now().strftime("%Y/%m/%d %H:%M:%S")), fontsize=6, loc='right' )
    fig.suptitle("%s  -  %s" % (tr_z.stats.starttime.strftime("%Y-%m-%d %H:%M:%S"), tr_z.stats.endtime.strftime("%Y-%m-%d %H:%M:%S")), y=0.97, fontsize=10)
  else:
    fig.suptitle("%s  -  %s" % (tr_z.stats.starttime.strftime("%Y-%m-%d %H:%M:%S"), tr_z.stats.endtime.strftime("%Y-%m-%d %H:%M:%S")), y=0.94, fontsize=10)


  if tr_z.stats.endtime-tr_z.stats.starttime>86400:
    date_format = DateFormatter('%Y-%m-%d\n%H:%M:%S')
  else:
    date_format = DateFormatter('%H:%M:%S')

  for axxx in [ax,ax1,ax2]:
    axxx.minorticks_on()
    axxx.tick_params(axis='both', which='major', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in')
    axxx.tick_params(axis='both', which='minor', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in')
    axxx.tick_params(axis='x', which='major', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in', rotation=20)
    axxx.tick_params(axis='x', which='minor', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in', rotation=20)
    axxx.spines['right'].set_visible(True)
    axxx.spines['top'].set_visible(False)
    axxx.spines['left'].set_visible(True)
    axxx.spines['bottom'].set_visible(False)
    axxx.xaxis.set_major_formatter(date_format)
    axxx.set_xlim([ date2num(tr_z.stats.starttime.datetime), date2num(tr_z.stats.endtime.datetime) ])
    axxx.grid(axis='x', lw=0.7, ls=':', color='0.4')
    if vel_units:
      axxx.set_ylabel("Velocity (m/s)", fontsize=8)
    else:
      axxx.set_ylabel("Counts", fontsize=8)

  for axxx in [axx,axx1,axx2]:
    axxx.minorticks_on()
    axxx.tick_params(axis='both', which='major', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in')
    axxx.tick_params(axis='both', which='minor', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in')
    axxx.tick_params(axis='x', which='major', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in', rotation=0)
    axxx.tick_params(axis='x', which='minor', labelsize=8, bottom='off', top='off', left='on', right='on', direction='in', rotation=0)
    axxx.spines['right'].set_visible(True)
    axxx.spines['top'].set_visible(False)
    axxx.spines['left'].set_visible(True)
    axxx.spines['bottom'].set_visible(False)
    axxx.set_xlim([ 0, 10 ])
    axxx.grid(axis='x', lw=0.7, ls=':', color='0.4')
    axxx.set_ylabel("Power", fontsize=8)
 
  # vertical
  ax.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
  ax.yaxis.offsetText.set_fontsize(6)
  plt.setp( ax.get_xticklabels(), visible=False)
  ax.plot(tr_z.times("matplotlib"), tr_z.data, lw=0.45, color="r", zorder=0, label=tr_z.id)
  ax.legend(fontsize=6, loc=1)

  Fs = tr_z.stats.sampling_rate       # sampling rate
  Ts = 1.0/Fs                       # sampling interval
  n = tr_z.stats.npts                 # length of the signal
  k = np.arange(n)
  T = n/Fs
  frq = k/T                         # two sides frequency range
  frq = frq[range(int(n/2))]        # one side frequency range
  Y = np.fft.fft(tr_z.data)           # fft computing
  #Y = Y/n                           # normalization
  fft_amp = abs(Y[range(int(n/2))]) 
  power_spectrum_db = 10*np.log10(2*(fft_amp**2)/n)
  axx.plot(frq, fft_amp**2, lw=0.5, color="r", zorder=0)
  axx.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
  axx.yaxis.offsetText.set_fontsize(6)
  plt.setp( axx.get_xticklabels(), visible=False)


  # northing
  ax1.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
  ax1.yaxis.offsetText.set_fontsize(6)
  plt.setp( ax1.get_xticklabels(), visible=False)
  ax1.plot(tr_n.times("matplotlib"), tr_n.data, lw=0.45, color="r", zorder=0, label=tr_n.id)
  ax1.legend(fontsize=6, loc=1)

  Fs = tr_n.stats.sampling_rate       # sampling rate
  Ts = 1.0/Fs                       # sampling interval
  n = tr_n.stats.npts                 # length of the signal
  k = np.arange(n)
  T = n/Fs
  frq = k/T                         # two sides frequency range
  frq = frq[range(int(n/2))]        # one side frequency range
  Y = np.fft.fft(tr_n.data)           # fft computing
  #Y = Y/n                           # normalization
  fft_amp = abs(Y[range(int(n/2))]) 
  power_spectrum_db = 10*np.log10(2*(fft_amp**2)/n)
  axx1.plot(frq, fft_amp**2, lw=0.5, color="r", zorder=0)
  axx1.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
  axx1.yaxis.offsetText.set_fontsize(6)
  plt.setp( axx1.get_xticklabels(), visible=False)


  # easting
  ax2.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
  ax2.yaxis.offsetText.set_fontsize(6)
  plt.setp( ax2.get_xticklabels(), visible=True)
  ax2.plot(tr_e.times("matplotlib"), tr_e.data, lw=0.45, color="r", zorder=0, label=tr_e.id)
  ax2.legend(fontsize=6, loc=1)

  Fs = tr_e.stats.sampling_rate       # sampling rate
  Ts = 1.0/Fs                       # sampling interval
  n = tr_e.stats.npts                 # length of the signal
  k = np.arange(n)
  T = n/Fs
  frq = k/T                         # two sides frequency range
  frq = frq[range(int(n/2))]        # one side frequency range
  Y = np.fft.fft(tr_e.data)           # fft computing
  #Y = Y/n                           # normalization
  fft_amp = abs(Y[range(int(n/2))]) 
  power_spectrum_db = 10*np.log10(2*(fft_amp**2)/n)
  axx2.plot(frq, fft_amp**2, lw=0.5, color="r", zorder=0)
  axx2.ticklabel_format(axis='y', style='sci', scilimits=(-1, 1))
  axx2.yaxis.offsetText.set_fontsize(6)
  plt.setp( axx2.get_xticklabels(), visible=True)

  ax2.set_xlabel("UTC time (local time = UTC - 03:00)", fontsize=8 )
  axx2.set_xlabel("Frequency (Hz)", fontsize=8 )


  if len(gaps)>0:
    for gap in gaps:
      t1 = date2num(UTCDateTime(gap[4]).datetime)
      t2 = date2num(UTCDateTime(gap[5]).datetime)
      if dark_background:
        y1 = ax.get_ylim()[0]; y2 = ax.get_ylim()[1]
        ax.add_patch( Rectangle((t1, y1), t2-t1, y2-y1, fill=True, edgecolor=None, facecolor='k', alpha=1, linewidth=0, zorder=1) )
        y1 = ax1.get_ylim()[0]; y2 = ax1.get_ylim()[1]
        ax1.add_patch( Rectangle((t1, y1), t2-t1, y2-y1, fill=True, edgecolor=None, facecolor='k', alpha=1, linewidth=0, zorder=1) )
        y1 = ax2.get_ylim()[0]; y2 = ax2.get_ylim()[1]
        ax2.add_patch( Rectangle((t1, y1), t2-t1, y2-y1, fill=True, edgecolor=None, facecolor='k', alpha=1, linewidth=0, zorder=1) )
      else:
        y1 = ax.get_ylim()[0]; y2 = ax.get_ylim()[1]
        ax.add_patch( Rectangle((t1, y1), t2-t1, y2-y1, fill=True, edgecolor=None, facecolor='white', alpha=1, linewidth=0, zorder=1) )
        y1 = ax1.get_ylim()[0]; y2 = ax1.get_ylim()[1]
        ax1.add_patch( Rectangle((t1, y1), t2-t1, y2-y1, fill=True, edgecolor=None, facecolor='white', alpha=1, linewidth=0, zorder=1) )
        y1 = ax2.get_ylim()[0]; y2 = ax2.get_ylim()[1]
        ax2.add_patch( Rectangle((t1, y1), t2-t1, y2-y1, fill=True, edgecolor=None, facecolor='white', alpha=1, linewidth=0, zorder=1) )



  if dark_background:
    ax.patch.set_facecolor('k')
  else:
    ax.patch.set_facecolor('w')


  if savedir:
    outdir = "%s/%s" % (savedir, st[0].stats.station)
    if not os.path.isdir(outdir):
      os.makedirs(outdir)

    figname = "%s/%s_waveforms.jpg" % (outdir, st[0].stats.station)
    plt.savefig(figname, dpi=300, bbox_inches='tight', transparent=False)
    if not show_plot:
      plt.close('all')

  if show_plot:
    plt.show()
    plt.close('all')



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 



def plot_record_section(st, stations, stlons, stlats, evtime, evlon, evlat, freqmin=2, freqmax=10, dark_background=False, show_plot=False, savedir=None):

  if dark_background:
    plt.style.use(['dark_background'])
  else:
    plt.style.use(['default'])

  for tr in st:
    try:
      #tr.detrend('demean')
      #tr.detrend('linear')
      #tr.taper(max_percentage=0.005,type='hann')
      tr = remove_instrument_response(tr, pre_filt=(0.01, 0.02, 50, 100), detrend=True, taper=True, dataless_file=None)
      tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=2)
      ind = np.where( tr.stats.station == stations )[0][0]
      tr.stats.distance = calc_vincenty_inverse( stlats[ind], stlons[ind], evlat, evlon )[0]
    except:
      st.remove(tr)
      continue


  fig = plt.figure()
  st.plot(type='section', method='full', orientation='vertical', time_down=True, linewidth=.25, grid_linewidth=.25, show=False, fig=fig )
  ax = fig.axes[0]
  fig.suptitle("Origin Time: %s" % (evtime.strftime("%Y-%m-%d %H:%M:%S")), fontsize=10)
  transform = blended_transform_factory(ax.transData, ax.transAxes)
  for tr in st:
    ax.text(tr.stats.distance / 1e3, 1.0, tr.stats.station, fontsize=6, rotation=45, va="bottom", ha="center", transform=transform, zorder=10)


  if dark_background:
    ax.patch.set_facecolor('k')
  else:
    ax.patch.set_facecolor('w')

  if savedir:
    outdir = "%s" % (savedir)
    if not os.path.isdir(outdir):
      os.makedirs(outdir)

    figname = "%s/record_section.jpg" % (outdir)
    plt.savefig(figname, dpi=300, bbox_inches='tight', transparent=False)
    if not show_plot:
      plt.close('all')

  if show_plot:
    plt.show()
    plt.close('all')



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def plot_triggers_stalta(networks, stations, starttime, endtime, freqmin=1, freqmax=10, include_trig_off=False, dark_background=False, show_plot=False, save_plot=True, savedir=None, format='jpg', dpi=150):

  if dark_background:
    plt.style.use(['dark_background'])
  else:
    plt.style.use(['default'])

  fig = plt.figure(dpi=dpi)
  adjustFigAspect(fig,aspect=1.75)
  gs = mpl.gridspec.GridSpec(len(stations), 1, hspace=0.) 
  date_format = DateFormatter('%H:%M:%S')

  for i in range(len(stations)):
    network = networks[i]
    station = stations[i]

    if i == 0:
      ax1 = fig.add_subplot(gs[i])
      ax1.set_title("last update: %s UTC" % (UTCDateTime().now().strftime("%Y/%m/%d %H:%M:%S")), fontsize=4.5, loc='right')
    else:
      ax1 = fig.add_subplot(gs[i], sharex=ax1)

    ax1.minorticks_on()
    ax1.tick_params(axis='both', which='major', labelsize=8, bottom='off', top='off', left='off', right='off', direction='in')
    ax1.tick_params(axis='both', which='minor', labelsize=8, bottom='off', top='off', left='off', right='off', direction='in')
    ax1.tick_params(axis='x', which='major', labelsize=6, bottom='off', top='off', left='off', right='off', direction='in', rotation=0)
    ax1.tick_params(axis='x', which='minor', labelsize=6, bottom='off', top='off', left='off', right='off', direction='in', rotation=0)
    ax1.spines['right'].set_visible(False)
    ax1.spines['top'].set_visible(False)
    ax1.spines['left'].set_visible(False)
    ax1.spines['bottom'].set_visible(False)
    ax1.set_ylabel(station, fontsize=6, rotation=0, labelpad=10)
    ax1.xaxis.set_major_formatter(date_format)
    ax1.set_xlim([ date2num(starttime.datetime), date2num(endtime.datetime) ])
    plt.setp( ax1.get_yticklabels(), visible=False)
    plt.setp(ax1.spines.values(), linewidth=.5)
    ax1.grid(axis='x', lw=0.7, ls=':', color='0.75')

    if i == len(stations)-1:
      plt.setp( ax1.get_xticklabels(), visible=True)
    else:
      plt.setp( ax1.get_xticklabels(), visible=False)

    try:
      st, gaps = get_streams_gema([network], [station], starttime, endtime, only_vertical_channel=True, local_dir_name=None)
      tr = st.select(station=station)
      if len(tr)>0:
        tr = tr[0]
        if not isinstance(tr.data, np.ma.masked_array):
          tr.detrend('demean')
          tr.detrend('linear')
          tr.taper(max_percentage=0.005,type='hann')
          tr.filter("bandpass", freqmin=freqmin, freqmax=freqmax, corners=2)

        if dark_background:
          ax1.plot(tr.times("matplotlib"), tr.data, lw=0.09, color="0.85")
        else:
          ax1.plot(tr.times("matplotlib"), tr.data, lw=0.09, color="k")

      triggers_list = select_triggers_stalta(station, starttime, endtime)
      for trigger in triggers_list:
        on = date2num(trigger[0].datetime)
        off = date2num(trigger[1].datetime)
        y1 = ax1.get_ylim()[0]; y2 = ax1.get_ylim()[1]
        ax1.axvline(on, y1, y2, lw=0.5, color='r', zorder=100, clip_on=True, alpha=0.9)
        #ax1.annotate( "P", (on, y2), color='r', zorder=100, fontsize=6)
        if include_trig_off:
          ax1.axvline(off, y1, y2, lw=0.7, color='g', zorder=100)
          if dark_background:
            ax1.add_patch( Rectangle((on, y1), off-on, y2-y1, fill=True, edgecolor=None, facecolor='w', alpha=0.8, linewidth=0, zorder=10) )
          else:
            ax1.add_patch( Rectangle((on, y1), off-on, y2-y1, fill=True, edgecolor=None, facecolor='w', alpha=0.8, linewidth=0, zorder=10) )

    except:
      continue



  if save_plot:
    if savedir is None:
      outdir = "figs" 
    else:
      outdir = "%s" % (savedir)

    if not os.path.isdir(outdir):
      os.makedirs(outdir)

    figname = "%s/triggers_stalta.%s" % (outdir, format)
    plt.savefig(figname, dpi=dpi, bbox_inches='tight', transparent=False)
    if not show_plot:
      plt.close('all')

  if show_plot:
    plt.show()
    plt.close('all')






