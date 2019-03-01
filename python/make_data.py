import numpy as np
from spss_util import apply_delta_win, dtw
from spss_util import my_distance_measure
import fileio as io
import copy
import matplotlib.pyplot as plt
import argparse
import sys
import os
import h5py

HTS_U_SYMBOL = -1e+10

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--prjdir', type=str, default="/home/ysaito/20180413_tamaru_aso_VC",
        help="dirname to find feature files")
    parser.add_argument('--dim', type=int, default=39,
        help="# of feature dim")
    parser.add_argument('src', type=str,
        help='name of source speaker')
    parser.add_argument('tgt', type=str,
        help='name of target speaker')
    return parser.parse_args()

# estimate threshold values for silence cut
def est_thvals(train_scp, src, tgt, dim, dbl_fmt):
    minpows_src = []
    minpows_tgt = []
    for fn_data in open(train_scp).readlines():
        fn_data = fn_data.rstrip("\n")
        # read data
        mgc_src = io.readmatrix(os.path.join('data', 'mgc', src, fn_data) + '.mgc', dim=args.dim+1, dfmt=dbl_fmt)
        mgc_tgt = io.readmatrix(os.path.join('data', 'mgc', tgt, fn_data) + '.mgc', dim=args.dim+1, dfmt=dbl_fmt)
        pow_src, pow_tgt = copy.copy(mgc_src[:, 0]), copy.copy(mgc_tgt[:, 0])
        minpows_src.append(min(pow_src))
        minpows_tgt.append(min(pow_tgt))
    thval_src = max(minpows_src)
    thval_tgt = max(minpows_tgt)
    return thval_src, thval_tgt

if __name__ == '__main__':
    args = parse_args()

    mgcs_src = np.zeros(args.dim*2, dtype=np.float64)
    mgcs_tgt = np.zeros(args.dim*2, dtype=np.float64)

    lf0s_src = np.zeros(1, dtype=np.float64)
    lf0s_tgt = np.zeros(1, dtype=np.float64)

    wins = [[1.0], [-0.5, 0.0, 0.5]]
    dist = my_distance_measure()
    dbl_fmt = False # binary double format. if False, binary float
    trace = True # debug mode
    train_scp = os.path.join(args.prjdir, 'data', 'scp', 'train.scp')
    test_scp = os.path.join(args.prjdir, 'data', 'scp', 'test.scp')

    # threshold values for silence cut
    thval_src, thval_tgt = est_thvals(train_scp, args.src, args.tgt, args.dim, dbl_fmt)

    # make training data
    if trace is True:
        print("=== Making train data ===")
    hdf5 = h5py.File(os.path.join(args.prjdir, 'data', args.src + "_" + args.tgt + "_train.h5"), "w")
    for fn_data in open(train_scp).readlines():
        fn_data = fn_data.rstrip("\n")
        print(fn_data)
        # read data
        mgc_src = io.readmatrix(os.path.join('data', 'mgc', args.src, fn_data) + '.mgc', dim=args.dim+1, dfmt=dbl_fmt)
        mgc_tgt = io.readmatrix(os.path.join('data', 'mgc', args.tgt, fn_data) + '.mgc', dim=args.dim+1, dfmt=dbl_fmt)
        pow_src, pow_tgt = copy.copy(mgc_src[:, 0]), copy.copy(mgc_tgt[:, 0])

        lf0_src = io.readvector(os.path.join('data', 'lf0', args.src, fn_data) + '.lf0', dfmt=dbl_fmt)
        lf0_tgt = io.readvector(os.path.join('data', 'lf0', args.tgt, fn_data) + '.lf0', dfmt=dbl_fmt)
        lf0s_src = np.hstack((lf0s_src, lf0_src[lf0_src != HTS_U_SYMBOL]))
        lf0s_tgt = np.hstack((lf0s_tgt, lf0_tgt[lf0_tgt != HTS_U_SYMBOL]))

        # calculate delta
        SRC = apply_delta_win(mgc_src[:, 1:], wins)
        TGT = apply_delta_win(mgc_tgt[:, 1:], wins)

        # remove sil
        SRC = SRC[np.where(pow_src >= thval_src)]
        TGT = TGT[np.where(pow_tgt >= thval_tgt)]

        # dtw and store
        path_src, path_tgt = dtw(SRC, TGT, dist.melcep_dist)

        mgcs_src = np.vstack((mgcs_src, SRC[path_src]))
        mgcs_tgt = np.vstack((mgcs_tgt, TGT[path_tgt]))

        hdf5.create_group(fn_data)
        hdf5.create_dataset(fn_data + "/src", data=SRC[path_src])
        hdf5.create_dataset(fn_data + "/tgt", data=TGT[path_tgt])

        if trace is True:
            print(" %s[%d], %s[%d] -> [%d]" % (fn_data + "(" + args.src + ")", len(SRC), fn_data + "(" + args.tgt + ")", len(TGT), len(path_src)) )
            sys.stdout.flush()
    hdf5.flush()
    hdf5.close()

    # calculate statistics
    mgcs_src = mgcs_src[1:]
    mgcs_tgt = mgcs_tgt[1:]

    lf0s_src = lf0s_src[1:]
    lf0s_tgt = lf0s_tgt[1:]

    hdf5 = h5py.File(os.path.join(args.prjdir, 'data', 'stats.h5'), 'w')
    hdf5.create_group('src')
    hdf5.create_group('src/mgc')
    hdf5.create_dataset('src/mgc/mean', data=np.mean(mgcs_src, axis=0))
    hdf5.create_dataset('src/mgc/var', data=np.var(mgcs_src, axis=0))
    hdf5.create_group('src/lf0')
    hdf5.create_dataset('src/lf0/mean', data=np.mean(lf0s_src))
    hdf5.create_dataset('src/lf0/var', data=np.var(lf0s_src))
    hdf5.create_group('tgt')
    hdf5.create_group('tgt/mgc')
    hdf5.create_dataset('tgt/mgc/mean', data=np.mean(mgcs_tgt, axis=0))
    hdf5.create_dataset('tgt/mgc/var', data=np.var(mgcs_tgt, axis=0))
    hdf5.create_group('tgt/lf0')
    hdf5.create_dataset('tgt/lf0/mean', data=np.mean(lf0s_tgt))
    hdf5.create_dataset('tgt/lf0/var', data=np.var(lf0s_tgt))
    hdf5.flush()
    hdf5.close()
