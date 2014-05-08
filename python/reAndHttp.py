import re
import urllib
import urllib2
saveDir = ""
url = "http://www2013.org/papers/proceedings.htm"
m = re.match(r"(https?)://([^/]*?)[/$](.*/)",url)
if not m:
    print "URL is not available"
    exit(-1)
proxy = m.group(1)
host = m.group(2)
dirt = m.group(3)
doc = urllib2.urlopen(url).read()
p = re.compile(r'href="(.*\.pdf)"')
pdfList = p.findall(doc)
for pdf in pdfList:
    m = re.match(r"(.*/)*([^/]*\.pdf)",pdf)
    name = m.group(2)
    print name
    if pdf[0:4]=="http":
        pdfUrl = pdf
    elif pdf[0] == "/":
        pdfUrl = host+pdf
    else:
        pdfUrl = host+"/"+dirt+pdf
    pdfUrl = proxy+"://"+pdfUrl
    print pdfUrl
    open(saveDir+name,"wb").write(urllib2.urlopen(pdfUrl).read())
    break;
