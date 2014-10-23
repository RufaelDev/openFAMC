/*!
 ************************************************************************
 *  \file
 *     FAMCParams.cpp
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
#include "FAMCParams.h"

FAMCParams::FAMCParams(void)
{
	sizeAnimSeg_ = 65;
	transformType_ = 0;
	globalRMSE_ = -1;
	ratioWeights_ = 0.05f;
	qAT_ = 16;
	qCoord_= 8;
	qNormal_ = 8;
	qColor_ = 8;
	qOther_ = 8;
	LD_L_ = 8;
	LD_l_ = 8;
	LD_P_ = -1;
	LD_B_ = 15;
	LD_Pred_ = 1;
	SNR_L_=1;
	SNR_l_=1;
	Normal_Pred_ = 1;
	strcpy(file_, "");
}
FAMCParams::FAMCParams(const FAMCParams &params){
	sizeAnimSeg_ = params.sizeAnimSeg_;
	transformType_ = params.transformType_;
	globalRMSE_ = params.globalRMSE_;
	ratioWeights_ = params.ratioWeights_;
	qAT_ = params.qAT_;
	qCoord_= params.qCoord_;
	qNormal_ = params.qNormal_;
	qColor_ = params.qColor_;
	qOther_ = params.qOther_;
	LD_L_ = params.LD_L_;
	LD_l_ = params.LD_l_;
	LD_P_ = params.LD_P_;
	LD_B_ = params.LD_B_;
	LD_Pred_ = params.LD_Pred_;
	SNR_L_=params.SNR_L_;
	SNR_l_=params.SNR_l_;
	Normal_Pred_ = params.Normal_Pred_;
	strcpy(file_, params.file_);
}
FAMCParams::~FAMCParams(void)
{
}

void FAMCParams::Check() {
	if (sizeAnimSeg_ < 0) sizeAnimSeg_ = 0;
	if (ratioWeights_ < 0.0) ratioWeights_ = 0.0;
	if (ratioWeights_ > 1.0) ratioWeights_ = 1.0;
	if (qAT_ < 0) qAT_ = 0;
	if (qAT_ > 16) qAT_ = 16;
	if (qCoord_ < 0) qCoord_ = 0;
	if (qCoord_ > 16) qCoord_ = 16;
	if (qNormal_ < 0) qNormal_ = 0;
	if (qNormal_ > 16) qNormal_ = 16;
	if (qColor_ < 0) qColor_ = 0;
	if (qColor_ > 16) qColor_ = 16;
	if (qOther_ < 0) qColor_ = 0;
	if (qOther_ > 16) qColor_ = 16;
	if (LD_L_ < 1) LD_L_ = 1;
	if (LD_L_ > 16) LD_L_ = 16;
	if (LD_l_ < 1) LD_l_ = 1;
	if (LD_l_ > LD_L_) LD_l_ = LD_L_;
	if (LD_P_ < -1) LD_P_ = -1;
	if (LD_P_ > sizeAnimSeg_-2) LD_P_ = sizeAnimSeg_-2;
	if (LD_B_ < 0) LD_B_ = 0;
	if (LD_B_ > sizeAnimSeg_-2) LD_B_ = sizeAnimSeg_-2; // need to be updated
	if (LD_Pred_ < 0) LD_Pred_ = 0;
	if (LD_Pred_ > 4) LD_Pred_ = 4;
}
