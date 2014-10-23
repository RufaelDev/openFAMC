/*
 * =====================================================================================
 * 
 *        Filename:  FAMCCabacVx3Decoder.h
 * 
 *     Description:  A cabac based decoder for quantized 3D points
 * 
 *         Version:  1.0
 *         Created:  07/25/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef FAMC_CABAC_VX3_DECODER_H
#define FAMC_CABAC_VX3_DECODER_H

#include <vector>
#include "MeshTypes.h"

class FAMCCabacVx3Decoder {
	unsigned char* buffer_;
	int nBytesDecTotal_;

public:
	FAMCCabacVx3Decoder(unsigned char* _buffer);

public:
	int decodeVx3 (int _nValues, std::vector<IPoint>& _res);
	int decodeBits(int _nValues, std::vector<bool>& _bits);
	int getNBytesDecoded() {return nBytesDecTotal_;}

private:
	int decodeSigMapAndSigns   (int _nValues, std::vector<IPoint>& _res, int _sigMapInitProb);
	int decodeAbsValuesMinusOne(int _nValues, std::vector<IPoint>& _res, int _numberOfAbsValuesBitPlanes, int _absInitProb);

};

#endif //FAMC_CABAC_VX3_DECODER_H
