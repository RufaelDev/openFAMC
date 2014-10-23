/*!
 ************************************************************************
 *  \file
 *     FAMCParams.h
 *  \brief
 *     FAMCParams class.
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
#include "string.h"
class FAMCParams
{
public:
	char file_[1024];
	int sizeAnimSeg_;
	int transformType_;
	float globalRMSE_;
	float ratioWeights_;
	int qAT_;
	int qCoord_;
	int qNormal_;
	int qColor_;
	int qOther_;
	int SNR_L_;
	int SNR_l_;
	int Normal_Pred_;
	int LD_auto_;
	int LD_L_;
	int LD_l_;
	int LD_P_;
	int LD_B_;
	int LD_Pred_;
	void Check();
	FAMCParams(void);
	FAMCParams(const FAMCParams &params);
	~FAMCParams(void);
};
