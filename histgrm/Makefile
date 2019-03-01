# Makefile for Segment Selection

TOP = ..
LIBDIR = lib
STDIR = straight
#CXXFLAGS = -O2 -Wall
CXXFLAGS = -Wall -Wno-deprecated
CPU = linux

PROGRAM = histgrm

SRCS = main.cpp \
	statistic.cpp

OBJS = $(SRCS:.cpp=.o)

$(PROGRAM): $(OBJS) $(TOP)/$(STDIR)/$(LIBDIR)/libsub.a $(TOP)/$(LIBDIR)/libsp.a
	@echo -n "Loading $(PROGRAM) ... "
	@$(CXX) $(CXXFLAGS) -o $(PROGRAM) $(OBJS) $(TOP)/$(STDIR)/$(LIBDIR)/libsub.a $(TOP)/$(LIBDIR)/libsp.a
	@echo "done"

clean:; @rm -f $(OBJS) $(PROGRAM)
