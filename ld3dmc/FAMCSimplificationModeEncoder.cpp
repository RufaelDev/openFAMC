#include "FAMCSimplificationModeEncoder.h"
#include "CABAC.h"
#include "SimplificationDecomposer.h"
#include <stdio.h>

FAMCSimplificationModeEncoder::FAMCSimplificationModeEncoder(unsigned char* _buffer) 
	: buffer_(_buffer), nBytesEncTotal_(0){}

int FAMCSimplificationModeEncoder::encode(const std::vector<int>& _mode){
	CABAC cabac;
	int nBytesEnc = 0;
	int streamSize = 0;

	cabac.arienco_start_encoding( cabac._eep, buffer_+nBytesEncTotal_, &streamSize);
	cabac.biari_init_context(cabac._ctx, 61);
	for (int c=0; c < _mode.size(); ++c){ 
		cabac.unary_exp_golomb_encode(cabac._eep, _mode[c], cabac._ctx, 2);
	}

	cabac.biari_encode_symbol_final(cabac._eep, 1);
	cabac.arienco_done_encoding(cabac._eep);

	nBytesEnc = streamSize;
	nBytesEncTotal_+=nBytesEnc;

	return nBytesEnc;
}