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
from mpl_toolkits.basemap import Basemap, shiftgrid, cm
from obspy.core import UTCDateTime
from obspy.geodetics.base import kilometer2degrees
import os, site
from pygema.core.parameters import load_volcanoes, load_station_metadata
from pygema.db.ask import select_events_manual_loc



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def plot_map(elon, wlon, nlat, slat, res='c', dpi=300, xpixels=800, add_holocene_volcanoes=False, add_seismic_stations=False, add_ralco=False, add_labels=False, add_faults=False, add_events_pygemadb=False, evstarttime=UTCDateTime(1970,1,1), evendtime=UTCDateTime(), dark_background=False, show_plot=True, savedir=None):

  if dark_background:
    plt.style.use(['dark_background'])
  else:
    plt.style.use(['default'])

  # add basemap matplotlib
  fig = plt.figure()
  ax = plt.axes([0.1, 0.3, 0.9, 0.6])
  map = Basemap(llcrnrlon=wlon, urcrnrlon=elon, llcrnrlat=slat, urcrnrlat=nlat, projection='mill', resolution=res, area_thresh=1000000, epsg=4269)
  if add_events_pygemadb:  
    map.drawparallels(np.arange(slat,nlat, abs(nlat-slat)/6.), dashes=[1,2], labels=[1,0,0,0], linewidth=0.004,fontsize=6, zorder=100, labelstyle='+/-')
    map.drawmeridians(np.arange(wlon,elon, abs(wlon-elon)/6.), dashes=[1,2], labels=[0,0,1,0], linewidth=0.004,fontsize=6, zorder=100, labelstyle='+/-')
  else:
    map.drawparallels(np.arange(slat,nlat, abs(nlat-slat)/6.), dashes=[1,2], labels=[1,1,0,0], linewidth=0.004,fontsize=6, zorder=100, labelstyle='+/-')
    map.drawmeridians(np.arange(wlon,elon, abs(wlon-elon)/6.), dashes=[1,2], labels=[0,0,1,1], linewidth=0.004,fontsize=6, zorder=100, labelstyle='+/-')

  map.drawcoastlines(linewidth=0.8,zorder=5)
  map.drawcountries(linewidth=0.5, linestyle='-',zorder=2)

  map.arcgisimage(service='World_Shaded_Relief', xpixels=xpixels, dpi=dpi, verbose=False, zorder=1)
  img = map.arcgisimage(service='ESRI_Imagery_World_2D', xpixels=xpixels, dpi=dpi, verbose=False, zorder=0)
  img.set_alpha(0.5)

  if add_events_pygemadb:
    map.fillcontinents(color='0.1', lake_color='steelblue',alpha=0.4,zorder=1000-1)

  alpha = 1.0
  color_stations = 'c'
  color_volcanoes = 'r'
  color_faults = 'k'
  color_places = 'g'

  # add volcanoes
  if add_holocene_volcanoes:
    vnames, xvolcs, yvolcs = load_volcanoes()
    xvolcs, yvolcs = map(xvolcs,yvolcs)
    ax.plot(xvolcs,yvolcs,marker='^',color='None',markeredgecolor=color_volcanoes,markeredgewidth=1.5,lw=0.,ms=4.5,zorder=10,alpha=alpha, clip_on=True)

    if add_labels:
      for x,y,vname in zip(xvolcs,yvolcs,vnames):
        if vname == 'Tolhuaca':
          ax.annotate('Vn. '+vname+'\n', (x, y), color='k', weight='bold', fontsize=4.5, ha='center', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Copahue':
          ax.annotate('   Vn. '+vname, (x, y), color='k', weight='bold', fontsize=4.5, ha='left', va='top', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Callaqui':
          ax.annotate(' Vn. '+vname, (x, y), color='k', weight='bold', fontsize=4.5, ha='left', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Trolon':
          ax.annotate('Vn. '+vname+' ', (x, y), color='k', weight='bold', fontsize=4.5, ha='right', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Lonquimay':
          ax.annotate('  Vn. '+vname, (x, y), color='k', weight='bold', fontsize=4.5, ha='left', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')


  # add stations
  if add_seismic_stations:
    networks, stations, stlons, stlats, stalts = load_station_metadata()
    for net, stat, lon, lat in zip(networks, stations, stlons, stlats):
      x,y = map(float(lon),float(lat))
      ax.scatter(x,y,marker='s',color='None', s=20, zorder=2000+2, alpha=alpha, clip_on=True, lw=1.4, edgecolors=color_stations)
      if add_labels:
        ax.annotate("\n\n "+stat, (x, y), weight='bold', fontsize=5, ha='left', va='center', clip_on=True, zorder=2000+3)


  # add ralco
  if add_ralco:
    si,sj = map(-71.611683, -37.910428)
    map.scatter(si, sj, c=color_places, linewidths=0.3, edgecolors='k', alpha=alpha, zorder=15, marker='X', s=30, clip_on=True)
    if add_labels:
      ax.annotate('Pangue  ',xy=(si,sj), ha='right', va='top', color='k',zorder=15,fontsize=4.5,fontweight='bold',xytext=(-0,-0),textcoords='offset points', fontstyle='normal', clip_on=True)

    si,sj = map(-71.475571, -38.046040)
    map.scatter(si, sj, c=color_places, linewidths=0.3, edgecolors='k', alpha=alpha, zorder=15, marker='X', s=30, clip_on=True)
    if add_labels:
      ax.annotate('Ralco  ',xy=(si,sj), ha='right', va='top', color='k',zorder=15,fontsize=4.5,fontweight='bold',xytext=(-0,-0),textcoords='offset points', fontstyle='normal', clip_on=True)


  # add faults
  if add_faults:
    PYGEMA_PATH = "%s/pygema" % (site.getsitepackages()[0])
    shp = map.readshapefile(PYGEMA_PATH+'/src/shapes/gerd/fallas_sielfeld_etal_2019_mod', 'fallas', drawbounds=False)
    types = np.unique(np.array([ info['Name']  for info, shape in zip(map.fallas_info, map.fallas) ]))
    #print("types of faults =", types)
    for info, shape in zip(map.fallas_info, map.fallas):
      if info['Name'] == 'LOFS':
        x, y = zip(*shape) 
        map.plot(x, y, marker=None, color=color_faults, alpha=alpha, linestyle='-', linewidth=0.5, zorder=9, clip_on=True)
      elif info['Name'] == 'ATF':
        x, y = zip(*shape) 
        map.plot(x, y, marker=None, color=color_faults, alpha=alpha, linestyle='--', linewidth=0.5, zorder=9, clip_on=True)



  # add seismicity
  if add_events_pygemadb:
    events_list = select_events_manual_loc( evstarttime, evendtime, table="LOC")

    zmin = 0
    zmax = 30
    cmap = plt.cm.jet_r #plt.cm.jet_r  #plt.cm.gnuplot_r
    nlevels = 10
    bounds = np.linspace(zmin, zmax,nlevels, endpoint=True)
    levels = np.linspace(zmin, zmax,nlevels, endpoint=True)
    norm = mpl.colors.BoundaryNorm(bounds, cmap.N)
  
    x = []; y = []; z = []; s = []; xerr = []; yerr = []; zerr = []
    for event in events_list:
      evlon = event[1]
      evlat = event[2]
      evdep = event[3]
      evmag = event[4]
      evdx = event[5]
      evdy = event[6]
      evdz = event[7]

      xi,yi = map(evlon, evlat)
      evdx_deg = kilometer2degrees(evdx)
      evdy_deg = kilometer2degrees(evdy)
      xi_new, yi_new = map(evlon+evdx_deg, evlat+evdy_deg)
      xi_err = abs(xi - xi_new)
      yi_err = abs(yi - yi_new)

      x.append(xi); y.append(yi); z.append(evdep); s.append( evmag**3*1.5 ); xerr.append(xi_err); yerr.append(yi_err); zerr.append(evdz)

    ax.scatter(x,y,c=z, cmap=cmap, s=s, marker='o', norm=norm, zorder=1000, alpha=1, clip_on=True, lw=0.3, edgecolors='k')
    #ax.errorbar(x, y, xerr=xerr, yerr=yerr, elinewidth=0.4, ecolor='0.4', capsize=1.7, capthick=0.4, linewidth=0, zorder=19)



  # add legend
  s1 = plt.scatter([], [], c='None', linewidths=1.4, edgecolors=color_stations, alpha=alpha, zorder=1000, marker='s', s=15)
  s2, = plt.plot([], [], c='None', markeredgecolor=color_volcanoes, markeredgewidth=1.4, lw=0., alpha=alpha, zorder=1000, marker='^', ms=4)
  f1, = plt.plot([-100,-99], [-100,-98], c=color_faults, linewidth=0.8, alpha=alpha, zorder=1000, ls='-') # darkgoldenrod
  f2, = plt.plot([-100,-99], [-100,-98], c=color_faults, linewidth=0.8, alpha=alpha, zorder=1000, ls='--') # darkgoldenrod

  leg = plt.legend([s1, s2, f1, f2], 
                   [ 'Estación Sísmica', 'Volcán', 'Sistema de Fallas\nLiquiñe-Ofqui', 'Falla Biobio' ], 
                 fontsize=4, ncol=1, frameon=True, fancybox=True, shadow=False, framealpha=0.6, loc=4 ) 
  leg.set_zorder(1000)



  if add_events_pygemadb:
    ax2 = plt.axes([0.362, 0.1, 0.375, 0.17])
    ax2.minorticks_on()
    ax2.tick_params(axis='both', which='major', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in')
    ax2.tick_params(axis='both', which='minor', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in')
    ax2.tick_params(axis='x', which='major', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
    ax2.tick_params(axis='x', which='minor', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
    ax2.spines['right'].set_visible(True)
    ax2.spines['top'].set_visible(True)
    ax2.spines['left'].set_visible(True)
    ax2.spines['bottom'].set_visible(True)
    ax2.set_xlim(wlon, elon)
    ax2.set_ylim(zmax, zmin)
    labels = [ r"%.1f$^{\circ}$" % (item) for item in ax2.get_xticks().tolist() ]
    ax2.set_xticklabels(labels)
    ax2.set_ylabel("Prof. (km)", fontsize=6)

    x = []; y = []; z = []; s = []; xerr = []; yerr = []; zerr = []
    for event in events_list:
      evlon = event[1]
      evlat = event[2]
      evdep = event[3]
      evmag = event[4]
      evdx = event[5]
      evdy = event[6]
      evdz = event[7]

      evdx_deg = kilometer2degrees(evdx)
      evdy_deg = kilometer2degrees(evdy)
      xi_new, yi_new = evlon+evdx_deg, evlat+evdy_deg
      xi_err = abs(evlon - xi_new)
      yi_err = abs(evlat - yi_new)

      x.append(evlon); y.append(evlat); z.append(evdep); s.append( evmag**3*1.5 ); xerr.append(xi_err); yerr.append(yi_err); zerr.append(evdz)

    ax2.scatter(x,z,c=z, cmap=cmap, s=s, marker='o', norm=norm, zorder=20, alpha=0.8, clip_on=True, lw=0.3, edgecolors='k')
    #ax2.errorbar(x, z, xerr=xerr, yerr=zerr, elinewidth=0.4, ecolor='0.4', capsize=1.7, capthick=0.4, linewidth=0, zorder=19)


    ax3 = plt.axes([0.76, 0.3, 0.15, 0.6])
    ax3.minorticks_on()
    ax3.yaxis.tick_right()
    ax3.xaxis.tick_top()
    ax3.tick_params(axis='both', which='major', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in')
    ax3.tick_params(axis='both', which='minor', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in')
    ax3.tick_params(axis='x', which='major', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
    ax3.tick_params(axis='x', which='minor', labelsize=6, bottom='on', top='on', left='on', right='on', direction='in', rotation=0)
    ax3.spines['right'].set_visible(True)
    ax3.spines['top'].set_visible(True)
    ax3.spines['left'].set_visible(True)
    ax3.spines['bottom'].set_visible(True)
    ax3.set_xlim(zmin, zmax)
    ax3.set_ylim(slat, nlat)
    labels = [ r"%.1f$^{\circ}$" % (item) for item in ax3.get_yticks().tolist() ]
    ax3.set_yticklabels(labels)
    ax3.set_xlabel("Profundidad (km)", fontsize=6)
    ax3.xaxis.set_label_position('top')

    x = []; y = []; z = []; s = []; xerr = []; yerr = []; zerr = []
    for event in events_list:
      evlon = event[1]
      evlat = event[2]
      evdep = event[3]
      evmag = event[4]
      evdx = event[5]
      evdy = event[6]
      evdz = event[7]

      evdx_deg = kilometer2degrees(evdx)
      evdy_deg = kilometer2degrees(evdy)
      xi_new, yi_new = evlon+evdx_deg, evlat+evdy_deg
      xi_err = abs(evlon - xi_new)
      yi_err = abs(evlat - yi_new)

      x.append(evlon); y.append(evlat); z.append(evdep); s.append( evmag**3*1.5 ); xerr.append(xi_err); yerr.append(yi_err); zerr.append(evdz)

    ax3.scatter(z,y,c=z, cmap=cmap, s=s, marker='o', norm=norm, zorder=20, alpha=0.8, clip_on=True, lw=0.3, edgecolors='k')
    #ax3.errorbar(z, y, xerr=zerr, yerr=yerr, elinewidth=0.4, ecolor='0.4', capsize=1.7, capthick=0.4, linewidth=0, zorder=19)

    ml2 = plt.scatter([],[], marker='o',color='w', s=1.5**3*1.5, zorder=2000+2, alpha=1, clip_on=True, lw=1, edgecolors='k')
    ml3 = plt.scatter([],[], marker='o',color='w', s=2.5**3*1.5, zorder=2000+2, alpha=1, clip_on=True, lw=1, edgecolors='k')
    ml4 = plt.scatter([],[], marker='o',color='w', s=3.5**3*1.5, zorder=2000+2, alpha=1, clip_on=True, lw=1, edgecolors='k')
    ml5 = plt.scatter([],[], marker='o',color='w', s=4.5**3*1.5, zorder=2000+2, alpha=1, clip_on=True, lw=1, edgecolors='k')

    leg = plt.legend([ml5, ml4, ml3, ml2, s1, s2, f1, f2], 
                 ['4.5', '3.5', '2.5', '1.5' ], title=r"M$_l$", title_fontsize=6, labelspacing=1.2, 
                 fontsize=6, ncol=1, frameon=True, fancybox=True, shadow=False, framealpha=0.2, bbox_to_anchor=(0.55, -0.03) ) 
    leg.set_zorder(1000)
    frame = leg.get_frame()
    frame.set_facecolor('w')

    sm = plt.cm.ScalarMappable(cmap=cmap, norm=norm)
    sm._A = []
    cax = fig.add_axes([0.87, 0.108, 0.017, 0.14])
    cb1 = plt.colorbar(sm, format='%i', extend='neither', norm=norm, spacing='proportional', orientation='vertical', cax=cax, ticks = np.linspace(zmin, zmax, 5)) 
    cb1.ax.invert_yaxis()
    cb1.set_label('Profundidad (km)', size=6)
    for j in cb1.ax.get_yticklabels(): j.set_fontsize(6)


  if dark_background:
    ax.patch.set_facecolor('k')
  else:
    ax.patch.set_facecolor('w')

  if savedir:
    outdir = "%s" % (savedir)
    if not os.path.isdir(outdir):
      os.makedirs(outdir)

    figname = "%s/map.jpg" % (outdir)
    plt.savefig(figname, dpi=300, bbox_inches='tight', transparent=False)
    if not show_plot:
      plt.close('all')

  if show_plot:
    plt.show()
    plt.close('all')



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def plot_map_event(evlon, evlat, evdep, dlon=0.3, dlat=0.3, res='c', dpi=300, xpixels=800, add_holocene_volcanoes=False, add_seismic_stations=False, add_ralco=False, add_labels=False, add_faults=False, dark_background=False, show_plot=True, savedir=None):

  if dark_background:
    plt.style.use(['dark_background'])
  else:
    plt.style.use(['default'])

  # add basemap matplotlib

  elon = evlon+dlon
  wlon = evlon-dlon
  nlat = evlat+dlat
  slat = evlat-dlat

  fig = plt.figure()
  ax = plt.axes([0.1, 0.3, 0.9, 0.6])
  map = Basemap(llcrnrlon=wlon, urcrnrlon=elon, llcrnrlat=slat, urcrnrlat=nlat, projection='mill', resolution=res, area_thresh=1000000, epsg=4269)
  map.drawparallels(np.arange(slat,nlat, abs(nlat-slat)/6.), dashes=[1,2], labels=[1,1,0,0], linewidth=0.004,fontsize=6, zorder=100, labelstyle='+/-')
  map.drawmeridians(np.arange(wlon,elon, abs(wlon-elon)/6.), dashes=[1,2], labels=[0,0,1,1], linewidth=0.004,fontsize=6, zorder=100, labelstyle='+/-')
  map.drawcoastlines(linewidth=0.8,zorder=5)
  map.drawcountries(linewidth=0.5, linestyle='-',zorder=2)
  #map.fillcontinents(color='0.3', lake_color='steelblue',alpha=0.4,zorder=1)
  #img = map.arcgisimage(service='NatGeo_World_Map', xpixels=xpixels, dpi=dpi, verbose=False, zorder=0)
  #img = map.arcgisimage(service='World_Topo_Map', xpixels=xpixels, dpi=dpi, verbose=False, zorder=0)
  #img = map.arcgisimage(service='ESRI_StreetMap_World_2D', xpixels=xpixels, dpi=dpi, verbose=False, zorder=0)
  map.arcgisimage(service='World_Shaded_Relief', xpixels=xpixels, dpi=dpi, verbose=False, zorder=1)
  img = map.arcgisimage(service='ESRI_Imagery_World_2D', xpixels=xpixels, dpi=dpi, verbose=False, zorder=0)
  img.set_alpha(0.6)
  color_stations = 'y'
  color_volcanoes = 'r'
  color_faults = 'k'
  color_places = 'c'
  alpha = 1.0

  # add event
  xi,yi = map(evlon, evlat)
  ax.scatter(xi,yi,color='green', s=120, marker='*', zorder=200, alpha=1, clip_on=True, lw=0.7, edgecolors='k')


  # add volcanoes
  if add_holocene_volcanoes:
    vnames, xvolcs, yvolcs = load_volcanoes()
    xvolcs, yvolcs = map(xvolcs,yvolcs)
    ax.plot(xvolcs,yvolcs,marker='^',color='None',markeredgecolor=color_volcanoes,markeredgewidth=1.7,lw=0.,ms=5.,zorder=10,alpha=alpha, clip_on=True)

    if add_labels:
      for x,y,vname in zip(xvolcs,yvolcs,vnames):
        if vname == 'Tolhuaca':
          ax.annotate('V. '+vname+'\n', (x, y), color='k', weight='bold', fontsize=4.5, ha='center', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Copahue':
          ax.annotate('   V. '+vname, (x, y), color='k', weight='bold', fontsize=4.5, ha='left', va='top', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Callaqui':
          ax.annotate(' V. '+vname, (x, y), color='k', weight='bold', fontsize=4.5, ha='left', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Trolon':
          ax.annotate('V. '+vname+' ', (x, y), color='k', weight='bold', fontsize=4.5, ha='right', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')
        elif vname == 'Lonquimay':
          ax.annotate('  V. '+vname, (x, y), color='k', weight='bold', fontsize=4.5, ha='left', va='bottom', clip_on=True, zorder=2000+3, fontstyle='normal')


  # add stations
  if add_seismic_stations:
    networks, stations, stlons, stlats, stalts = load_station_metadata()
    for net, stat, lon, lat in zip(networks, stations, stlons, stlats):
      x,y = map(float(lon),float(lat))
      ax.scatter(x,y,marker='s',color='None', s=20, zorder=2000+2, alpha=alpha, clip_on=True, lw=1.4, edgecolors=color_stations)
      if add_labels:
        ax.annotate(stat, (x, y), weight='bold', fontsize=4.5, ha='left', va='center', clip_on=True, zorder=2000+3)


  # add ralco
  if add_ralco:
    si,sj = map(-71.611683, -37.910428)
    map.scatter(si, sj, c=color_places, linewidths=0.3, edgecolors='k', alpha=alpha, zorder=15, marker='X', s=40, clip_on=True)
    if add_labels:
      ax.annotate('Pangue  ',xy=(si,sj), ha='right', va='top', color='k',zorder=15,fontsize=4.5,fontweight='bold',xytext=(-0,-0),textcoords='offset points', fontstyle='normal', clip_on=True)

    si,sj = map(-71.475571, -38.046040)
    map.scatter(si, sj, c=color_places, linewidths=0.3, edgecolors='k', alpha=alpha, zorder=15, marker='X', s=40, clip_on=True)
    if add_labels:
      ax.annotate('Ralco  ',xy=(si,sj), ha='right', va='top', color='k',zorder=15,fontsize=4.5,fontweight='bold',xytext=(-0,-0),textcoords='offset points', fontstyle='normal', clip_on=True)


  # add faults
  if add_faults:
    PYGEMA_PATH = "%s/pygema" % (site.getsitepackages()[0])
    shp = map.readshapefile(PYGEMA_PATH+'/src/shapes/gerd/fallas_sielfeld_etal_2019_mod', 'fallas', drawbounds=False)
    types = np.unique(np.array([ info['Name']  for info, shape in zip(map.fallas_info, map.fallas) ]))
    #print("types of faults =", types)
    for info, shape in zip(map.fallas_info, map.fallas):
      if info['Name'] == 'LOFS':
        x, y = zip(*shape) 
        map.plot(x, y, marker=None, color=color_faults, alpha=alpha, linestyle='-', linewidth=0.8, zorder=9, clip_on=True)
      elif info['Name'] == 'ATF':
        x, y = zip(*shape) 
        map.plot(x, y, marker=None, color=color_faults, alpha=alpha, linestyle='--', linewidth=0.8, zorder=9, clip_on=True)


  # add legend
  s1 = plt.scatter([], [], c='None', linewidths=1.4, edgecolors=color_stations, alpha=alpha, zorder=1000, marker='s', s=25)
  s2, = plt.plot([], [], c='None', markeredgecolor=color_volcanoes, markeredgewidth=1.4, lw=0., alpha=alpha, zorder=1000, marker='^', ms=5)
  f1, = plt.plot([-100,-99], [-100,-98], c=color_faults, linewidth=0.8, alpha=alpha, zorder=1000, ls='-') # darkgoldenrod
  f2, = plt.plot([-100,-99], [-100,-98], c=color_faults, linewidth=0.8, alpha=alpha, zorder=1000, ls='--') # darkgoldenrod

  leg = plt.legend([s2, s1, f1, f2], 
                   [ 'Volcano', 'Seismic station', 'Liquiñe-Ofqui\nFault System', 'Biobio Fault' ], 
                 fontsize=5, ncol=1, frameon=True, fancybox=True, shadow=False, framealpha=0.5, loc=4 ) 
  leg.set_zorder(1000)



  if dark_background:
    ax.patch.set_facecolor('k')
  else:
    ax.patch.set_facecolor('w')

  if savedir:
    outdir = "%s" % (savedir)
    if not os.path.isdir(outdir):
      os.makedirs(outdir)

    figname = "%s/map_event.jpg" % (outdir)
    plt.savefig(figname, dpi=300, bbox_inches='tight', transparent=False)
    if not show_plot:
      plt.close('all')

  if show_plot:
    plt.show()
    plt.close('all')








