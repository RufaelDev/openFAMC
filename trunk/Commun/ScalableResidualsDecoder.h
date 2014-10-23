/*!
 ************************************************************************
 *  \file
 *     ScalableResidualsDecoder.h
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
#pragma once
#include "ResidualCoder.h"
#include "DCTEncoder.h"
#include "LiftCodec.h"

class ScalableResidualsDecoder
{
	ResidualCoder *					m_Res;
	std::vector<ResidualDecoder *>	m_LayerResDec;
	int								m_iLen;
	int					   			m_iNbr;
	unsigned char					m_ucNbits;
	unsigned char					m_ucNMaxLayers;
	unsigned char					m_ucNLayers;
public:
	ScalableResidualsDecoder( int iLen, int iNbr)
		: m_iLen      ( iLen                      )
		, m_iNbr      ( iNbr                      )	
	{
		m_Res = new ResidualCoder(m_iLen, m_iNbr);
		m_Res->m_ucNbits = m_ucNbits;
		m_ucNMaxLayers = 3;
		m_ucNLayers = 3;
		m_ucNbits = 8;
	}

	~ScalableResidualsDecoder(void){
		if (m_Res != NULL) delete m_Res;
		for (int i = 0; i < (int) m_LayerResDec.size(); i++) {
			if (m_LayerResDec[i] != NULL) delete m_LayerResDec[i];
		}
	}

	inline unsigned char GetNLayers() { return m_ucNLayers;}
	inline void SetNLayers(unsigned char ucNLayers) { m_ucNLayers = ucNLayers;}
	inline unsigned char GetNMaxLayers() { return m_ucNMaxLayers;}
	inline void SetNMaxLayers(unsigned char ucNMaxLayers) { m_ucNMaxLayers = ucNMaxLayers;}

	inline unsigned char GetNBits() { return m_ucNbits;}
	inline void GetNBits(unsigned char ucNbits) { m_ucNbits = ucNbits;}


	inline void SetData(DCTEncoder &dctX, DCTEncoder &dctY, DCTEncoder &dctZ){
		dctX.Unquantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 0, &(m_Res->m_fMax.data[0]), &(m_Res->m_fMin.data[0]) );
		dctY.Unquantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 1, &(m_Res->m_fMax.data[1]), &(m_Res->m_fMin.data[1]) );
		dctZ.Unquantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 2, &(m_Res->m_fMax.data[2]), &(m_Res->m_fMin.data[2]) );
	}
	inline void SetData(LiftCodec &liftX, LiftCodec &liftY, LiftCodec &liftZ){
		liftX.Unquantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 0, &(m_Res->m_fMax.data[0]), &(m_Res->m_fMin.data[0]) );
		liftY.Unquantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 1, &(m_Res->m_fMax.data[1]), &(m_Res->m_fMin.data[1]) );
		liftZ.Unquantize(m_ucNbits, m_Res->m_paiRes, m_Res->getStride(), 2, &(m_Res->m_fMax.data[2]), &(m_Res->m_fMin.data[2]) );
	}

	void GenerateLayers(FILE * in);
	int DecodeLayer(int layer, FILE * in);
};
