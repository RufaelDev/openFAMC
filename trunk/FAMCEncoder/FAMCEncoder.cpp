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
#include "FAMCEncoder.h"
#include "Animation.h"
#include "Console.h"
#include "DCTEncoder.h"
#include "HierarchicalPartitionner.h"
#include "AffineMotionPredictor.h"
#include "MathTools.h"
#include "LiftCodec.h"
#include "LDSegmentDecomposer.h"
#include "LDWriter.h"
#include "LDFrameOrderFactory.h"
#include "SConnectivity.h"
#include "LayeredDecomposerAutomatic.h"
#include "LDSegmentEncoder.h"
#include "LDSegmentDecoder.h"
#include "LiftScheme.h"
#include "DCTEncoder.h"
#include "LayeredSimplificationOperationsFactory.h"
#include <math.h>
//#include <Load3DMeshFiles2Anim.h>

extern CConsole logger;

FAMCEncoder::FAMCEncoder(const FAMCParams & param){
	params_ = param;
	isCoordAnimated_ = false;
	isNormalAnimated_ = false;
	isColorAnimated_ = false;
	isOtherAttributeAnimated_ = false;

	isInterpolationNeeded_ = false;

	numberOfFrames_ = 0;
	numberOfVertices_ = 0;
	numberOfNormals_ = 0;
	numberOfColors_ = 0;
	numberOfOtherAttributes_ = 0;
}
FAMCEncoder::~FAMCEncoder(void)
{
}
int FAMCEncoder::EncodePartition(std::vector<int> * partition, int K, FILE * out) {
	int V = (int) partition->size();
	int nbits = 1;
	if ( K > 1) nbits = (int) MathTools::nBinaryBits(K-1);
	//-------------------------------
	// RLE encoding with CABAC
	CABAC cabac;


	// we allocate memory for the encoded stream

	int N = V*4;
	int stream_sizeE = 0;
	unsigned char * bufferE = new unsigned char[N];		
	// we initialize the encoding process	
	cabac.arienco_start_encoding(cabac._eep, bufferE, &stream_sizeE);

	// we initialize the context
	cabac.biari_init_context(cabac._ctx, 61);


	int currentSymbol = 0;
	int occurence = 1;
	for (int v = 0; v < (int) partition->size(); v++) {
		if ( v == 0) {
			currentSymbol = (*partition)[0];
			occurence = 1;
			
		}
		else {
			if ( (*partition)[v] != currentSymbol){
				for (int bp = nbits-1; bp>= 0; bp--) {
					int res= ((currentSymbol & (1<<bp)) != 0)?1:0;
					cabac.biari_encode_symbol_eq_prob(cabac._eep, res);

				}
				cabac.unary_exp_golomb_encode(cabac._eep, occurence-1, cabac._ctx, 2);
				occurence = 1;
				currentSymbol = (*partition)[v];
			}
			else {
				occurence++;
			}
		}

	}
	// encode the last symbol
	for (int bp = nbits-1; bp>= 0; bp--) {
		int res= ((currentSymbol & (1<<bp)) != 0)?1:0;
		cabac.biari_encode_symbol_eq_prob(cabac._eep, res);
	}
	cabac.unary_exp_golomb_encode(cabac._eep, occurence-1, cabac._ctx, 2);

	cabac.biari_encode_symbol_final(cabac._eep, 1);

	// we end the encoding process
	cabac.arienco_done_encoding(cabac._eep);

	// write to file
	fwrite(&K, sizeof(int), 1, out);
	fwrite(&stream_sizeE, sizeof(int), 1, out);
	fwrite(bufferE, sizeof(unsigned char), stream_sizeE, out);

	// free memory
	delete [] bufferE;
	return stream_sizeE+8;
}
int FAMCEncoder::EncodeGolobalMotion(Animation &anim, FILE * out, int nbits) {

	int F = anim.GetNKeyCoord()-1;
	int V = anim.GetNCoord();
	DCTEncoder dctGlobalMotionX(F, 1);
	DCTEncoder dctGlobalMotionY(F, 1);
	DCTEncoder dctGlobalMotionZ(F, 1);


	// we compute the barycenters of all the frames
	float coord[3] = {0.0, 0.0, 0.0};
	float xg = 0.0;
	float yg = 0.0;
	float zg = 0.0;
	float xg0 = 0.0;
	float yg0 = 0.0;
	float zg0 = 0.0;
	for(int f = 0; f < F+1; f++){
		xg = 0.0;
		yg = 0.0;
		zg = 0.0;
		for (int v = 0; v < V; v++) {
			anim.GetCoord(f, v, coord);
			xg += coord[0];
			yg += coord[1];
			zg += coord[2];
		}
		xg/=V;
		yg/=V;
		zg/=V;
		if ( f==0 ) {
			xg0 = xg;
			yg0 = yg;
			zg0 = zg;
		}
		else {
			dctGlobalMotionX.function_[0][f-1] = xg;
			dctGlobalMotionY.function_[0][f-1] = yg;
			dctGlobalMotionZ.function_[0][f-1] = zg;
		}
	}

	dctGlobalMotionX.DCT();
	dctGlobalMotionY.DCT();
	dctGlobalMotionZ.DCT();
	
	// Encoding
	ScalableResidualsEncoder SREnc(F, 1, nbits);
	SREnc.GetData(dctGlobalMotionX, dctGlobalMotionY, dctGlobalMotionZ);
	SREnc.SetNMaxLayers(1);
	SREnc.SetNLayers(1);
	int globalMotionSize = SREnc.GenerateLayers(out);
	globalMotionSize += SREnc.EncodeLayer(0, out);	

	dctGlobalMotionX.IDCT();
	dctGlobalMotionY.IDCT();
	dctGlobalMotionZ.IDCT();

	// we center all the frames
	for(int f = 0; f < F+1; f++){
		for (int v = 0; v < V; v++) {
			anim.GetCoord(f, v, coord);
			if (f==0) {
				coord[0] -= xg0;
				coord[1] -= yg0;
				coord[2] -= zg0;
			}
			else {
				coord[0] -= dctGlobalMotionX.function_[0][f-1];
				coord[1] -= dctGlobalMotionY.function_[0][f-1];
				coord[2] -= dctGlobalMotionZ.function_[0][f-1];
			}
			anim.SetCoord(f, v, coord);
		}
	}
	return globalMotionSize;
}
int FAMCEncoder::EncodeHeader(Animation &anim, IndexedFaceSet & ifs, FILE * out) {
	int headerSize = 0;
	unsigned int startCode = FAMCAnimationSegmentStartCode;
	unsigned int animationSegmentSize = 0;
	unsigned char staticMeshDecodingType = 0;	// read static mesh from BIFS scene
	fwrite(&startCode, sizeof(unsigned int), 1, out); headerSize+=4;
	fwrite(&animationSegmentSize, sizeof(unsigned int), 1, out); headerSize+=4;
	fwrite(&staticMeshDecodingType, sizeof(unsigned char), 1, out); headerSize+=1;

	if ( anim.GetNCoord() > 0) {
		isCoordAnimated_ = true;
	}
	if ( anim.GetNNormal() > 0){
		isNormalAnimated_ = true;
	}
	if ( anim.GetNColor() > 0) {
		isColorAnimated_ = true;
	}
	// should be updated if others attributes can be animated in MPEG standard
	isOtherAttributeAnimated_ = false;


	unsigned char mask = 0;
	numberOfFrames_ = anim.GetNKeyCoord();

	if (isCoordAnimated_) {
		mask += 1;
	}
	if (isNormalAnimated_) {
		mask += 2;
	}

	if (isColorAnimated_) {
		mask += 4;
	}

	if (isOtherAttributeAnimated_) {
		mask += 8;
	}

	mask = mask | (params_.transformType_<<4) | (isInterpolationNeeded_<<7);
	numberOfVertices_ = anim.GetNCoord();
	numberOfNormals_ = anim.GetNNormal();
	numberOfColors_ = anim.GetNColor();
	numberOfOtherAttributes_ = anim.GetNAttributes();

	unsigned char maskPred = params_.Normal_Pred_;

	
	logger.write_2_log("numberOfFrames = %i\n", numberOfFrames_);
	logger.write_2_log("numberOfVertices = %i\n", numberOfVertices_);
	logger.write_2_log("numberOfNormals = %i\n", numberOfNormals_);
	logger.write_2_log("numberOfColors = %i\n", numberOfColors_);
	logger.write_2_log("numberOfOtherAttributes = %i\n", numberOfOtherAttributes_);

	fwrite(&mask, sizeof(unsigned char), 1, out); headerSize+=1;
	fwrite(&maskPred, sizeof(unsigned char), 1, out); headerSize+=1;
	fwrite(&numberOfFrames_, sizeof(unsigned int), 1, out); headerSize+=4;

	for (int frame = 1; frame < numberOfFrames_; frame++) {
		unsigned int timeFrame = (unsigned int) (anim.GetDuration() * 1000.0 * anim.GetKeyCoord()[frame]);
		fwrite(&timeFrame, sizeof(unsigned int), 1, out); headerSize+=4;
	}
	return headerSize;
}
void FAMCEncoder::ComputeAffineTransforms(Animation * anim, std::vector<int> * partition, int K, MotionModel &motionModel){
	// we fill the clusters by pushing their corresponding vertices
	std::vector<int> * clusters  = new std::vector<int> [K];
	for (int i = 0; i < (int) partition->size(); i++) {
		clusters[(*partition)[i]].push_back(i);
	}
	// we create our motion model
	motionModel.SetWeightFromPartiton(*partition);
	printf("Affine transforms\n");
	printf("0%%");
	//we fill the affine trtansforms
	for (int k = 0; k < K; k++) {
		AffineTransform at;
		AffineMotionPredictor pred;
		pred.Init(anim, &(clusters[k]));
		for(int f = 0; f < (int) numberOfFrames_; f++){
			pred.ComputeAffineTransform(f, at);
			motionModel.SetAffineTransform(at, f, k);
		}
		printf("\r");
		int progress = (int) ((float) k * 100.0) / K;
		printf("%i%%", progress);
	}
}
void FAMCEncoder::ComputeWeights(Animation * anim, IndexedFaceSet &ifs,  std::vector<int> * partition, int K, MotionModel &motionModel){
	// we compute clusters adjacency
	anim->ComputeClustersAdjacency(partition, ifs.GetCoordIndex(), ifs.GetNCoordIndex());
	// we compute the weights
	IntMultiVect * adj = anim->GetAdj();

	printf("\r");
	printf("Weights\n");
	printf("0%%");

	for (int vertex = 0; vertex < (int) numberOfVertices_; vertex++){	
		printf("\r");
		int progress = (int) ((float) vertex * 100.0) / numberOfVertices_;
		printf("%i%%", progress);

		int vertexCluster = (*partition)[vertex];
		int n = 3 * numberOfFrames_;
		int m = (int) (*adj)[vertexCluster].size();

		Mat_DP A(n,m);
		Mat_DP V(m,m);
		Vec_DP x(m);
		Vec_DP b(n);
		Vec_DP w(m);

		// Filling b
		float coord[3];
		int p = 0;
		for(int frame = 0; frame < (int) numberOfFrames_; frame++) {
			anim->GetCoord(frame, vertex, coord);
			b[p] = coord[0];p++;
			b[p] = coord[1];p++;
			b[p] = coord[2];p++;
		}

		//	Filling A
		AffineTransform at;
		float coord0[3];
		float coordp[3];
		anim->GetCoord(0, vertex, coord0);
		for(int frame = 0; frame < (int) numberOfFrames_; frame++){
			for (int k = 0; k < m; k++) {
				motionModel.GetAffineTransform(at, frame, (*adj)[vertexCluster][k]);
				at.Get(coord0, coordp);
				for (int h = 0; h < 3; h++) {
					A[frame*3+h][k] = coordp[h];
				}
			}
		}

		// computing svd
		NR::svdcmp(A,w,V);

		// Computing the pseudo-inverse
		Mat_DP pinvA(m,n);
		for (int k = 0; k < w.size(); k++) {
			if ( fabs(w[k]) > TOL) {
				w[k] = 1.0 /w[k];
			}
			else {
				w[k] = 0.0;
			}
		}

		for (int k = 0; k < V.nrows(); k++) {
			for (int l = 0; l < V.ncols(); l++) {
				V[k][l] *= w[l];
			}
		}

		for (int k = 0; k < pinvA.nrows(); k++) {
			for (int l = 0; l < pinvA.ncols(); l++) {
				pinvA[k][l] = 0.0;
				for (int h = 0; h < V.ncols(); h++) {
						pinvA[k][l] += V[k][h] * A[l][h]; 
				}
			}
		}


		// we compute x = pinvA * b
		for (int k = 0; k < pinvA.nrows(); k++) {
			x[k] = 0.0;
			for (int l = 0; l < pinvA.ncols(); l++) {
				x[k] += pinvA[k][l] * b[l];
			}
		}

		// we fill the weights
		for(int k =0; k < K; k++) {
			motionModel.SetWeight(0.0, vertex, k);
		}
		for(int cluster =0; cluster < m; cluster++) {
			motionModel.SetWeight((float) x[cluster], vertex, (*adj)[vertexCluster][cluster]);
		}
	}
	printf("\r     \n");
}
int FAMCEncoder::CompressAffineTransformsPoints(Animation * anim, std::vector<int> * partition, MotionModel & motionModel, int nbits, FILE * out) {
	// we encode the affine transforms
	int affineTransformsSize = 0;
	int K = motionModel.GetNClusters();
	int F = numberOfFrames_-1;

	int nbr = 4 * K;
	int len = F;

	DCTEncoder dctEncoderX(len, nbr);
	DCTEncoder dctEncoderY(len, nbr);
	DCTEncoder dctEncoderZ(len, nbr);

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
			anim->GetCoord(0, vertex, coord);
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
			anim->GetCoord(0, vertex, coord);
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

	float coord_pred[4][3];
	for (int k = 0; k < K; k++) {
		for (int frame = 0; frame < F; frame++){
			AffineTransform at;
			motionModel.GetAffineTransform(at, frame+1, k);
			for (int p = 0; p < 4; p++) {
				at.Get(coord_org[k*4+p], coord_pred[p]);
			}
			dctEncoderX.function_[k*4+0][frame] = coord_pred[0][0];
			dctEncoderX.function_[k*4+1][frame] = coord_pred[1][0];
			dctEncoderX.function_[k*4+2][frame] = coord_pred[2][0];
			dctEncoderX.function_[k*4+3][frame] = coord_pred[3][0];

			dctEncoderY.function_[k*4+0][frame] = coord_pred[0][1];
			dctEncoderY.function_[k*4+1][frame] = coord_pred[1][1];
			dctEncoderY.function_[k*4+2][frame] = coord_pred[2][1];
			dctEncoderY.function_[k*4+3][frame] = coord_pred[3][1];

			dctEncoderZ.function_[k*4+0][frame] = coord_pred[0][2];
			dctEncoderZ.function_[k*4+1][frame] = coord_pred[1][2];
			dctEncoderZ.function_[k*4+2][frame] = coord_pred[2][2];
			dctEncoderZ.function_[k*4+3][frame] = coord_pred[3][2];
		}
	}

	//---------------------------


	dctEncoderX.DCT();
	dctEncoderY.DCT();
	dctEncoderZ.DCT();

	// Encoding
	ScalableResidualsEncoder SREnc(len, nbr, nbits);
	SREnc.GetData(dctEncoderX, dctEncoderY, dctEncoderZ);
	SREnc.SetNMaxLayers(1);
	SREnc.SetNLayers(1);
	affineTransformsSize += SREnc.GenerateLayers(out);
	affineTransformsSize += SREnc.EncodeLayer(0, out);	

	// to have the same values as those of the decoder
	dctEncoderX.IDCT();
	dctEncoderY.IDCT();
	dctEncoderZ.IDCT();

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
	delete [] clusters;


	return affineTransformsSize;
}

