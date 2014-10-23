/*
 * =====================================================================================
 * 
 *        Filename:  FAMCSimplificationModeDecoder.h
 * 
 *     Description:  A cabac based decoder for simplification modes
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

#ifndef FAMC_SIMPLIFICATION_MODE_DECODER_H
#define FAMC_SIMPLIFICATION_MODE_DECODER_H

#include "SimplificationDecomposer.h"
#include <vector>

class FAMCSimplificationModeDecoder{
	unsigned char* buffer_;
	int nBytesDecTotal_;

public:
	FAMCSimplificationModeDecoder(unsigned char* _buffer);
	int decode (int _nValues, std::vector<int>& _mode);
};

#endif //FAMC_SIMPLIFICATION_MODE_DECODER_H