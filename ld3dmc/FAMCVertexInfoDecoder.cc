#include "FAMCVertexInfoDecoder.h"
#include "CABAC.h"
#include <stdio.h>

FAMCVertexInfoDecoder::FAMCVertexInfoDecoder(unsigned char* _buffer) : buffer_(_buffer){}

int FAMCVertexInfoDecoder::decode (int _nC, int _nV, std::vector<int>& _partition){
	CABAC cabac;

	assert(_partition.size()==_nV);
	int nBytesDec = 0;
	int streamSize = 0;

	cabac.arideco_start_decoding( cabac._dep, buffer_, 0, &streamSize);
	cabac.biari_init_context(cabac._ctx, 61);

	int numberOfBits = (int) (log((double) _nC-1)/log(2.0)+1.0);

	int occurenceMinusOne = 0;
	int currentSymbol = 0;
	int v=0;
	while (v < _nV){
		currentSymbol = 0;
		for (int pb = numberOfBits-1; pb >= 0; pb--){
			int bitOfBitPlane = cabac.biari_decode_symbol_eq_prob(cabac._dep);
			currentSymbol += (bitOfBitPlane * (1<<pb));
		}
		occurenceMinusOne = cabac.unary_exp_golomb_decode(cabac._dep, cabac._ctx, 2);
		for (int i=0; i < occurenceMinusOne+1; ++i){
			_partition[v]=currentSymbol;
			++v;
		}
	}

	cabac.biari_decode_final(cabac._dep);
	cabac.arideco_done_decoding(cabac._dep);

	nBytesDec = streamSize-1;

	return nBytesDec;
}