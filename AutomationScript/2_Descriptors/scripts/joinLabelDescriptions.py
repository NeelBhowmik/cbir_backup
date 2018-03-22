import ntpath
import sys

argc = int(len(sys.argv))
if argc != 3:
	print '\nUsage: python joinLabeledDescriptors.py Descriptors LabeledDescriptors\n'
	print 'Descriptors          <file>   Descriptors wanted to join to the LabeledDescriptors file\n'
	print 'LabeledDescriptors   <file>   LabeledDescriptors file\n'
	quit()

Descriptors=sys.argv[1]
LD=sys.argv[2]

Keys=open(Descriptors,'r')
LabeledDescriptors=open(LD,'a')

Keys.readline()

img_nm=ntpath.basename(Descriptors)[:-5]

for values in Keys:
	values=values.split()
	LabeledDescriptors.write(' '.join(values)+' '+img_nm+'\n')

Keys.close()
LabeledDescriptors.close()
