/*!
 ************************************************************************
 *  \file
 *     Console.cpp
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

#include "Console.h"
#include <stdio.h>
#include <stdarg.h>

CConsole::CConsole(const char* filename)
{
	file_log = fopen(filename, "w");
}

void CConsole::ChangeFileName(const char* filename) {
	if (file_log != NULL) {
		fclose(file_log);
		file_log = fopen(filename, "w");
	}

}

CConsole::~CConsole()
{
  write_2_log("");
  write_2_log("END OF LOG");

  if (file_log) 
	  fclose(file_log);
}


void CConsole::write_2_log(char *fmt, ...)
{
  if (!file_log) 
	  return;

  va_list	argptr;
  char		str[1024*4];	// 4 Kb should be enougth

  // re-build the string
  va_start(argptr,fmt);
  vsprintf(str,fmt,argptr);
  va_end(argptr);

  fprintf(file_log, "%s", str);
  printf("%s", str);
  fflush(file_log);
}

void CConsole::tic() {
	    start_ = clock();
}

void CConsole::toc() {
	finish_ = clock();
	write_2_log("%f s\n", ((double)(finish_ - start_))/CLOCKS_PER_SEC);
}
