import sys
import os
from scipy.interpolate import interp1d
import matplotlib.pyplot as plt
import numpy as np
import fileio as io

HTS_U_SYMBOL = -1e+10

def rm_micro_prosody(lf0, n_frame=4):

   if len(np.where(lf0 == HTS_U_SYMBOL)[0]) != 0:
      raise ValueError("unvoiced frames are included.")

   smooth_lf0 = np.zeros(len(lf0))
   for t in range(len(lf0)):
      smooth_lf0[t] = np.average(lf0[ max(t-n_frame, 0) : min(t+n_frame+1, len(lf0) - 1) ])

   return smooth_lf0

def lf02clf0(lf0, smooth=False):
   clf0 = np.zeros(len(lf0))
   t = np.arange(len(lf0))
   t_voiced = t[np.where(lf0 != HTS_U_SYMBOL)]
   lf0_voiced = lf0[t_voiced]

   # uv
   uv = np.zeros(len(lf0))
   uv[t_voiced] = 1

   # smoothing & interpolation
   contlf0 = interp1d(t_voiced, lf0_voiced)   # lienar interpolation
   clf0[t_voiced[0]:t_voiced[-1]+1] = contlf0(np.arange(t_voiced[0], t_voiced[-1]+1))
   clf0[t_voiced[0]:t_voiced[-1]+1] = rm_micro_prosody(clf0[t_voiced[0]:t_voiced[-1]+1])

   # smoothing & extrapolation
   clf0[:t_voiced[0]] = lf0[t_voiced[0]]
   clf0[t_voiced[-1]+1:] = lf0[t_voiced[-1]]
   clf0 = rm_micro_prosody(clf0)

   # restore voiced frames
   clf0[t_voiced] = lf0[t_voiced]

   # smoothing (trajectory smoothing)
   if smooth is True:
      clf0 = rm_micro_prosody(clf0)

   return clf0, uv

# convert (clf0, uv) to f0
def clf02lf0(clf0, uv):
   lf0 = np.ones(len(clf0)) * HTS_U_SYMBOL
   lf0[np.where(uv == 1)] = clf0[np.where(uv == 1)]

   return lf0

# convert lf0 to f0
def lf02f0(lf0):
    T = len(lf0)
    f0 = np.zeros(T)
    f0[lf0 != HTS_U_SYMBOL] = np.exp(lf0[lf0 != HTS_U_SYMBOL])
    return f0

if __name__ == "__main__":
   name_lf0 = sys.argv[1]
   name_clf0 = sys.argv[2]
   name_uv = sys.argv[3]
   lf0 = io.readvector(name_lf0) # HTS-format log F0
   clf0, uv = lf02clf0(lf0, smooth = True) # lf0 -> continuous lf0
   io.writevector(clf0, name_clf0)
   io.writevector(uv, name_uv)
