#import os
import sys
import numpy as np
#import sklearn
from sklearn.neighbors import KDTree
import pickle

argc = int(len(sys.argv))

if argc != 3:
     print ('')
     print ('USAGE: python ' + sys.argv[0] + ' <CBPath> <OutputPath>')
     print (' CBPath           [i] CB file path')
     print (' OutputPath       [o] CB kdtree path')
     print ('')
     quit()

fn1=sys.argv[1]
fn2=sys.argv[2]

"""
####To generate kdtree index
with open('CodeBook10000.dat') as f:
    file_cont = f.read().split()
####String to float
file_cont_fl = np.array(file_cont, float)
#print (np_float_1[2::])
####1D numpy array to 2D numpy array
file_cont_fl_2D = np.reshape(file_cont_fl[2::], (-1, file_cont_fl[1].astype(np.int)))
#print (file_cont_fl_2D)

CB_tree = KDTree(file_cont_fl_2D, leaf_size=2)

with open('cbkdtree10000.kdtree', 'wb') as f:
    pickle.dump(CB_tree, f, pickle.HIGHEST_PROTOCOL)
"""
####Read file into numpy array float
def fileread(filename):
    with open(filename) as f:
        file_cont = f.read().split()
    ####String to float
    file_cont_fl = np.array(file_cont, float)
    #print (np_float_1[2::])
    ####1D numpy array to 2D numpy array
    file_cont_fl_2D = np.reshape(file_cont_fl[2::], (-1, file_cont_fl[1].astype(np.int)))
    #print (file_cont_fl_2D)
    return file_cont_fl_2D

####generate and save kdtree
def genkdtree(CBarray,filename):
    CB_tree = KDTree(CBarray, leaf_size=4)
    with open(filename, 'wb') as f:
        pickle.dump(CB_tree, f, 2)
    return CB_tree

"""
####Write and save generated kdtree
def writekdtree(CB_tree,filename):
    with open(filename, 'wb') as f:
        pickle.dump(CB_tree, f, pickle.HIGHEST_PROTOCOL)
"""
      
CB_cont = fileread(fn1)        
CB_kdtree = genkdtree(CB_cont,fn2)
