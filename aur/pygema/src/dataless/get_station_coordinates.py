import numpy as np
import matplotlib.pyplot as plt
import glob, os, subprocess
from math import *
from obspy import read_inventory
from obspy.io.xseed import Parser

stations = ["COPA", "COP2", "CALL", "MAYA", "TOL1", "LONQ", "MANZ", "LR03", "BI02", "LC01", "BI03", "ML02", "BI04", "LC09", "LC02", "BI05"]
for station in stations:
  dataless = glob.glob("*%s.dataless" % (station))[0]
  network = dataless.split("_")[0]
  parser = Parser(dataless)
  dic = parser.get_coordinates("%s.%s..BHZ" % (network, station))
  latitude = dic["latitude"]
  longitude = dic["longitude"]
  elevation = dic["elevation"]
  #pattern = "GTSRCE %4s LATLON %.6f %.6f 0.0 %.3f" % (station, latitude, longitude, elevation/1000. )
  pattern = "%2s %4s %.6f %.6f %i" % (network, station, latitude, longitude, elevation )
  print(pattern)
