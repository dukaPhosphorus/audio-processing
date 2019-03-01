/* **** warning ****
STRAIGHT is always under construction. Please use it with your own risk.
**** end of warning ****

The procedures and data, so-called STRAIGHT-suite, in this directory
are under the copyright of ATR Human Information Processing Research
laboratories, Wakayama University, CREST and Prof. Hideki Kawahara. No
programs and data in this directory should be distributed without the
written permission by Prof. Hideki Kawahara.

Hideki Kawahara, Professor
Media Design Informatics Group 
Design and Information Science Dept.
Faculty of Systems Engineering, Wakayama University
Sakaedani, Wakayama, Wakayama 640-8510, Japan
Phone: +81-734-57-8461 Fax: +81-734-57-8112
E-mail: kawahara@sys.wakayama-u.ac.jp

However, the copyright holders do not wish to restrict the use of
STRAIGHT-suite for research purpose. Permission for research use
may be provided without any problem, if requested. If you wish to
apply STRAIGHT-suite for commercial use, please contact Dr. Katagiri
of ATR Human Information Processing Research laboratories.

Shigeru Katagiri, Dr.
Head, 1st department
ATR Human Information Processing Research Laboratories
2-2 Hikaridai, Seika-cho, Soraku-gun,
Kyoto 619-0288, Japan
Phone: +81-774-95-1054  Fax: +81-774-95-1008
E-mail: katagiri@hip.atr.co.jp

NO WARRANTY: STRAIGHT-suite is always under construction. The
copyright holders do not have any responsibilities for possible damage
or loss of assets which may be caused by improper or proper use of
STRAIGHT-suite. Please use it at your own risk.

STRAIGHT support page:
Please do not disclose these URL. They are subject to change.
http://www.sys.wakayama-u.ac.jp/~kawahara/puzzlet/STRAIGHTtipse/

Last update: 23 December, 2000.
*/

/*********************************************************************/
/*                                                                   */
/*            Nagoya Institute of Technology, Aichi, Japan,          */
/*       Nara Institute of Science and Technology, Nara, Japan       */
/*                                and                                */
/*             Carnegie Mellon University, Pittsburgh, PA            */
/*                      Copyright (c) 2003-2004                      */
/*                        All Rights Reserved.                       */
/*                                                                   */
/*  Permission is hereby granted, free of charge, to use and         */
/*  distribute this software and its documentation without           */
/*  restriction, including without limitation the rights to use,     */
/*  copy, modify, merge, publish, distribute, sublicense, and/or     */
/*  sell copies of this work, and to permit persons to whom this     */
/*  work is furnished to do so, subject to the following conditions: */
/*                                                                   */
/*    1. The code must retain the above copyright notice, this list  */
/*       of conditions and the following disclaimer.                 */
/*    2. Any modifications must be clearly marked as such.           */
/*    3. Original authors' names are not deleted.                    */
/*                                                                   */    
/*  NAGOYA INSTITUTE OF TECHNOLOGY, NARA INSTITUTE OF SCIENCE AND    */
/*  TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, AND THE CONTRIBUTORS TO  */
/*  THIS WORK DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,  */
/*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, */
/*  IN NO EVENT SHALL NAGOYA INSTITUTE OF TECHNOLOGY, NARA           */
/*  INSTITUTE OF SCIENCE AND TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, */
/*  NOR THE CONTRIBUTORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR      */
/*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM   */
/*  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,  */
/*  NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN        */
/*  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.         */
/*                                                                   */
/*********************************************************************/
/*                                                                   */
/*          Author :  Tomoki Toda (tomoki@ics.nitech.ac.jp)          */
/*          Date   :  June 2004                                      */
/*                                                                   */
/*-------------------------------------------------------------------*/
/*                                                                   */
/*  TEMPO V30k18 (matlab)                                            */
/*                                                                   */
/*-------------------------------------------------------------------*/

#ifndef __TEMPO_H
#define __TEMPO_H

extern DVECTORS tempo(DVECTOR x, double maxf0, double minf0, double f0ceil,
		      double f0floor, double fs, double shiftm,
		      double f0shiftm, char *cf0file, XBOOL f0var_flag,
		      XBOOL allparam_flag, XBOOL msg_flag, XBOOL fast_flag);
extern DMATRICES fixpF0VexMltpBG4(DVECTOR x, double fs, double f0floor,
				 long nvc, long nvo, double mu,
				  double shiftm, double smp, double minm,
				 double pc, int nc, XBOOL allparam_flag);

#endif /* __TEMPO_H */
