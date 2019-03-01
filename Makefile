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
##  TECHNOLOGY, CARNEGIE MELLON UNiVERSITY, AND THE CONTRIBUTORS TO  ##
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
##  Makefile for A Voice Conversion Package                          ##
##                                                                   ##
#######################################################################

TOP = .

all:	
	@cd sp && make -f Makefile
	@cd sub && make -f Makefile
	@cd analysis && make -f Makefile
	@cd synthesis && make -f Makefile
	@cd cov2dia && make -f Makefile
	@cd dtw && make -f Makefile
	@cd extdim && make -f Makefile
	@cd extfrm && make -f Makefile
	@cd f0map && make -f Makefile
	@cd f0stats && make -f Makefile
	@cd gmm && make -f Makefile
	@cd gmmmap && make -f Makefile
	@cd gmmpara && make -f Makefile
	@cd gmmpara && make -f Makefile_ext
	@cd jdmat && make -f Makefile
	@cd mlpg && make -f Makefile
	@cd mlpg && make -f Makefile_dlt
	@cd mlpg && make -f Makefile_vit
	@cd vq && make -f Makefile
	@cd vqlbl && make -f Makefile
	@cd addmat && make -f Makefile
	@echo ====================
	@echo Programs for Voice Conversion, ALL O.K.
	@echo ====================
	@cd straight && make -f Makefile
	@cd normpow && make -f Makefile
	# @cd histgrm && make -f Makefile

clean:
	@cd sp && make clean -f Makefile
	@cd sub && make clean -f Makefile
	@cd analysis && make clean -f Makefile
	@cd synthesis && make clean -f Makefile
	@cd cov2dia && make clean -f Makefile
	@cd dtw && make clean -f Makefile
	@cd extdim && make clean -f Makefile
	@cd extfrm && make clean -f Makefile
	@cd f0map && make clean -f Makefile
	@cd f0stats && make clean -f Makefile
	@cd gmm && make clean -f Makefile
	@cd gmmmap && make clean -f Makefile
	@cd gmmpara && make clean -f Makefile
	@cd gmmpara && make clean -f Makefile_ext
	@cd jdmat && make clean -f Makefile
	@cd mlpg && make clean -f Makefile
	@cd mlpg && make clean -f Makefile_dlt
	@cd mlpg && make clean -f Makefile_vit
	@cd vq && make clean -f Makefile
	@cd vqlbl && make clean -f Makefile
	@cd addmat && make clean -f Makefile
	@echo ====================
	@echo Programs for Voice Conversion, ALL CLEAN O.K.
	@echo ====================
	@cd straight && make clean -f Makefile
	# @cd histgrm && make clean -f Makefile
	@cd normpow && make clean -f Makefile
