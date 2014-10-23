#include "FAMCCabacVx3Encoder.h"
#include "CABAC.h"
#include "MathTools.h"
#include <stdio.h>

FAMCCabacVx3Encoder::FAMCCabacVx3Encoder(unsigned char* _buffer) 
	: buffer_(_buffer), nBytesEncTotal_(0){
}

int FAMCCabacVx3Encoder::encodeSigMapAndSigns(const std::vector<IPoint>& _res, int _sigMapInitProb){
	int nValues=_res.size();
	int nBytesEnc=0;
	int streamSize=0;
	short value;
	
	cabac.arienco_start_encoding( cabac._eep, buffer_, &streamSize);
	cabac.biari_init_context(cabac._ctx, _sigMapInitProb);
	for (int v=0; v < nValues; ++v){ 
		for (int d = 0; d < 3; d++){	
			value = (_res[v][d]==0) ? 0 : 1;
			cabac.biari_encode_symbol(cabac._eep, value, cabac._ctx);
		}
	}
	for (int v = 0; v < nValues; v++){
		for (int d = 0; d < 3; d++){
			if (_res[v][d] != 0) {
				value = (_res[v][d]<0) ? 1 : 0;
				cabac.biari_encode_symbol_eq_prob(cabac._eep, value);
			}
		}
	}
	cabac.biari_encode_symbol_final(cabac._eep, 1);
	cabac.arienco_done_encoding(cabac._eep);

	nBytesEnc = streamSize;

	return nBytesEnc;
}

int FAMCCabacVx3Encoder::encodeAbsValuesMinusOne(const std::vector<IPoint>& _res, int _numberOfAbsValuesBitPlanes, int _absInitProb){
	int nValues=_res.size();
	int nBytesEnc=0;
	int streamSize=0;
	short value=0;
	unsigned int value_tmp=0;

	cabac.arienco_start_encoding( cabac._eep, buffer_, &streamSize);
	cabac.biari_init_context(cabac._ctx, _absInitProb); 
	for (int pb = _numberOfAbsValuesBitPlanes - 1; pb >= 0; pb--) {
		for (int v = 0; v < nValues; v++){
			for (int d = 0; d < 3; d++){
				if (_res[v][d] != 0) {
					value_tmp = (abs(_res[v][d])-1) & (1<<pb);
					value = (value_tmp!=0) ? 1 : 0;
					cabac.biari_encode_symbol(cabac._eep, value, cabac._ctx);
				}
			}
		}
	}
	cabac.biari_encode_symbol_final(cabac._eep, 1);
	cabac.arienco_done_encoding(cabac._eep);

	nBytesEnc = streamSize;

	return nBytesEnc;
}

int FAMCCabacVx3Encoder::encodeBits(const std::vector<bool>& _bits, unsigned char _bitsInitProb){
	int nValues=_bits.size();
	int nBytesEnc=0;
	int streamSize=0;
	short value;

	buffer_[0]=_bitsInitProb; // PDAM: 6-bits
	++nBytesEnc;
	++buffer_;

	cabac.arienco_start_encoding(cabac._eep, buffer_, &streamSize);
	cabac.biari_init_context(cabac._ctx, _bitsInitProb);
	for (int v=0; v<(int)_bits.size(); ++v){
		value = (_bits[v]) ? 1 : 0;
		cabac.biari_encode_symbol(cabac._eep, value, cabac._ctx);
	}
	cabac.biari_encode_symbol_final(cabac._eep, 1);
	cabac.arienco_done_encoding(cabac._eep);

	nBytesEnc+=streamSize;
	buffer_+=streamSize;

	nBytesEncTotal_+=nBytesEnc;

	return nBytesEnc;
}

