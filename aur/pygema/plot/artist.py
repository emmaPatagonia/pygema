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
from matplotlib.patches import Rectangle
from matplotlib.dates import date2num, num2date, DateFormatter
from obspy.core import UTCDateTime, read, Stream
import sys, os, glob, datetime, MySQLdb, time, socket, subprocess


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def adjustFigAspect(fig,aspect=1):
  xsize,ysize = fig.get_size_inches()
  minsize = min(xsize,ysize)
  xlim = .4*minsize/xsize
  ylim = .4*minsize/ysize
  if aspect < 1:
    xlim *= aspect
  else:
    ylim /= aspect

  fig.subplots_adjust(left=.5-xlim, right=.5+xlim, bottom=.5-ylim, top=.5+ylim)

