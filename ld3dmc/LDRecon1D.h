/*
* =====================================================================================
* 
*        Filename:  
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  03/07/07 13:33:54 CET
*        Revision:  none
*        Compiler:  gcc
* 
*          Author:  Nikolce Stefanoski (nst)
*         Company:  Institut für Informationsverarbeitung
*           Email:  stefanos@tnt.uni-hannover.de
* 
* =====================================================================================
*/


#ifndef  LD_RECON_1D_H
#define  LD_RECON_1D_H


#include <cmath>
#include <cassert>

class LDRecon1D{
public:
	LDRecon1D( double _delta): delta_(_delta){assert(delta_>0);}
	double operator()(int _i) const { return double(_i)*delta_;}
private:
	const double delta_;
};

#endif   // LD_RECON_1D_H 
