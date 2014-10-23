/*
 * =====================================================================================
 * 
 *        Filename:  LDQuant1D.h
 * 
 *     Description:  Scalar Uniform 1D Quantizer
 * 
 *         Version:  1.0
 *         Created:  03/06/07 16:49:57 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */


#ifndef  LD_QUANT_1D_H
#define  LD_QUANT_1D_H

#include <cmath>
#include <cassert>

class LDQuant1D{

public:
	LDQuant1D(double  _delta) : delta_(_delta) {assert(delta_>0);}
	int operator()(double  _d) const { return int(floor(_d/delta_ + 0.5));}

private:
	const double  delta_;
};

#endif   //LD_QUANT_1D_H
