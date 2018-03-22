import sys
import os

argc = int(len(sys.argv))

if argc != 4:
	print ''
	print 'USAGE: python ' + sys.argv[0] + ' <dataset> <ext>'
	print ' dataset      [i] path to the color image dataset to convert'
	print ' ext          [i] extension of image used (e.g.: jpg, png, etc.)'
	print ' size         [i] resolution of the image(e.g.: 640, 256, etc.)'
	print ''
	quit()

dataset=sys.argv[1]
ext=sys.argv[2]
size=sys.argv[3]

print 'Converting images to ...'

path_i = '0_ImgDatasets/' + dataset + '/color'
path_o = '0_ImgDatasets/' + dataset + '/resized'

os.system('mkdir -p ' + path_o)

path=[]
for c in sorted(os.listdir(path_i)):
	print ' ... ' + c + ' class'
	path.append(c)
	os.system('mkdir -p ' + path_o + '/' + '/'.join(path))
	for img in sorted(os.listdir(path_i + '/' + '/'.join(path))):
		resized = path_o + '/' + '/'.join(path) + '/' + img[:-(len(ext)+1)] + '.jpg'
		img = path_i + '/' + '/'.join(path) + '/' + img
		os.system('convert ' + img + ' -resize ' + size + 'x ' + resized)
	path.pop()

print 'Done!'
