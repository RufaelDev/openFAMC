#ifndef NO_FAMC_STANDALONE_BUILD
#include <String.h>
#include "Console.h"
#include "IndexedFaceSet.h"
#include "Animation.h"
#include "FAMCEncoder.h"
#include "FAMCDecoder.h"
#include "FAMCParams.h"

#include "FAMCVertexInfoEncoder.h"
#include "FAMCVertexInfoDecoder.h"
#include "FAMCSimplificationModeEncoder.h"
#include "FAMCSimplificationModeDecoder.h"

CConsole logger("C:\\log.txt");
int main(int argc, char* argv[])
{
	printf("Usage:\n");
	printf("FAMCEncoder inputFileName sizeAnimSeg transform globalRMSE ratioWeights qAT qCoord qNormal qColor qOther LD_L LD_l LD_P LD_B LD_Pred SNR_L SNR_l Normal_Pred statFile\n\n");

	printf(" inputFileName: animation in VRML 2.0 with Interpolators.\n");
	printf(" sizeAnimSeg:   Size of each animation segment (e.g., sizeAnimSeg = 65).\n");
	printf(" transform:     0 = DCT, 1 = lifting, 2 = LD, 3 = lifting+LD, 4 = DCT + LD.\n");
	printf(" globalRMSE:   globalRMSE = if -1, the partition will be read from the file _partition.txt Otherwise, a single cluster will be considered.\n");
	printf(" ratioWeights:  controles the use of animation weights in [0, 1].\n");
	printf(" qAT:           quantization bits for dct-based affine transform encoding.\n");
	printf(" qCoord:        quantization bits for coordinates residuals.\n");
	printf(" qNormal:       quantization bits for normals residuals.\n");
	printf(" qColor:        quantization bits for colors residuals.\n");
	printf(" qOther:        quantization bits for other attributes residuals.\n");
	printf(" LD_auto:       specifies if simplification operations should be encoded (0=false, 1=true).\n");
	printf(" LD_L:          number of decomposition layers for LD in {1,...,16}.\n");
	printf(" LD_l:          number of encoded decomposition layers for LD in {1,...,LD_L}.\n");
	printf(" LD_P:          number of P frames between two consecutive I frames\n");
	printf("                in {-1,0,...,number of frames in the animation segment - 2}.\n");
	printf("                if (LD_P = -1) then all frames except the first one are P or B.\n");
	printf(" LD_B:          number of B frames between two frames of type I or P.\n");
	printf(" LD_Pred:       predction mode for LD.\n");
	printf(" SNR_L:         number of layers for DCT/lifting.\n");
	printf(" SNR_l:         number of encoded layers for DCT/lifting in {1,...,SNR_L}.\n");
	printf(" Normal_Pred:   predction mode for normals:\n");
	printf("                0=delta, 1=skinning, 2=tangent skinning, 3=adaptive.\n");
	printf(" statFile:      file to output the stream size.\n\n\n");

	if (argc != 21) {
		printf("Error: we need 20 arguments! (see usage)\n");
		return -1;
	}

	
	//(char * fileName, double globalRMSE, int nAT, int nDCT, float ptg)
	FAMCParams famcParam;
	char sfile[1024] = "";

	strcpy(famcParam.file_, argv[1]);
	famcParam.sizeAnimSeg_ = atoi(argv[2]);
	famcParam.transformType_ = atoi(argv[3]);
	famcParam.globalRMSE_ = (float) atof(argv[4]);
	famcParam.ratioWeights_ = (float) atof(argv[5]);
	famcParam.qAT_ = atoi(argv[6]);
	famcParam.qCoord_ = atoi(argv[7]);
	famcParam.qNormal_ = atoi(argv[8]);
	famcParam.qColor_ = atoi(argv[9]);
	famcParam.qOther_ = atoi(argv[10]);
	famcParam.LD_auto_ = atoi(argv[11]);
	famcParam.LD_L_ = atoi(argv[12]);
	famcParam.LD_l_ = atoi(argv[13]);
	famcParam.LD_P_ = atoi(argv[14]);
	famcParam.LD_B_ = atoi(argv[15]);
	famcParam.LD_Pred_ = atoi(argv[16]);
	famcParam.SNR_L_ = atoi(argv[17]);
	famcParam.SNR_l_ = atoi(argv[18]);
	famcParam.Normal_Pred_ = atoi(argv[19]);
	strcpy(sfile, argv[20]);
	


	char fileLog[1024];
	sprintf(fileLog, "%s_Encoding_log.txt", famcParam.file_);
	logger.ChangeFileName(fileLog);

	logger.write_2_log("This software was developped by:\n");
	logger.write_2_log(" - Khaled MAMMOU from the ARTEMIS Department/INT.\n");
	logger.write_2_log("   Khaled.Mamou@int-evry.fr\n");
	logger.write_2_log("   http://www-artemis.int-evry.fr/\n\n");
	logger.write_2_log(" - Nikolce STEFANOSKI from the TNT Department/ University of Hannover.\n");
	logger.write_2_log("   stefanos@tnt.uni-hannover.de\n");
	logger.write_2_log("   http://www.tnt.uni-hannover.de/\n\n");
	logger.write_2_log("   http://iphome.hhi.de/muller/\n\n");
	logger.write_2_log(" - Heiner KIRCHHOFFER from HHI.\n");
	logger.write_2_log("   Heiner.Kirchhoffer@hhi.fraunhofer.de\n\n");
	logger.write_2_log(" - Detlev MARPE from HHI.\n");
	logger.write_2_log("   marpe@hhi.fraunhofer.de\n");
	logger.write_2_log("   http://iphome.hhi.de/marpe/\n\n");
	logger.write_2_log(" - Karsten MÜLLER from HHI.\n");
	logger.write_2_log("   kmuller@hhi.de\n");
	logger.write_2_log("   http://iphome.hhi.de/muller/\n\n");

	logger.write_2_log("sizeAnimSeg\t\t%i\n", famcParam.sizeAnimSeg_);
	logger.write_2_log("transformType\t\t%i\n", famcParam.transformType_);
	logger.write_2_log("globalRMSE\t%f\n", famcParam.globalRMSE_);
	logger.write_2_log("ratioWeights\t%f\n", famcParam.ratioWeights_);
	logger.write_2_log("qAT\t\t%i\n", famcParam.qAT_);
	logger.write_2_log("qCoord\t\t%i\n", famcParam.qCoord_);
	logger.write_2_log("qNormal\t\t%i\n", famcParam.qNormal_);
	logger.write_2_log("qColor\t\t%i\n", famcParam.qColor_);
	logger.write_2_log("qOther\t\t%i\n", famcParam.qOther_);
	logger.write_2_log("LD_auto\t\t%i\n", famcParam.LD_auto_);
	logger.write_2_log("LD_L\t\t%i\n", famcParam.LD_L_);
	logger.write_2_log("LD_l\t\t%i\n", famcParam.LD_l_);
	logger.write_2_log("LD_P\t\t%i\n", famcParam.LD_P_);
	logger.write_2_log("LD_B\t\t%i\n", famcParam.LD_B_);
	logger.write_2_log("LD_Pred\t\t%i\n", famcParam.LD_Pred_);
	logger.write_2_log("SNR_L\t\t%i\n", famcParam.SNR_L_);
	logger.write_2_log("SNR_l\t\t%i\n", famcParam.SNR_l_);
	logger.write_2_log("Normal_Pred\t%i\n", famcParam.Normal_Pred_);
	logger.write_2_log("sizeFile\t%s\n", sfile);


	//------------
	FAMCEncoder encoder(famcParam);
	encoder.EncodeStreamFromDir(sfile);
	//------------
	sprintf(fileLog, "%s_Decoding_log.txt", famcParam.file_);
	logger.ChangeFileName(fileLog);

	FAMCDecoder decoder;
	decoder.DecodeStream(famcParam.file_, famcParam.LD_l_);

	return 0;
}
#endif