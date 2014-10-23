
#include <String.h>
#include "Console.h"
#include "IndexedFaceSet.h"
#include "Animation.h"
#include "FAMCDecoder.h"

CConsole logger("C:\\log.txt");
int main(int argc, char* argv[])
{
	printf("Usage:\n");
	printf("FAMCDecoder inputFileName numberOfSpatialLayers\n\n");

	if (argc != 3) {
		printf("Error: we need 2 arguments! (see usage)\n");
		return -1;
	}

	
	//(char * fileName, double globalRMSE, int nAT, int nDCT, float ptg)
	char file[1024] = "";

	strcpy(file, argv[1]);

	int numberOfSpatialLayers = atoi(argv[2]);


	char fileLog[1024];

	sprintf(fileLog, "%s_Decoding_log.txt", file);
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

	FAMCDecoder decoder;
	decoder.DecodeStream(file, numberOfSpatialLayers);

	return 0;
}