import sys
import os

path_i=sys.argv[1]
c=sys.argv[2]
split=int(sys.argv[3])

print ' ... ' + c + ' class'

for desc_nm in sorted(os.listdir(path_i + '/' + c)):
	desc_nm = c + '/' + desc_nm

	f_i=open(path_i+'/'+desc_nm,'r')
	f_o=[]

	#Writting header
	ndescs, dims=f_i.readline().split()
	new_dims=int(dims)/split
	for i in xrange(1,split+1):
		f=open(path_i+str(i)+'/'+desc_nm,'w')
		f.write(ndescs + ' ' + str(new_dims) + '\n')
		f_o.append(f)

	#Writting body
	for desc in f_i:
		desc=desc.split()
		for i in xrange(0,split):
			new_desc=desc[new_dims*i:new_dims*(i+1)]
			f_o[i-1].write(' '+' '.join(new_desc)+'\n')

	f_i.close()
	for f in f_o:
		f.close()

print ' ... ' + c + ' class done!'
