import chainer
import chainer.functions as F
import chainer.links as L
from parameter_generation import parameter_generation

class Generator(chainer.Chain):
    def __init__(self, n_src, n_hid, n_tgt):
        super(Generator, self).__init__(
            l1=L.Linear(n_src, n_hid),
            l2=L.Linear(n_hid, n_hid),
            l3=L.Linear(n_hid, n_hid),
            l4=L.Linear(n_hid, n_tgt),
        )

    def __call__(self, src):
        h = F.relu(self.l1(src))
        h = F.relu(self.l2(h))
        h = F.relu(self.l3(h))
        return self.l4(h)

    def generate_mgc(self, src, R=None):
        pred = self(src)
        if R is not None:
            pred = parameter_generation(R, pred)
        return pred

    def forward(self, src, tgt):
        pred = self(src)
        loss = F.mean_squared_error(pred, tgt)
        return loss, pred
