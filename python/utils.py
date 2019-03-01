import numpy as np
import os
import copy
import fileio as io
import matplotlib.pyplot as plt

# make dir if it is not exists
def mkdir(dirname):
    if not os.path.exists(dirname):
        os.mkdir(dirname)

# zero mean, unit variance normarization
def normalize_gauss(z, mean, var, inv=False):
    if inv == False:
        return 1.0 / np.sqrt(var) * ( z - mean ) + 0
    else:
        return np.sqrt(var) / 1.0 * ( z - 0 ) + mean

# [0, 1] normalization
def normalize(z, min_z, max_z, margin = 0.01, inv=False):
   if inv == False:
      return (z - min_z) / (max_z - min_z) * (1.0 - margin * 2) + margin
   else:
      return (z - margin) / (1.0 - margin * 2) * (max_z - min_z) + min_z

# save and plot mcep trajectory
def save_mgc(nat, gen, outdir, base, ndim_mgc=24, dim=5, mge=False):
    fname_gen = os.path.join(outdir, 'mgc', 'gen', base)
    fname_png = os.path.join(outdir, 'mgc', 'plot', base)

    d1 = dim-1
    d2 = dim-1 + ndim_mgc+1
    plt.clf()
    plt.title('mgc trajectory (dim=%s)' % (str(dim)))
    plt.plot(nat[:, d1], color='blue')
    plt.plot(gen[:, d1], color='red')
    plt.savefig(fname_png + '.png')
    io.writematrix(gen, fname_gen + '.mgc', dfmt=True)
