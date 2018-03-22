import sys
import os

fn=sys.argv[1]

fp_i=open(fn)
fp_o=open(fn+'_red','w')

c = {'defense':'01', 'eiffel':'02', 'general':'03', 'invalides':'04', 'louvre':'05', 'moulinrouge':'06', 'museedorsay':'07', 'notredame':'08', 'pantheon':'09', 'pompidou':'10', 'sacrecoeur':'11', 'triomphe':'12'}

fp_i.readline()

for line in fp_i:
	values=line.split()
	nm=values[len(values)-1].split('_')[1]
	values[len(values)-1]=c[nm]
	fp_o.write(' '.join(values) + '\n')

fp_i.close()
fp_o.close()