int FAMCEncoder::CompressWeights(Animation &anim, MotionModel & motionModel, std::vector<int> * partition, float ratioWeights, unsigned char nbits, FILE * out){
	//  We keep only efficient weights
	int * filter = new int [numberOfVertices_];
	float * errors = new float [numberOfVertices_];

	for (int v = 0; v < (int) numberOfVertices_; v++) {
		filter[v] = 1;
		errors[v] = 0.0f;
	}

	float diagonal  = (float) anim.ComputeFirstFrameBBDiagonal();
	float de = 0.0;
	float max = 0.0;

	// we compute per vertex error
	for (int vertex = 0; vertex < (int) numberOfVertices_; vertex++){	
		de = anim.ComputeVertexErrorsDiff(vertex, motionModel, partition) / diagonal;
		if ( max < de) max = de;
		errors[vertex] = de;
	}

	logger.write_2_log("Weights: max = %f, %f\n", max, max * ratioWeights);

	int nbr = 0;
	for (int v = 0; v < (int) numberOfVertices_; v++) {
		if (errors[v] <= max * ratioWeights) {
			filter[v] = 0;
			nbr++;
		}
	}

	logger.write_2_log("Eliminated vertices %3.2f%%\n", (float) nbr / numberOfVertices_ * 100.0);


//  We compute clusters adjStream
	IntMultiVect * adj = anim.GetAdj();
	int K = (int) adj->size();

	std::vector<int> adjStream;

	for(int k = 0; k < K; k++) {
		adjStream.push_back((int) (*adj)[k].size()-1);
		for(int n = 0; n < (int) (*adj)[k].size(); n++) {
			if ( (*adj)[k][n] != k) {
				adjStream.push_back((*adj)[k][n]);
			}
		}
	}

	// We quantize weights only for retained vertices
	std::vector<float> weights;
	float w = 0.0;
	float minw = 0.0;
	float maxw = 0.0;
	int first = 0;
	for(int vertex = 0; vertex < (int) numberOfVertices_; vertex++) {
		int vertexCluster = (*partition)[vertex];
		if ( filter[vertex] == 1) {
			for(int cluster =0; cluster < (int) (*anim.GetAdj())[vertexCluster].size(); cluster++) {
				motionModel.GetWeight(w, vertex, (*anim.GetAdj())[vertexCluster][cluster]);
				weights.push_back(w);
				if (first == 0) {
					minw = maxw = w;
					first =1;
				}
				else {
					if ( w < minw) minw = w;
					if ( w > maxw) maxw = w;
				}
			}
		}
	}

	//-------------------------------
	// CABAC encoding
	//-------------------------------

	CABAC cabac;


	int nbitsC = (int) MathTools::nBinaryBits(K-1);

	// we allocate memory for the encoded stream

	int N = numberOfVertices_ + adjStream.size() * 4+ weights.size() * 4;
	int stream_sizeE = 0;
	unsigned char * bufferE = new unsigned char[N];		
	// we initialize the encoding process	
	cabac.arienco_start_encoding(cabac._eep, bufferE, &stream_sizeE);

	// we initialize the context
	cabac.biari_init_context(cabac._ctx, 61);

	// encoding retained vertices
	for (int v = 0; v < (int) numberOfVertices_; v++) {
		cabac.biari_encode_symbol(cabac._eep, filter[v], cabac._ctx);
	}

	// encoding clusters adjacency
	cabac.biari_init_context(cabac._ctx, 61);
	int s = 0;
	while (s < (int) adjStream.size()) {
		int nbrNeighbours = adjStream[s];
		cabac.unary_exp_golomb_encode(cabac._eep, nbrNeighbours, cabac._ctx, 2);
		s++;
		for(int n = 0; n < nbrNeighbours; n++) {
			for (int bp = nbitsC-1; bp>= 0; bp--) {
				int res= ((adjStream[s] & (1<<bp)) != 0)?1:0;
				cabac.biari_encode_symbol_eq_prob(cabac._eep, res);
			}
			s++;
		}
	}


	// encoding weights
	cabac.biari_init_context(cabac._ctx, 2);
	// quantization constant
	float delta = 1.0f;
	if ((maxw - minw) != 0.0) delta = (float) (1<<(nbits-1)) / (maxw - minw);


	unsigned int qs = 0;
	std::vector<unsigned int> qWeights;
	for(int p = 0; p < (int) weights.size(); p++){
		qs = (unsigned int) ((weights[p] - minw) * delta + 0.5);
		qWeights.push_back(qs);
		weights[p] = qs/delta + minw;	// to have the same values at the encoder and the decoder sides
	}

	for (int bp = nbits-1; bp>= 0; bp--) {
		for(int p = 0; p < (int) qWeights.size(); p++){
			int res= ((qWeights[p] & (1<<bp)) != 0)?1:0;
			cabac.biari_encode_symbol(cabac._eep, res, cabac._ctx);
		}
	}


	// to have the same values at the encoder and the decoder sides
	motionModel.SetWeightFromPartiton(*partition);
	int pos = 0;
	for(int vertex = 0; vertex < (int) numberOfVertices_; vertex++) {
		int vertexCluster = (*partition)[vertex];
		if ( filter[vertex] == 1) {
			for(int cluster =0; cluster < (int) (*anim.GetAdj())[vertexCluster].size(); cluster++) {
				w = weights[pos];
				motionModel.SetWeight(w, vertex, (*anim.GetAdj())[vertexCluster][cluster]);
				pos++;
			}
		}
	}

	cabac.biari_encode_symbol_final(cabac._eep, 1);

	// we end the encoding process
	cabac.arienco_done_encoding(cabac._eep);

	// write to file
	fwrite(&nbits, sizeof(unsigned char), 1, out);
	fwrite(&minw, sizeof(float), 1, out);
	fwrite(&maxw, sizeof(float), 1, out);
	fwrite(&stream_sizeE, sizeof(int), 1, out);
	fwrite(bufferE, sizeof(unsigned char), stream_sizeE, out);

	// free memory
	delete [] bufferE;
	delete [] errors;
	delete [] filter;

	return stream_sizeE+13;
}

