/*
 * =====================================================================================
 * 
 *        Filename:  FAMCSimplificationModeEncoder.h
 * 
 *     Description:  A cabac based encoder for simplification modes
 * 
 *         Version:  1.0
 *         Created:  10/04/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef FAMC_SIMPLIFICATION_MODE_ENCODER_H
#define FAMC_SIMPLIFICATION_MODE_ENCODER_H

#include "SimplificationDecomposer.h"
#include <vector>

class FAMCSimplificationModeEncoder{
	unsigned char* buffer_;
	int nBytesEncTotal_;

public:
	FAMCSimplificationModeEncoder(unsigned char* _buffer);
	int encode (const std::vector<int>& _mode);
};

#endif //FAMC_SIMPLIFICATION_MODE_ENCODER_H