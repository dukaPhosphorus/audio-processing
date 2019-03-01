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
import warnings
warnings.filterwarnings('ignore')

np.random.seed(2016)

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--prjdir', type=str, default="/home/ysaito/20180413_tamaru_aso_VC",
        help="name of project directory")
    parser.add_argument('--batch', type=int, default=256,
        help="batch size")
    parser.add_argument('--epoch', type=int, default=25,
        help="# of epochs for pre-training of generator")
    parser.add_argument('--gpu', type=int, default=-1,
        help="GPU ID (negative value indicates CPU)")
    parser.add_argument('--lr', type=float, default=0.01,
        help="learning rate")
    parser.add_argument('--nhid', type=int, default=256,
        help="# of hidden units")
    parser.add_argument('--ndim_mgc', type=int, default=39,
        help="# of dimension of mgc")
    parser.add_argument('src', type=str,
        help='name of source speaker')
    parser.add_argument('tgt', type=str,
        help='name of target speaker')
    parser.add_argument('outdir', type=str,
        help="dirname to save outputs")
    return parser.parse_args()

# train generator
def train_g(g, train, gpu, batchsize, optimizer,
    norm_src=None, norm_tgt=None):
    keys = train.keys()

    # training
    mean_loss = 0.0
    for key in np.random.permutation(keys):
        batch_src = train[key]['src'].value.astype(np.float32)
        batch_tgt = train[key]['tgt'].value.astype(np.float32)

        # suffling
        T = len(batch_src)
        batch_idx = range(T)
        np.random.shuffle(batch_idx)
        batch_src = batch_src[batch_idx]
        batch_tgt = batch_tgt[batch_idx]

        # normalization
        if norm_src is not None:
            batch_src = utils.normalize_gauss(batch_src, norm_src[0], norm_src[1])
        if norm_tgt is not None:
            batch_tgt = utils.normalize_gauss(batch_tgt, norm_tgt[0], norm_tgt[1])
        x_batch = np.vsplit(batch_src.astype(np.float32), [x for x in range(batchsize, T, batchsize)])
        y_batch = np.vsplit(batch_tgt.astype(np.float32), [y for y in range(batchsize, T, batchsize)])
        batch_loss = 0.0
        for x, y in zip(x_batch, y_batch):
            if gpu >= 0:
                x = cuda.to_gpu(x.astype(np.float32), gpu)
                y = cuda.to_gpu(y.astype(np.float32), gpu)

            optimizer.target.cleargrads()

            # forward-propagation
            loss, pred = g.forward(Variable(x), Variable(y))
            # back-propagation & update
            loss.backward()
            optimizer.update()

            batch_T = x.shape[0]
            batch_loss += float(loss.data) * batch_T
        mean_loss += batch_loss / T

    return mean_loss / len(keys)

def main():
    args = parse_args()
    data_dir = os.path.join(args.prjdir, 'data')
    logging.basicConfig( format="[%(asctime)s] %(message)s", level=logging.DEBUG)
    logger = logging.getLogger(__name__)

    # make directory
    utils.mkdir(args.outdir)
    for d in ('train', 'test'):
        utils.mkdir(os.path.join(args.outdir, d))
        utils.mkdir(os.path.join(args.outdir, d, 'mgc'))
        for d2 in ('gen', 'plot'):
            utils.mkdir(os.path.join(args.outdir, d, 'mgc', d2))

    # statistics
    stats = h5py.File(os.path.join(data_dir, 'stats.h5'), 'r')
    norm_src = stats['src']['mgc']['mean'].value, stats['src']['mgc']['var'].value
    norm_tgt = stats['tgt']['mgc']['mean'].value, stats['tgt']['mgc']['var'].value
    stats.close()

    # training data
    train = h5py.File(os.path.join(data_dir, args.src + "_" + args.tgt + "_train.h5"), "r")

    # window for delta
    wins = [[1.0], [-0.5, 0.0, 0.5]]

    # model definition
    g = Generator(args.ndim_mgc*2, args.nhid, args.ndim_mgc*2)

    # optimizer
    optimizer_g = chainer.optimizers.AdaGrad(lr=args.lr) # g training
    optimizer_g.setup(g)

    if args.gpu >= 0:
        cuda.get_device(args.gpu).use()
        g.to_gpu()

    # training
    logger.info('generator training...')
    training_loss_g = np.zeros(args.epoch)
    for epoch in six.moves.range(1, args.epoch + 1):
        print( 'epoch %d:' % (epoch) )
        start = time.time()
        loss_train_g = train_g(
            g, train, args.gpu,
            args.batch, optimizer_g,
            norm_src, norm_tgt
        )
        end = time.time()
        ltypes = ('sum', 'mgc', 'clf0', 'bap', 'uv')
        print( 'training loss of g: %f' % (loss_train_g) )
        print( 'training time: %f' % (end-start) )
        training_loss_g[epoch-1] = loss_train_g
        sys.stdout.flush()

        # Save the model and the optimizer
        logger.info('saving the models')
        serializers.save_hdf5(os.path.join(args.outdir, 'g.model'), g)
        logger.info('saving the optimizers')
        serializers.save_hdf5(os.path.join(args.outdir, 'g.state'), optimizer_g)

        # test prediction
        # data for plot
        key = train.keys()[0]
        plot_src = train[key]['src'].value.astype(np.float32)
        plot_tgt = train[key]['tgt'].value.astype(np.float32)
        R, W = mlpg_test.mlpg_from_pdf(plot_src, wins)
        R = R.toarray()
        if norm_src is not None:
            plot_src = utils.normalize_gauss(plot_src, norm_src[0], norm_src[1]).astype(np.float32)
        if args.gpu >= 0:
            plot_src = cuda.to_gpu(plot_src.astype(np.float32), args.gpu)
            R = cuda.to_gpu(R.astype(np.float32), args.gpu)

        gen = g.generate_mgc(Variable(plot_src), R)

        base = 'train' + str(epoch)
        plot_gen = gen.data
        if norm_tgt is not None:
            plot_gen = utils.normalize_gauss(plot_gen, norm_tgt[0][:args.ndim_mgc], norm_tgt[1][:args.ndim_mgc], inv=True)
        out = os.path.join(args.outdir, 'train')
        utils.save_mgc(plot_tgt, plot_gen, out, base, args.ndim_mgc, dim=5)

    train.flush()
    train.close()

    # save results
    hdf5 = h5py.File(os.path.join(args.outdir, 'results.h5'), 'w')
    hdf5.create_dataset('train_loss_g', data=training_loss_g)
    hdf5.flush()
    hdf5.close()

if __name__ == "__main__":
    sys.exit(main())
