/*
 * =====================================================================================
 * 
 *        Filename:  LDSegmentDecoder.hh
 * 
 *     Description:  
 * 
 *         Version:  1.0
 *         Created:  07/25/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef LD_SEGMENT_DECODER_H
#define LD_SEGMENT_DECODER_H

#include <stdio.h>
#include <vector>
#include "LDReader.h"
#include "LDFrameTypeData.h"
#include "LayeredDecomposition.h"
#include "Animation.h"
#include "LDSegmentReconstructor.h"
#include "SimplificationDecomposer.h"

class LDSegmentDecoder{
	
public:
	LDSegmentDecoder(int _numberOfFrames, int _numberOfVertices, bool _isCoordAnimated, bool _isNormalAnimated, bool _isColorAnimated, bool _decodeLS);
	~LDSegmentDecoder();

	enum LDType {oldLD, newLDFromConn, newLDFromConnAndDecodedData};
	void decodeLDSegmentHeader(LDType& _ldType, unsigned int& _numberOfDecomposedLayers, LDReader& _reader);
	void initLDReconstructers(const LayeredDecomposition* _ld);
	void decodeLDFrame(LDReader& _reader);
	void decodeLDAllFrames(LDReader& _reader);

	void getAnimation(Animation& _anim, int _startFrameNumber);
	const std::vector< std::vector<SimplificationOperation> >& getSimplificationOperations() { return vvsop_; }

	static void createLayeredDecompositionFromConn(IndexedFaceSet& _ifs, unsigned int _numberOfDecomposedLayers, LayeredDecomposition& _ld);
	static void createLayeredDecompositionFromConnAndDecodedData(IndexedFaceSet& _ifs, const std::vector< std::vector<SimplificationOperation> >& _vvsop, LayeredDecomposition& _ld);

private:
	void  decodeLDFrame(const LDFrameTypeData& _frameTypeData, unsigned int _coordsPredictionMode, unsigned int _normalsPredictionMode, unsigned int _colorsPredictionMode, unsigned char* _buffer);

	int numberOfFrames_;
	int numberOfVertices_;
	bool isCoordAnimated_;
	bool isNormalAnimated_;
	bool isColorAnimated_;

	
	unsigned int numberOfEncodedLayers_;

	double coordsQuantizationStepLD_;
	double normalsQuantizationStepLD_;
	double colorsQuantizationStepLD_;
	

	std::vector<int> frameNumberDec2DisList_;
	int frameNumberPrevDis_;
	int frameNumberDec_;

	std::vector<int> numberOfVerticesInLayer_;

	std::vector<IPoint> res_;
	std::vector<bool> predBits_;

	LDSegmentReconstructor* coordsRecon_;
	LDSegmentReconstructor* normalsRecon_;
	LDSegmentReconstructor* colorsRecon_;

	std::vector< std::vector<SimplificationOperation> > vvsop_;

	bool decodeLS_;
};

#endif //LD_SEGMENT_DECODER_H