#include "LDSegmentEncoder.h"
#include "LDWriter.h"
#include "LayeredDecomposition.h"
#include "LayeredDecomposerAutomatic.h"
#include "LDSegmentDecomposer.h"
#include "LDFrameOrderFactory.h"
#include "FAMCCabacVx3Encoder.h"
#include "LayeredDecomposition.h"
#include "FAMCVertexInfoEncoder.h"
#include "FAMCSimplificationModeEncoder.h"

LDSegmentEncoder::LDSegmentEncoder(Animation& _anim, int _startFrameNumber, int _endFrameNumber, bool _isCoordAnimated, bool _isNormalAnimated, bool _isColorAnimated, FAMCParams& _params, bool _encodeLS) 
: anim_(_anim)
, startFrameNumber_(_startFrameNumber)
, endFrameNumber_(_endFrameNumber)
, isCoordAnimated_(_isCoordAnimated)
, isNormalAnimated_(_isNormalAnimated)
, isColorAnimated_(_isColorAnimated)
, params_(_params)
, encodeLS_(_encodeLS){
	numberOfFrames_=_endFrameNumber-_startFrameNumber+1;
	numberOfCoords_ = anim_.GetNCoord();
	printf("numberOfFrames: %d\n",numberOfFrames_);
	printf("numberOfCoords: %d\n",numberOfCoords_);
	
	numberOfEncodedLayers_=params_.LD_l_;

	coordsQuantizationStep_=0;
	normalsQuantizationStep_=0;
	colorsQuantizationStep_=0;

	segmentHeaderSize_=0;
	frameHeaderSize_=0;
	coordsSize_=0;
	normalsSize_=0;
	colorsSize_=0;

	res_.reserve(numberOfCoords_);
	predBits_.reserve(numberOfCoords_);

	coordsDecomp_=0;
	normalsDecomp_=0;
	colorsDecomp_=0;
	

	frameNumberEnc_=0;
	frameNumberPrevDis_=0;
	frameNumberDis2EncList_.resize(numberOfFrames_, -1);
}

LDSegmentEncoder::~LDSegmentEncoder(){
	if (coordsDecomp_) delete coordsDecomp_;
	if (normalsDecomp_) delete normalsDecomp_;
	if (colorsDecomp_) delete colorsDecomp_;
}

int LDSegmentEncoder::encodeOldLD(LDWriter& _writer) {
	//newLayeredDecompositionNeeded:	bit (1)
	//layeredDecompositionIsEncoded:	bit (1)
	//not defined:						bit (6)
	unsigned char newLayeredDecompositionNeeded = 0;
	unsigned char layeredDecompositionIsEncoded = 0;
	unsigned char notDefined = 0;
	unsigned char mask = 0;
	mask |= ((newLayeredDecompositionNeeded && 1) <<0);
	mask |= ((layeredDecompositionIsEncoded && 1) <<1);
	mask |= ((notDefined && 63) <<2);

	int oldLDSize = 0;
	oldLDSize += _writer.writeUInt8(mask); 
	return oldLDSize;
}

int LDSegmentEncoder::encodeNewLD(LDWriter& _writer) {
	//newLayeredDecompositionNeeded:	bit (1)
	//layeredDecompositionIsEncoded:	bit (1)
	//not defined:						bit (6)
	unsigned char newLayeredDecompositionNeeded = 1;
	unsigned char layeredDecompositionIsEncoded = 0;
	unsigned char notDefined = 0;
	unsigned char mask = 0;
	mask |= ((newLayeredDecompositionNeeded && 1) <<0);
	mask |= ((layeredDecompositionIsEncoded && 1) <<1);
	mask |= ((notDefined && 63) <<2);

	int oldLDSize = 0;
	oldLDSize += _writer.writeUInt8(mask);
	
	//numberOfDecomposedLayers: unsigned int (32)
	oldLDSize += _writer.writeUInt32(params_.LD_L_);

	return oldLDSize;
}

