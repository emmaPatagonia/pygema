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
import site, requests

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def get_credentials_sqldb():
  PYGEMA_PATH = "%s/pygema" % (site.getsitepackages()[0])
  r = requests.get(r'http://jsonip.com')
  public_ip= r.json()['ip']

  if public_ip=="xxx.xxx.xxx.51":
    credentials_file = "%s/src/credentials_mysqldb_admin" % (PYGEMA_PATH)
  else:
    credentials_file = "%s/src/credentials_mysqldb_user" % (PYGEMA_PATH)

  host, port, user, password, database = np.loadtxt(credentials_file, dtype='str').T

  return host, port, user, password, database

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def get_credentials_email():
  PYGEMA_PATH = "%s/pygema" % (site.getsitepackages()[0])
  credentials_file = "%s/src/credentials_email" % (PYGEMA_PATH)
  from_email, to_email, pass_email = np.loadtxt(credentials_file, dtype='str').T

  return from_email, to_email, pass_email




