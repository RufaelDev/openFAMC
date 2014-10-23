/*
 * =====================================================================================
 * 
 *        Filename:  LDFrameOrderFactory.h
 * 
 *     Description:  A class generating a frame order 
 * 
 *         Version:  1.0
 *         Created:  03/04/07 11:29:39 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef  LD_FRAME_ORDER_FACTORY_H
#define  LD_FRAME_ORDER_FACTORY_H

#include "LDFrameTypeData.h"
#include <vector>

class LDFrameOrderFactory {
    //CONSTRUCTORS
    public:
	LDFrameOrderFactory(int _nFrames, int _LD_P, int _LD_B);

    //MEMBER FUNCTIONS
    public:
	LDFrameTypeData getFrameTypeData(int _frameNumberEnc);

    //MEMBER VARIABLES
    private: 
	const int nFrames_;
	const int LD_P_;
	const int LD_B_;

	std::vector<LDFrameType> frameType_;
	std::vector<int> refFrameNumberDis0_;
	std::vector<int> refFrameNumberDis1_;
	std::vector<int> frameNumberEnc2DisList_;
	std::vector<int> frameNumberDis2EncList_;

	std::vector<LDFrameTypeData> ftd_;

};

#endif   //LD_FRAME_ORDER_FACTORY_H
