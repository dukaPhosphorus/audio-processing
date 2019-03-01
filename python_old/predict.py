import sys
import os
import logging
import argparse
import numpy as np
import six
import copy
from matplotlib import pylab as plt
import utils
import time
import chainer
from chainer import cuda, Variable
import chainer.links as L
import chainer.functions as F
from chainer import optimizers
from chainer import serializers
from generator import Generator
import mlpg_test
import h5py
from spss_util import apply_delta_win
import util_f0
import fileio as io
import warnings
warnings.filterwarnings('ignore')

HTS_U_SYMBOL = -1e+10

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--prjdir', type=str, default="/home/ysaito/20180413_tamaru_aso_VC",
        help="name of project directory")
    parser.add_argument('--gpu', type=int, default=-1,
        help="GPU ID (negative value indicates CPU)")
    parser.add_argument('--nhid', type=int, default=256,
        help="# of hidden units")
    parser.add_argument('--ndim_mgc', type=int, default=39,
        help="# of dimension of mgc")
    parser.add_argument('--ndim_bap', type=int, default=5,
        help="# of dimension of bap")
    parser.add_argument('src', type=str,
        help='name of source speaker')
    parser.add_argument('tgt', type=str,
        help='name of target speaker')
    parser.add_argument('outdir', type=str,
        help="dirname to save outputs")
    return parser.parse_args()

# F0 conversion
def f0map_linear(lf0, mean_src, std_src, mean_tgt, std_tgt):
    std = std_tgt / std_src
    std=1
    lf0[lf0 != HTS_U_SYMBOL] = (lf0[lf0 != HTS_U_SYMBOL] - mean_src) * std + mean_tgt
    return util_f0.lf02f0(lf0)

def main():
    args = parse_args()
    data_dir = os.path.join(args.prjdir, 'data')
    logging.basicConfig( format="[%(asctime)s] %(message)s", level=logging.DEBUG)
    logger = logging.getLogger(__name__)
    dbl_fmt = False

    for d in ('mgc', 'f0', 'bap'):
        utils.mkdir(os.path.join(args.outdir, d))

    # statistics
    stats = h5py.File(os.path.join(data_dir, 'stats.h5'), 'r')
    norm_src_mgc = stats['src']['mgc']['mean'].value, stats['src']['mgc']['var'].value
    norm_tgt_mgc = stats['tgt']['mgc']['mean'].value, stats['tgt']['mgc']['var'].value
    norm_src_lf0 = stats['src']['lf0']['mean'].value, stats['src']['lf0']['var'].value
    norm_tgt_lf0 = stats['tgt']['lf0']['mean'].value, stats['tgt']['lf0']['var'].value
    stats.close()

    # list of test data
    test_scp = os.path.join(data_dir, 'scp', 'test.scp')

    # window for delta
    wins = [[1.0], [-0.5, 0.0, 0.5]]

    # model definition
    g = Generator(args.ndim_mgc*2, args.nhid, args.ndim_mgc*2)
    logger.info('loading pre-trained generator model from ' + args.outdir)
    serializers.load_hdf5(os.path.join(args.outdir, 'g.model'), g)

    if args.gpu >= 0:
        cuda.get_device(args.gpu).use()
        g.to_gpu()

    for fn_data in open(test_scp).readlines():
        fn_data = fn_data.rstrip("\n")
        print(fn_data)
        # read data
        # mgc -> converted by using trained DNNs
        mgc_src = io.readmatrix(os.path.join(data_dir, 'mgc', args.src, fn_data) + '.mgc', dim=args.ndim_mgc+1, dfmt=dbl_fmt)
        pow_src = copy.copy(mgc_src[:, 0]) # 0th mgc is not converted

        # lf0 -> linearly converted by using statistics of src & tgt lf0
        lf0_src = io.readvector(os.path.join(data_dir, 'lf0', args.src, fn_data) + '.lf0', dfmt=dbl_fmt)
        f0_gen = f0map_linear(lf0_src, norm_src_lf0[0], np.sqrt(norm_src_lf0[1]), norm_tgt_lf0[0], np.sqrt(norm_tgt_lf0[1]))

        # bap -> not converted
        bap_src = io.readmatrix(os.path.join(data_dir, 'bap', args.src, fn_data) + '.bap', dim=args.ndim_bap, dfmt=dbl_fmt)

        # predict mgc
        x = apply_delta_win(mgc_src[:, 1:], wins)
        R, W = mlpg_test.mlpg_from_pdf(x, wins)
        R = R.toarray()
        if norm_src_mgc is not None:
            x = utils.normalize_gauss(x, norm_src_mgc[0], norm_src_mgc[1]).astype(np.float32)
        if args.gpu >= 0:
            x = cuda.to_gpu(x.astype(np.float32), args.gpu)
            R = cuda.to_gpu(R.astype(np.float32), args.gpu)

        mgc_gen = g.generate_mgc(Variable(x), R)

        mgc_gen = cuda.to_cpu(mgc_gen.data)
        if norm_tgt_mgc is not None:
            mgc_gen = utils.normalize_gauss(mgc_gen, norm_tgt_mgc[0][:args.ndim_mgc], norm_tgt_mgc[1][:args.ndim_mgc], inv=True)

        mgc_gen = np.hstack((pow_src.reshape((-1, 1)), mgc_gen))

        # save generated parameters
        # mgc -> save as double format
        # f0  -> save as ascii format
        # bap -> save as double format
        io.writematrix(mgc_gen, os.path.join(args.outdir, 'mgc', fn_data + '.mgc'), dfmt=True)
        io.writematrix(bap_src, os.path.join(args.outdir, 'bap', fn_data + '.bap'), dfmt=True)
        np.savetxt(os.path.join(args.outdir, 'f0', fn_data + '.f0'), f0_gen)

if __name__ == "__main__":
    sys.exit(main())
