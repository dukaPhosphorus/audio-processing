# Makefile for Library
#
#	Tomoki Toda (tomoki.toda@atr.co.jp)
#			From Mar. 2001 to Sep. 2003

TOP = ../
LIBDIR = lib
CXXFLAGS = -O2 -Wall
CPU = linux
SUFFIXES := .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.SUFFIXES: .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.cpp.o:
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c $< $(OUTPUT_OPTION)

PROGRAM = $(TOP)/$(LIBDIR)/libsp.$(CPU).a

SRCS = basic.cpp \
       complex.cpp \
       fft.cpp \
       fileio.cpp \
       filter.cpp \
       kaiser.cpp \
       matrix.cpp \
       memory.cpp \
       option.cpp \
       vector.cpp \
       voperate.cpp \
       window.cpp

OBJS = $(SRCS:.cpp=.o)

$(PROGRAM): $(OBJS)
	@echo -n "Loading $(PROGRAM) ... "
	@ar r $(PROGRAM) $(OBJS)
	@ranlib $(PROGRAM)
	@echo "done"

clean:; @rm -f $(OBJS) $(PROGRAM)
