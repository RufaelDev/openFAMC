/*!
 ************************************************************************
 *  \file
 *     FileIO.cpp
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
#include "FileIO.h"
#include <string.h>
#include <stdlib.h>
extern CConsole logger;

FileIO::FileIO(char * fileName, char * mode)
{
	strcpy(sep_, " ,[]{}\t");
	strcpy(fileName_, fileName);
	strcpy(mode_, mode);
	file_ = fopen(fileName_, mode);
	word_ = NULL;
	lineNbr_ = 0;
	wordNbr_ = 0;
	if (file_) {
		logger.write_2_log("File %s opened (mode = %s)\n", fileName_, mode_);
	}
	else {
		logger.write_2_log("Can't open file %s (mode = %s)\n", fileName_, mode_);
	}
}

FileIO::~FileIO(void)
{
	if ( file_ != NULL) {
		fclose(file_);
	}
}

bool FileIO::ReadLine() {
		if (!feof(file_)) {	// we read a line
			fgets(line_ , FILEIO_MAX_BUFFER, file_);
			char *p = line_;
			while(*p) {
				if (*p == 10) {
					*p = ' ';
				}
				p++;
			}
//			logger.write_2_log("(line %i) \t %s\n", lineNbr_, line_);
			lineNbr_++;
			wordNbr_ = 0;
			return true;
		}
		else {
			return false;
		}

}
bool FileIO::GetWord(char * word) {
	if (wordNbr_ == 0) {
		word_ = strtok (line_, sep_);
	}
	else {

		if (word_ == NULL) {		// if we don't have nothing in the buffer
			return false;
		}
		word_ = strtok (NULL, sep_);
	}

	if (word_) {
		wordNbr_++;
		strcpy(word, word_);
		return true;
	}
	else {
		return false;
	}

}

bool FileIO::GetWordAll(char * word) {
	while ( (!GetWord(word)) )  {
		if (!ReadLine()) {
			return false;
		}
	}

	return true;
}
bool FileIO::Goto(char * target, char * word) {
	if (GetWordAll(word)) {
		while (strcmp(target, word)) {
			if (!GetWordAll(word)) {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool FileIO::GotoEOL(char * word) {
	while (GetWord(word));
	return true;
}

bool FileIO::GetFloat(char * word, float & f) {
	return sscanf(word, "%f", &f) != 0;
}

bool FileIO::GetInt(char * word, int & i) {
	return sscanf(word, "%i", &i) != 0;
}