int LDSegmentEncoder::encodeNewLD(const std::vector< std::vector<SimplificationOperation> >& _lsop, LDWriter& _writer) {
	//newLayeredDecompositionNeeded:	bit (1)
	//layeredDecompositionIsEncoded:	bit (1)
	//not defined:						bit (6)
	unsigned char newLayeredDecompositionNeeded = 1;
	unsigned char layeredDecompositionIsEncoded = 1;
	unsigned char notDefined = 0;
	unsigned char mask = 0;
	mask |= ((newLayeredDecompositionNeeded && 1) <<0);
	mask |= ((layeredDecompositionIsEncoded && 1) <<1);
	mask |= ((notDefined && 63) <<2);

	int oldLDSize = 0;
	oldLDSize += _writer.writeUInt8(mask);
	
	//numberOfDecomposedLayers: unsigned int (32)
	oldLDSize += _writer.writeUInt32(params_.LD_L_);

	//create and write partition
	std::vector<int> partition(numberOfCoords_, 0);
	for (int l = 0; l<_lsop.size(); ++l){
		for (int c=0; c<_lsop[l].size(); ++c){
			partition[_lsop[l][c].vertexIndex]=params_.LD_L_-1-l;
		}
	}
	
	const int VI_BUFFER_SIZE = numberOfCoords_* params_.LD_L_ * 5;
	unsigned char* viBuffer = new unsigned char[VI_BUFFER_SIZE];
	FAMCVertexInfoEncoder viEnc(viBuffer);
	unsigned int compressedPartitionBufferSize = viEnc.encode(params_.LD_L_, partition);
	oldLDSize += _writer.writeUInt32(compressedPartitionBufferSize);
	oldLDSize += _writer.writeBuffer(viBuffer, compressedPartitionBufferSize);
	delete [] viBuffer;

	//create and write modes
	std::vector< std::vector<int> > vvmode(_lsop.size());
	for (int l = 0; l<_lsop.size(); ++l){
		vvmode[l].resize(_lsop[l].size());
		for (int c=0; c<_lsop[l].size(); ++c){
			vvmode[l][c]=_lsop[l][c].mode;
		}
	}
	const int SM_BUFFER_SIZE = numberOfCoords_* params_.LD_L_ * 5;
	unsigned char* smBuffer = new unsigned char[SM_BUFFER_SIZE];
	FAMCSimplificationModeEncoder smEnc(smBuffer); 
	unsigned int simplificationModeBufferSize = 0;
	for (int layer = 0; layer < vvmode.size(); ++layer){
		simplificationModeBufferSize += smEnc.encode(vvmode[layer]);
	}
	oldLDSize += _writer.writeUInt32(simplificationModeBufferSize);
	oldLDSize += _writer.writeBuffer(smBuffer, simplificationModeBufferSize);
	delete [] smBuffer;

	return oldLDSize;
}

void LDSegmentEncoder::encodeOldLDSegmentHeader(LDWriter& _writer) {
	segmentHeaderSize_ += encodeOldLD(_writer);
	
	//numberOfEncodedLayers: unsigned int (32)
	segmentHeaderSize_ += _writer.writeUInt32(numberOfEncodedLayers_);								//numberOfEncodedLayers: unsigned int (32)

	if (isCoordAnimated_){
		coordsQuantizationStep_= anim_.ComputeFirstFrameBBDiagonal()/(1<<params_.qCoord_);
		segmentHeaderSize_ += _writer.writeFloat64(coordsQuantizationStep_);	//coordsQuantizationStepLD:float (64)
		printf("coordsQuantizationStep = %f\n", coordsQuantizationStep_);
	}
	if (isNormalAnimated_){
		normalsQuantizationStep_=1.0/(1<<params_.qNormal_);	
		segmentHeaderSize_ += _writer.writeFloat64(normalsQuantizationStep_);	//normalsQuantizationStepLD:float (64)
		printf("normalsQuantizationStep = %f\n", normalsQuantizationStep_);
	}
	if (isColorAnimated_){ 
		colorsQuantizationStep_=1.0/(1<<params_.qColor_);
		segmentHeaderSize_ += _writer.writeFloat64(colorsQuantizationStep_);	//colorsQuantizationStepLD:float (64)
		printf("colorsQuantizationStep = %f\n", colorsQuantizationStep_);
	}
}

