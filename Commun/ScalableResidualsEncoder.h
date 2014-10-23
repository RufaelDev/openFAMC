/*!
 ************************************************************************
 *  \file
 *     ScalableResidualsEncoder.h
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
#pragma once
#include "ResidualCoder.h"
#include "DCTEncoder.h"
#include "LiftCodec.h"

class ScalableResidualsEncoder
{
	ResidualCoder *					m_Res;
	std::vector<ResidualEncoder *>	m_LayerResEnc;
	int								m_iLen;
	int					   			m_iNbr;
	unsigned char					m_ucNbits;
	unsigned char					m_ucNMaxLayers;
	unsigned char					m_ucNLayers;

public:

	ScalableResidualsEncoder( int iLen, int iNbr, unsigned char ucNBits)
		: m_iLen      ( iLen                      )
		, m_iNbr      ( iNbr                      )
		, m_ucNbits	  ( ucNBits					  )
	{
		m_Res = new ResidualCoder(m_iLen, m_iNbr);
		m_Res->m_ucNbits = m_ucNbits;
		m_ucNLayers = 3;
		m_ucNMaxLayers = 3;
	}

	~ScalableResidualsEncoder(void){
		if (m_Res != NULL) delete m_Res;
		for (int i = 0; i < (int) m_LayerResEnc.size(); i++) {
			if (m_LayerResEnc[i] != NULL) delete m_LayerResEnc[i];
		}
	}

	inline unsigned char GetNLayers() { return m_ucNLayers;}
	inline void SetNLayers(unsigned char ucNLayers) { m_ucNLayers = ucNLayers;}
	inline unsigned char GetNMaxLayers() { return m_ucNMaxLayers;}
	inline void SetNMaxLayers(unsigned char ucNMaxLayers) { m_ucNMaxLayers = ucNMaxLayers;}
	
	inline void GetData(DCTEncoder &dctX, DCTEncoder &dctY, DCTEncoder &dctZ){
		dctX.Quantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 0, &(m_Res->m_fMax.data[0]), &(m_Res->m_fMin.data[0]) );
		dctY.Quantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 1, &(m_Res->m_fMax.data[1]), &(m_Res->m_fMin.data[1]) );
		dctZ.Quantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 2, &(m_Res->m_fMax.data[2]), &(m_Res->m_fMin.data[2]) );
	}
	inline void GetData(LiftCodec &liftX, LiftCodec &liftY, LiftCodec &liftZ){
		liftX.Quantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 0, &(m_Res->m_fMax.data[0]), &(m_Res->m_fMin.data[0]) );
		liftY.Quantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 1, &(m_Res->m_fMax.data[1]), &(m_Res->m_fMin.data[1]) );
		liftZ.Quantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 2, &(m_Res->m_fMax.data[2]), &(m_Res->m_fMin.data[2]) );
	}
	int GenerateLayers(FILE * out);
	int EncodeLayer(int layer, FILE * out);
};
