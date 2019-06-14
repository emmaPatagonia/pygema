from obspy.core import read, UTCDateTime, Stream
import numpy as np
import os, glob, time


#######################################################################
stname_scream = "pomso"

stname_seiscomp = "LONQ"
stnet_seiscomp = "GM"
stch_seiscomp = ["BHZ", "BHN", "BHE"]

deadtime = 5
while True:
  utc_now = UTCDateTime()
  try:
    for channel in stch_seiscomp:
      st = Stream()
      gcffiles = glob.glob("%s/scream/data/%s/%s/%s/%s_*%s*_%s_*.gcf" % (os.getenv("HOME"), utc_now.strftime("%Y"), utc_now.strftime("%Y%m"), utc_now.strftime("%Y%m%d"), stname_scream, channel[2].lower(), utc_now.strftime("%Y%m%d")))
      for gcffile in gcffiles:
        st += read(gcffile)

      if len(st)>0:
        for tr in st:
          tr.stats.network = stnet_seiscomp
          tr.stats.station = stname_seiscomp
          tr.stats.channel = channel

        msfile = "%s/archive/%s/%s/%s/%s.D/%s.%s..%s.D.%s" % (os.getenv("HOME"), utc_now.strftime("%Y"), stnet_seiscomp, stname_seiscomp,channel, stnet_seiscomp,stname_seiscomp, channel, utc_now.strftime("%4Y.%03j"))
        st.sort()
        st.write(msfile, format="MSEED", reclen=512)

  except:
    pass

  time.sleep(deadtime)