void LDSegmentEncoder::encodeNewLDSegmentHeader(LDWriter& _writer) {
	segmentHeaderSize_ += encodeNewLD(_writer);
	
	//numberOfEncodedLayers: unsigned int (32)
	segmentHeaderSize_ += _writer.writeUInt32(numberOfEncodedLayers_);								//numberOfEncodedLayers: unsigned int (32)

	if (isCoordAnimated_){
		coordsQuantizationStep_= anim_.ComputeFirstFrameBBDiagonal()/(1<<params_.qCoord_);
		segmentHeaderSize_ += _writer.writeFloat64(coordsQuantizationStep_);	//coordsQuantizationStepLD:float (64)
		printf("coordsQuantizationStep = %f\n", coordsQuantizationStep_);
	}
	if (isNormalAnimated_){
		normalsQuantizationStep_=1.0/(1<<params_.qNormal_);	
		segmentHeaderSize_ += _writer.writeFloat64(normalsQuantizationStep_);	//normalsQuantizationStepLD:float (64)
		printf("normalsQuantizationStep = %f\n", normalsQuantizationStep_);
	}
	if (isColorAnimated_){ 
		colorsQuantizationStep_=1.0/(1<<params_.qColor_);
		segmentHeaderSize_ += _writer.writeFloat64(colorsQuantizationStep_);	//colorsQuantizationStepLD:float (64)
		printf("colorsQuantizationStep = %f\n", colorsQuantizationStep_);
	}
}
void LDSegmentEncoder::encodeNewLDSegmentHeader(LayeredDecomposition _ld, const std::vector< std::vector<SimplificationOperation> >& _lsop, LDWriter& _writer) {
	segmentHeaderSize_ += encodeNewLD(_lsop, _writer);
	
	//numberOfEncodedLayers: unsigned int (32)
	segmentHeaderSize_ += _writer.writeUInt32(numberOfEncodedLayers_);								//numberOfEncodedLayers: unsigned int (32)

	if (isCoordAnimated_){
		coordsQuantizationStep_= anim_.ComputeFirstFrameBBDiagonal()/(1<<params_.qCoord_);
		segmentHeaderSize_ += _writer.writeFloat64(coordsQuantizationStep_);	//coordsQuantizationStepLD:float (64)
		printf("coordsQuantizationStep = %f\n", coordsQuantizationStep_);
	}
	if (isNormalAnimated_){
		normalsQuantizationStep_=1.0/(1<<params_.qNormal_);	
		segmentHeaderSize_ += _writer.writeFloat64(normalsQuantizationStep_);	//normalsQuantizationStepLD:float (64)
		printf("normalsQuantizationStep = %f\n", normalsQuantizationStep_);
	}
	if (isColorAnimated_){ 
		colorsQuantizationStep_=1.0/(1<<params_.qColor_);
		segmentHeaderSize_ += _writer.writeFloat64(colorsQuantizationStep_);	//colorsQuantizationStepLD:float (64)
		printf("colorsQuantizationStep = %f\n", colorsQuantizationStep_);
	}
}

void LDSegmentEncoder::initLDDecomposers(const LayeredDecomposition* _ld){
	coordsDecomp_ = new LDSegmentDecomposer (*_ld, coordsQuantizationStep_);
	normalsDecomp_ = new LDSegmentDecomposer(*_ld, normalsQuantizationStep_);
	colorsDecomp_ = new LDSegmentDecomposer(*_ld, colorsQuantizationStep_);
	if (isCoordAnimated_){
		coordsDecomp_->setDynamicMesh(anim_, LDSegmentDecomposer::coordData, 1, anim_.GetNKeyCoord()-1);
	}
	if (isNormalAnimated_){
		assert(anim_.GetNCoord()==anim_.GetNNormal());
		normalsDecomp_->setDynamicMesh(anim_, LDSegmentDecomposer::normalData, 1, anim_.GetNKeyNormal()-1);
	}
	if (isColorAnimated_){
		assert(anim_.GetNCoord()==anim_.GetNNormal()==anim_.GetNColor());
		colorsDecomp_->setDynamicMesh(anim_, LDSegmentDecomposer::colorData, 1, anim_.GetNKeyColor()-1);
	}
}

void LDSegmentEncoder::encodeLDFrame(const LDFrameTypeData& _frameTypeData, LDWriter& _writer) {
	//encode header
	signed char frameNumberOffsetDis = 0;
	unsigned char frameType = _frameTypeData.frameType;
	unsigned char refFrameNummberOffsetEnc0 = 0;
	unsigned char refFrameNummberOffsetEnc1 = 0;
		
	frameNumberOffsetDis = _frameTypeData.frameNumberDis - frameNumberPrevDis_;
	if (frameType==1 || frameType==2) //P-frame or B-frame
		refFrameNummberOffsetEnc0 = frameNumberEnc_-frameNumberDis2EncList_[_frameTypeData.refFrameNumberDis0];
	if (frameType==2) //B-frame
		refFrameNummberOffsetEnc1 = frameNumberEnc_-frameNumberDis2EncList_[_frameTypeData.refFrameNumberDis1];

	frameHeaderSize_ += _writer.writeInt8(frameNumberOffsetDis);	//signed int (8)
	unsigned short mask = 0;
	mask |= (frameType & 3);							//unsigned int (2)
	mask |= ((refFrameNummberOffsetEnc0 & 127)<<2);	//unsigned int (7)
	mask |= ((refFrameNummberOffsetEnc1 & 127)<<9);	//unsigned int (7)
	frameHeaderSize_ += _writer.writeUInt16(mask);	

	frameNumberDis2EncList_[_frameTypeData.frameNumberDis]=frameNumberEnc_;
	frameNumberPrevDis_=_frameTypeData.frameNumberDis;
	++frameNumberEnc_;

	//encode prediction modes
	unsigned char coordsPredictionMode  = params_.LD_Pred_;
	unsigned char normalsPredictionMode = 1;
	unsigned char colorsPredictionMode  = 0;
	unsigned char otherAttribPredictionMode  = 0;
	mask = 0;
	mask |= (coordsPredictionMode & 15)<<0;		//unsigned int (4)
	mask |= (normalsPredictionMode & 15)<<4;		//unsigned int (4)
	mask |= (colorsPredictionMode & 15)<<8;		//unsigned int (4)
	mask |= (otherAttribPredictionMode & 15)<<12;	//unsigned int (4)
	frameHeaderSize_ += _writer.writeUInt16(mask);	

	//encode residuals
	const int BPVF_MAX = 128;
	const int BUF_SIZE  = numberOfCoords_ * BPVF_MAX;
	unsigned char* buffer = new unsigned char[BUF_SIZE];
	unsigned int resSize = encodeLDFrame(_frameTypeData, coordsPredictionMode, normalsPredictionMode, colorsPredictionMode, buffer);
	frameHeaderSize_ += _writer.writeUInt32(resSize);
	_writer.writeBuffer(buffer, resSize);
	delete [] buffer;
}

