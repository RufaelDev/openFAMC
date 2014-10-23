#include "LDSegmentDecoder.h"
#include "FamcCabacVx3Decoder.h"
#include "LDFrameTypeData.h"
#include "LDSegmentReconstructor.h"
#include "SConnectivity.h"
#include "LayeredDecomposerAutomatic.h"
#include "LayeredDecomposerUserBased.h"
#include "FAMCVertexInfoDecoder.h"
#include "FAMCSimplificationModeDecoder.h"


LDSegmentDecoder::LDSegmentDecoder(int _numberOfFrames, int _numberOfVertices, bool _isCoordAnimated, bool _isNormalAnimated, bool _isColorAnimated, bool _decodeLS)
	: numberOfFrames_(_numberOfFrames)
	, numberOfVertices_(_numberOfVertices)
	, isCoordAnimated_(_isCoordAnimated)
	, isNormalAnimated_(_isNormalAnimated)
	, isColorAnimated_(_isColorAnimated)
    , decodeLS_(_decodeLS){
		
		printf("numberOfFrames: %d\n",numberOfFrames_);
		numberOfEncodedLayers_=0;
		coordsQuantizationStepLD_=-1;
		normalsQuantizationStepLD_=-1;
		colorsQuantizationStepLD_=-1;
		frameNumberDec2DisList_.resize(numberOfFrames_, -1);
		frameNumberPrevDis_=0;
		frameNumberDec_=0;

		coordsRecon_=0;
		normalsRecon_=0;
		colorsRecon_=0;
}

LDSegmentDecoder::~LDSegmentDecoder(){
	if (coordsRecon_) delete coordsRecon_;
	if (normalsRecon_) delete normalsRecon_;
	if (colorsRecon_) delete colorsRecon_;
};

void LDSegmentDecoder::decodeLDSegmentHeader(LDType& _ldType, unsigned int& _numberOfDecomposedLayers, LDReader& _reader){
	//newLayeredDecompositionNeeded:	bit (1)
	//layeredDecompositionIsEncoded:	bit (1)
	//not defined:						bit (6)
	unsigned char newLayeredDecompositionNeeded=0;
	unsigned char layeredDecompositionIsEncoded=0;
	unsigned char notDefined=0;
	
	unsigned char mask;
	_reader.readUInt8(mask);
	newLayeredDecompositionNeeded |= (mask & 1); 
	mask>>=1;
	layeredDecompositionIsEncoded |= (mask & 1); 
	mask>>=1;
	notDefined |= (mask & 63); 
	mask>>=6;

	//numberOfDecomposedLayers:	unsigned int (32)
	unsigned int numberOfDecomposedLayers=0;

	std::vector< std::vector<int> > vvmode;
	if (newLayeredDecompositionNeeded!=0){	
		_reader.readUInt32(numberOfDecomposedLayers);

		if (layeredDecompositionIsEncoded){
			//read partition
			unsigned int compressedPartitionBufferSize = 0;
			_reader.readUInt32(compressedPartitionBufferSize);
			unsigned char* viBuffer = new unsigned char[compressedPartitionBufferSize];
			_reader.readBuffer(viBuffer, compressedPartitionBufferSize);
			FAMCVertexInfoDecoder viDec(viBuffer);
			std::vector<int> partition(numberOfVertices_, 0);
			viDec.decode(numberOfDecomposedLayers, numberOfVertices_, partition);

			//derive layer sizes
			std::vector<int> numberOfVerticesInLayer(numberOfDecomposedLayers, 0);
			for (int v=0; v<numberOfVertices_; ++v){
				numberOfVerticesInLayer[partition[v]]++;
			}

			//read modes
			unsigned int simplificationModeBufferSize = 0;
			_reader.readUInt32(simplificationModeBufferSize);
			unsigned char* smBuffer = new unsigned char[simplificationModeBufferSize];
			_reader.readBuffer(smBuffer, simplificationModeBufferSize);
			FAMCSimplificationModeDecoder smDec(smBuffer);
			vvmode.resize(numberOfDecomposedLayers-1);
			for (int layer=numberOfDecomposedLayers-1; layer>=1; --layer){
				vvmode[numberOfDecomposedLayers-1-layer].resize(numberOfVerticesInLayer[layer]);
				smDec.decode(numberOfVerticesInLayer[layer], vvmode[numberOfDecomposedLayers-1-layer]);
			}

			//derive simplification operations from modes and partition
			vvsop_.resize(numberOfDecomposedLayers-1);
			for (int v=0; v<numberOfVertices_; ++v){
				SimplificationOperation sop;
				sop.vertexIndex = v;
				if (partition[v]>0) vvsop_[numberOfDecomposedLayers-1-partition[v]].push_back(sop);
			}
			for (int layer=numberOfDecomposedLayers-1; layer>=1; --layer){
				for (int c=0; c<numberOfVerticesInLayer[layer]; ++c){
					vvsop_[numberOfDecomposedLayers-1-layer][c].mode = vvmode[numberOfDecomposedLayers-1-layer][c];
				}
			}
		}
	}

	//set some variables needed for derivation of a layered decomposition
	if (newLayeredDecompositionNeeded==0) _ldType = oldLD;
	if (newLayeredDecompositionNeeded!=0 && layeredDecompositionIsEncoded==0) _ldType = newLDFromConn;
	if (newLayeredDecompositionNeeded!=0 && layeredDecompositionIsEncoded!=0) _ldType = newLDFromConnAndDecodedData;
	_numberOfDecomposedLayers = numberOfDecomposedLayers;

	printf("newLayeredDecompositionNeeded=%u\n",newLayeredDecompositionNeeded);
	printf("numberOfDecomposedLayers=%u\n",numberOfDecomposedLayers);
	printf("layeredDecompositionIsEncoded=%u\n",layeredDecompositionIsEncoded);

	_reader.readUInt32(numberOfEncodedLayers_); //unsigned int (32)
	printf("numberOfEncodedLayers=%u\n",numberOfEncodedLayers_);

	if (isCoordAnimated_) _reader.readFloat64(coordsQuantizationStepLD_);	// float (64)
	if (isNormalAnimated_) _reader.readFloat64(normalsQuantizationStepLD_);	// float (64)
	if (isColorAnimated_) _reader.readFloat64(colorsQuantizationStepLD_);	// float (64)

	printf("coordsQuantizationStepLD=%f\n",coordsQuantizationStepLD_);
	printf("normalsQuantizationStepLD=%f\n",normalsQuantizationStepLD_);
	printf("colorsQuantizationStepLD=%f\n",colorsQuantizationStepLD_);
}

