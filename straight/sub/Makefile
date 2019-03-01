# Makefile for Library
#
#	Tomoki Toda (tomoki.toda@atr.co.jp)
#			From Mar. 2001 to Sep. 2003

TOP = ../..
SPLIBDIR = lib
LIBDIR = straight/lib
CXXFLAGS = -O2 -Wall
SUFFIXES := .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.SUFFIXES: .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.cpp.o:
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c $< $(OUTPUT_OPTION)

PROGRAM = $(TOP)/$(LIBDIR)/libsub.a

SRCS = straight_body_sub.cpp \
       straight_sub.cpp \
       straight_synth_sub.cpp \
       straight_vconv_sub.cpp \
       tempo_sub.cpp

OBJS = $(SRCS:.cpp=.o)

$(PROGRAM): $(OBJS) $(TOP)/$(SPLIBDIR)/libsp.a
	@echo -n "Loading $(PROGRAM) ... "
	@ar r $(PROGRAM) $(OBJS)
	@ranlib $(PROGRAM)
	@echo "done"

clean:; @rm -f $(OBJS) $(PROGRAM)
