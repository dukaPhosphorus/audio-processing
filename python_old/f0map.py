
import numpy as np
import fileio as io
import copy
import matplotlib.pyplot as plt
import argparse
import sys
import os
import h5py
import util_f0

HTS_U_SYMBOL = -1e+10

def f0map_linear(lf0, mean_src, std_src, mean_tgt, std_tgt):
    std = std_tgt / std_src
    std=1
    lf0[lf0 != HTS_U_SYMBOL] = (lf0[lf0 != HTS_U_SYMBOL] - mean_src) * std + mean_tgt
    return util_f0.lf02f0(lf0)

def parse_args():
   parser = argparse.ArgumentParser()
   parser.add_argument('lf0', type=str,
      help='name of input lf0 file')
   parser.add_argument('mean_src', type=float,
      help='mean lf0 of source speaker')
   parser.add_argument('std_src', type=float,
      help='std lf0 of source speaker')
   parser.add_argument('mean_tgt', type=float,
      help='mean lf0 of target speaker')
   parser.add_argument('std_tgt', type=float,
      help='std lf0 of target speaker')
   parser.add_argument('out', type=str,
      help='name of output f0 file')
   return parser.parse_args()

if __name__ == '__main__':
   args = parse_args()

   dbl_fmt = False # binary double format. if False, binary float
   lf0 = io.readvector(args.lf0, dfmt=dbl_fmt)
   f0 = f0map_linear(lf0, args.mean_src, args.std_src, args.mean_tgt, args.std_tgt)
   np.savetxt(args.out, f0)