int FAMCEncoder::GetCoordResidualErrorsDCT(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel) {
	DCTEncoder dctX(numberOfFrames_-1, numberOfVertices_);
	DCTEncoder dctY(numberOfFrames_-1, numberOfVertices_);
	DCTEncoder dctZ(numberOfFrames_-1, numberOfVertices_);
	float coord1[3] = {0.0f, 0.0f, 0.0f};
	for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
		int v = ifs.GetVerticesTraversal()[pos];
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetCoord(f, v, coord1);
			dctX.function_[pos][f-1] = coord1[0];
			dctY.function_[pos][f-1] = coord1[1];
			dctZ.function_[pos][f-1] = coord1[2];
		}
	}
	dctX.DCT();
	dctY.DCT();
	dctZ.DCT();		
				
	SREnc.GetData(dctX, dctY, dctZ);
	SREnc.SetNMaxLayers(params_.SNR_L_);
	SREnc.SetNLayers(params_.SNR_l_);
	return 0;
}

int FAMCEncoder::CompressNormalPredictors(FILE * out, int * pred, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel) {
	if ( params_.Normal_Pred_ == 3) {
		float normal0[3]={0.0f, 0.0f, 0.0f};
		float normal1[3]={0.0f, 0.0f, 0.0f};
		float normalSkinning[3]={0.0f, 0.0f, 0.0f};
		float normalTangentSkinning[3]={0.0f, 0.0f, 0.0f};
		for (int n = 0; n < (int) numberOfNormals_; n++) {
			double deltaError = 0.0;
			double skinningError = 0.0;
			double tangentSkinningError = 0.0;	
			for(int f = 1; f < (int) numberOfFrames_; f++){
				anim.GetNormal(f, n, normal1);
				anim.GetNormal(0, n, normal0);
				MathTools::vectorUnitary(normal0);
				motionModel->GetNO(normal0, n, f, normalTangentSkinning);
				motionModel->GetN(normal0, n, f, normalSkinning);
				// We normalize the two vectors
				MathTools::vectorUnitary(normal1);
				MathTools::vectorUnitary(normalSkinning);
				MathTools::vectorUnitary(normalTangentSkinning);

				deltaError += pow((double) normal1[0]-normal0[0], 2.0) +
					          pow((double) normal1[1]-normal0[1], 2.0) +
							  pow((double) normal1[2]-normal0[2], 2.0);

				skinningError += pow((double) normal1[0]-normalSkinning[0], 2.0) +
								 pow((double) normal1[1]-normalSkinning[1], 2.0) +
								 pow((double) normal1[2]-normalSkinning[2], 2.0);			

				tangentSkinningError +=	pow((double) normal1[0]-normalTangentSkinning[0], 2.0) +
										pow((double) normal1[1]-normalTangentSkinning[1], 2.0) +
										pow((double) normal1[2]-normalTangentSkinning[2], 2.0);			
			}
			pred[n] = 2;
			if ( (deltaError <= skinningError) && (deltaError <= tangentSkinningError)) pred[n] = 0;
			if ( (skinningError <= deltaError) && (skinningError <= tangentSkinningError)) pred[n] = 1;
		}

		//---------------------------------------------------------
		//					Encoding with CABAC
		//---------------------------------------------------------
		CABAC cabac;

		// we allocate memory for the encoded stream
		int N = numberOfNormals_;
		int stream_sizeE = 0;
		unsigned char * bufferE = new unsigned char[N];		

		// we initialize the encoding process	
		cabac.arienco_start_encoding(cabac._eep, bufferE, &stream_sizeE);
		cabac.biari_init_context(cabac._ctx, 61);

		for (int n = 0; n < (int) numberOfNormals_; n++) {
			cabac.unary_exp_golomb_encode(cabac._eep, 2-pred[n], cabac._ctx, 2);
		}

		// we end the encoding process
		cabac.biari_encode_symbol_final(cabac._eep, 1);
		cabac.arienco_done_encoding(cabac._eep);	

		fwrite(&stream_sizeE, sizeof(int), 1, out);
		fwrite(bufferE, sizeof(unsigned char), stream_sizeE, out);
 
		// free memory
		delete [] bufferE;
		return stream_sizeE+4;
	}
	for (int n = 0; n < (int) numberOfNormals_; n++) {
		pred[n] = params_.Normal_Pred_;
	}
	return 0;
}
int FAMCEncoder::GetNormalResidualErrorsDCT(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel, int * pred) {
	DCTEncoder dctNX(numberOfFrames_-1, numberOfNormals_);
	DCTEncoder dctNY(numberOfFrames_-1, numberOfNormals_);
	DCTEncoder dctNZ(numberOfFrames_-1, numberOfNormals_);
	float normal1[3]={0.0f, 0.0f, 0.0f};
	for (int n = 0; n < (int) numberOfNormals_; n++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetNormal(f, n, normal1);
			dctNX.function_[n][f-1] = normal1[0];
			dctNY.function_[n][f-1] = normal1[1];
			dctNZ.function_[n][f-1] = normal1[2];
		}
	}

	dctNX.DCT();
	dctNY.DCT();
	dctNZ.DCT();		
				
	SREnc.GetData(dctNX, dctNY, dctNZ);
	SREnc.SetNMaxLayers(params_.SNR_L_);
	SREnc.SetNLayers(params_.SNR_l_);

	return 0;
}