int FAMCCabacVx3Encoder::encodeBits(const std::vector<bool>& _bits){
	int nValues=_bits.size();
	int nBytesEnc=0;
	int streamSize=0;
	short value;

	static int bitsInitProb=61;

	buffer_[0]=bitsInitProb; // PDAM: 6-bits
	++nBytesEnc;
	++buffer_;

	cabac.arienco_start_encoding(cabac._eep, buffer_, &streamSize);
	cabac.biari_init_context(cabac._ctx, bitsInitProb);
	for (int v=0; v<(int)_bits.size(); ++v){
		value = (_bits[v]) ? 1 : 0;
		cabac.biari_encode_symbol(cabac._eep, value, cabac._ctx);
	}
	cabac.biari_encode_symbol_final(cabac._eep, 1);
	cabac.arienco_done_encoding(cabac._eep);

	bitsInitProb = getProb();

	nBytesEnc+=streamSize;
	buffer_+=streamSize;

	nBytesEncTotal_+=nBytesEnc;

	return nBytesEnc;
}

int FAMCCabacVx3Encoder::encodeVx3(const std::vector<IPoint>& _res, int _sigMapInitProb, int _absInitProb){
	int nBytesEnc=0;
	int nBytesEncCabac=0;
	int streamSize=0;

	//encoding parameter, sig map, and signs
	buffer_[0]=_sigMapInitProb; // PDAM: 6-bits
	++nBytesEnc;
	++buffer_;
	
	nBytesEncCabac = encodeSigMapAndSigns(_res, _sigMapInitProb);
	nBytesEnc += nBytesEncCabac;
	buffer_ += nBytesEncCabac;

	//encoding parameters and abs values minus one
	buffer_[0]=_absInitProb; // PDAM: 6-bits
	++nBytesEnc;
	++buffer_;

	int numberOfAbsValuesBitPlanes = getNumberOfAbsValuesBitPlanes(_res);
	buffer_[0]=numberOfAbsValuesBitPlanes; // PDAM: 8-bits
	++nBytesEnc;
	++buffer_;

	nBytesEncCabac = encodeAbsValuesMinusOne(_res, numberOfAbsValuesBitPlanes, _absInitProb);
	nBytesEnc += nBytesEncCabac;
	buffer_ += nBytesEncCabac;

	nBytesEncTotal_ += nBytesEnc;

	return nBytesEnc;
}

int FAMCCabacVx3Encoder::encodeVx3(const std::vector<IPoint>& _res){
	int nBytesEnc=0;
	int nBytesEncCabac=0;
	int streamSize=0;

	static int sigMapInitProb=61;
	static int absInitProb=61;

	//encoding parameter, sig map, and signs
	buffer_[0]=sigMapInitProb; // PDAM: 6-bits
	++nBytesEnc;
	++buffer_;
	
	nBytesEncCabac = encodeSigMapAndSigns(_res, sigMapInitProb);
	nBytesEnc += nBytesEncCabac;
	buffer_ += nBytesEncCabac;

	sigMapInitProb=getProb();

	//encoding parameters and abs values minus one
	buffer_[0]=absInitProb; // PDAM: 6-bits
	++nBytesEnc;
	++buffer_;

	int numberOfAbsValuesBitPlanes = getNumberOfAbsValuesBitPlanes(_res);
	buffer_[0]=numberOfAbsValuesBitPlanes; // PDAM: 8-bits
	++nBytesEnc;
	++buffer_;

	nBytesEncCabac = encodeAbsValuesMinusOne(_res, numberOfAbsValuesBitPlanes, absInitProb);
	nBytesEnc += nBytesEncCabac;
	buffer_ += nBytesEncCabac;

	absInitProb=getProb();


	nBytesEncTotal_ += nBytesEnc;

	return nBytesEnc;
}

int FAMCCabacVx3Encoder::getNumberOfAbsValuesBitPlanes(const std::vector<IPoint>& _res) const{
	int maxAbs=0;
	for (int c=0; c<_res.size(); ++c){
		maxAbs = std::max(std::max(std::max(abs(_res[c][0]), abs(_res[c][1])), abs(_res[c][2])), maxAbs);
	}
	return MathTools::nBinaryBits(maxAbs);
}

int FAMCCabacVx3Encoder::getProb() const {
	if (cabac._ctx->MPS == 1) {
		return cabac._ctx->state + 64;
	}
	else {
		return 63 - cabac._ctx->state;
	}
}