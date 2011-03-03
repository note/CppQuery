from pyquery import PyQuery as pq

file = open("selectors_precedence.html")
html = file.read()
html = pq(html)

el = html(".abc > p a")
els = html(".abc.def")

print len(els)
#print el.eq(0).text()
