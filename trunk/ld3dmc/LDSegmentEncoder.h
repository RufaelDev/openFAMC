/*
 * =====================================================================================
 * 
 *        Filename:  LDSegmentEncoder.h
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  15/07/07 12:39:10 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst), stefanos@tnt.uni-hannover.de
 *         Company:  Institut für Informationsverarbeitung
 * 
 * =====================================================================================
 */

#ifndef  LD_SEGMENT_ENCODER_H
#define  LD_SEGMENT_ENCODER_H

#include "IndexedFaceSet.h"
#include "Animation.h"
#include "FAMCParams.h"
#include "MeshTypes.h"
#include "LayeredDecomposition.h"
#include "LDFrameTypeData.h"
#include "MeshTypes.h"
#include "LDSegmentDecomposer.h"
#include "LDWriter.h"
#include "SimplificationOperationsFactory.h"

class LDSegmentEncoder {
public:
	LDSegmentEncoder(Animation& _anim, int _startFrameNumber, int _endFrameNumber, bool _isCoordAnimated, bool _isNormalAnimated, bool _isColorAnimated, FAMCParams& _params, bool _encodeLS);
	~LDSegmentEncoder();

	enum LDType {oldLD, newLDFromConn, newLDFromConnAndEncodedData};
	void encodeOldLDSegmentHeader(LDWriter& _writer);
	void encodeNewLDSegmentHeader(LDWriter& _writer);
	void encodeNewLDSegmentHeader(LayeredDecomposition _ld, const std::vector< std::vector<SimplificationOperation> >& lsop, LDWriter& _writer);
	void initLDDecomposers(const LayeredDecomposition* _ld);
	void encodeLDFrame(const LDFrameTypeData& _frameTypeData, LDWriter& _writer);
	void encodeLDAllFrames(LDWriter& _writer);

	int getCoordsNBytesEnc() const {return coordsSize_;}
	int getNormalsNBytesEnc() const {return normalsSize_;}
	int getColorsNBytesEnc() const {return colorsSize_;}
	int getSegmentHeaderNBytesEnc() const {return segmentHeaderSize_;}
	int getFrameHeaderNBytesEnc() const {return frameHeaderSize_;}

private:
	int encodeOldLD( LDWriter& _writer);
	int encodeNewLD(LDWriter& _writer);
	int encodeNewLD(const std::vector< std::vector<SimplificationOperation> >& lsop, LDWriter& _writer);
	int encodeLDFrame(const LDFrameTypeData& _frameTypeData, unsigned int _coordsPredictionMode, unsigned int _normalsPredictionMode, unsigned int _colorsPredictionMode, unsigned char* _buffer) ;

private:
	Animation&		anim_;
	int				startFrameNumber_;
	int				endFrameNumber_;
	FAMCParams&		params_;

	int numberOfCoords_;
	int numberOfFrames_;

	bool isCoordAnimated_;
	bool isNormalAnimated_;
	bool isColorAnimated_;

	unsigned int numberOfEncodedLayers_;

	double coordsQuantizationStep_;
	double normalsQuantizationStep_;
	double colorsQuantizationStep_;

	int segmentHeaderSize_;
	int frameHeaderSize_;
	int coordsSize_;
	int normalsSize_;
	int colorsSize_;
	
	std::vector<IPoint> res_;
	std::vector<bool> predBits_;

	//std::vector< std::vector< std::vector<bool> > > coordsPredBits_; 
	//std::vector< std::vector< std::vector<bool> > > normalsPredBits_; 
	//std::vector< std::vector< std::vector<bool> > > colorsPredBits_; 

	//std::vector< std::vector< std::vector<IPoint> > > resCoords_;
	//std::vector< std::vector< std::vector<IPoint> > > resNormals_;
	//std::vector< std::vector< std::vector<IPoint> > > resColors_;

	LDSegmentDecomposer* coordsDecomp_;
	LDSegmentDecomposer* normalsDecomp_;
	LDSegmentDecomposer* colorsDecomp_;

	int frameNumberEnc_;
	int frameNumberPrevDis_;
	std::vector<int> frameNumberDis2EncList_;

	bool encodeLS_;

};



#endif // LD_SEGMENT_ENCODER_H