int FAMCEncoder::GetColorResidualErrorsDCT(ScalableResidualsEncoder &SREnc, Animation &anim) {
	DCTEncoder dctR(numberOfFrames_-1, numberOfColors_);
	DCTEncoder dctG(numberOfFrames_-1, numberOfColors_);
	DCTEncoder dctB(numberOfFrames_-1, numberOfColors_);
	float color1[3]={0.0f, 0.0f, 0.0f};
	for (int v = 0; v < (int) numberOfColors_; v++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetColor(f, v, color1);
			dctR.function_[v][f-1] = color1[0];
			dctG.function_[v][f-1] = color1[1];
			dctB.function_[v][f-1] = color1[2];
		}
	}
	dctR.DCT();
	dctG.DCT();
	dctB.DCT();		
				
	SREnc.GetData(dctR, dctG, dctB);
	SREnc.SetNMaxLayers(params_.SNR_L_);
	SREnc.SetNLayers(params_.SNR_l_);
	return 0;
}
int FAMCEncoder::GetCoordResidualErrorsLift(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel) {
	LiftCodec liftX(numberOfFrames_-1, numberOfVertices_);
	LiftCodec liftY(numberOfFrames_-1, numberOfVertices_);
	LiftCodec liftZ(numberOfFrames_-1, numberOfVertices_);
	float coord1[3] = {0.0f, 0.0f, 0.0f};
	for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
		int v = ifs.GetVerticesTraversal()[pos];
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetCoord(f, v, coord1);
			liftX.function_[pos][f-1] = coord1[0];
			liftY.function_[pos][f-1] = coord1[1];
			liftZ.function_[pos][f-1] = coord1[2];
		}
	}		
	SREnc.GetData(liftX, liftY, liftZ);
	SREnc.SetNMaxLayers(params_.SNR_L_);
	SREnc.SetNLayers(params_.SNR_l_);
	return 0;
}