void LDSegmentEncoder::encodeLDAllFrames(LDWriter& _writer){
	LDFrameOrderFactory frameOrderFactory(numberOfFrames_, params_.LD_P_, params_.LD_B_);
	LDFrameTypeData frameTypeData;

	for (int frameNumberEnc=0; frameNumberEnc<numberOfFrames_; ++frameNumberEnc){
		frameTypeData = frameOrderFactory.getFrameTypeData(frameNumberEnc);
		encodeLDFrame(frameTypeData, _writer);
	}
}

int LDSegmentEncoder::encodeLDFrame(const LDFrameTypeData& _frameTypeData, unsigned int _coordsPredictionMode, unsigned int _normalsPredictionMode, unsigned int _colorsPredictionMode, unsigned char* _buffer) {
	FAMCCabacVx3Encoder cabacVx3Enc(_buffer);
	int nBytesTmp=0;
	
	for (int layerNumber=0; layerNumber<numberOfEncodedLayers_; ++layerNumber){
		if (isCoordAnimated_){
			if (encodeLS_){
				coordsDecomp_->decomposeLDFrameLayerLS(layerNumber, _frameTypeData, _coordsPredictionMode, res_, predBits_);
			}
			else{
				coordsDecomp_->decomposeLDFrameLayer(layerNumber, _frameTypeData, _coordsPredictionMode, res_, predBits_);
			}
			cabacVx3Enc.encodeVx3(res_);
			if (_coordsPredictionMode==3 || _coordsPredictionMode==4){
				cabacVx3Enc.encodeBits(predBits_);
			}
			coordsSize_ += cabacVx3Enc.getNBytesEncoded()-nBytesTmp;
			nBytesTmp = cabacVx3Enc.getNBytesEncoded();
		}
		if (isNormalAnimated_){
			if (encodeLS_){
				normalsDecomp_->decomposeLDFrameLayerLS(layerNumber, _frameTypeData, _normalsPredictionMode, res_, predBits_);
			}
			else{
				normalsDecomp_->decomposeLDFrameLayer(layerNumber, _frameTypeData, _normalsPredictionMode, res_, predBits_);
			}
			cabacVx3Enc.encodeVx3(res_);
			if (_normalsPredictionMode==3 || _normalsPredictionMode==4){
				cabacVx3Enc.encodeBits(predBits_);
			}
			normalsSize_ += cabacVx3Enc.getNBytesEncoded()-nBytesTmp;
			nBytesTmp = cabacVx3Enc.getNBytesEncoded();
		}
		if (isColorAnimated_){
			if (encodeLS_){
				colorsDecomp_->decomposeLDFrameLayerLS(layerNumber, _frameTypeData, _colorsPredictionMode, res_, predBits_);
			}
			else{
				colorsDecomp_->decomposeLDFrameLayer(layerNumber, _frameTypeData, _colorsPredictionMode, res_, predBits_);
			}
			cabacVx3Enc.encodeVx3(res_);
			if (_colorsPredictionMode==3 || _colorsPredictionMode==4){
				cabacVx3Enc.encodeBits(predBits_);
			}
			colorsSize_ += cabacVx3Enc.getNBytesEncoded()-nBytesTmp;
			nBytesTmp = cabacVx3Enc.getNBytesEncoded();
		}
	}

	return cabacVx3Enc.getNBytesEncoded();
}


