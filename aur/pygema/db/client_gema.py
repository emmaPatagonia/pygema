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

import os, subprocess, requests, socket, glob

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def mount_client(server_user, server_dns, server_port, server_buffer_seiscomp, server_archive_seiscomp, local_dir_name=None):
  r = requests.get(r'http://jsonip.com')
  public_ip= r.json()['ip']
  if (not socket.gethostname()=='sirius') and (not socket.gethostname()=='tremor') and (not socket.gethostname()=='maniedba') and (not public_ip=="xxx.xxx.xxx.51"):
    if not local_dir_name:
      local_dir_name = "%s/mount" % (os.getenv("HOME"))

    local_buffer = "%s/seiscomp_data_buffer" % (local_dir_name)
    local_archive = "%s/seiscomp_data_archive" % (local_dir_name)

    if not os.path.exists(local_buffer):
      os.makedirs(local_buffer)

    if not os.path.exists(local_archive):
      os.makedirs(local_archive)

    if not os.path.ismount(local_buffer):
      cmd1 = "sshfs %s@%s:%s %s -p %i" % (server_user, server_dns, server_buffer_seiscomp, local_buffer, server_port)
      subprocess.call(cmd1, shell=True)

    if not os.path.ismount(local_archive):
      cmd2 = "sshfs %s@%s:%s %s -p %i" % (server_user, server_dns, server_archive_seiscomp, local_archive, server_port)
      subprocess.call(cmd2, shell=True)



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def umount_client(local_dir_name=None):
  r = requests.get(r'http://jsonip.com')
  public_ip= r.json()['ip']
  if (not socket.gethostname()=='sirius') and (not socket.gethostname()=='tremor') and (not socket.gethostname()=='maniedba') and (not public_ip=="xxx.xxx.xxx.51"):
    if not local_dir_name:
      local_dir_name = "%s/mount" % (os.getenv("HOME"))

    local_buffer = "%s/seiscomp_data_buffer" % (local_dir_name)
    local_archive = "%s/seiscomp_data_archive" % (local_dir_name)

    if os.path.exists(local_buffer) and os.path.ismount(local_buffer):
      cmd = "sudo umount %s" % (local_buffer)
      subprocess.call(cmd, shell=True)

    if os.path.exists(local_archive) and os.path.ismount(local_archive):
      cmd = "sudo umount %s" % (local_archive)
      subprocess.call(cmd, shell=True)



# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
