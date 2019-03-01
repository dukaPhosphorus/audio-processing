import sys
import os
import matplotlib.pyplot as plt
import numpy as np
import fileio as io

if __name__ == "__main__":
    scp = sys.argv[1]
    name = sys.argv[2]
    out = sys.argv[3]

    F0 = np.array([0.0])

    with open(scp) as fns:
        for fn in fns:
            f0file = 'data/f0def/' + name + '/' +  fn[:-1] + '.f0'
            f0 = np.loadtxt(f0file)
            F0 = np.hstack((F0, f0[f0 != 0.0]))

    plt.hist(F0[1:], bins=760, normed=True)
    plt.xlabel('Frequency [Hz]')
    plt.ylabel('Probability')
    plt.savefig(out)