int FAMCEncoder::GetNormalResidualErrorsLift(ScalableResidualsEncoder &SREnc, Animation &anim,  IndexedFaceSet & ifs, MotionModel * motionModel, int * pred) {
	LiftCodec liftNX(numberOfFrames_-1, numberOfNormals_);
	LiftCodec liftNY(numberOfFrames_-1, numberOfNormals_);
	LiftCodec liftNZ(numberOfFrames_-1, numberOfNormals_);
	float normal1[3]={0.0f, 0.0f, 0.0f};
	for (int n = 0; n < (int) numberOfNormals_; n++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetNormal(f, n, normal1);
			liftNX.function_[n][f-1] = normal1[0];
			liftNY.function_[n][f-1] = normal1[1];
			liftNZ.function_[n][f-1] = normal1[2];
		}
	}
	SREnc.GetData(liftNX, liftNY, liftNZ);
	SREnc.SetNMaxLayers(params_.SNR_L_);
	SREnc.SetNLayers(params_.SNR_l_);
	return 0;
}

int FAMCEncoder::GetColorResidualErrorsLift(ScalableResidualsEncoder &SREnc, Animation &anim) {
	LiftCodec liftR(numberOfFrames_-1, numberOfColors_);
	LiftCodec liftG(numberOfFrames_-1, numberOfColors_);
	LiftCodec liftB(numberOfFrames_-1, numberOfColors_);
	float color1[3]={0.0f, 0.0f, 0.0f};
	for (int v = 0; v < (int) numberOfColors_; v++) {
		for(int f = 1; f < (int) numberOfFrames_; f++){
			anim.GetColor(f, v, color1);
			liftR.function_[v][f-1] = color1[0];
			liftG.function_[v][f-1] = color1[1];
			liftB.function_[v][f-1] = color1[2];
		}
	}			
	SREnc.GetData(liftR, liftG, liftB);
	SREnc.SetNMaxLayers(params_.SNR_L_);
	SREnc.SetNLayers(params_.SNR_l_);
	return 0;
}
bool FAMCEncoder::EncodeAU(Animation &anim, IndexedFaceSet &ifs, FILE * outFinal, FILE * stat) {
	FILE * out = fopen("tmp.mp4", "wb");

	int headerSize = EncodeHeader(anim, ifs, out);
	int globalMotionSize = 0;


	MotionModel * motionModel = NULL;

	int keysSize = 0;
	int partitionSize = 0;
	int affineTransformSize = 0;
	int weightsSize = 0;
	int coordResidualErrorsSize = 0;
	int normalResidualErrorsSize = 0;
	int colorResidualErrorsSize = 0;
	if (isCoordAnimated_) {
		globalMotionSize = EncodeGolobalMotion(anim, out, 16);

		// we partition the dynamic mesh
		HierarchicalPartitionner hPartitionner;
		char fileNamePartition[1024];
		sprintf(fileNamePartition, "%s_partition.txt", params_.file_);
		if (params_.globalRMSE_!= -1.0) {
			hPartitionner.PartitionTrivial(&anim);
			hPartitionner.SavePartition(fileNamePartition);
		}
		else {
			hPartitionner.LoadPartition(fileNamePartition);
		}

		// we save the partition as VRML file (just for visualization)
		char pFileOW[1024];
		sprintf(pFileOW, fileNamePartition);
//		anim.SavePartitionnedFirstFrame(pFileOW, ifs, hPartitionner.GetPartition(), hPartitionner.GetNClusters());
		partitionSize = EncodePartition(hPartitionner.GetPartition(), hPartitionner.GetNClusters(), out);

		ifs.ComputeVerticesTraversal(hPartitionner.GetPartition());

		if ( hPartitionner.GetNClusters() == 1 ) {
			params_.ratioWeights_ = 1.0;
		}

		// We compute the motion model
		logger.write_2_log("\nCompute and encode motion model\n");
		logger.tic();
		motionModel = new MotionModel(hPartitionner.GetNClusters(), numberOfVertices_, numberOfFrames_);
		ComputeAffineTransforms(&anim, hPartitionner.GetPartition(), hPartitionner.GetNClusters(), *motionModel);
		affineTransformSize = CompressAffineTransformsPoints(&anim, hPartitionner.GetPartition(), *motionModel, params_.qAT_, out);
		ComputeWeights(&anim, ifs, hPartitionner.GetPartition(), hPartitionner.GetNClusters(), *motionModel);
		weightsSize = CompressWeights(anim, *motionModel, hPartitionner.GetPartition(), params_.ratioWeights_, 16, out);

		std::vector<float> errors;
		float errMM = 0.0;
		float errAM = 0.0;
		anim.ComputeErrorDistribution(*motionModel, errors, hPartitionner.GetPartition(), errMM, errAM);
		logger.write_2_log("\nWeights %f -> %f\n", errAM, errMM);
		logger.toc();
	}
	logger.tic();

	int * predNormal = NULL;
	if (isNormalAnimated_) {
		predNormal = new int[numberOfNormals_];
		normalResidualErrorsSize += CompressNormalPredictors(out, predNormal, anim, ifs,  motionModel);
	}

	// we apply skinning-based motion compensation
	if (isCoordAnimated_) {
		float coord0[3]={0.0f, 0.0f, 0.0f};
		float coordp[3]={0.0f, 0.0f, 0.0f};
		float coord1[3]={0.0f, 0.0f, 0.0f};
		for (int v = 0; v < (int) numberOfVertices_; v++) {
			for(int f = 1; f < (int) numberOfFrames_; f++){
				anim.GetCoord(0, v, coord0);
				anim.GetCoord(f, v, coord1);
				motionModel->Get(coord0, v, f, coordp);
				coord1[0] -= coordp[0];
				coord1[1] -= coordp[1];
				coord1[2] -= coordp[2];
				anim.SetCoord(f, v, coord1);
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

				// We normalize the two vectors
				MathTools::vectorUnitary(normal1);
				MathTools::vectorUnitary(normalp);

				normal1[0]-= normalp[0];
				normal1[1]-= normalp[1];
				normal1[2]-= normalp[2];
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
				color1[0]-=color0[0];
				color1[1]-=color0[1];
				color1[2]-=color0[2];
				anim.SetColor(f, v, color1);
			}
		}			
	}
	//------------------------------------

	ScalableResidualsEncoder SREncCoord(numberOfFrames_-1, numberOfVertices_, params_.qCoord_);
	ScalableResidualsEncoder SREncNormal(numberOfFrames_-1, numberOfNormals_, params_.qNormal_);
	ScalableResidualsEncoder SREncColor(numberOfFrames_-1, numberOfColors_, params_.qColor_);

	if (params_.transformType_==0) { // encode LIFTING-based
		if (isCoordAnimated_) {
			GetCoordResidualErrorsLift(SREncCoord, anim, ifs,  motionModel);
			coordResidualErrorsSize += SREncCoord.GenerateLayers(out);
		}
		if (isNormalAnimated_) {
			GetNormalResidualErrorsLift(SREncNormal, anim, ifs,  motionModel, predNormal);
			normalResidualErrorsSize += SREncNormal.GenerateLayers(out);
		}
		if (isColorAnimated_) {
			GetColorResidualErrorsLift(SREncColor, anim);
			colorResidualErrorsSize += SREncColor.GenerateLayers(out);
		}

		for (int layerSNR = 0; layerSNR < params_.SNR_l_; layerSNR++) {
			if (isCoordAnimated_) coordResidualErrorsSize += SREncCoord.EncodeLayer(layerSNR, out);
			if (isNormalAnimated_) normalResidualErrorsSize += SREncNormal.EncodeLayer(layerSNR, out);
			if (isColorAnimated_) colorResidualErrorsSize += SREncColor.EncodeLayer(layerSNR, out);
		}
	}

	if (params_.transformType_==1) { // encode DCT-based
		if (isCoordAnimated_) {
			GetCoordResidualErrorsDCT(SREncCoord, anim, ifs,  motionModel);
			coordResidualErrorsSize += SREncCoord.GenerateLayers(out);
		}
		if (isNormalAnimated_) {
			GetNormalResidualErrorsDCT(SREncNormal, anim, ifs,  motionModel, predNormal);
			normalResidualErrorsSize += SREncNormal.GenerateLayers(out);
		}
		if (isColorAnimated_) {
			GetColorResidualErrorsDCT(SREncColor, anim);
			colorResidualErrorsSize += SREncColor.GenerateLayers(out);
		}
		for (int layerSNR = 0; layerSNR < params_.SNR_l_; layerSNR++) {
			if (isCoordAnimated_) coordResidualErrorsSize += SREncCoord.EncodeLayer(layerSNR, out);
			if (isNormalAnimated_) normalResidualErrorsSize += SREncNormal.EncodeLayer(layerSNR, out);
			if (isColorAnimated_) colorResidualErrorsSize += SREncColor.EncodeLayer(layerSNR, out);
		}
	}

	
	if (params_.transformType_==3) { // apply LIFTING-transform to anim
		printf("\n<======== LIFTING-transform start ==========>\n");


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
			float maxX = 0.0f;
			float minX = 0.0f;
			float maxY = 0.0f;
			float minY = 0.0f;
			float maxZ = 0.0f;
			float minZ = 0.0f;
			unsigned char nBits = params_.qCoord_;
			liftX.Quantize2(nBits, maxX, minX);
			liftY.Quantize2(nBits, maxY, minY);
			liftZ.Quantize2(nBits, maxZ, minZ);

			AOF( 1 == fwrite(&nBits,  sizeof(unsigned char), 1, out ) );
			AOF( 1 == fwrite(&maxX, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&maxY, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&maxZ, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minX, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minY, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minZ, sizeof(float), 1, out ) );
			coordResidualErrorsSize += 25;

			liftX.LiftLS(true);
			liftY.LiftLS(true);
			liftZ.LiftLS(true);
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
			float maxX = 0.0f;
			float minX = 0.0f;
			float maxY = 0.0f;
			float minY = 0.0f;
			float maxZ = 0.0f;
			float minZ = 0.0f;
			unsigned char nBits = params_.qNormal_;
			liftNX.Quantize2(nBits, maxX, minX);
			liftNY.Quantize2(nBits, maxY, minY);
			liftNZ.Quantize2(nBits, maxZ, minZ);

			AOF( 1 == fwrite(&nBits,  sizeof(unsigned char), 1, out ) );
			AOF( 1 == fwrite(&maxX, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&maxY, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&maxZ, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minX, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minY, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minZ, sizeof(float), 1, out ) );
			normalResidualErrorsSize += 25;

			liftNX.LiftLS(true);
			liftNY.LiftLS(true);
			liftNZ.LiftLS(true);
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
			float maxR = 0.0f;
			float minR = 0.0f;
			float maxG = 0.0f;
			float minG = 0.0f;
			float maxB = 0.0f;
			float minB = 0.0f;
			unsigned char nBits = params_.qColor_;
			liftR.Quantize2(nBits, maxR, minR);
			liftG.Quantize2(nBits, maxG, minG);
			liftB.Quantize2(nBits, maxB, minB);

			AOF( 1 == fwrite(&nBits,  sizeof(unsigned char), 1, out ) );
			AOF( 1 == fwrite(&maxR, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&maxG, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&maxB, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minR, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minG, sizeof(float), 1, out ) );
			AOF( 1 == fwrite(&minB, sizeof(float), 1, out ) );
			colorResidualErrorsSize += 25;

			liftR.LiftLS(true);
			liftG.LiftLS(true);
			liftB.LiftLS(true);
			for (int pos = 0; pos < (int) numberOfColors_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					color1[0] = liftR.function_[pos][f-1];
					color1[1] = liftG.function_[pos][f-1];
					color1[2] = liftB.function_[pos][f-1];
					anim.SetColor(f, pos, color1);
				}
			}
		}
		printf("\n<======== LIFTING-transform end   ==========>\n");
	}

	if (params_.transformType_==4) { // apply DCT-transform to anim
		printf("\n<======== DCT-transform start ==========>\n");
		DCTEncoder dctX(numberOfFrames_-1, numberOfVertices_);
		DCTEncoder dctY(numberOfFrames_-1, numberOfVertices_);
		DCTEncoder dctZ(numberOfFrames_-1, numberOfVertices_);
		float coord1[3] = {0.0f, 0.0f, 0.0f};

		if (isCoordAnimated_) {	
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetCoord(f, pos, coord1);
					dctX.function_[pos][f-1] = coord1[0];
					dctY.function_[pos][f-1] = coord1[1];
					dctZ.function_[pos][f-1] = coord1[2];
				}
			}
			dctX.DCT();
			dctY.DCT();
			dctZ.DCT();		
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					coord1[0] = dctX.spectrum_[pos][f-1];
					coord1[1] = dctY.spectrum_[pos][f-1];
					coord1[2] = dctZ.spectrum_[pos][f-1];
					anim.SetCoord(f, pos, coord1);
				}
			}
		}

		if (isNormalAnimated_) {
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetNormal(f, pos, coord1);
					dctX.function_[pos][f-1] = coord1[0];
					dctY.function_[pos][f-1] = coord1[1];
					dctZ.function_[pos][f-1] = coord1[2];
				}
			}
			dctX.DCT();
			dctY.DCT();
			dctZ.DCT();		
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					coord1[0] = dctX.spectrum_[pos][f-1];
					coord1[1] = dctY.spectrum_[pos][f-1];
					coord1[2] = dctZ.spectrum_[pos][f-1];
					anim.SetNormal(f, pos, coord1);
				}
			}
		}

		if (isColorAnimated_) {
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					anim.GetColor(f, pos, coord1);
					dctX.function_[pos][f-1] = coord1[0];
					dctY.function_[pos][f-1] = coord1[1];
					dctZ.function_[pos][f-1] = coord1[2];
				}
			}
			dctX.DCT();
			dctY.DCT();
			dctZ.DCT();		
			for (int pos = 0; pos < (int) numberOfVertices_; pos++) {
				for(int f = 1; f < (int) numberOfFrames_; f++){
					coord1[0] = dctX.spectrum_[pos][f-1];
					coord1[1] = dctY.spectrum_[pos][f-1];
					coord1[2] = dctZ.spectrum_[pos][f-1];
					anim.SetColor(f, pos, coord1);
				}
			}
		}
		printf("\n<======== DCT-transform end   ==========>\n");
	}
		
	if (params_.transformType_==2 || 
		params_.transformType_==3 || 
		params_.transformType_==4) { // encode anim LD-based 		
		printf("\n<======== LD start ==========>\n");
		
		// init
		static int segmentNumber=0;
		static LayeredDecomposition ld;
		bool encodeLossless = (params_.transformType_==3);//integer based Lift+LD
		LDSegmentEncoder enc(anim, 1, anim.GetNKeyCoord()-1, isCoordAnimated_, isNormalAnimated_, isColorAnimated_, params_, encodeLossless);
		LDWriter writer(out);

		// create new layered decomposition (optionally) and encode segment header
		if (segmentNumber==0) {
			if (params_.LD_auto_==0){
				LDSegmentDecoder::createLayeredDecompositionFromConn(ifs, params_.LD_L_, ld);
				enc.encodeNewLDSegmentHeader(writer);
			}
			if (params_.LD_auto_==1){
				std::vector< std::vector<SimplificationOperation> > lsop(params_.LD_L_-1);
				LayeredSimplificationOperationsFactory::getSimplificationOperationsAutomaticOrderedByVertexIndex(lsop, ifs);
				LDSegmentDecoder::createLayeredDecompositionFromConnAndDecodedData(ifs, lsop, ld);
				enc.encodeNewLDSegmentHeader(ld, lsop, writer);
			}
		}
		else {
			enc.encodeOldLDSegmentHeader(writer);
		}

		// encode all frames
		enc.initLDDecomposers(&ld);
		enc.encodeLDAllFrames(writer);
		
		// add bit-sizes
		headerSize += enc.getSegmentHeaderNBytesEnc() + enc.getFrameHeaderNBytesEnc();
		coordResidualErrorsSize += enc.getCoordsNBytesEnc(); 
		normalResidualErrorsSize += enc.getNormalsNBytesEnc();
		colorResidualErrorsSize += enc.getColorsNBytesEnc();

		++segmentNumber;
		printf("<======== LD end ==========>\n\n");
	}
	logger.toc();

	// free memory
	if (motionModel != NULL) delete motionModel;
	if (predNormal != NULL) delete predNormal;

	// output performances
	int totalSize = headerSize + globalMotionSize + keysSize + partitionSize + affineTransformSize + weightsSize + coordResidualErrorsSize + normalResidualErrorsSize + colorResidualErrorsSize;
	float ratio = (8.0f * 25.0f) / (float)(1024 * (int)numberOfFrames_);

	logger.write_2_log("\n\nStream \t KBytes \t KBits/s \t Ptge \n");
	logger.write_2_log("------ \t ------ \t ------- \t -------- \n");
	logger.write_2_log("Head.  \t %3.2f     \t %3.2f   \t  %3.2f \n", headerSize/1024.0, (float) headerSize * ratio, 100.0 * headerSize / totalSize);
	logger.write_2_log("G.M.   \t %3.2f     \t %3.2f   \t  %3.2f \n", globalMotionSize/1024.0, (float) globalMotionSize * ratio, 100.0 * globalMotionSize / totalSize);
	logger.write_2_log("Keys   \t %3.2f     \t %3.2f   \t  %3.2f \n", keysSize/1024.0, (float) keysSize * ratio, 100.0 * keysSize / totalSize);
	logger.write_2_log("Part.  \t %3.2f     \t %3.2f   \t  %3.2f \n", partitionSize/1024.0, (float) partitionSize * ratio, 100.0 * partitionSize / totalSize);
	logger.write_2_log("A.T.   \t %3.2f     \t %3.2f   \t  %3.2f \n", affineTransformSize/1024.0, (float) affineTransformSize * ratio, 100.0 * affineTransformSize / totalSize);
	logger.write_2_log("Weights\t %3.2f     \t %3.2f   \t  %3.2f \n", weightsSize/1024.0, (float) weightsSize * ratio, 100.0 * weightsSize / totalSize);
	logger.write_2_log("Coord. \t %3.2f     \t %3.2f   \t  %3.2f \n", coordResidualErrorsSize/1024.0, (float) coordResidualErrorsSize * ratio, 100.0 * coordResidualErrorsSize / totalSize);
	logger.write_2_log("Normal \t %3.2f     \t %3.2f   \t  %3.2f \n", normalResidualErrorsSize/1024.0, (float) normalResidualErrorsSize * ratio, 100.0 * normalResidualErrorsSize / totalSize);
	logger.write_2_log("Color  \t %3.2f     \t %3.2f   \t  %3.2f \n", colorResidualErrorsSize/1024.0, (float) colorResidualErrorsSize * ratio, 100.0 * colorResidualErrorsSize / totalSize);
	logger.write_2_log("------ \t ------ \t ------- \t -------- \n");
	logger.write_2_log("Total  \t %3.2f     \t %3.2f   \t  %3.2f \n", totalSize/1024.0, (float) totalSize * ratio, 100.0 * totalSize / totalSize);

	// write bitrate to stats
	fprintf(stat, "%3.2f\t%3.2f\n", (float) (totalSize-normalResidualErrorsSize) * ratio, (float) (normalResidualErrorsSize) * ratio);

	// we compute the segmentAnimationSize
	unsigned int animationSegmentSize = ftell(out);
	// We write the Segment stream to the file
	fclose(out);
	out = fopen("tmp.mp4", "rb");
	unsigned int tmp = 0;
	fread(&tmp, sizeof(unsigned int), 1, out);
	fwrite(&tmp, sizeof(unsigned int), 1, outFinal);
	fread(&tmp, sizeof(unsigned int), 1, out);
	fwrite(&animationSegmentSize, sizeof(unsigned int), 1, outFinal);
	unsigned int bufferSize = animationSegmentSize - 2 * sizeof(unsigned int);
	unsigned char *  buffer= new unsigned char [bufferSize];
	fread(buffer, sizeof(unsigned char), bufferSize, out);
	fwrite(buffer, sizeof(unsigned char), bufferSize, outFinal);
	delete [] buffer;
	fclose(out);
	//--------------------------------------------

	return true;
}
bool FAMCEncoder::EncodeStream(char * sFile) {

	char fin[1024];
	sprintf(fin, "%s.wrl", params_.file_);
	char fout[1024];
	sprintf(fout, "%s.mp4", params_.file_);

	IndexedFaceSet myIFS;
	myIFS.LoadIFSVRML2(fin);

	Animation myAnim;
	myAnim.LoadInterpolatorsVRML2(fin);

	// we save the first frame
	char fFirstFrame[1024];
	sprintf(fFirstFrame, "%sFF.wrl", params_.file_);
	myIFS.SaveIFSVRML2(fFirstFrame);


	FILE * stat = fopen(sFile, "a");
	FILE * out = fopen(fout, "wb");

	int numberOfFrames = myAnim.GetNKeyCoord();
	int nbrOfFramesPerSegment = params_.sizeAnimSeg_;
	if ( params_.sizeAnimSeg_ == -1) {
		nbrOfFramesPerSegment = numberOfFrames+1;
	}
	int nbrSegments = numberOfFrames/nbrOfFramesPerSegment;
	int sizeLastSegment = numberOfFrames%nbrOfFramesPerSegment;
	for (int s = 0; s < nbrSegments; s++) {
		Animation animSeg;
		logger.write_2_log("AnimationSegment(%i)->(FF, %i->%i )\n", s, s*nbrOfFramesPerSegment, (s+1)*nbrOfFramesPerSegment -1);
		myAnim.GetAnimationSegment(animSeg, myIFS, s*nbrOfFramesPerSegment, (s+1)*nbrOfFramesPerSegment -1);
		EncodeAU(animSeg, myIFS, out, stat);
	}
	if ( sizeLastSegment != 0) {
		Animation animSeg;
		logger.write_2_log("AnimationSegment(%i)->(FF, %i->%i )\n", nbrSegments, nbrSegments*nbrOfFramesPerSegment, numberOfFrames-1);
		myAnim.GetAnimationSegment(animSeg, myIFS, nbrSegments*nbrOfFramesPerSegment, numberOfFrames-1);
		EncodeAU(animSeg, myIFS, out, stat);
	}

	fclose(out);
	fclose(stat);
	return true;
}