void LDSegmentDecoder::initLDReconstructers(const LayeredDecomposition* _ld) {
	numberOfVerticesInLayer_.resize(_ld->layerContext.size());
	for (int l=0; l<numberOfVerticesInLayer_.size(); ++l){
		numberOfVerticesInLayer_[l]=_ld->layerContext[l].vertexContext.size();
	}
	if (isCoordAnimated_) coordsRecon_ = new LDSegmentReconstructor(*_ld, coordsQuantizationStepLD_, numberOfFrames_, numberOfVertices_);
	if (isNormalAnimated_) normalsRecon_ = new LDSegmentReconstructor(*_ld, normalsQuantizationStepLD_, numberOfFrames_, numberOfVertices_);
	if (isColorAnimated_) colorsRecon_ = new LDSegmentReconstructor(*_ld, colorsQuantizationStepLD_, numberOfFrames_, numberOfVertices_);
}

void LDSegmentDecoder::decodeLDFrame(LDReader& _reader) {
	//decode header
	signed char frameNumberOffsetDis = 0;
	unsigned char frameType = 0;
	unsigned char refFrameNumberOffsetDec0 = 0;
	unsigned char refFrameNumberOffsetDec1 = 0;
	
	_reader.readInt8(frameNumberOffsetDis); // signed int (8)
	unsigned short mask = 0;
	_reader.readUInt16(mask);
	frameType = (mask & 3);					//unsigned int (2)
	mask>>=2;
	refFrameNumberOffsetDec0 = (mask & 127);	//unsigned int (7)
	mask>>=7;
	refFrameNumberOffsetDec1 = (mask & 127);	//unsigned int (7)
	mask>>=7;
	
	LDFrameTypeData frameTypeData;
	frameTypeData.frameNumberDis = frameNumberPrevDis_ + frameNumberOffsetDis;
	frameTypeData.frameType = (LDFrameType) frameType;
	if (frameType==P || frameType==B)	frameTypeData.refFrameNumberDis0 = frameNumberDec2DisList_[frameNumberDec_-refFrameNumberOffsetDec0];
	if (frameType==B)					frameTypeData.refFrameNumberDis1 = frameNumberDec2DisList_[frameNumberDec_-refFrameNumberOffsetDec1];

	frameNumberDec2DisList_[frameNumberDec_]=frameTypeData.frameNumberDis;
	frameNumberPrevDis_=frameTypeData.frameNumberDis;
	frameNumberDec_++;

	//decode prediction modes
	unsigned char coordsPredictionMode = 0;
	unsigned char normalsPredictionMode = 0;
	unsigned char colorsPredictionMode  = 0;
	unsigned char otherAttribPredictionMode  = 0;
	mask = 0;
	_reader.readUInt16(mask);	

	coordsPredictionMode = mask & 15;		//unsigned int (4)
	mask>>=4;
	normalsPredictionMode = mask & 15;		//unsigned int (4)
	mask>>=4;
	colorsPredictionMode = mask & 15;		//unsigned int (4)
	mask>>=4;
	otherAttribPredictionMode = mask & 15;	//unsigned int (4)

	//decode residuals
	unsigned int bufSize=0;
	_reader.readUInt32(bufSize);
	unsigned char* buffer = new unsigned char[bufSize];
	_reader.readBuffer(buffer, bufSize);
	decodeLDFrame(frameTypeData, coordsPredictionMode, normalsPredictionMode, colorsPredictionMode, buffer);
	delete [] buffer;	
}

