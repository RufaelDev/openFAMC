/*
 * =====================================================================================
 * 
 *        Filename:  LDRecon3D.h
 * 
 *     Description:  A 3D reconstructor
 * 
 *         Version:  1.0
 *         Created:  03/07/07 15:53:41 CET
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef  LD_RECON_3D_H
#define  LD_RECON_3D_H

#include "LDRecon1D.h"

class LDRecon3D{
public:
	LDRecon3D( Point  _delta): recon0_(_delta[0]), recon1_(_delta[1]), recon2_(_delta[2]){}
	LDRecon3D( double _delta): recon0_(_delta)   , recon1_(_delta)   , recon2_(_delta){}
	Point operator()(IPoint  _ip) const { return Point(recon0_(_ip[0]), recon1_(_ip[1]), recon2_(_ip[2]));}

private:
	const LDRecon1D recon0_;
	const LDRecon1D recon1_;
	const LDRecon1D recon2_;
};

#endif   // LD_RECON_3D_H
