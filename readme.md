# PyGema
### Seismic data analysis tool box for volcano monitoring. 

Routines includes automatic localization of seismic events, 
computation of RSAM and SSAM, magnitude calculation and sql data management. 

by Diego M. González-Vidal (diego@gema.udec.cl)

contributors:
   Marcelo Ramirez (marcelo@gema.udec.cl)


#### PRE-REQUISITES (from yay)


yay aur/pyshapelib
aur/python-obspy
aur/python-pyshp
aur/pyshapelib
community/python-basemap
community/python-basemap-common
community/python-mysql-connector
community/python-mysqlclient
community/python-pillow
community/shapelib
community/sshfs
extra/jdk-openjdk
extra/python-django
extra/python-django


```
sudo pacman -S python-django jdk-openjdk sshfs shapelib python-pillow python-mysqlclient python-mysql-connector python-basemap-common python-basemap pyshapelib python-pyshp python-obspy pyshapelib python-django
```

## INSTALLATION
 Download the pygema package from github
```
git clone https://github.com/emmaPatagonia/pygema.git
sudo cp -r aur/pygema /usr/lib/python3.X/site-packages
```
The working space are at the folders realtime and scripts



## CONFIGURATION 
Add dataless files of the stations
```
pygema/src/dataless/NETWORK_STATION.dataless
```

Configure credentials, parameters, stations file and email

```
nano pygema/src/credentials_mysqldb_admin 
nano pygema/src/credentials_mysqldb_user
nano pygema/src/params_rsam.txt
nano pygema/src/params_ssam.txt
nano pygema/src/params_stalta.txt
nano pygema/src/stationALL.lst
nano pygema/db/email.py
nano pygema/db/credentials.py
nano pygema/db/client_gema.py
```

Configure BINDER NOSC for automatic localization
```
$ git clone https://github.com/emmaPatagonia/binder_nosc.git
```
or

```
cp -r pygema/core/BINDER_NOSC /home/user
cd /home/user/BINDER_NOSC
chmod a+x binder_nosc_AR
```
Then configure the parameters, velocity model and station info

```
nano /home/user/BINDER_NOSC/param.txt
nano /home/user/BINDER_NOSC/info.dat
nano /home/user/BINDER_NOSC/velmod.hdr
```

 Configure NonLinLoc

```
git clone https://github.com/emmaPatagonia/nonlinloc.git
```
or
```
cp -r pygema/core/nonlinloc /home/user
mv nonlinloc NLL
cp -r /home/user/NLL
```
Then, follow this instructions
```
more /home/user/NLL/readme.rd
```
Finally, copy the execSG2KMOD2.py file to NLL



#Configure realtime routines for volcano monitoring

Configure web services files: 
```
nano realtime/web/PyGema_Web/PyGema_Web/settings.py 
nano realtime/gema-init-monitoring
```
Configure syncronization with other server

```
nano sync.py
```

Configure seedlink server:

```
# instalacion ringserver
$ git clone https://github.com/iris-edu/ringserver.git
$ cd ringserver
$ make
$ mkdir tlog

# Ante errores comunes de complicacion
$ sudo pacman -S libdigidocpp	
$ yay automake #instalar version 1-15

# agregar path 
$ sudo ln -s ~/ringserver/ringserver /usr/local/bin/ringserver

#crear un archivo de configuracion
$ nano server.conf
RingDirectory ring
DataLinkPort 16000
SeedLinkPort 18000
ServerID "Seismic Network"
TransferLogDirectory tlog
TransferLogRX 0
MSeedScan /path/GM/ StateFile=scan.state InitCurrentState


#lanzar servidor
$ ringserver server.conf
```

Execute realtime rountines
```
chmod a+x gema-init-monitoring
./gema-init-monitoring
```