void LDSegmentDecoder::decodeLDAllFrames(LDReader& _reader){
	for (int frameNumberEnc=0; frameNumberEnc<numberOfFrames_; ++frameNumberEnc){
		decodeLDFrame(_reader);
	}
}

void LDSegmentDecoder::getAnimation(Animation& _anim, int _startFrameNumber){
	if (isCoordAnimated_) coordsRecon_->getAnimation(_anim, LDSegmentReconstructor::coordData, 1);
	if (isNormalAnimated_) normalsRecon_->getAnimation(_anim, LDSegmentReconstructor::normalData, 1);
	if (isColorAnimated_) colorsRecon_->getAnimation(_anim, LDSegmentReconstructor::colorData, 1);
}

void LDSegmentDecoder::decodeLDFrame(const LDFrameTypeData& _frameTypeData, unsigned int _coordsPredictionMode, unsigned int _normalsPredictionMode, unsigned int _colorsPredictionMode, unsigned char* _buffer){
	FAMCCabacVx3Decoder dec(_buffer);
	for (int layerNumber=0; layerNumber<numberOfEncodedLayers_; ++layerNumber){
		if (isCoordAnimated_){
			dec.decodeVx3(numberOfVerticesInLayer_[layerNumber], res_);
			if (_coordsPredictionMode==3 || _coordsPredictionMode==4){
				dec.decodeBits(numberOfVerticesInLayer_[layerNumber], predBits_);
			}
			if (decodeLS_){
				coordsRecon_->reconstructLDFrameLayerLS(layerNumber, _frameTypeData, _coordsPredictionMode, res_, predBits_);
			}
			else{
				coordsRecon_->reconstructLDFrameLayer(layerNumber, _frameTypeData, _coordsPredictionMode, res_, predBits_);
			}
		}
		if (isNormalAnimated_){
			dec.decodeVx3(numberOfVerticesInLayer_[layerNumber], res_);
			if (_normalsPredictionMode==3 || _normalsPredictionMode==4){
				dec.decodeBits(numberOfVerticesInLayer_[layerNumber], predBits_);
			}
			if (decodeLS_){
				normalsRecon_->reconstructLDFrameLayerLS(layerNumber, _frameTypeData, _normalsPredictionMode, res_, predBits_);
			}
			else{
				normalsRecon_->reconstructLDFrameLayer(layerNumber, _frameTypeData, _normalsPredictionMode, res_, predBits_);
			}
		}
		if (isColorAnimated_){
			dec.decodeVx3(numberOfVerticesInLayer_[layerNumber], res_);
			if (_colorsPredictionMode==3 || _colorsPredictionMode==4){
				dec.decodeBits(numberOfVerticesInLayer_[layerNumber], predBits_);
			}
			if (decodeLS_){
				colorsRecon_->reconstructLDFrameLayerLS(layerNumber, _frameTypeData, _colorsPredictionMode, res_, predBits_);
			}
			else{
				colorsRecon_->reconstructLDFrameLayer(layerNumber, _frameTypeData, _colorsPredictionMode, res_, predBits_);
			}
		}
	}
}

void LDSegmentDecoder::createLayeredDecompositionFromConn(IndexedFaceSet& _ifs, unsigned int _numberOfDecomposedLayers, LayeredDecomposition& _ld) {
	int nFaces = _ifs.GetNCoordIndex();
	std::vector< std::vector<int> > sifs(nFaces, std::vector<int>(3,0));
	int coordIndex[3] = {-1, -1, -1};
	for (int t=0; t<nFaces; ++t){
		_ifs.GetCoordIndex(t, coordIndex);
		sifs[t][0]=coordIndex[0];
		sifs[t][1]=coordIndex[1];
		sifs[t][2]=coordIndex[2];
	}

	printf("<-- calculate layered decomposition automatically, start -->\n");
	SConnectivity sconn(sifs);	
	LayeredDecomposerAutomatic lda(_numberOfDecomposedLayers, sconn);
	lda.produce(_ld);
	printf("<-- calculate layered decomposition automatically, end -->\n");
}
void LDSegmentDecoder::createLayeredDecompositionFromConnAndDecodedData(IndexedFaceSet& _ifs, const std::vector< std::vector<SimplificationOperation> >& _lsop, LayeredDecomposition& _ld){
	int nFaces = _ifs.GetNCoordIndex();
	std::vector< std::vector<int> > sifs(nFaces, std::vector<int>(3,0));
	int coordIndex[3] = {-1, -1, -1};
	for (int t=0; t<nFaces; ++t){
		_ifs.GetCoordIndex(t, coordIndex);
		sifs[t][0]=coordIndex[0];
		sifs[t][1]=coordIndex[1];
		sifs[t][2]=coordIndex[2];
	}

	printf("<-- calculate layered decomposition from encoded/decoded data, start -->\n");
	SConnectivity sconn(sifs);	
	LayeredDecomposerUserBased ldub(_lsop, sconn);
	ldub.produce(_ld);
	printf("<-- calculate layered decomposition from encoded/decoded data, end -->\n");

}