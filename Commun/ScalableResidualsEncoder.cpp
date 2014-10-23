/*!
 ************************************************************************
 *  \file
 *     ScalableResidualsEncoder.cpp
 *  \brief
 *     ScalableResidualsEncoder class.
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
#include "ScalableResidualsEncoder.h"

int ScalableResidualsEncoder::GenerateLayers(FILE * out){
	AOF( 1 == fwrite( &m_ucNbits,  sizeof(unsigned char), 1, out ) );
	int coordResidualErrorsSize = 1;
	if (m_ucNbits > 0) {
		AOF( 3 == fwrite( m_Res->m_fMax.data,     sizeof(float), 3, out ) );
		AOF( 3 == fwrite( m_Res->m_fMin.data,     sizeof(float), 3, out ) );
		AOF( 1 == fwrite( &m_ucNMaxLayers,  sizeof(unsigned char), 1, out ) );
		AOF( 1 == fwrite( &m_ucNLayers,  sizeof(unsigned char), 1, out ) );
		coordResidualErrorsSize += 14;
	
		int encodedFreqBand = 0;
		int nFreqBand = m_iLen / m_ucNMaxLayers;
		int layer = 1;
		while (encodedFreqBand < m_iLen) {
			int bandDim = nFreqBand;
			if (layer == m_ucNMaxLayers) bandDim = m_iLen - encodedFreqBand;

			ResidualEncoder * resEnc = new ResidualEncoder( NULL, bandDim, m_iNbr);

			for (int k = encodedFreqBand; k < encodedFreqBand+bandDim; k++) {
				for (int v = 0; v < m_iNbr; v++){
					for (int iCoord = 0; iCoord < 3; iCoord++){
						resEnc->m_paiRes[v * bandDim + k - encodedFreqBand].data[iCoord] = m_Res->m_paiRes[v * m_iLen + k].data[iCoord];
					}
				}
			}
			resEnc->m_ucNbits = m_ucNbits;
			m_LayerResEnc.push_back(resEnc);
			encodedFreqBand+=bandDim;
			layer++;
		}
	}
	else {
		m_ucNMaxLayers = m_ucNLayers = 0;
	}
	return coordResidualErrorsSize;
}
int ScalableResidualsEncoder::EncodeLayer(int layer, FILE * out){
	if ((layer >= m_ucNLayers) || (layer < 0)) return 0;
	CABAC cabac;
	int iMaxStreamSize = 3*(m_iLen-1)*m_iNbr*4; // a guess of the maximum cabac bit stream size
	unsigned char *paucBuf = new unsigned char[iMaxStreamSize];
	int iBytesToWrite = 0;
	cabac.arienco_start_encoding( cabac._eep, paucBuf, &iBytesToWrite );

	ResidualEncoder * cResEnc = m_LayerResEnc[layer];

	cResEnc->SetEncodingEnvironment(cabac._eep);
	cResEnc->encode();

	cabac.biari_encode_symbol_final( cabac._eep, 1 );
	cabac.arienco_done_encoding( cabac._eep );


	// write to file
	int coordResidualErrorsSize = 0;
	AOF( 1             == fwrite( &iBytesToWrite, sizeof(int), 1, out ) );
	AOF( iBytesToWrite == fwrite( paucBuf, sizeof(unsigned char), iBytesToWrite, out ) );

	coordResidualErrorsSize += iBytesToWrite + 5;
	delete[] paucBuf;
	return coordResidualErrorsSize;
}

