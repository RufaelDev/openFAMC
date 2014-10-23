#include "FAMCVertexInfoEncoder.h"
#include "CABAC.h"
#include <stdio.h>

FAMCVertexInfoEncoder::FAMCVertexInfoEncoder(unsigned char* _buffer)
: buffer_(_buffer) {}

int FAMCVertexInfoEncoder::encode (int _nC, const std::vector<int>& _partition){
	CABAC cabac;

	int nV = _partition.size();
	int nBytesEnc = 0;
	int streamSize = 0;

	cabac.arienco_start_encoding( cabac._eep, buffer_, &streamSize);
	cabac.biari_init_context(cabac._ctx, 61);

	int numberOfBits = (int) (log((double) _nC-1)/log(2.0)+1.0);
	int occurence = 0;
	int currentSymbol = 0;
	int v=0;
	while (v < nV){
		currentSymbol = _partition[v];
		for (int pb = numberOfBits-1; pb >= 0; pb--){
			short bitOfBitPlane = currentSymbol & (1<<pb);
			cabac.biari_encode_symbol_eq_prob(cabac._eep, bitOfBitPlane);
		}
		occurence=0;
		while (v<nV && _partition[v] == currentSymbol){
			occurence++;
			v++;
		}
		assert(occurence>0);
		cabac.unary_exp_golomb_encode(cabac._eep, occurence-1, cabac._ctx, 2);
	}

	cabac.biari_encode_symbol_final(cabac._eep, 1);
	cabac.arienco_done_encoding(cabac._eep);

	nBytesEnc = streamSize;

	return nBytesEnc;
}