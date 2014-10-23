/*!
 ************************************************************************
 *  \file
 *     ScalableResidualsDecoder.cpp
 *  \brief
 *     ScalableResidualsDecoder class.
 *  \author
 *     Copyright (C) 2007  ARTEMIS Department INT/GET, Paris, France.
 *
 *     Khaled MAMOU               <khaled.mamou@int-evry.fr>
 *
 *	   Institut National des Telecommunications		tel.: +33 (0)1 60 76 40 94
 *     9, Rue Charles Fourier,						fax.: +33 (0)1 60 76 43 81
 *     91011 Evry Cedex France
 *
 ************************************************************************
 */
#include "ScalableResidualsDecoder.h"

void ScalableResidualsDecoder::GenerateLayers(FILE * in){
	AOF( 1 == fread( &m_ucNbits,  sizeof(unsigned char), 1, in ) );
	if (m_ucNbits > 0) {
		AOF( 3 == fread( m_Res->m_fMax.data,     sizeof(float), 3, in ) );
		AOF( 3 == fread( m_Res->m_fMin.data,     sizeof(float), 3, in ) );
		AOF( 1 == fread( &m_ucNMaxLayers,  sizeof(unsigned char), 1, in ) );
		AOF( 1 == fread( &m_ucNLayers,  sizeof(unsigned char), 1, in ) );
		
		int encodedFreqBand = 0;
		int nFreqBand = m_iLen / m_ucNMaxLayers;
		int layer = 1;
		while (encodedFreqBand < m_iLen) {
			int bandDim = nFreqBand;
			if (layer == m_ucNMaxLayers) bandDim = m_iLen - encodedFreqBand;

			ResidualDecoder * resDec = new ResidualDecoder( NULL, bandDim, m_iNbr);
			resDec->m_fMax.data[0] = m_Res->m_fMax.data[0];
			resDec->m_fMax.data[1] = m_Res->m_fMax.data[1];
			resDec->m_fMax.data[2] = m_Res->m_fMax.data[2];
			resDec->m_ucNbits = m_ucNbits;
			m_LayerResDec.push_back(resDec);
			encodedFreqBand+=bandDim;
			layer++;
		}
	}
	else {
		m_ucNMaxLayers = m_ucNLayers = 0;
	}
}

int ScalableResidualsDecoder::DecodeLayer(int layer, FILE * in){
	if ((layer >= m_ucNLayers) || (layer < 0)) return -1;
	int iStreamSize = 0;
	// read from file
	AOF( 1 == fread( &iStreamSize, sizeof(int), 1, in ) );
	unsigned char *paucBuf = new unsigned char[iStreamSize];		
	AOF( paucBuf );
	AOF( iStreamSize == fread( paucBuf, sizeof(unsigned char), iStreamSize, in ) );
	CABAC cabac;
	cabac.arideco_start_decoding( cabac._dep, paucBuf, 0, &iStreamSize );
	ResidualDecoder * cResDec = m_LayerResDec[layer];
	cResDec->SetDecodingEnvironment(cabac._dep);
	cResDec->decode();
	cabac.biari_decode_final(cabac._dep);
	cabac.arideco_done_decoding(cabac._dep);
	delete[] paucBuf;

	int nFreqBand = m_iLen / m_ucNMaxLayers;
	int decodedFreqBand = nFreqBand * layer;
	int bandDim = nFreqBand;
	if (layer == m_ucNMaxLayers-1) bandDim = m_iLen - decodedFreqBand;
	for (int k = decodedFreqBand; k < decodedFreqBand+bandDim; k++) {
		for (int v = 0; v < m_iNbr; v++){
			for (int iCoord = 0; iCoord < 3; iCoord++){
				m_Res->m_paiRes[v * m_iLen + k].data[iCoord] = cResDec->m_paiRes[v * bandDim + k - decodedFreqBand].data[iCoord];
			}
		}
	}
	return 0;
}