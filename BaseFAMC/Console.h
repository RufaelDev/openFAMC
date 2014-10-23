/*!
 ************************************************************************
 *  \file
 *     Console.h
 *  \brief
 *     CConsole class. Outputs data to log file.
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
#include <stdlib.h>
#include <time.h>

class CConsole  
{
		FILE	*file_log;
		clock_t start_;
		clock_t finish_;
	public:
		void tic();
		void toc();
		void ChangeFileName(const char* filename);
		CConsole(const char* filename);
		virtual ~CConsole();
		void write_2_log(char *fmt, ...);
		inline 	void error(char * msg) { write_2_log("%s", msg); exit(1);};
};
