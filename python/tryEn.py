#coding=utf-8
import sys
import urllib
reload(sys)
sys.setdefaultencoding("utf-8")
s = urllib.quote("大众汽车")
print s
f = urllib.unquote(s)
print f
us = unicode(s)
uf = urllib.unquote(us)
print uf
print len(s)
for i in range(len(s)):
    print s[i], us[i]
print s.split("%")
print us.split("%")
print len(us)
_hexdig = '0123456789ABCDEFabcdef'
_hextochr = dict((a+b, chr(int(a+b,16))) for a in _hexdig for b in _hexdig)
r1 = s.split("%")
r2 = us.split("%")
print type(_hextochr[r1[1][0:2]])
print type(_hextochr[r2[1][0:2]])
res = s.split("%")
for i in xrange(1, len(res)):
    item = res[i]
    res[i] = _hextochr[item[:2]] + item[2:]
print res
res = us.split("%")
for i in xrange(1, len(res)):
    item = res[i]
    res[i] = _hextochr[item[:2]] + str(item[2:])
print res
