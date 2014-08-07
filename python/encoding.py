#/usr/bin/python
#coding: utf8
import json
import sys
import urllib
reload(sys)
#sys.setdefaultencoding("gbk")
sys.setdefaultencoding("utf-8")

with open("gbk.txt") as infile:
    buf = infile.read()
    print type(buf)
    print buf
    text = buf.decode("gbk")
    print type(text)

#str_str = u"encode王根国"
str_str = "encode王根国"
#t = str_str.encode("gbk")
#print t

with open("utf8.txt") as infile:
    buf = infile.read()
    print type(buf)
    print buf
    text = buf.decode("utf-8")
    print type(text)


a = [1, 2, 3]
print str(a)

str_str = "encode王根国"
#print unicode(str_str)
for i in range(len(str_str)):
    print "%x" % ord(str_str[i])

quoted = urllib.quote(str_str)
print quoted
print urllib.unquote(quoted)

str_utf = u"encode王根国"
print str_str == str_utf
print str_utf
print ord(str_utf[0])
print type(chr(1))

dict = { }
dict[str_str] = 1
dict[str_utf] = 2
print dict[str_str]
print dict[str_utf]
str_str = "abcd"
for i in range(len(str_str)):
    print "%x" % ord(str_str[i])
str_utf = u"abcd"
for i in range(len(str_utf)):
    print "%x" % ord(str_utf[i])
print str_str == str_utf
exit(0)
with open("test","w") as outfile:
    outfile.write(str_utf)
with open("test4","w") as outfile:
    outfile.write(str_str.encode("gbk"))
print str_str.decode("utf-8")
