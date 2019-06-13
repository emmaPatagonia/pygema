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

def mount_client(server_user, server_dns, server_port, server_archive_path, local_dir_name=None):
  r = requests.get(r'http://jsonip.com')
  public_ip= r.json()['ip']
  if not public_ip=="152.74.135.51":
    if not local_dir_name:
      local_dir_name = "%s/mount" % (os.getenv("HOME"))

    if not os.path.exists(local_dir_name):
      os.makedirs(local_dir_name)

    if not os.path.ismount(local_dir_name):
      cmd1 = "sshfs %s@%s:%s %s -p %i" % (server_user, server_dns, server_archive_path, local_dir_name, server_port)
      subprocess.call(cmd1, shell=True)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def umount_client(local_dir_name=None):
  r = requests.get(r'http://jsonip.com')
  public_ip= r.json()['ip']
  if not public_ip=="152.74.135.51":
    if not local_dir_name:
      local_dir_name = "%s/mount" % (os.getenv("HOME"))

    if os.path.exists(local_dir_name) and os.path.ismount(local_dir_name):
      cmd = "sudo umount -f %s" % (local_dir_name)
      subprocess.call(cmd, shell=True)


# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
