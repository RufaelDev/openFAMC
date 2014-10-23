/*!
 ************************************************************************
 *  \file
 *     FAMCDecoder.h
 *  \brief
 *     FAMCDecoder class.
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
#include "ScalableResidualsDecoder.h"
#include "CABAC.h"
#define FAMCAnimationSegmentStartCode  0x00001F0

class FAMCDecoder
{
	int numberOfDecomposedLayers_;
	// animated fields
	bool isCoordAnimated_;
	bool isNormalAnimated_;
	bool isColorAnimated_;
	bool isOtherAttributeAnimated_;
	bool useSkinningToPredictNormals_;
	bool isInterpolationNeeded_;
	unsigned int numberOfFrames_;
	std::vector<unsigned char> keys_;

	int transformType_;

	unsigned int numberOfVertices_;
	unsigned int numberOfNormals_;
	unsigned int numberOfColors_;
	unsigned int numberOfOtherAttributes_;
	std::vector<unsigned int> dimOfOtherAttributes_;

	int normalPred_;

	float ** globalMotion_;
	unsigned int * timeFrame_;
public:
	bool DecodeAU(FILE * in, Animation &anim, IndexedFaceSet & ifs);
	int  GetNumberOfDecomposedLayers() const { return numberOfDecomposedLayers_; }

	bool Decode(char * file);
	bool Decode(const char * file, Animation &myAnimDec);

	bool DecodeStream(char * file, int numberOfSpatialLayers);

	// CWI allow laoading of the animation
	bool DecodeStream(char * file, int numberOfSpatialLayers,  Animation &myAnimDec);

	bool DecodeHeader(Animation &anim, FILE * out, IndexedFaceSet & ifs);
	bool DecodeGolobalMotion(FILE * out, Animation &anim);
	bool DecodePartition(std::vector<int> * partition, int &K, FILE * in);
	int DecompressTransformsPoints(Animation &anim, MotionModel & motionModel, std::vector<int> * partition, FILE * in);
	bool DecompressWeights(Animation &anim, MotionModel & motionModel, std::vector<int> * partition, FILE * in);


	//DCT
	bool SetCoordResidualErrorsDCT(ScalableResidualsDecoder &SRDecCoord, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel);
	bool SetNormalResidualErrorsDCT(ScalableResidualsDecoder &SRDec, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel, int * pred);
	bool SetColorResidualErrorsDCT(ScalableResidualsDecoder &SRDec, Animation &anim);

	bool SetCoordResidualErrorsLift(ScalableResidualsDecoder &SRDec, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel);
	bool SetNormalResidualErrorsLift(ScalableResidualsDecoder &SRDec, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel, int * pred);
	bool SetColorResidualErrorsLift(ScalableResidualsDecoder &SRDec, Animation &anim);

	int DecompressNormalPredictors(FILE * in, int * predNormal);

	bool DecodeNormalResidualErrorsLift(Animation &anim, FILE * in);
	bool DecodeNormalResidualErrorsSkinningLift(Animation &anim, MotionModel & motionModel, FILE * in);
	bool DecodeColorResidualErrorsLift(Animation &anim, FILE * in);

	FAMCDecoder(void);
	~FAMCDecoder(void);
};
