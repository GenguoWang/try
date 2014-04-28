# -*- coding: utf-8 -*-  
import os
import re
import shutil
import smtplib
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email.Utils import COMMASPACE, formatdate
from email import Encoders
import os
import time
import sys
timestamp = time.strftime('%Y-%m-%d', time.gmtime(time.time() - 24 * 60 * 60))
#smtp configure
#smtpserver = 'smtp.live.com'
#port = 587
#smtpuser = 'wanggenguo@outlook.com'
#smtppass = 'King@W^ngMS'

smtpserver = 'smtp.gmail.com'
port = 587
smtpuser = 'liuhaiyu@bebeyond.com'
smtppass = 'lhy19900927'

reload(sys)
sys.setdefaultencoding('utf-8')

class SendMailException(Exception):
    def __init__(self,msg):
        Exception.__init__(self,msg)
        self.msg = msg
def sendMail(to, cc, bcc, subject, text, files=[]):
    assert type(to) == list
    assert type(files) == list
    fro = smtpuser  
    msg = MIMEMultipart()
    msg['From'] = fro
    msg['To'] = COMMASPACE.join(to) 
    msg['Cc'] =COMMASPACE.join(cc) 
    msg['Bcc'] =COMMASPACE.join(bcc) 
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = subject
    print msg.as_string()
    msg.attach(MIMEText(text,'html','utf-8'))
    for file in files:
        part = MIMEBase('application', "octet-stream")
        part.set_payload(open(file, "rb").read())
        Encoders.encode_base64(part)
        part.add_header('Content-Disposition', 'attachment; filename="%s"'
        % os.path.basename(file))
        #part.add_header('Content-Id', 'test1')
        #part.add_header('Content-', 'test1')
        msg.attach(part)
    return
    sendSuccess = False
    for i in range(5):
        try:
            smtp = smtplib.SMTP()
            #smtp.set_debuglevel(1)
            smtp.connect(smtpserver,port)
            smtp.starttls()
            smtp.login(smtpuser, smtppass)
            smtp.sendmail(smtpuser, to+cc+bcc, msg.as_string())
            smtp.quit()
            sendSuccess = True
            break
        except Exception, e:
            print "try again"
    if sendSuccess:
        print "email is sended"
    else:
        raise SendMailException("send mail failed")

sendto = "wanggenguo@sina.com,haiyu_liu1113@163.com"
dirfile = "dirs.dat"
def main():
    file = open(dirfile)
    a = file.readline()
    flag = ""
    settings = {"sendemail":"true","address":sendto}
    messages=u""
    while a:
        messages = messages + a
        a = file.readline()
    if settings["sendemail"] == "true" or settings["sendemail"] == "yes" :
        for i in range(1):
            print i
            sendMail([],[],settings["address"].split(","),"Duty",messages,[])

if __name__=='__main__':
    main()

