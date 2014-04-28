# -*- coding: utf-8 -*-  
import sendMail
#config
LIMIT=90
accountIndex=0
subject = u"【BeBeyond申请讲座】2014年MBA申请第一轮，你必须知道的!"
accountFile = "account1.dat"
messagesFile = "dirs.dat"
sendtoFile = "sendto"
unsendFile = "unsend"

account = open(accountFile)
unsend = open(unsendFile,'w')
user = []
for line in account:
    seg = line.split()
    if(len(seg)>0):
        user.append(seg)

sendtoList = []
sendTo = open(sendtoFile)
messages = open(messagesFile).read()
cntSend=0
cntUnsend=0
cntTotal=0
for line in sendTo:
    strTemp = line.strip()
    if strTemp=="":
        continue
    cntTotal += 1
    sendtoList.append(line[:-1])
    if len(sendtoList) >= LIMIT:
        if accountIndex<len(user):
            sendMail.smtpuser = user[accountIndex][0] 
            sendMail.smtppass = user[accountIndex][1]
            sendMail.sendMail([],[],sendtoList,subject,messages,[])
            accountIndex += 1
            cntSend += len(sendtoList)
            print cntSend
            print "send from:"+sendMail.smtpuser+" to:"
            print sendtoList
        else:
            print "no user available"
            for i in range(len(sendtoList)):
                unsend.write(sendtoList[i]+"\n")
            cntUnsend += len(sendtoList)
        sendtoList = []
if len(sendtoList) >= 1:
    if accountIndex<len(user):
        sendMail.smtpuser = user[accountIndex][0] 
        sendMail.smtppass = user[accountIndex][1]
        sendMail.sendMail([],[],sendtoList,subject,messages,[])
        accountIndex += 1
        cntSend += len(sendtoList)
        print cntSend
        print "send from:"+sendMail.smtpuser+" to:"
        print sendtoList
    else:
        print "no user available"
        for i in range(len(sendtoList)):
            unsend.write(sendtoList[i]+"\n")
        cntUnsend += len(sendtoList)
    sendtoList = []
print "total:"+str(cntTotal)+" sent:"+str(cntSend)+" unsend:"+str(cntUnsend)
