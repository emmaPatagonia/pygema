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

import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email import encoders
import os, smtplib, glob
from pygema.db.credentials import get_credentials_email

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 

def send_email_with_attached_files(message, subject, figsdir=None): 

  from_email, to_email, pass_email = get_credentials_email()

  msg = MIMEMultipart()
  msg['From'] = from_email
  msg['To'] = to_email
  msg['Subject'] = subject

  msg.attach(MIMEText(message, 'plain'))

  if figsdir:
    files = glob.glob(figsdir+"/*")
    for file_location in files:
      #print(file_location)
      attachment = open(file_location, "rb")
      part = MIMEBase('application', 'octet-stream')
      part.set_payload((attachment).read())
      encoders.encode_base64(part)
      part.add_header('Content-Disposition', "attachment; filename= %s" % (os.path.basename(file_location)) )
      msg.attach(part)

  server = smtplib.SMTP('smtp.gmail.com:587')
  server.starttls()
  server.login(msg['From'], pass_email)
  server.sendmail(msg['From'], msg['To'], msg.as_string())
  server.quit()


