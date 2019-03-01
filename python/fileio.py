import struct
import numpy
import os

def writematrix(x, fn, dfmt=False):
   odata = ''
   if (dfmt == True):
      fmt = '<d'
   else:
      fmt = '<f'

   wdir = os.path.dirname(fn)

   if (os.path.exists(wdir) is False and len(wdir) > 0):
      os.makedirs(os.path.dirname(fn))

   f = open(fn, 'wb')
   for t in range(len(x)):
      for d in range(len(x[t])):
         odata += struct.pack(fmt, x[t][d])
   f.write(odata)
   f.close()

def writevector(x, fn, dfmt=False):
   odata = ''
   if (dfmt == True):
      fmt = '<d'
   else:
      fmt = '<f'

   wdir = os.path.dirname(fn)

   if (os.path.exists(wdir) is False and len(wdir) > 0):
      os.makedirs(os.path.dirname(fn))

   f = open(fn, 'wb')
   for t in range(len(x)):
      odata += struct.pack(fmt, x[t])
   f.write(odata)
   f.close()

def readmatrix(fn, dim, dfmt=False):
   x = []
   if (dfmt == True):
      fmt = '<' + str(dim) + 'd'
      len = 8
   else:
      fmt = '<' + str(dim) + 'f'
      len = 4

   f = open(fn, 'rb')
   while True:
      tmp = f.read(dim * len)
      if (tmp == ''):
         break
      x.append(numpy.array(struct.unpack(fmt, tmp)))
   f.close()
   return numpy.array(x)

def readvector(fn, dfmt=False):
   x = []
   if (dfmt == True):
      fmt = '<d'
      len = 8
   else:
      fmt = '<f'
      len = 4

   f = open(fn, 'rb')
   while True:
      tmp = f.read(len)
      if (tmp == ''):
         break
      x.extend(numpy.array(struct.unpack(fmt, tmp)))
   f.close()
   return numpy.array(x)

def readmatrices(fn_list, dim, dfmt=False):
   x = []

   for f in open(fn_list).readlines():
      f = f.strip("\n")
      x.append(readmatrix(f, dim, dfmt=dfmt))
   return numpy.array(x)

def readvectors(fn_list, dfmt=False):
   x = []

   for f in open(fn_list).readlines():
      f = f.strip("\n")
      x.append(readvector(f, dfmt=dfmt))
   return numyp.array(x)