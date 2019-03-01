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



PROGRAM = vlpf

SRCS = main.cc bw_filter.cc

OBJS = $(SRCS:.cc=.o)

$(PROGRAM): $(OBJS) $(TOP)/$(LIBDIR)/libsub.a $(TOP)/$(LIBDIR)/libsp.a
	@echo -n "Loading $(PROGRAM) ... "
	@$(CXX) $(CXXFLAGS) -o $(PROGRAM) $(OBJS) $(TOP)/$(LIBDIR)/libsub.a $(TOP)/$(LIBDIR)/libsp.a
	@echo "done"

clean:; @rm -f $(OBJS) $(PROGRAM)
