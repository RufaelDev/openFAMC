#include "FAMCCabacVx3Decoder.h"
#include "CABAC.h"
#include <stdio.h>

FAMCCabacVx3Decoder::FAMCCabacVx3Decoder(unsigned char* _buffer) 
	: buffer_(_buffer), nBytesDecTotal_(0){}

int FAMCCabacVx3Decoder::decodeSigMapAndSigns(int _nValues, std::vector<IPoint>& _res, int _sigMapInitProb) {
	CABAC cabac;
	assert(_res.size()==_nValues);
	int nBytesDec = 0;
	int streamSize = 0;
	
	cabac.arideco_start_decoding( cabac._dep, buffer_, 0, &streamSize);
	cabac.biari_init_context(cabac._ctx, _sigMapInitProb);
	for (int v=0; v < _nValues; ++v){ 
		for (int d = 0; d < 3; d++){	
			_res[v][d]=cabac.biari_decode_symbol(cabac._dep, cabac._ctx);
		}
	}
	for (int v = 0; v < _nValues; v++){
		for (int d = 0; d < 3; d++){
			if (_res[v][d] != 0) {
				_res[v][d] = (cabac.biari_decode_symbol_eq_prob(cabac._dep)==0) ? 1 : -1;
			}
		}
	}
	cabac.biari_decode_final(cabac._dep);
	cabac.arideco_done_decoding(cabac._dep);

	nBytesDec = streamSize-1;

	return nBytesDec;
}

int FAMCCabacVx3Decoder::decodeAbsValuesMinusOne(int _nValues, std::vector<IPoint>& _res, int _numberOfAbsValuesBitPlanes, int _absInitProb) {
	CABAC cabac;
	assert(_res.size()==_nValues);
	int nBytesDec = 0;
	int streamSize = 0;

	cabac.arideco_start_decoding( cabac._dep, buffer_, 0, &streamSize);
	cabac.biari_init_context(cabac._ctx, _absInitProb);
	for (int pb = _numberOfAbsValuesBitPlanes - 1; pb >= 0; pb--) {
		for (int v = 0; v < _nValues; v++){
			for (int d = 0; d < 3; d++){
				if (_res[v][d] != 0) {
					if (cabac.biari_decode_symbol(cabac._dep, cabac._ctx)){
						_res[v][d] += (_res[v][d] < 0) ? -(1<<pb) : (1<<pb);
					}
					
				}
			}
		}
	}

	cabac.biari_decode_final(cabac._dep);
	cabac.arideco_done_decoding(cabac._dep);

	nBytesDec = streamSize-1;

	return nBytesDec;
}

int FAMCCabacVx3Decoder::decodeBits(int _nValues, std::vector<bool>& _bits){
	CABAC cabac;
	_bits.resize(_nValues);
	int nBytesDec=0;
	int nBytesDecCabac=0;

	int bitsInitProb = buffer_[0]; // PDAM: 6-bits
	//printf("bitsInitProb=%d\n", bitsInitProb);
	++nBytesDec;
	++buffer_;

	int streamSize = 0;
	cabac.arideco_start_decoding( cabac._dep, buffer_, 0, &streamSize);
	cabac.biari_init_context(cabac._ctx, bitsInitProb);
	for (int v=0; v<_nValues; ++v){
		_bits[v]=(cabac.biari_decode_symbol(cabac._dep, cabac._ctx)!=0);
	}
	cabac.biari_decode_final(cabac._dep);
	cabac.arideco_done_decoding(cabac._dep);

	nBytesDecCabac = streamSize-1;
	nBytesDec += nBytesDecCabac;
	buffer_ += nBytesDecCabac;

	nBytesDecTotal_ += nBytesDec;

	return nBytesDec;
}

int FAMCCabacVx3Decoder::decodeVx3 (int _nValues, std::vector<IPoint>& _res){
	CABAC cabac;
	_res.resize(_nValues);
	int nBytesDec=0;
	int nBytesDecCabac=0;

	//decoding parameter, sig map, and signs
	int sigMapInitProb = buffer_[0]; // PDAM: 6-bits
	//printf("sigMapInitProb=%d\n", sigMapInitProb);
	++nBytesDec;
	++buffer_;
	
	nBytesDecCabac = decodeSigMapAndSigns(_nValues, _res, sigMapInitProb);
	nBytesDec += nBytesDecCabac;
	buffer_ += nBytesDecCabac;
	
	//decoding parameters and abs values minus one
	int absInitProb = buffer_[0]; // PDAM: 6-bits
	//printf("absInitProb=%d\n", absInitProb);
	++nBytesDec;
	++buffer_;

	int numberOfAbsValuesBitPlanes = buffer_[0]; // PDAM: 8-bits
	//printf("numberOfAbsValuesBitPlanes=%d\n", numberOfAbsValuesBitPlanes);
	++nBytesDec;
	++buffer_;

	nBytesDecCabac = decodeAbsValuesMinusOne(_nValues, _res, numberOfAbsValuesBitPlanes, absInitProb);
	nBytesDec += nBytesDecCabac;
	buffer_ += nBytesDecCabac;

	nBytesDecTotal_ += nBytesDec;
	
	return nBytesDec;
}