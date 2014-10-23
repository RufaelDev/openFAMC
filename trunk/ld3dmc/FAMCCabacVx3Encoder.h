/*
 * =====================================================================================
 * 
 *        Filename:  FAMCCabacVx3Encoder.h
 * 
 *     Description:  A cabac based encoder for quantized 3D points
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

#ifndef FAMC_CABAC_VX3_ENCODER_H
#define FAMC_CABAC_VX3_ENCODER_H

#include <vector>
#include "MeshTypes.h"
#include "CABAC.h"

class FAMCCabacVx3Encoder {

	unsigned char* buffer_;
	int nBytesEncTotal_;
	CABAC cabac;

public:
	FAMCCabacVx3Encoder(unsigned char* _buffer);

public:
	int encodeVx3(const std::vector<IPoint>& _res, int _sigMapInitProb, int _absInitProb);
	int encodeVx3(const std::vector<IPoint>& _res);
	int encodeBits(const std::vector<bool>& _bits, unsigned char _bitsInitProb);
	int encodeBits(const std::vector<bool>& _bits);
	int getNBytesEncoded() {return nBytesEncTotal_;}
	int getProb() const;

private:
	int encodeSigMapAndSigns(const std::vector<IPoint>& _res, int _sigMapInitProb);
	int encodeAbsValuesMinusOne(const std::vector<IPoint>& _res, int _numberOfAbsValuesBitPlanes, int _absInitProb);
	int getNumberOfAbsValuesBitPlanes(const std::vector<IPoint>& _res) const;

};

#endif //FAMC_CABAC_VX3_ENCODER_H
