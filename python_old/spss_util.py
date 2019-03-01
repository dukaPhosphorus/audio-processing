import numpy as np
import time
from matplotlib import pyplot as plt

def apply_delta_win(x, wins):
   o = [np.zeros(np.array(x).shape) for i in range(len(wins))]

   for win in range(len(wins)):
      if len(wins) % 2 != 1:
         ValueError("win length must be odd.")

      for w in range(len(wins[win])):
         shift_t = int(len(wins[win]) * 0.5) - w
         shift_x = np.roll(x, shift_t, axis = 0)

         if shift_t > 0:
            shift_x[:shift_t, :] = 0.0
         elif shift_t < 0:
            shift_x[shift_t:, :] = 0.0

         o[win] += wins[win][w] * shift_x

   # concatenate
   X = o[0]
   for win in range(1, len(wins)):
      X = np.c_[X, o[win]]

   return X

def dtw(x, y, dist, wgt = [[1.0, 1.0], [1.0, 1.0]]):
    assert len(x)
    assert len(y)
    r, c = len(x), len(y)
    D0 = np.zeros((r + 1, c + 1))
    D0[0, 1:] = np.inf
    D0[1:, 0] = np.inf
    D1 = D0[1:, 1:] # view
    w = np.array(wgt) # transition weight
    # print w

    # frame-wise calculation
    # t1 = time.time()
    I = np.ones((c, len(x[0])))
    for i in range(r):
       D1[i, :] = dist(I * x[i], y)
    # t2 = time.time()
    # print t2 - t1

    #for i in range(r):
    #    for j in range(c):
    #        D1[i, j] = dist(x[i], y[j])
    C = D1.copy()
    for i in range(r):
        for j in range(c):
            D1[i, j] += min(w[1, 1] * D0[i, j], w[0, 1] * D0[i, j+1], w[1, 0] * D0[i+1, j])

    if len(x)==1:
        path = np.zeros(len(y)), range(len(y))
    elif len(y) == 1:
        path = range(len(x)), zeros(len(x))
    else:
        path = _traceback(D0, wgt)
    # return D1[-1, -1] / sum(D1.shape), C, D1, path (distance, cost, accuracy, path)
    # print path
    # plt.plot(np.array(path)[0, :], np.array(path)[1, :])
    # plt.show()
    return path

def _traceback(D, wgt):
    i, j = np.array(D.shape) - 2
    p, q = [i], [j]
    w = np.array(wgt)

    while ((i > 0) or (j > 0)):
        tb = np.argmin((w[1, 1] * D[i, j], w[0, 1] * D[i, j+1], w[1, 0] * D[i+1, j]))
        # print i, j, tb, w[1, 1] * D[i, j], w[0, 1] * D[i, j+1], w[1, 0] * D[i+1, j]
        if (tb == 0):
            i -= 1
            j -= 1
        elif (tb == 1):
            i -= 1
        else: # (tb == 2):
            j -= 1
        p.insert(0, i)
        q.insert(0, j)
    return np.array(p), np.array(q)


class my_distance_measure():
   def __init__(self, idx = None):
      self.idx = idx

   def euclid(self, x, y):
      index = range(x.shape[0]) if (self.idx is None) else self.idx
      return np.sum(np.square(x[index] - y[index]), axis = len(x.shape) - 1)
      # return (x[index] - y[index]).T.dot(x[index] - y[index])

   def melcep_dist(self, x, y):
      index = range(len(x)) if (self.idx is None) else self.idx
      return 10.0 / np.log(10.0) * np.sqrt(2.0 * self.euclid(x, y))

if __name__ == '__main__':
   wins = [[1.0], [-0.5, 0.0, 0.5], [1, -2, 1]]
   c = np.array([[-1.0, -2.0], [6.0, 5.0], [1.0, 2.0], [1.0, 2.0]])
   c2 = np.array([[-1.0, -2.0], [2.0, 3.0], [1.0, 2.0], [2.0, 3.0], [3.0, 3.0], [1.0, 2.0], [1.0, 2.0]])

   o = apply_delta_win(c, wins)
   # print o

   dist = my_distance_measure()
   path = dtw(c, c2, dist.euclid, wgt = [[1, 1], [1.0, 1.0]])
   print path
   # print dist.euclid(c[1], c2[1])
   # print dist.euclid(c[0:2], c2[0:2])


