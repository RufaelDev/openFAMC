/*!
 ************************************************************************
 *  \file
 *     FAMCEncoder.h
 *  \brief
 *     FAMCEncoder class.
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
#include <stdio.h>
#include "Animation.h"
#include "IndexedFaceSet.h"
#include "HierarchicalPartitionner.h"
#include "CABAC.h"
#include "MotionModel.h"
#include "ScalableResidualsEncoder.h"
#include "FAMCParams.h"
#define FAMCAnimationSegmentStartCode  0x00001F0

class FAMCEncoder
{
	FAMCParams params_;
	// animated fields
	bool isCoordAnimated_;
	bool isNormalAnimated_;
	bool isColorAnimated_;
	bool isOtherAttributeAnimated_;
	bool useSkinningToPredictNormals_;
	bool isInterpolationNeeded_;
	unsigned int numberOfFrames_;
	std::vector<unsigned char> keys_;

	unsigned int numberOfVertices_;
	unsigned int numberOfNormals_;
	unsigned int numberOfColors_;
	unsigned int numberOfOtherAttributes_;
	std::vector<unsigned int> dimOfOtherAttributes_;

public:
	bool EncodeAU(Animation &anim, IndexedFaceSet &ifs, FILE * outFinal, FILE * stat);
	bool EncodeStream(char * sFile);
	bool EncodeStream( Animation &anim ,IndexedFaceSet &ifs,const char * outFile, const char * statFile);
	int EncodeHeader(Animation &anim, IndexedFaceSet & ifs, FILE * out);
	
	int EncodeGolobalMotion(Animation &anim, FILE * out, int nbits);
	int EncodePartition(std::vector<int> * partition, int K, FILE * out);

	void ComputeAffineTransforms(Animation * anim, std::vector<int> * partition, int K, MotionModel &motionModel);
	void ComputeWeights(Animation * anim, IndexedFaceSet &ifs, std::vector<int> * partition, int K, MotionModel &motionModel);
	int CompressAffineTransformsPoints(Animation * anim, std::vector<int> * partition, MotionModel & motionModel, int nbits, FILE * out);

	
	int CompressWeights(Animation &anim, MotionModel & motionModel, std::vector<int> * partition, float ratioWeights, unsigned char nbits, FILE * out);

	int GetCoordResidualErrorsDCT(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel);
	int GetNormalResidualErrorsDCT(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel, int * pred);
	int GetColorResidualErrorsDCT(ScalableResidualsEncoder &SREnc, Animation &anim);

	int CompressNormalPredictors(FILE * out, int * pred, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel);

	int GetCoordResidualErrorsLift(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel);
	int GetNormalResidualErrorsLift(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel, int * pred);
	int GetColorResidualErrorsLift(ScalableResidualsEncoder &SREnc, Animation &anim);

	//void QuantizeAnim(Animation& _anim, double _coordsQuantizationStep, double _normalsQuantizationStep, double _colorsQuantizationStep) const;

	FAMCEncoder(const FAMCParams & param);
	~FAMCEncoder(void);
};
