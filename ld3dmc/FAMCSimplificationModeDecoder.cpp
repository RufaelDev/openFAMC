#include "FAMCSimplificationModeDecoder.h"
#include "CABAC.h"
#include "SimplificationDecomposer.h"
#include <stdio.h>

FAMCSimplificationModeDecoder::FAMCSimplificationModeDecoder(unsigned char* _buffer) 
	: buffer_(_buffer), nBytesDecTotal_(0){}

int FAMCSimplificationModeDecoder::decode(int _nValues, std::vector<int>& _mode){
	CABAC cabac;
	assert(_mode.size()==_nValues);
	int nBytesDec = 0;
	int streamSize = 0;

	cabac.arideco_start_decoding( cabac._dep, buffer_+nBytesDecTotal_, 0, &streamSize);
	cabac.biari_init_context(cabac._ctx, 61);
	for (int c=0; c < _nValues; ++c){ 
		_mode[c]=cabac.unary_exp_golomb_decode(cabac._dep, cabac._ctx, 2);
	}

	cabac.biari_decode_final(cabac._dep);
	cabac.arideco_done_decoding(cabac._dep);

	nBytesDec = streamSize-1;
	nBytesDecTotal_ += nBytesDec;

	return nBytesDec;
}