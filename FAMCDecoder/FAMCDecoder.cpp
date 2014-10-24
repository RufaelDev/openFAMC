/*!
 ************************************************************************
 *  \file
 *     FAMCDecoder.cpp
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
#include "FAMCDecoder.h"
#include "Console.h"
#include "DCTEncoder.h"
#include "LiftCodec.h"
#include "MathTools.h"
#include "ScalableResidualsDecoder.h"
#include "MathTools.h"
#include "LDSegmentDecoder.h"
#include "LiftScheme.h"
#include "DCTEncoder.h"
#include "nrutil_nr.h"
#include <math.h>
#define TOL 0.00001 //0.001	
extern CConsole logger;

FAMCDecoder::FAMCDecoder(void){
	numberOfDecomposedLayers_ = -1;


	isCoordAnimated_ = false;
	isNormalAnimated_ = false;
	isColorAnimated_ = false;
	isOtherAttributeAnimated_ = false;

	isInterpolationNeeded_ = false;

	transformType_ = 0;

	numberOfFrames_ = 0;
	numberOfVertices_ = 0;
	numberOfNormals_ = 0;
	numberOfColors_ = 0;
	numberOfOtherAttributes_ = 0;
	globalMotion_ = NULL;
	timeFrame_ = NULL;
	normalPred_ = 1;
}

FAMCDecoder::~FAMCDecoder(void)
{
	if (timeFrame_) delete [] timeFrame_;
	if (globalMotion_) {
		for (int i = 0; i < (int) numberOfFrames_; i++) {
			delete [] globalMotion_[i];
		}
		delete [] globalMotion_;
	}
}
bool FAMCDecoder::DecodeGolobalMotion(FILE * in, Animation &anim) {

	int F = numberOfFrames_-1;
	int V = numberOfVertices_;
	DCTEncoder dctGlobalMotionX(F, 1);
	DCTEncoder dctGlobalMotionY(F, 1);
	DCTEncoder dctGlobalMotionZ(F, 1);

	ScalableResidualsDecoder SRDec(F, 1);
	SRDec.GenerateLayers(in);
	SRDec.DecodeLayer(0, in);	
	SRDec.SetData(dctGlobalMotionX, dctGlobalMotionY, dctGlobalMotionZ);

	dctGlobalMotionX.IDCT();
	dctGlobalMotionY.IDCT();
	dctGlobalMotionZ.IDCT();

	// we compute the barycenter of the first frame
	float xg0 = 0.0;
	float yg0 = 0.0;
	float zg0 = 0.0;
	float coord[3]={0.0f, 0.0f, 0.0f};
	for (int v = 0; v < V; v++) {
		anim.GetCoord(0, v, coord);
		xg0 += coord[0];
		yg0 += coord[1];
		zg0 += coord[2];
	}
	xg0/=V;
	yg0/=V;
	zg0/=V;
	
	// we center all the frames
	globalMotion_ = new float * [F+1];
	for(int f = 0; f < F+1; f++){
		globalMotion_[f] = new float[3];
		if ( f==0) {
			globalMotion_[f][0] = xg0;
			globalMotion_[f][1] = yg0;
			globalMotion_[f][2] = zg0;
		}
		else {
			globalMotion_[f][0] = dctGlobalMotionX.function_[0][f-1];
			globalMotion_[f][1] = dctGlobalMotionY.function_[0][f-1];
			globalMotion_[f][2] = dctGlobalMotionZ.function_[0][f-1];
		}
	}
	// we center the first frame
	for (int v = 0; v < V; v++) {
		anim.GetCoord(0, v, coord);
		coord[0] -= globalMotion_[0][0];
		coord[1] -= globalMotion_[0][1];
		coord[2] -= globalMotion_[0][2];
		anim.SetCoord(0, v, coord);
	}
	return true;
}

bool FAMCDecoder::DecodePartition(std::vector<int> * partition, int &K, FILE * in){
	// read from file
	int stream_sizeE = 0;

	// write to file
	fread(&K, sizeof(int), 1, in);
	fread(&stream_sizeE, sizeof(int), 1, in);
	// we allocate memory for the encoded stream
	unsigned char * bufferE = new unsigned char[stream_sizeE];		
	fread(bufferE, sizeof(unsigned char), stream_sizeE, in);
	int nbits = 1;
	if ( K > 1) nbits = (int) MathTools::nBinaryBits(K-1);

	//-------------------------------
	// Decoding with CABAC
	CABAC cabac;


	// we initialize the decoding process	
	cabac.arideco_start_decoding(cabac._dep, bufferE, 0, &stream_sizeE);

	// decoding the significance map
	// we initialize the context
	cabac.biari_init_context(cabac._ctx, 61);

	// encoding predictors
	int occurence = 0;
	int currentSymbol = 0;
	partition->clear();
	while( partition->size() < numberOfVertices_ ) {
		currentSymbol = 0;
		for (int pb = nbits - 1; pb >= 0; pb--) {
			int b = cabac.biari_decode_symbol_eq_prob(cabac._dep);
			currentSymbol += (b * (1<<pb));
		}
		occurence = cabac.unary_exp_golomb_decode(cabac._dep, cabac._ctx, 2);
		for (int i =0; i < occurence+1; i++) {
			partition->push_back(currentSymbol);
		}
	}
	cabac.biari_decode_final(cabac._dep);

	// we end the decoding process
	cabac.arideco_done_decoding(cabac._dep);
	// free memory
	delete [] bufferE;
	return true;
}
bool FAMCDecoder::DecodeHeader(Animation &anim, FILE * in, IndexedFaceSet & ifs) {
	anim.FreeMem();

	unsigned int startCode = 0;
	fread(&startCode, sizeof(unsigned int), 1, in);


	if ( startCode != FAMCAnimationSegmentStartCode) return false;
	unsigned int animationSegmentSize = 0;
	fread(&animationSegmentSize, sizeof(unsigned int), 1, in);
	unsigned char staticMeshDecodingType = 0;
	fread(&staticMeshDecodingType, sizeof(unsigned char), 1, in);



	unsigned char mask = 0;
	unsigned char maskPred = 0;

	fread(&mask, sizeof(unsigned char), 1, in);
	fread(&maskPred, sizeof(unsigned char), 1, in);
	fread(&numberOfFrames_, sizeof(unsigned int), 1, in);


	normalPred_ = maskPred;
	isCoordAnimated_ = false;
	isNormalAnimated_ = false;
	isColorAnimated_ = false;
	isOtherAttributeAnimated_ = false;
	isInterpolationNeeded_ = false;


	if ( (mask&1)) {
		isCoordAnimated_ = true;
	}
	if ( (mask&2) > 0){
		isNormalAnimated_ = true;
	}
	if ( (mask&4)) {
		isColorAnimated_ = true;
	}
	if ( (mask&8)) {
		isOtherAttributeAnimated_ = true;
	}
	if ( (mask&128)) {
		isInterpolationNeeded_ = true;
	}

	transformType_ = ((mask&112)>>4);
	numberOfVertices_ = ifs.GetNCoord();
	numberOfNormals_ = ifs.GetNNormal();
	numberOfColors_ = ifs.GetNColor();
	numberOfOtherAttributes_ = 0;
	logger.write_2_log("transformType = %i\n", transformType_);
	logger.write_2_log("numberOfFrames = %i\n", numberOfFrames_);
	logger.write_2_log("numberOfVertices = %i\n", numberOfVertices_);
	logger.write_2_log("numberOfNormals = %i\n", numberOfNormals_);
	logger.write_2_log("numberOfColors = %i\n", numberOfColors_);
	logger.write_2_log("numberOfOtherAttributes = %i\n", numberOfOtherAttributes_);
	

	if (isCoordAnimated_) {
		anim.SetNKeyCoord(numberOfFrames_);
		anim.SetNCoord(numberOfVertices_);
	}
	if (isNormalAnimated_) {
		anim.SetNKeyNormal(numberOfFrames_);
		anim.SetNNormal(numberOfNormals_);
	}
	if (isColorAnimated_) {
		anim.SetNKeyColor(numberOfFrames_);
		anim.SetNColor(numberOfColors_);
	}
	if (isOtherAttributeAnimated_) {
		anim.SetNKeyAttributes(numberOfFrames_);
		anim.SetNAttributes(numberOfOtherAttributes_);
	}

	anim.AllocateMem();

	timeFrame_ = new unsigned int [numberOfFrames_];
	fread(timeFrame_, sizeof(unsigned int), numberOfFrames_-1, in);
	return true;
}


int FAMCDecoder::DecompressTransformsPoints(Animation &anim,MotionModel & motionModel, std::vector<int> * partition, FILE * in) {
	// we decode the affine transforms

	int K = motionModel.GetNClusters();
	int F = numberOfFrames_-1;

	int nbr = 4 * K;
	int len = F;

	DCTEncoder dctEncoderX(len, nbr);
	DCTEncoder dctEncoderY(len, nbr);
	DCTEncoder dctEncoderZ(len, nbr);

	ScalableResidualsDecoder SRDec(len, nbr);
	SRDec.GenerateLayers(in);
	SRDec.DecodeLayer(0, in);	
	SRDec.SetData(dctEncoderX, dctEncoderY, dctEncoderZ);

	dctEncoderX.IDCT();
	dctEncoderY.IDCT();
	dctEncoderZ.IDCT();

	// computing the clusters BBs
	// we fill the clusters by pushing their corresponding vertices
	std::vector<int> * clusters  = new std::vector<int> [K];
	for (int i = 0; i < (int) (*partition).size(); i++) {
		clusters[(*partition)[i]].push_back(i);
	}


	float ** coord_org = new float * [K*4];
	for (int v = 0; v < K*4; v++) {
		coord_org[v] = new float[3];
	}

	float coord[3] = {0.0,0.0,0.0};
	for (int k = 0; k < K; k++) {
		// Init to zero
		for(int c = 0; c < 3; c++){
			coord_org[k*4][c] = 0.0f;
		}

		// Compute barycenter
		for (int v = 0; v < (int) clusters[k].size(); v++) {
			int vertex = clusters[k][v];
			anim.GetCoord(0, vertex, coord);
			for(int c = 0; c < 3; c++){
				coord_org[k*4][c] += coord[c];
			}
		}
		for(int c = 0; c < 3; c++){
			coord_org[k*4][c] /= clusters[k].size();
		}

		// we compute the BB
		float dx = 0.0;
		float dx_max = 0.0;
		float dy = 0.0;
		float dy_max = 0.0;
		float dz = 0.0;
		float dz_max = 0.0;

		for (int v = 0; v < (int) clusters[k].size(); v++) {
			int vertex = clusters[k][v];
			anim.GetCoord(0, vertex, coord);
			dx= fabs(coord_org[k*4][0] - coord[0]);
			dy= fabs(coord_org[k*4][1] - coord[1]);
			dz= fabs(coord_org[k*4][2] - coord[2]);
			if ( dx > dx_max) {
				dx_max = dx;
			}
			if ( dy > dy_max) {
				dy_max = dy;
			}
			if ( dz > dz_max) {
				dz_max = dz;
			}
		}

		for(int p = 0; p < 4; p++){
			for(int c = 0; c < 3; c++){
				coord_org[k*4+p][c] = coord_org[k*4][c];
			}
		}			
		dx_max = MAX(dx_max, TOL);
		dy_max = MAX(dy_max, TOL);
		dz_max = MAX(dz_max, TOL);
		coord_org[k*4+1][0] += dx_max;
		coord_org[k*4+2][1] += dy_max;
		coord_org[k*4+3][2] += dz_max;
	}

	float px0 = 0.0;
	float py0 = 0.0;
	float pz0 = 0.0;
	float px1 = 0.0;
	float py1 = 0.0;
	float pz1 = 0.0;
	float px2 = 0.0;
	float py2 = 0.0;
	float pz2 = 0.0;
	float px3 = 0.0;
	float py3 = 0.0;
	float pz3 = 0.0;
	AffineTransform at;
	for (int k = 0; k < K; k++) {
		float dx = coord_org[k*4+1][0] - coord_org[k*4][0];
		float dy = coord_org[k*4+2][1] - coord_org[k*4][1];
		float dz = coord_org[k*4+3][2] - coord_org[k*4][2];

		for (int frame = 0; frame < F; frame++){

			px0 = dctEncoderX.function_[k*4+0][frame];
			px1 = dctEncoderX.function_[k*4+1][frame];
			px2 = dctEncoderX.function_[k*4+2][frame];
			px3 = dctEncoderX.function_[k*4+3][frame];

			py0 = dctEncoderY.function_[k*4+0][frame];
			py1 = dctEncoderY.function_[k*4+1][frame];
			py2 = dctEncoderY.function_[k*4+2][frame];
			py3 = dctEncoderY.function_[k*4+3][frame];

			pz0 = dctEncoderZ.function_[k*4+0][frame];
			pz1 = dctEncoderZ.function_[k*4+1][frame];
			pz2 = dctEncoderZ.function_[k*4+2][frame];
			pz3 = dctEncoderZ.function_[k*4+3][frame];
		
			at.a_ = (px1-px0)/dx;
			at.b_ = (px2-px0)/dy;
			at.c_ = (px3-px0)/dz;
			at.tx_ = px0 - at.a_ * coord_org[k*4][0] - at.b_ * coord_org[k*4][1] - at.c_ * coord_org[k*4][2];

			at.d_ = (py1-py0)/dx;
			at.e_ = (py2-py0)/dy;
			at.f_ = (py3-py0)/dz;
			at.ty_ = py0 - at.d_ * coord_org[k*4][0] - at.e_ * coord_org[k*4][1] - at.f_ * coord_org[k*4][2];

			at.g_ = (pz1-pz0)/dx;
			at.h_ = (pz2-pz0)/dy;
			at.i_ = (pz3-pz0)/dz;
			at.tz_ = pz0 - at.g_ * coord_org[k*4][0] - at.h_ * coord_org[k*4][1] - at.i_ * coord_org[k*4][2];

			motionModel.SetAffineTransform(at, frame+1, k);
		}
	}

	for (int v = 0; v < K*4; v++) {
		delete [] coord_org[v];
	}	 
	delete [] coord_org;

	return 0;
}

bool FAMCDecoder::DecompressWeights(Animation &anim, MotionModel & motionModel, std::vector<int> * partition, FILE * in){
	//  Allocate memory
	int * filter = new int [numberOfVertices_];
	for (int v = 0; v < (int) numberOfVertices_; v++) {
		filter[v] = 0;
	}
	// read from file
	unsigned char nbits;
	float minw = 0.0;
	float maxw = 0.0;
	int stream_sizeE = 0;
	fread(&nbits, sizeof(unsigned char), 1, in);
	fread(&minw, sizeof(float), 1, in);
	fread(&maxw, sizeof(float), 1, in);
	fread(&stream_sizeE, sizeof(int), 1, in);
	// we allocate memory for the encoded stream
	unsigned char * bufferE = new unsigned char[stream_sizeE];		
	fread(bufferE, sizeof(unsigned char), stream_sizeE, in);


//  We compute clusters adjStream
	IntMultiVect * adj = anim.GetAdj();
	int K = motionModel.GetNClusters();



	//-------------------------------
	// Decoding with CABAC
	//-------------------------------

	CABAC cabac;


	// we initialize the decoding process	
	cabac.arideco_start_decoding(cabac._dep, bufferE, 0, &stream_sizeE);

	// decoding retained vertices
	// we initialize the context
	cabac.biari_init_context(cabac._ctx, 61);
	for (int v = 0; v < (int) numberOfVertices_; v++) {
		filter[v] = cabac.biari_decode_symbol(cabac._dep, cabac._ctx);
	}
	int nbr = 0;
	for (int v = 0; v < (int) numberOfVertices_; v++) {
		if (filter[v] == 0) {
			nbr++;
		}
	}

	logger.write_2_log("Eliminated vertices %3.2f%%\n", (float) nbr / numberOfVertices_ * 100.0);

	// decoding clusters adjacency
	int nbitsC = (int) MathTools::nBinaryBits(K-1);
		// we initialize the context
	cabac.biari_init_context(cabac._ctx, 61);

	for(int k = 0; k < K; k++) {
		int nbrNeighbours = cabac.unary_exp_golomb_decode(cabac._dep, cabac._ctx, 2);
		(*adj)[k].push_back(k);
		for(int n = 0; n < nbrNeighbours; n++) {
			int clusterName = 0;
			for (int bp = nbitsC-1; bp>= 0; bp--) {
				int res= cabac.biari_decode_symbol_eq_prob(cabac._dep);
				clusterName += res * (1<<bp);
			}
			(*adj)[k].push_back(clusterName);
		}
	}


	cabac.biari_init_context(cabac._ctx, 2);
	float delta = 1.0f;
	if ((maxw - minw) != 0.0) delta = (float) (1<<(nbits-1))/(maxw - minw);

	float w = 0.0;
	
	std::vector<unsigned int> qWeights;
	for (int bp = nbits-1; bp>= 0; bp--) {
		int pos = 0;
		for(int vertex = 0; vertex < (int) numberOfVertices_; vertex++) {
			int vertexCluster = (*partition)[vertex];
			if ( filter[vertex] == 1) {
				for(int cluster =0; cluster < (int) (*anim.GetAdj())[vertexCluster].size(); cluster++) {
					int res= cabac.biari_decode_symbol(cabac._dep, cabac._ctx);
					if (bp == nbits-1) {
						qWeights.push_back(res * (1<<bp));
					} 
					else {
						qWeights[pos] += res * (1<<bp);
					}
					if (bp == 0) {
						w = (float) qWeights[pos] / delta + minw;
						motionModel.SetWeight(w, vertex, (*anim.GetAdj())[vertexCluster][cluster]);			
					}
					pos++;
				}
			}
		}
	}

	cabac.biari_decode_final(cabac._dep);

	// we end the decoding process
	cabac.arideco_done_decoding(cabac._dep);

	// free memory
	delete [] bufferE;
	delete [] filter;

	return true;
}
bool FAMCDecoder::SetCoordResidualErrorsDCT(ScalableResidualsDecoder &SRDec, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel) {
	DCTEncoder dctX(numberOfFrames_-1, numberOfVertices_);
	DCTEncoder dctY(numberOfFrames_-1, numberOfVertices_);
	DCTEncoder dctZ(numberOfFrames_-1, numberOfVertices_);

	SRDec.SetData(dctX, dctY, dctZ);


	dctX.IDCT();
	dctY.IDCT();
	dctZ.IDCT();
	
	
	float coord0[3]={0.0f, 0.0f, 0.0f};
	float coordp[3]={0.0f, 0.0f, 0.0f};
	float coord1[3]={0.0f, 0.0f, 0.0f};
	
	for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
		int v = ifs.GetVerticesTraversal()[pos];
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetCoord(0, v, coord0);
			motionModel->Get(coord0, v, f, coordp);
			coord1[0] = dctX.function_[pos][f-1] + coordp[0];
			coord1[1] = dctY.function_[pos][f-1] + coordp[1];
			coord1[2] = dctZ.function_[pos][f-1] + coordp[2];

			anim.SetCoord(f, v, coord1);
		}
	}

	// global motion
	for (int v = 0; v < (int) numberOfVertices_; v++) {
		for(int f = 0; f < (int) numberOfFrames_; f++){
			anim.GetCoord(f, v, coordp);
			coordp[0] += globalMotion_[f][0];
			coordp[1] += globalMotion_[f][1];
			coordp[2] += globalMotion_[f][2];
			anim.SetCoord(f, v, coordp);
		}
	}
	return true;
}

int FAMCDecoder::DecompressNormalPredictors(FILE * in, int * predNormal) {
	if (normalPred_ == 3) {
		int stream_sizeE = 0;
		fread(&stream_sizeE, sizeof(int), 1, in);
		unsigned char * bufferE = new unsigned char[stream_sizeE];	
		fread(bufferE, sizeof(unsigned char), stream_sizeE, in);
		//---------------------------------------------------------
		//					Decoding with CABAC
		//---------------------------------------------------------
		CABAC cabac;
		// we initialize the decoding process	
		cabac.arideco_start_decoding(cabac._dep, bufferE, 0, &stream_sizeE);
		cabac.biari_init_context(cabac._ctx, 61);

		
		for (int n = 0; n < (int) numberOfNormals_; n++) {
			predNormal[n] = 2 - cabac.unary_exp_golomb_decode(cabac._dep, cabac._ctx, 2);
		}

		// we end the decoding process
		cabac.biari_decode_final(cabac._dep);
		cabac.arideco_done_decoding(cabac._dep);

		// free memory
		delete [] bufferE;
	}
	else {
		for (int n = 0; n < (int) numberOfNormals_; n++) {
			predNormal[n] = normalPred_;
		}
	}
	return 0;
}
bool FAMCDecoder::SetNormalResidualErrorsDCT(ScalableResidualsDecoder &SRDec, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel, int * pred) {
	DCTEncoder dctNX(numberOfFrames_-1, numberOfNormals_);
	DCTEncoder dctNY(numberOfFrames_-1, numberOfNormals_);
	DCTEncoder dctNZ(numberOfFrames_-1, numberOfNormals_);

	SRDec.SetData(dctNX, dctNY, dctNZ);


	dctNX.IDCT();
	dctNY.IDCT();
	dctNZ.IDCT();
	
	
	float normal0[3]={0.0f, 0.0f, 0.0f};
	float normalp[3]={0.0f, 0.0f, 0.0f};
	float normal1[3]={0.0f, 0.0f, 0.0f};

	for (int n = 0; n < (int) numberOfNormals_; n++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetNormal(0, n, normal0);
			MathTools::vectorUnitary(normal0);
			if (pred[n] == 0) {
				for (int k = 0; k < 3 ; k++) normalp[k] = normal0[k];
			}
			if (pred[n] == 1) {
				motionModel->GetN(normal0, n, f, normalp);
			}
			if (pred[n] == 2) {
				motionModel->GetNO(normal0, n, f, normalp);
			}

			MathTools::vectorUnitary(normalp);

			normal1[0] = dctNX.function_[n][f-1] + normalp[0];
			normal1[1] = dctNY.function_[n][f-1] + normalp[1];
			normal1[2] = dctNZ.function_[n][f-1] + normalp[2];

			MathTools::vectorUnitary(normal1);
			anim.SetNormal(f, n, normal1);
		}
	}
	return true;
}

bool FAMCDecoder::SetColorResidualErrorsDCT(ScalableResidualsDecoder &SRDec, Animation &anim) {
	DCTEncoder dctR(numberOfFrames_-1, numberOfColors_);
	DCTEncoder dctG(numberOfFrames_-1, numberOfColors_);
	DCTEncoder dctB(numberOfFrames_-1, numberOfColors_);

	SRDec.SetData(dctR, dctG, dctB);


	dctR.IDCT();
	dctG.IDCT();
	dctB.IDCT();
	
	float color0[3]={0.0f, 0.0f, 0.0f};
	float color1[3]={0.0f, 0.0f, 0.0f};
	for (int c = 0; c < (int) numberOfColors_; c++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetColor(f, c, color1);
			anim.GetColor(0, c, color0);
			color1[0] = dctR.function_[c][f-1] + color0[0];
			color1[1] = dctG.function_[c][f-1] + color0[1];
			color1[2] = dctB.function_[c][f-1] + color0[2];
		}
	}
	return true;
}
bool FAMCDecoder::SetCoordResidualErrorsLift(ScalableResidualsDecoder &SRDec, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel) {
	LiftCodec liftX(numberOfFrames_-1, numberOfVertices_);
	LiftCodec liftY(numberOfFrames_-1, numberOfVertices_);
	LiftCodec liftZ(numberOfFrames_-1, numberOfVertices_);

	SRDec.SetData(liftX, liftY, liftZ);
	float coord0[3]={0.0f, 0.0f, 0.0f};
	float coordp[3]={0.0f, 0.0f, 0.0f};
	float coord1[3]={0.0f, 0.0f, 0.0f};
	
	for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
		int v = ifs.GetVerticesTraversal()[pos];
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetCoord(0, v, coord0);
			motionModel->Get(coord0, v, f, coordp);
			coord1[0] = (float)(liftX.function_[pos][f-1] + coordp[0]);
			coord1[1] = (float)(liftY.function_[pos][f-1] + coordp[1]);
			coord1[2] = (float)(liftZ.function_[pos][f-1] + coordp[2]);

			anim.SetCoord(f, v, coord1);
		}
	}

	// global motion
	for (int v = 0; v < (int) numberOfVertices_; v++) {
		for(int f = 0; f < (int) numberOfFrames_; f++){
			anim.GetCoord(f, v, coordp);
			coordp[0] += globalMotion_[f][0];
			coordp[1] += globalMotion_[f][1];
			coordp[2] += globalMotion_[f][2];
			anim.SetCoord(f, v, coordp);
		}
	}
	return true;
}
bool FAMCDecoder::SetNormalResidualErrorsLift(ScalableResidualsDecoder &SRDec, Animation &anim, IndexedFaceSet & ifs, MotionModel * motionModel, int * pred) {
	LiftCodec liftNX(numberOfFrames_-1, numberOfNormals_);
	LiftCodec liftNY(numberOfFrames_-1, numberOfNormals_);
	LiftCodec liftNZ(numberOfFrames_-1, numberOfNormals_);

	SRDec.SetData(liftNX, liftNY, liftNZ);
	
	float normal0[3]={0.0f, 0.0f, 0.0f};
	float normalp[3]={0.0f, 0.0f, 0.0f};
	float normal1[3]={0.0f, 0.0f, 0.0f};

	for (int n = 0; n < (int) numberOfNormals_; n++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetNormal(0, n, normal0);
			MathTools::vectorUnitary(normal0);
			if (pred[n] == 0) {
				for (int k = 0; k < 3 ; k++) normalp[k] = normal0[k];
			}
			if (pred[n] == 1) {
				motionModel->GetN(normal0, n, f, normalp);
			}
			if (pred[n] == 2) {
				motionModel->GetNO(normal0, n, f, normalp);
			}

			MathTools::vectorUnitary(normalp);

			normal1[0] = (float) (liftNX.function_[n][f-1] + normalp[0]);
			normal1[1] = (float) (liftNY.function_[n][f-1] + normalp[1]);
			normal1[2] = (float) (liftNZ.function_[n][f-1] + normalp[2]);

			MathTools::vectorUnitary(normal1);
			anim.SetNormal(f, n, normal1);
		}
	}
	return true;
}
bool FAMCDecoder::SetColorResidualErrorsLift(ScalableResidualsDecoder &SRDec, Animation &anim) {
	LiftCodec liftR(numberOfFrames_-1, numberOfColors_);
	LiftCodec liftG(numberOfFrames_-1, numberOfColors_);
	LiftCodec liftB(numberOfFrames_-1, numberOfColors_);

	SRDec.SetData(liftR, liftG, liftB);
	float color0[3]={0.0f, 0.0f, 0.0f};
	float color1[3]={0.0f, 0.0f, 0.0f};
	for (int c = 0; c < (int) numberOfColors_; c++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetColor(f, c, color1);
			anim.GetColor(0, c, color0);
			color1[0] = (float) (liftR.function_[c][f-1] + color0[0]);
			color1[1] = (float) (liftG.function_[c][f-1] + color0[1]);
			color1[2] = (float) (liftB.function_[c][f-1] + color0[2]);
		}
	}
	return true;
}
bool FAMCDecoder::DecodeAU(FILE * in, Animation &anim, IndexedFaceSet & ifs){
	if ( DecodeHeader(anim, in, ifs) == false) return false;
	anim.IFSToAnim(ifs, 0);
	MotionModel * motionModel = NULL;
	if (isCoordAnimated_) {
		// we decode the motion model
		DecodeGolobalMotion(in, anim);

		int K = 0;
		
		std::vector<int> partition;
		DecodePartition(&partition, K, in);
		motionModel = new MotionModel (K, numberOfVertices_, numberOfFrames_);
		DecompressTransformsPoints(anim, *motionModel,&partition, in);
		motionModel->SetWeightFromPartiton(partition);
		DecompressWeights(anim, *motionModel, &partition, in);

		ifs.ComputeVerticesTraversal(&partition);
	}

	ScalableResidualsDecoder *SRDecCoord= new ScalableResidualsDecoder(numberOfFrames_-1, numberOfVertices_);
	ScalableResidualsDecoder *SRDecNormal= new ScalableResidualsDecoder(numberOfFrames_-1, numberOfNormals_);
	ScalableResidualsDecoder *SRDecColor = new ScalableResidualsDecoder(numberOfFrames_-1, numberOfColors_);


	int * predNormal = NULL;
	if (isNormalAnimated_) {
		predNormal = new int[numberOfNormals_];
		DecompressNormalPredictors(in, predNormal);
	}
	// we decode residual errors
	if (transformType_==0) { // decode LIFTING-based
			if (isCoordAnimated_) SRDecCoord->GenerateLayers(in);
			if (isNormalAnimated_) SRDecNormal->GenerateLayers(in);
			if (isColorAnimated_) SRDecColor->GenerateLayers(in);

			for (int layerSNR = 0; layerSNR < SRDecCoord->GetNLayers(); layerSNR++) {
				if (isCoordAnimated_) SRDecCoord->DecodeLayer(layerSNR, in);
				if (isNormalAnimated_) SRDecNormal->DecodeLayer(layerSNR, in);
				if (isColorAnimated_) SRDecColor->DecodeLayer(layerSNR, in);
			}
			if (isCoordAnimated_) SetCoordResidualErrorsLift(*SRDecCoord, anim, ifs,  motionModel);
			if (isNormalAnimated_) SetNormalResidualErrorsLift(*SRDecNormal, anim, ifs,  motionModel, predNormal);
			if (isColorAnimated_) SetColorResidualErrorsLift(*SRDecColor, anim);
	}

	if (transformType_==1) { // decode DCT-based
			if (isCoordAnimated_) SRDecCoord->GenerateLayers(in);
			if (isNormalAnimated_) SRDecNormal->GenerateLayers(in);
			if (isColorAnimated_) SRDecColor->GenerateLayers(in);

			for (int layerSNR = 0; layerSNR < SRDecCoord->GetNLayers(); layerSNR++) {
				if (isCoordAnimated_) SRDecCoord->DecodeLayer(layerSNR, in);
				if (isNormalAnimated_) SRDecNormal->DecodeLayer(layerSNR, in);
				if (isColorAnimated_) SRDecColor->DecodeLayer(layerSNR, in);
			}
			if (isCoordAnimated_) SetCoordResidualErrorsDCT(*SRDecCoord, anim, ifs,  motionModel);
			if (isNormalAnimated_) SetNormalResidualErrorsDCT(*SRDecNormal, anim, ifs,  motionModel, predNormal);
			if (isColorAnimated_) SetColorResidualErrorsDCT(*SRDecColor, anim);
	}


	float maxCoord[3] = {0.0f, 0.0f, 0.0f};
	float minCoord[3] = {0.0f, 0.0f, 0.0f};
	unsigned char nBitsCoord = 8;
	float maxNormal[3] = {0.0f, 0.0f, 0.0f};
	float minNormal[3] = {0.0f, 0.0f, 0.0f};
	unsigned char nBitsNormal = 8;
	float maxColor[3] = {0.0f, 0.0f, 0.0f};
	float minColor[3] = {0.0f, 0.0f, 0.0f};
	unsigned char nBitsColor = 8;

	if (transformType_==3){	// needed for unquantization
		if (isCoordAnimated_) {
			AOF( 1 == fread(&nBitsCoord,  sizeof(unsigned char), 1, in ) );
			AOF( 1 == fread(&maxCoord[0], sizeof(float), 1, in ) );
			AOF( 1 == fread(&maxCoord[1], sizeof(float), 1, in ) );
			AOF( 1 == fread(&maxCoord[2], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minCoord[0], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minCoord[1], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minCoord[2], sizeof(float), 1, in ) );
		}
		if (isNormalAnimated_) {
			AOF( 1 == fread(&nBitsNormal,  sizeof(unsigned char), 1, in ) );
			AOF( 1 == fread(&maxNormal[0], sizeof(float), 1, in ) );
			AOF( 1 == fread(&maxNormal[1], sizeof(float), 1, in ) );
			AOF( 1 == fread(&maxNormal[2], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minNormal[0], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minNormal[1], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minNormal[2], sizeof(float), 1, in ) );
		}
		if (isColorAnimated_) {
			AOF( 1 == fread(&nBitsColor,  sizeof(unsigned char), 1, in ) );
			AOF( 1 == fread(&maxColor[0], sizeof(float), 1, in ) );
			AOF( 1 == fread(&maxColor[1], sizeof(float), 1, in ) );
			AOF( 1 == fread(&maxColor[2], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minColor[0], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minColor[1], sizeof(float), 1, in ) );
			AOF( 1 == fread(&minColor[2], sizeof(float), 1, in ) );
		}
	}


	if (transformType_==2 || 
		transformType_==3 ||
		transformType_==4){// decode anim LD-based 	
		printf("\n<======== LD start ==========>\n");

		// init
		static int segmentNumber = 0;
		printf("Decoding segment %d\n",segmentNumber);
		static LayeredDecomposition ld;
		bool decodeLossless = (transformType_==3);//integer based Lift+LD
		LDSegmentDecoder dec(anim.GetNKeyCoord()-1, anim.GetNCoord(), isCoordAnimated_, isNormalAnimated_, isColorAnimated_, decodeLossless);
		LDReader reader(in);

		// decode segment header
		LDSegmentDecoder::LDType ldType;
		unsigned int numberOfDecomposedLayers = 0;
		dec.decodeLDSegmentHeader(ldType, numberOfDecomposedLayers, reader);
		numberOfDecomposedLayers_=numberOfDecomposedLayers; //used for LOD
		 
		// derive new layered decomposition (optionally)
		if (ldType==LDSegmentDecoder::newLDFromConn) {
			LDSegmentDecoder::createLayeredDecompositionFromConn(ifs, numberOfDecomposedLayers, ld);
		}
		if (ldType==LDSegmentDecoder::newLDFromConnAndDecodedData) {
			LDSegmentDecoder::createLayeredDecompositionFromConnAndDecodedData(ifs, dec.getSimplificationOperations(), ld);
		}
		
		// decode all frames
		dec.initLDReconstructers(&ld);
		dec.decodeLDAllFrames(reader);
		dec.getAnimation(anim, 1);

		++segmentNumber;
		printf("<======== LD end ==========>\n\n");
	}

	if (transformType_==3){ //apply inv. LIFITING-transform to anim
		printf("\n<======== inv. LIFTING-transform start ==========>\n");

		if (isCoordAnimated_) {
			float coord1[3] = {0.0f, 0.0f, 0.0f};
			LiftCodec liftX(numberOfFrames_-1, numberOfVertices_);
			LiftCodec liftY(numberOfFrames_-1, numberOfVertices_);
			LiftCodec liftZ(numberOfFrames_-1, numberOfVertices_);

			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetCoord(f, pos, coord1);
					liftX.function_[pos][f-1] = coord1[0];
					liftY.function_[pos][f-1] = coord1[1];
					liftZ.function_[pos][f-1] = coord1[2];
				}
			}

			liftX.LiftLS(false);
			liftY.LiftLS(false);
			liftZ.LiftLS(false);

			liftX.Unquantize2(nBitsCoord, maxCoord[0], minCoord[0]);
			liftY.Unquantize2(nBitsCoord, maxCoord[1], minCoord[1]);
			liftZ.Unquantize2(nBitsCoord, maxCoord[2], minCoord[2]);

			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					coord1[0] = liftX.function_[pos][f-1];
					coord1[1] = liftY.function_[pos][f-1];
					coord1[2] = liftZ.function_[pos][f-1];
					anim.SetCoord(f, pos, coord1);
				}
			}
		}
		if (isNormalAnimated_) {
			float normal1[3] = {0.0f, 0.0f, 0.0f};
			LiftCodec liftNX(numberOfFrames_-1, numberOfNormals_);
			LiftCodec liftNY(numberOfFrames_-1, numberOfNormals_);
			LiftCodec liftNZ(numberOfFrames_-1, numberOfNormals_);

			for (int pos = 0; pos < (int) numberOfNormals_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetNormal(f, pos, normal1);
					liftNX.function_[pos][f-1] = normal1[0];
					liftNY.function_[pos][f-1] = normal1[1];
					liftNZ.function_[pos][f-1] = normal1[2];
				}
			}

			liftNX.LiftLS(false);
			liftNY.LiftLS(false);
			liftNZ.LiftLS(false);

			liftNX.Unquantize2(nBitsNormal, maxNormal[0], minNormal[0]);
			liftNY.Unquantize2(nBitsNormal, maxNormal[1], minNormal[1]);
			liftNZ.Unquantize2(nBitsNormal, maxNormal[2], minNormal[2]);

			for (int pos = 0; pos < (int) numberOfNormals_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					normal1[0] = liftNX.function_[pos][f-1];
					normal1[1] = liftNY.function_[pos][f-1];
					normal1[2] = liftNZ.function_[pos][f-1];
					anim.SetNormal(f, pos, normal1);
				}
			}
		}
		if (isColorAnimated_) {
			float color1[3] = {0.0f, 0.0f, 0.0f};
			LiftCodec liftR(numberOfFrames_-1, numberOfColors_);
			LiftCodec liftG(numberOfFrames_-1, numberOfColors_);
			LiftCodec liftB(numberOfFrames_-1, numberOfColors_);

			for (int pos = 0; pos < (int) numberOfColors_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetColor(f, pos, color1);
					liftR.function_[pos][f-1] = color1[0];
					liftG.function_[pos][f-1] = color1[1];
					liftB.function_[pos][f-1] = color1[2];
				}
			}

			liftR.LiftLS(false);
			liftG.LiftLS(false);
			liftB.LiftLS(false);

			liftR.Unquantize2(nBitsColor, maxColor[0], minColor[0]);
			liftG.Unquantize2(nBitsColor, maxColor[1], minColor[1]);
			liftB.Unquantize2(nBitsColor, maxColor[2], minColor[2]);

			for (int pos = 0; pos < (int) numberOfColors_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					color1[0] = liftR.function_[pos][f-1];
					color1[1] = liftG.function_[pos][f-1];
					color1[2] = liftB.function_[pos][f-1];
					anim.SetColor(f, pos, color1);
				}
			}
		}
		printf("\n<======== inv. LIFTING-transform end   ==========>\n");
	}

	if (transformType_==4){//apply inv. DCT-transform to anim
		printf("\n<======== inv. DCT-transform end   ==========>\n");
		DCTEncoder dctX(numberOfFrames_-1, numberOfVertices_);
		DCTEncoder dctY(numberOfFrames_-1, numberOfVertices_);
		DCTEncoder dctZ(numberOfFrames_-1, numberOfVertices_);
		float coord1[3] = {0.0f, 0.0f, 0.0f};

		if (isCoordAnimated_) {
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetCoord(f, pos, coord1);
					dctX.spectrum_[pos][f-1] = coord1[0];
					dctY.spectrum_[pos][f-1] = coord1[1];
					dctZ.spectrum_[pos][f-1] = coord1[2];
				}
			}
			dctX.IDCT();
			dctY.IDCT();
			dctZ.IDCT();		
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					coord1[0] = dctX.function_[pos][f-1];
					coord1[1] = dctY.function_[pos][f-1];
					coord1[2] = dctZ.function_[pos][f-1];
					anim.SetCoord(f, pos, coord1);
				}
			}
		}

		if (isNormalAnimated_) {
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetNormal(f, pos, coord1);
					dctX.spectrum_[pos][f-1] = coord1[0];
					dctY.spectrum_[pos][f-1] = coord1[1];
					dctZ.spectrum_[pos][f-1] = coord1[2];
				}
			}
			dctX.IDCT();
			dctY.IDCT();
			dctZ.IDCT();		
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					coord1[0] = dctX.function_[pos][f-1];
					coord1[1] = dctY.function_[pos][f-1];
					coord1[2] = dctZ.function_[pos][f-1];
					anim.SetNormal(f, pos, coord1);
				}
			}
		}

		if (isColorAnimated_) {
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetColor(f, pos, coord1);
					dctX.spectrum_[pos][f-1] = coord1[0];
					dctY.spectrum_[pos][f-1] = coord1[1];
					dctZ.spectrum_[pos][f-1] = coord1[2];
				}
			}
			dctX.IDCT();
			dctY.IDCT();
			dctZ.IDCT();		
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					coord1[0] = dctX.function_[pos][f-1];
					coord1[1] = dctY.function_[pos][f-1];
					coord1[2] = dctZ.function_[pos][f-1];
					anim.SetColor(f, pos, coord1);
				}
			}
		}
		printf("\n<======== inv. DCT-transform end   ==========>\n");
	}

	if (transformType_==2 || 
		transformType_==3 ||
		transformType_==4){// preform inv. motion compensation

			// we apply inv. skinning-based motion compensation
			if (isCoordAnimated_) {
				float coord0[3]={0.0f, 0.0f, 0.0f};
				float coordp[3]={0.0f, 0.0f, 0.0f};
				float coord1[3]={0.0f, 0.0f, 0.0f};
				for (int v = 0; v < (int) numberOfVertices_; v++) {
					for(int f = 1; f < (int) numberOfFrames_; f++){
						anim.GetCoord(0, v, coord0);
						anim.GetCoord(f, v, coord1);
						motionModel->Get(coord0, v, f, coordp);
						coord1[0] += coordp[0];
						coord1[1] += coordp[1];
						coord1[2] += coordp[2];
						anim.SetCoord(f, v, coord1);
					}
				}

				// we apply inv. global motion compensation
				float coord[3] = {0, 0, 0};
				for(int f = 0; f < (int) numberOfFrames_; f++){
					for (int v = 0; v < (int) numberOfVertices_; v++) {
						anim.GetCoord(f, v, coord);
						coord[0] += globalMotion_[f][0];
						coord[1] += globalMotion_[f][1];
						coord[2] += globalMotion_[f][2];
						anim.SetCoord(f, v, coord);
					}
				}
			}
			if (isNormalAnimated_) {
				float normal0[3]={0.0f, 0.0f, 0.0f};
				float normalp[3]={0.0f, 0.0f, 0.0f};
				float normal1[3]={0.0f, 0.0f, 0.0f};
				for (int n = 0; n < (int) numberOfNormals_; n++) {
					for(int f = 1; f < (int) numberOfFrames_; f++){
						anim.GetNormal(f, n, normal1);
						anim.GetNormal(0, n, normal0);
						MathTools::vectorUnitary(normal0);
						if (predNormal[n] == 0) {
							for (int k = 0; k < 3 ; k++) normalp[k] = normal0[k];
						}
						if (predNormal[n] == 1) {
							motionModel->GetN(normal0, n, f, normalp);
						}
						if (predNormal[n] == 2) {
							motionModel->GetNO(normal0, n, f, normalp);
						}

						// We normalize the vector
						MathTools::vectorUnitary(normalp);

						normal1[0]+= normalp[0];
						normal1[1]+= normalp[1];
						normal1[2]+= normalp[2];

						// We normalize the vector
						MathTools::vectorUnitary(normal1);

						anim.SetNormal(f, n, normal1);
					}
				}
			}
			if (isColorAnimated_) {
				float color0[3]={0.0f, 0.0f, 0.0f};
				float color1[3]={0.0f, 0.0f, 0.0f};
				for (int v = 0; v < (int) numberOfColors_; v++) {
					for(int f = 1; f < (int) numberOfFrames_; f++){
						anim.GetColor(f, v, color1);
						anim.GetColor(0, v, color0);
						color1[0]+=color0[0];
						color1[1]+=color0[1];
						color1[2]+=color0[2];
						anim.SetColor(f, v, color1);
					}
				}			
			}			
	}


	// free memory
	if (motionModel != NULL) delete motionModel;
	if (predNormal != NULL) delete [] predNormal;
	return true;
}

bool FAMCDecoder::Decode(char * file){
	Animation myAnimDec;
	char fin[1024];
	sprintf(fin, "%s.mp4", file);

	// we save the first frame
	char fFirstFrame[1024];
	sprintf(fFirstFrame, "%sFF.wrl", file);
	IndexedFaceSet firstFrame;
	firstFrame.LoadIFSVRML2(fFirstFrame);

	FAMCDecoder decoder;
	FILE * in = fopen(fin, "rb");
	decoder.DecodeAU(in, myAnimDec, firstFrame);
	fclose(in);

	char fAnimOut[1024];
	sprintf(fAnimOut, "%s_dec.wrl", file);
	myAnimDec.SaveInterpolatorVRML2(fAnimOut, firstFrame);
	return true;
}


bool FAMCDecoder::Decode(const char * file, Animation &myAnimDec){
	char fin[1024];
	sprintf(fin, "%s.mp4", file);

	// we save the first frame
	char fFirstFrame[1024];
	sprintf(fFirstFrame, "%sFF.wrl", file);
	IndexedFaceSet firstFrame;
	firstFrame.LoadIFSVRML2(fFirstFrame);

	FAMCDecoder decoder;
	FILE * in = fopen(fin, "rb");
	decoder.DecodeAU(in, myAnimDec, firstFrame);
	fclose(in);

	return true;
}

bool FAMCDecoder::DecodeStream(char * file, int numberOfSpatialLayers){
	
	Animation myAnimDec;
	char fin[1024];
	sprintf(fin, "%s.mp4", file);

	// we save the first frame
	char fFirstFrame[1024];
	sprintf(fFirstFrame, "%sFF.wrl", file);
	IndexedFaceSet firstFrame;
	firstFrame.LoadIFSVRML2(fFirstFrame);

	FAMCDecoder decoder;
	FILE * in = fopen(fin, "rb");
	bool stillAU = true;
	std::vector<float> keys;
	std::vector<float> coord;
	std::vector<float> normal;
	std::vector<float> color;

	int s = 0;
	while (stillAU) {
		Animation animSeg;
		stillAU = decoder.DecodeAU(in, animSeg, firstFrame);
		if (stillAU) {	
			animSeg.GetAnimationData(keys, coord, normal, color, 1);
		}
		s++;
	}
	fclose(in);

	// update keys
	for (int f = 0; f < (int) keys.size(); f++) {
		keys[f] = (float) f /(keys.size()-1);
	}

	int numberOfDecomposedLayers = decoder.GetNumberOfDecomposedLayers();

	myAnimDec.CreateAnimation(keys, coord, normal, color);
	if (numberOfSpatialLayers>0 && numberOfSpatialLayers<numberOfDecomposedLayers) {
		printf("<========== LOD start ==========>\n");
		// get layered decomposition
		LayeredDecomposition ld;
		LDSegmentDecoder::createLayeredDecompositionFromConn(firstFrame, numberOfDecomposedLayers, ld);

		printf("writing decimated connectivity: layer %d of %d\n", numberOfSpatialLayers, numberOfDecomposedLayers);
		// we copy the decimated IFS
		int coordIndex[3] = {-1, -1, -1};
		coordIndex[0]= -1;
		coordIndex[1]= -1;
		coordIndex[2]= -1;
		int T = firstFrame.GetNCoordIndex(); // number of triangles
		for(int t = 0; t < T; t++) {
			firstFrame.SetCoordIndex(t, coordIndex);
		}
		int Td = (int) ld.layerIFS[numberOfSpatialLayers-1].size();
		for(int t = 0; t < Td; t++) {
			for (int k = 0; k < 3; k++) {
				coordIndex[k] = ld.layerIFS[numberOfSpatialLayers-1][t][k];
			}
			firstFrame.SetCoordIndex(t, coordIndex);
		}
		printf("<========== LOD end ============>\n");
		
	}


	char fAnimOut[1024];
	sprintf(fAnimOut, "%s_dec.wrl", file);
	myAnimDec.SaveInterpolatorVRML2(fAnimOut, firstFrame);

	// added to output the animation as a sequence of IFSs
//	char fileOutM[1024];
//	for (int frame = 0; frame < myAnimDec.GetNKeyCoord(); frame++) {
//		sprintf(fileOutM, "%s_%i_dec.wrl", file, frame);
//		myAnimDec.SaveFrame(fileOutM, firstFrame, frame);
//	}
	return true;
}

bool FAMCDecoder::DecodeStream(char * file, int numberOfSpatialLayers,  Animation &myAnimDec){
	
	char fin[1024];
	sprintf(fin, "%s.mp4", file);

	// we load the first frame
	char fFirstFrame[1024];
	sprintf(fFirstFrame, "%sFF.wrl", file);
	IndexedFaceSet firstFrame;
	firstFrame.LoadIFSVRML2(fFirstFrame);

	FAMCDecoder decoder;
	FILE * in = fopen(fin, "rb");
	bool stillAU = true;
	std::vector<float> keys;
	std::vector<float> coord;
	std::vector<float> normal;
	std::vector<float> color;

	int s = 0;
	while (stillAU) {
		Animation animSeg;
		stillAU = decoder.DecodeAU(in, animSeg, firstFrame);
		if (stillAU) {	
			animSeg.GetAnimationData(keys, coord, normal, color, 1);
		}
		s++;
	}
	fclose(in);

	// update keys
	for (int f = 0; f < (int) keys.size(); f++) {
		keys[f] = (float) f /(keys.size()-1);
	}

	int numberOfDecomposedLayers = decoder.GetNumberOfDecomposedLayers();

	myAnimDec.CreateAnimation(keys, coord, normal, color);
	if (numberOfSpatialLayers>0 && numberOfSpatialLayers<numberOfDecomposedLayers) {
		printf("<========== LOD start ==========>\n");
		// get layered decomposition
		LayeredDecomposition ld;
		LDSegmentDecoder::createLayeredDecompositionFromConn(firstFrame, numberOfDecomposedLayers, ld);

		printf("writing decimated connectivity: layer %d of %d\n", numberOfSpatialLayers, numberOfDecomposedLayers);
		// we copy the decimated IFS
		int coordIndex[3] = {-1, -1, -1};
		coordIndex[0]= -1;
		coordIndex[1]= -1;
		coordIndex[2]= -1;
		int T = firstFrame.GetNCoordIndex(); // number of triangles
		for(int t = 0; t < T; t++) {
			firstFrame.SetCoordIndex(t, coordIndex);
		}
		int Td = (int) ld.layerIFS[numberOfSpatialLayers-1].size();
		for(int t = 0; t < Td; t++) {
			for (int k = 0; k < 3; k++) {
				coordIndex[k] = ld.layerIFS[numberOfSpatialLayers-1][t][k];
			}
			firstFrame.SetCoordIndex(t, coordIndex);
		}
		printf("<========== LOD end ============>\n");
		
	}

	// added to output the animation as a sequence of IFSs
//	char fileOutM[1024];
//	for (int frame = 0; frame < myAnimDec.GetNKeyCoord(); frame++) {
//		sprintf(fileOutM, "%s_%i_dec.wrl", file, frame);
//		myAnimDec.SaveFrame(fileOutM, firstFrame, frame);
//	}
	return true;
}
