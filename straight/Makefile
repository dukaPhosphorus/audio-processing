#######################################################################
##                                                                   ##
##          Author :  Tomoki Toda (tomoki@ics.nitech.ac.jp)          ##
##          Date   :  June 2004                                      ##
##                                                                   ##
#######################################################################

TOP = .

all:	
	@cd sub && make -f Makefile
	@cd tempo && make -f Makefile
	@cd analysis && make -f Makefile
	@cd analysis && make -f Makefile_ap
	@echo ====================
	@echo Programs for STRAIGHT, ALL O.K.

clean:
	@cd sub && make clean -f Makefile
	@cd tempo && make clean -f Makefile
	@cd analysis && make clean -f Makefile
	@cd analysis && make clean -f Makefile_ap
	@echo ====================
	@echo Programs for STRAIGHT, ALL CLEAN O.K.
