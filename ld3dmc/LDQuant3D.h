/*
 * =====================================================================================
 * 
 *        Filename:  LDQuant3D.h
 * 
 *     Description: A scalar unform 3D quantizer
 * 
 *         Version:  1.0
 *         Created:  03/07/07 15:12:30 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef  LD_QUANT_3D_H
#define  LD_QUANT_3D_H

#include "MeshTypes.h"
#include "LDQuant1D.h"

class LDQuant3D {

public:
	LDQuant3D(Point  _delta): quat0_(_delta[0]), quat1_(_delta[1]), quat2_(_delta[2]){}
	LDQuant3D(double _delta): quat0_(_delta)   , quat1_(_delta)   , quat2_(_delta){}
	IPoint operator()(Point  _p) const { return IPoint(quat0_(_p[0]), quat1_(_p[1]), quat2_(_p[2]));}	

private:
	const LDQuant1D quat0_;
	const LDQuant1D quat1_;
	const LDQuant1D quat2_;
};

#endif   //LD_QUANT_3D_H
