import urllib2
from pyquery import PyQuery as pq

file = open('pyquery.html')
html = file.read()
html = pq(html)
print(html("#quickstart h1").text())

print(len(html(".section")))

print(html(".section").eq(2).attr("id"))

file = open("simple.html")
html = file.read()
html = pq(html)

print('-------simple.html:')
print(html(".some").text())
el = html("#nonexisting")
print(html("a").text())

print(html("#first").attr("class"))
print(html("#first").eq(0).attr("class"))
