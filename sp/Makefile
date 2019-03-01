#######################################################################
##                                                                   ##
##            Nagoya Institute of Technology, Aichi, Japan,          ##
##       Nara Institute of Science and Technology, Nara, Japan       ##
##                                and                                ##
##             Carnegie Mellon University, Pittsburgh, PA            ##
##                      Copyright (c) 2003-2004                      ##
##                        All Rights Reserved.                       ##
##                                                                   ##
##  Permission is hereby granted, free of charge, to use and         ##
##  distribute this software and its documentation without           ##
##  restriction, including without limitation the rights to use,     ##
##  copy, modify, merge, publish, distribute, sublicense, and/or     ##
##  sell copies of this work, and to permit persons to whom this     ##
##  work is furnished to do so, subject to the following conditions: ##
##                                                                   ##
##    1. The code must retain the above copyright notice, this list  ##
##       of conditions and the following disclaimer.                 ##
##    2. Any modifications must be clearly marked as such.           ##
##    3. Original authors' names are not deleted.                    ##
##                                                                   ##    
##  NAGOYA INSTITUTE OF TECHNOLOGY, NARA INSTITUTE OF SCIENCE AND    ##
##  TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, AND THE CONTRIBUTORS TO  ##
##  THIS WORK DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,  ##
##  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, ##
##  IN NO EVENT SHALL NAGOYA INSTITUTE OF TECHNOLOGY, NARA           ##
##  INSTITUTE OF SCIENCE AND TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, ##
##  NOR THE CONTRIBUTORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR      ##
##  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM   ##
##  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,  ##
##  NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN        ##
##  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.         ##
##                                                                   ##
#######################################################################
##                                                                   ##
##          Author :  Tomoki Toda (tomoki@ics.nitech.ac.jp)          ##
##          Date   :  June 2004                                      ##
##                                                                   ##
#######################################################################
##                                                                   ##
##  Makefile for Signal Processing Library                           ##
##                                                                   ##
#######################################################################

TOP = ..
LIBDIR = lib
CXX = g++
CXXFLAGS = -O2 -Wall

OUTPUT_OPTION = -o $@
SUFFIXES := .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.SUFFIXES: .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.cc.o:
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c $< $(OUTPUT_OPTION)

PROGRAM = $(TOP)/$(LIBDIR)/libsp.a

SRCS = basic.cc \
       complex.cc \
       fft.cc \
       fileio.cc \
       filter.cc \
       kaiser.cc \
       matrix.cc \
       memory.cc \
       option.cc \
       vector.cc \
       voperate.cc \
       window.cc

OBJS = $(SRCS:.cc=.o)

$(PROGRAM): $(OBJS)
	@echo -n "Loading $(PROGRAM) ... "
	@ar r $(PROGRAM) $(OBJS)
	@ranlib $(PROGRAM)
	@echo "done"

clean:; @rm -f $(OBJS) $(PROGRAM)
