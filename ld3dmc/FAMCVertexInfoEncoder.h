/*
 * =====================================================================================
 * 
 *        Filename:  FAMCVertexInfoEncoder.h
 * 
 *     Description:  A cabac based encoder for a partirion
 * 
 *         Version:  1.0
 *         Created:  10/04/07 17:15:18 CEST
 *        Revision:  none
 *        Compiler:  gcc
 * 
 *          Author:  Nikolce Stefanoski (nst)
 *         Company:  Institut für Informationsverarbeitung
 *           Email:  stefanos@tnt.uni-hannover.de
 * 
 * =====================================================================================
 */

#ifndef FAMC_VERTEX_INFO_ENCODER_H
#define FAMC_VERTEX_INFO_ENCODER_H

#include <vector>

class FAMCVertexInfoEncoder {
	unsigned char* buffer_;

public:
	FAMCVertexInfoEncoder(unsigned char* _buffer);
	int encode (int _nC, const std::vector<int>& _partition);
};

#endif //FAMC_VERTEX_INFO_ENCODER_H