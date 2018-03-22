import sys
import os

fn=sys.argv[1]
k=int(sys.argv[2])

f=open(fn)

line=f.readline()
values=line.split()
minDist=float(values[0])
maxDists=values[1:]

for line in f:
	values=line.split()
	cur_min=float(values[0])
	cur_maxs=values[1:]
	if cur_min < minDist:
		minDist=cur_min
	for i in xrange(0,k):
		if float(cur_maxs[i]) > float(maxDists[i]):
			maxDists[i]=cur_maxs[i]

f.close()

f=open(fn,'w')
f.write(str(minDist))
for i in xrange(0,k):
	f.write(' ' + str(maxDists[i]))
f.write('\n')
f.close()
