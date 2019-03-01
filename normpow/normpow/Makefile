# Makefile for STRAIGHT analysis
#
#	Tomoki Toda (tomoki.toda@atr.co.jp)
#			From Mar. 2001 to Sep. 2003

TOP = ..
LIBDIR = lib
CXX = g++
CXXFLAGS = -O2 -Wall
#CXXFLAGS = -Wall
CPU = linux
SUFFIXES := .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.SUFFIXES: .out .a .ln .o .c .cc .cpp .C .p .f .F .r .y .l .s .S .mod .sym .def .h .info .dvi .tex .texinfo .texi .txinfo .w .ch .web .sh .elc .el
.cpp.o:
	$(CXX) $(CXXFLAGS) $(TARGET_ARCH) -c $< $(OUTPUT_OPTION)

PROGRAM = normpow

SRCS = main.cpp

OBJS = $(SRCS:.cpp=.o)

$(PROGRAM): $(OBJS) $(TOP)/$(LIBDIR)/libsub.$(CPU).a $(TOP)/$(LIBDIR)/libsp.$(CPU).a
	@echo -n "Loading $(PROGRAM) ... "
	@$(CXX) $(CXXFLAGS) -o $(PROGRAM) $(OBJS) $(TOP)/$(LIBDIR)/libsub.$(CPU).a $(TOP)/$(LIBDIR)/libsp.$(CPU).a
	@echo "done"

clean:; @rm -f $(OBJS) $(PROGRAM)
