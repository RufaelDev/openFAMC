/*!
 ************************************************************************
 *  \file
 *     FileIO.h
 *  \brief
 *     FileIO class. I/O.
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
#include "Console.h"
#define FILEIO_MAX_BUFFER 10000

class FileIO
{
	FILE * file_;
	char fileName_[1024];
	char mode_[4];
	char * word_;
	char line_[FILEIO_MAX_BUFFER];
	int lineNbr_;
	int wordNbr_;
	char sep_[30];
public:
	bool ReadLine();
	bool GetWordAll(char * word);
	bool GetWord(char * word);
	bool Goto(char * target, char * word);
	bool GotoEOL(char * word);
	bool GetFloat(char * word, float & f);
	bool GetInt(char * word, int & i);

	FileIO(char * fileName, char * mode);
	inline bool IsOpned() { return file_ != NULL; };
public:
	~FileIO(void);
};
