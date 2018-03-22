import sys
import os

argc = int(len(sys.argv))
if argc != 4:
	print '\nUsage: python formatDescriptor.py desc_path desc\n'
	print ' desc_path      <string> path to the descriptor to format'
	print ' desc           <string> name of the descriptor to format'
	print ' detect         <string> name of the detector if used'
	quit()

desc_path=sys.argv[1]
desc=sys.argv[2]
detector=sys.argv[3]

f_i=open(desc_path,'r')
f_o=open(desc_path+'tmp','w')

if desc == 'sift' and detector == '-':
	nkeys, dim = f_i.readline().split()
	extra_data=4
#desc == 'surf' | 'hesaffsift' | 'hesaffsurf' | 'haraffsift' | 'haraffsurf' | etc ...
else:
	dim = f_i.readline()
	nkeys = f_i.readline()
	extra_data=5

nkeys = int(nkeys)
dim = int(dim)

f_o.write(str(nkeys) + ' ' + str(dim) + '\n')

values=f_i.readline().split()
count=0
while len(values)>0:
	for value in values:
		count = count + 1
		if count > extra_data:
			f_o.write(value+' ')
		if count == dim + extra_data:
			f_o.write('\n')
			count = 0
	values=f_i.readline().split()

f_i.close()
f_o.close()

os.system('rm ' + desc_path)
os.system('mv ' + desc_path + 'tmp ' + desc_path)