bool FAMCEncoder::EncodeStream( Animation &myAnim ,IndexedFaceSet &myIFS,const char * outFile,const  char * statFile) {

	// we save the first frame
	char fFirstFrame[1024];
	sprintf(fFirstFrame, "%sFF.wrl", outFile);
	myIFS.SaveIFSVRML2(fFirstFrame);

	FILE * stat = fopen(statFile, "a");

	std::string out_file_name = std::string(outFile) + std::string(".mp4");

	FILE * out = fopen(out_file_name .c_str(), "wb");

	int numberOfFrames = myAnim.GetNKeyCoord();
	int nbrOfFramesPerSegment = params_.sizeAnimSeg_;
	if ( params_.sizeAnimSeg_ == -1) {
		nbrOfFramesPerSegment = numberOfFrames+1;
	}
	int nbrSegments = numberOfFrames/nbrOfFramesPerSegment;
	int sizeLastSegment = numberOfFrames%nbrOfFramesPerSegment;
	for (int s = 0; s < nbrSegments; s++) {
		Animation animSeg;
		logger.write_2_log("AnimationSegment(%i)->(FF, %i->%i )\n", s, s*nbrOfFramesPerSegment, (s+1)*nbrOfFramesPerSegment -1);
		myAnim.GetAnimationSegment(animSeg, myIFS, s*nbrOfFramesPerSegment, (s+1)*nbrOfFramesPerSegment -1);
		EncodeAU(animSeg, myIFS, out, stat);
	}
	if ( sizeLastSegment != 0) {
		Animation animSeg;
		logger.write_2_log("AnimationSegment(%i)->(FF, %i->%i )\n", nbrSegments, nbrSegments*nbrOfFramesPerSegment, numberOfFrames-1);
		myAnim.GetAnimationSegment(animSeg, myIFS, nbrSegments*nbrOfFramesPerSegment, numberOfFrames-1);
		EncodeAU(animSeg, myIFS, out, stat);
	}

	fclose(out);
	fclose(stat);
	return true;
}