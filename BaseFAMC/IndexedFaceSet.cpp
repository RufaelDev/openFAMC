/*!
 ************************************************************************
 *  \file
 *     IndexedFaceSet.cpp
 *  \brief
 *     IndexedFaceSet class. It supports only triangulair meshes.
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
#include <stdio.h>
#include <math.h>
#include "IndexedFaceSet.h"
#include "Console.h"
#include "FileIO.h"
#include <vector>

extern CConsole logger;

IndexedFaceSet::IndexedFaceSet(void)
{
	coord_ = NULL;			
	color_ = NULL;
	normal_ = NULL;
	texCoord_ = NULL;

	nCoord_ = 0;
	nColor_ = 0;
	nNormal_ = 0;
	nTexCoord_ = 0;
	
	ccw_ = true;
	solid_ = true;
	convex_ = true;
	colorPerVertex_ = true;
	normalPerVertex_ = true;

	creaseAngle_ = 0.0;

	coordIndex_ = NULL;
	colorIndex_ = NULL;
	normalIndex_ = NULL;
	texCoordIndex_ = NULL;

	nCoordIndex_ = 0;
	nColorIndex_ = 0;
	nNormalIndex_ = 0;
	nTexCoordIndex_ = 0;
	traversal_ = NULL;
}
IndexedFaceSet::~IndexedFaceSet(void)
{
	if (traversal_ != NULL) delete [] traversal_;
	FreeMem();
}



bool IndexedFaceSet::GetCoord(int vertex, float * coord) {
	if (vertex < nCoord_) {
		for(int h = 0; h < 3; h++) {
			coord[h] = coord_[vertex*3+h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetCoord(int vertex, float * coord) {
	if (vertex < nCoord_) {
		for(int h = 0; h < 3; h++) {
			coord_[vertex*3+h] = coord[h];
		}
		return true;
	}
	return false;
}

bool IndexedFaceSet::GetNormal(int pos, float * normal) {
	if (pos < nNormal_) {
		for(int h = 0; h < 3; h++) {
			normal[h] = normal_[pos*3+h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetNormal(int pos, float * normal) {
	if (pos < nNormal_) {
		for(int h = 0; h < 3; h++) {
			normal_[pos*3+h] = normal[h];
		}
		return true;
	}
	return false;
}

bool IndexedFaceSet::GetColor(int pos, float * color) {
	if (pos < nColor_) {
		for(int h = 0; h < 3; h++) {
			color[h] = color_[pos*3+h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetColor(int pos, float * color) {
	if (pos < nColor_) {
		for(int h = 0; h < 3; h++) {
			color_[pos*3+h] = color[h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::GetTexCoord(int pos, float * texCoord) {
	if (pos < nTexCoord_) {
		for(int h = 0; h < 2; h++) {
			texCoord[h] = texCoord_[pos*2+h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetTexCoord(int pos, float * texCoord) {
	if (pos < nTexCoord_) {
		for(int h = 0; h < 2; h++) {
			texCoord_[pos*2+h] = texCoord[h];
		}
		return true;
	}
	return false;
}

bool IndexedFaceSet::GetCoordIndex(int pos, int * coordIndex) {
	if (pos < nCoordIndex_) {
		for(int h = 0; h < 3; h++) {
			coordIndex[h] = coordIndex_[pos][h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetCoordIndex(int pos, int * coordIndex) {
	if (pos < nCoordIndex_) {
		for(int h = 0; h < 3; h++) {
			coordIndex_[pos][h] = coordIndex[h];
		}
		return true;
	}
	return false;
}

bool IndexedFaceSet::GetNormalIndex(int pos, int * normalIndex) {
	int dim = 1;
	if (GetNormalPerVertex()) dim = 3;	

	if (pos < nNormalIndex_) {
		for(int h = 0; h < dim; h++) {
			normalIndex[h] = normalIndex_[pos][h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetNormalIndex(int pos, int * normalIndex) {
	int dim = 1;
	if (GetNormalPerVertex()) dim = 3;	

	if (pos < nNormalIndex_) {
		for(int h = 0; h < dim; h++) {
			normalIndex_[pos][h] = normalIndex[h];
		}
		return true;
	}
	return false;
}

bool IndexedFaceSet::GetColorIndex(int pos, int * colorIndex) {
	int dim = 1;
	if (GetColorPerVertex()) dim = 3;
	if (pos < nColorIndex_) {
		for(int h = 0; h < dim; h++) {
			colorIndex[h] = colorIndex_[pos][h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetColorIndex(int pos, int * colorIndex) {
	int dim = 1;
	if (GetColorPerVertex()) dim = 3;
	if (pos < nColorIndex_) {
		for(int h = 0; h < dim; h++) {
			colorIndex_[pos][h] = colorIndex[h];
		}
		return true;
	}
	return false;
}

bool IndexedFaceSet::GetTexCoordIndex(int pos, int * texCoordIndex) {
	if (pos < nTexCoordIndex_) {
		for(int h = 0; h < 3; h++) {
			texCoordIndex[h] = texCoordIndex_[pos][h];
		}
		return true;
	}
	return false;
}
bool IndexedFaceSet::SetTexCoordIndex(int pos, int * texCoordIndex) {
	if (pos < nTexCoordIndex_) {
		for(int h = 0; h < 3; h++) {
			texCoordIndex_[pos][h] = texCoordIndex[h];
		}
		return true;
	}
	return false;
}

void IndexedFaceSet::FreeMem()
{
	if (coord_) delete [] coord_;
	if (color_) delete [] color_;
	if (normal_) delete [] normal_;
	if (texCoord_) delete [] texCoord_;
	nCoord_ = 0;
	nColor_ = 0;
	nNormal_ = 0;
	nTexCoord_ = 0;

	ccw_ = true;
	solid_ = true;
	convex_ = true;
	colorPerVertex_ = true;
	normalPerVertex_ = true;

	creaseAngle_ = 0.0;

	FreeInt(coordIndex_, nCoordIndex_);
	FreeInt(normalIndex_, nNormalIndex_);
	FreeInt(colorIndex_, nColorIndex_);
	FreeInt(texCoordIndex_, nTexCoordIndex_);

	nCoordIndex_ = 0;
	nColorIndex_ = 0;
	nNormalIndex_ = 0;
	nTexCoordIndex_ = 0;

}
void IndexedFaceSet::FreeInt(int ** tab, int dim)
{
	if (tab) {
		for (int i = 0; i < dim; i++) {
			delete [] tab[i];
		}
		delete [] tab;
	}
}

bool IndexedFaceSet::LoadIFSVRML2(char * fileName) {
	FreeMem();

	std::vector<float> coord;
	std::vector<float> color;
	std::vector<float> normal;
	std::vector<float> texCoord;

	std::vector<int> coordIndex;
	std::vector<int> colorIndex;
	std::vector<int> normalIndex;
	std::vector<int> texCoordIndex;

	FileIO vrml(fileName, "r");
	if (!vrml.IsOpned()) return false;

	logger.write_2_log("Parsing ");
	logger.tic();

	char word[1024];
	int t = 0;
	vrml.ReadLine();

	// we search for the first IndexedFaceSet object
	bool foundIFS  = vrml.Goto("IndexedFaceSet", word);
	if (!foundIFS) return false;
	int notEOF = 1;
	int readWord = 1;
	while(notEOF) {
		readWord = 1;

		//---------------------
		if (!strcmp(word, "creaseAngle")) {
			vrml.GetWordAll(word);
			vrml.GetFloat(word,creaseAngle_);
		}
		//---------------------
		if (!strcmp(word, "normalPerVertex")) {
			vrml.GetWordAll(word);
			if (!strcmp(word, "FALSE")) {
				normalPerVertex_ = false;
			}
			else {
				normalPerVertex_ = true;
			}
		}
		//---------------------
		if (!strcmp(word, "colorPerVertex")) {
			vrml.GetWordAll(word);
			if (!strcmp(word, "FALSE")) {
				colorPerVertex_ = false;
			}
			else {
				colorPerVertex_ = true;
			}
		}
		//---------------------
		if (!strcmp(word, "convex")) {
			vrml.GetWordAll(word);
			if (!strcmp(word, "FALSE")) {
				convex_ = false;
			}
			else {
				convex_ = true;
			}
		}
		//---------------------
		if (!strcmp(word, "ccw")) {
			vrml.GetWordAll(word);
			if (!strcmp(word, "FALSE")) {
				ccw_ = false;
			}
			else {
				ccw_ = true;
			}
		}
		//---------------------
		if (!strcmp(word, "solid")) {
			vrml.GetWordAll(word);
			if (!strcmp(word, "FALSE")) {
				solid_ = false;
			}
			else {
				solid_ = true;
			}
		}
		//---------------------
		// we get the normals
		if (!strcmp(word, "Normal")) {
			vrml.Goto("vector", word);			
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					float f = 0.0f;
					bool isFloat = vrml.GetFloat(word,f);
					if (isFloat) {
						normal.push_back(f);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}
		//---------------------
		// we get the colors
		if (!strcmp(word, "Color")) {
			vrml.Goto("color", word);			
			while (	vrml.GetWordAll(word) ) {
			logger.write_2_log("%s\n", word);
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					float f = 0.0f;
					bool isFloat = vrml.GetFloat(word,f);
					if (isFloat) {
						color.push_back(f);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}
		//---------------------
		// we get the coordinates
		if (!strcmp(word, "Coordinate")) {
			vrml.Goto("point", word);			
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					float f = 0.0f;
					bool isFloat = vrml.GetFloat(word,f);
					if (isFloat) {
						coord.push_back(f);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}
		//-------------------------
		// we get the texture coordinates
		if (!strcmp(word, "TextureCoordinate")) {
			vrml.Goto("point", word);			
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					float f = 0.0f;
					bool isFloat = vrml.GetFloat(word,f);
					if (isFloat) {
						texCoord.push_back(f);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}
		//-------------------------
		// we get the coordIndex
		if (!strcmp(word, "coordIndex")) {
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					int i = -1;
					bool isInt = vrml.GetInt(word,i);
					if (isInt) {
						coordIndex.push_back(i);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}
		//-------------------------
		// we get the texCoordIndex
		if (!strcmp(word, "texCoordIndex")) {
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					int i = -1;
					bool isInt = vrml.GetInt(word,i);
					if (isInt) {
						texCoordIndex.push_back(i);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}
		//-------------------------
		// we get the normalIndex
		if (!strcmp(word, "normalIndex")) {
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					int i = -1;
					bool isInt = vrml.GetInt(word,i);
					if (isInt) {
						normalIndex.push_back(i);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}

		//-------------------------
		// we get the colorIndex
		if (!strcmp(word, "colorIndex")) {
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					int i = -1;
					bool isInt = vrml.GetInt(word,i);
					if (isInt) {
						colorIndex.push_back(i);
					}
					else {
						readWord = 0;
						break;
					}
				}
			}
		}

		if (readWord) {
			notEOF = vrml.GetWordAll(word);
		}
	}


	logger.toc();

	logger.write_2_log("Filling the data structures\n");

	nCoord_ = (int) coord.size()/3;
	nCoordIndex_ = (int) coordIndex.size()/4;
	nNormal_ = (int) normal.size()/3;
	if (normalPerVertex_) {
		nNormalIndex_ = (int) normalIndex.size()/4;
	}
	else {
		nNormalIndex_ = (int) normalIndex.size();
	}
	nColor_ = (int) color.size()/3;
	if (colorPerVertex_) {
		nColorIndex_ = (int) colorIndex.size()/4;
	}
	else {
		nColorIndex_ = (int) colorIndex.size();
	}
	nTexCoord_ = (int) texCoord.size()/2;
	nTexCoordIndex_ = (int) texCoordIndex.size()/4;

	// coord
	logger.write_2_log("Filling coord\t");
	logger.tic();
	coord_ = new float [3 * nCoord_];
	for (int v = 0; v < 3 * nCoord_; v++) {
		coord_[v] = coord[v];
	}
	logger.toc();
	
	// normal
	logger.write_2_log("Filling normal\t");
	logger.tic();
	normal_ = new float [3 * nNormal_];
	for (int v = 0; v < 3 * nNormal_; v++) {
		normal_[v] = normal[v];
	}
	logger.toc();

	// color
	logger.write_2_log("Filling color\t");
	logger.tic();
	color_ = new float [3 * nColor_];
	for (int v = 0; v < 3 * nColor_; v++) {
		color_[v] = color[v];
	}
	logger.toc();
	// texCoord
	logger.write_2_log("Filling texCoord\t");
	logger.tic();
	texCoord_ = new float [2 * nTexCoord_];
	for (int v = 0; v < 2 * nTexCoord_; v++) {
		texCoord_[v] = texCoord[v];
	}
	logger.toc();

	// fill coordIndex
	logger.write_2_log("Filling coordIndex\t");
	logger.tic();
	coordIndex_ = new int * [nCoordIndex_];
	int k=0;
	for (int p = 0; p < nCoordIndex_; p++) {
		coordIndex_[p] = new int[3];
		for (int h = 0; h <3; h++) {
			coordIndex_[p][h] = coordIndex[k];k++;
		}
		k++;
	}	
	logger.toc();

	// fill normalIndex
	logger.write_2_log("Filling normalIndex\t");
	logger.tic();
	normalIndex_ = new int * [nNormalIndex_];
	
	if (normalPerVertex_){
		k=0;
		for (int p = 0; p < nNormalIndex_; p++) {
			normalIndex_[p] = new int[3];
			for (int h = 0; h <3; h++) {
				normalIndex_[p][h] = normalIndex[k];k++;
			}
			k++;
		}
	}
	else {
		for (int p = 0; p < nNormalIndex_; p++) {
			normalIndex_[p] = new int[1];
			normalIndex_[p][0] = normalIndex[p];
		}	
	}
	logger.toc();

	// fill colorIndex
	logger.write_2_log("Filling colorIndex\t");
	logger.tic();
	colorIndex_ = new int * [nColorIndex_];
	if (colorPerVertex_){
		k=0;
		for (int p = 0; p < nColorIndex_; p++) {
			colorIndex_[p] = new int[3];
			for (int h = 0; h <3; h++) {
				colorIndex_[p][h] = colorIndex[k];k++;
			}
			k++;
		}	
	}
	else {
		for (int p = 0; p < nColorIndex_; p++) {
			colorIndex_[p] = new int[1];
			colorIndex_[p][0] = colorIndex[p];
		}
	}
	logger.toc();
	
	// fill texCoordIndex
	logger.write_2_log("Filling texCoordIndex\t");
	logger.tic();
	texCoordIndex_ = new int * [nTexCoordIndex_];
	k=0;
	for (int p = 0; p < nTexCoordIndex_; p++) {
		texCoordIndex_[p] = new int[3];
		for (int h = 0; h <3; h++) {
			texCoordIndex_[p][h] = texCoordIndex[k];k++;
		}
		k++;
	}	
	logger.toc();
	logger.write_2_log("Filling done!\n");
	return true;
}

void IndexedFaceSet::AddNeighborVertex2Vertex(int v1, int v2)
{
	int found = 0;	
	for (IntVect::iterator posc = vertex2Vertex_[v1].begin(); posc != vertex2Vertex_[v1].end(); ++posc) {
		if ((*posc) == v2) {
			found = 1;
			break;
		}
	}
	if (found == 0)
		vertex2Vertex_[v1].push_back(v2);
}

void IndexedFaceSet::ComputeVertex2Vertex(){
	for (int f = 0; f < nCoordIndex_; f++) {
		AddNeighborVertex2Vertex(coordIndex_[f][0], coordIndex_[f][1]);
		AddNeighborVertex2Vertex(coordIndex_[f][1], coordIndex_[f][2]);
		AddNeighborVertex2Vertex(coordIndex_[f][2], coordIndex_[f][0]);
	}
}

bool IndexedFaceSet::SaveIFSVRML2(char * fileName) {

		FILE * fid = fopen(fileName, "w");
		logger.write_2_log("Saving %s\t", fileName);
		logger.tic();

		if (fid) {
			int p = 0;
			int i = 0;
			char tmpbufTime[128];
			char tmpbufDate[128];
			_strtime( tmpbufTime );
			_strdate( tmpbufDate );
			fprintf(fid,"#VRML V2.0 utf8\n");	    	
			fprintf(fid,"# Produced by Khaled MAMOU, khaled.mamou@int-evry.fr. Copyright %s %s.\n", tmpbufTime, tmpbufDate);
			fprintf(fid,"\n");
			fprintf(fid,"Group {\n");
			fprintf(fid,"	children [\n");
			fprintf(fid,"		Shape {\n");
			fprintf(fid,"			appearance Appearance {\n");
			fprintf(fid,"				material Material {\n");
			fprintf(fid,"					diffuseColor 0.6 0.4 0.6\n");  
			fprintf(fid,"					ambientIntensity 0.3333\n");
			fprintf(fid,"					specularColor 0.8 0.8 0.8\n");
			fprintf(fid,"					shininess 0.411\n");
			fprintf(fid,"					transparency 0\n");
			fprintf(fid,"				}\n");
			fprintf(fid,"			}\n");
			fprintf(fid,"			geometry IndexedFaceSet {\n");
			if (GetCCW()) {
				fprintf(fid,"				ccw TRUE\n");
			}
			else {
				fprintf(fid,"				ccw FALSE\n");
			}
			if (GetSolid()) {
				fprintf(fid,"				solid TRUE\n");
			}
			else {
				fprintf(fid,"				solid FALSE\n");
			}
			if (GetConvex()) {
				fprintf(fid,"				convex TRUE\n");
			}
			else {
				fprintf(fid,"				convex FALSE\n");
			}
			if (GetColorPerVertex()) {
				fprintf(fid,"				colorPerVertex TRUE\n");
			}
			else {
				fprintf(fid,"				colorPerVertex FALSE\n");
			}
			if (GetNormalPerVertex()) {
				fprintf(fid,"				normalPerVertex TRUE\n");
			}
			else {
				fprintf(fid,"				normalPerVertex FALSE\n");
			}
			if (GetNCoord() > 0) {
				fprintf(fid,"				coord DEF co Coordinate {\n");
				fprintf(fid,"					point [\n");
				float coord[3];
				for (p = 0; p < GetNCoord() ; p++) {
					GetCoord(p, coord);
					fprintf(fid,"						%f %f %f,\n", coord[0], coord[1], coord[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (GetNCoordIndex() > 0) {
				fprintf(fid,"				coordIndex [ \n");
				int face[3];
				for (p = 0; p < (int) GetNCoordIndex(); p++) {
					GetCoordIndex(p, face);
					fprintf(fid,"						%i, %i, %i, -1,\n", face[0], face[1], face[2]);
				}	
				fprintf(fid,"				]\n");
			}
			if (GetNTexCoord() > 0) {
				fprintf(fid,"				texCoord DEF co TextureCoordinate {\n");
				fprintf(fid,"					point [\n");
				float TexCoord[2];
				for (p = 0; p < GetNTexCoord() ; p++) {
					GetTexCoord(p, TexCoord);
					fprintf(fid,"						%f %f,\n", TexCoord[0], TexCoord[1]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (GetNTexCoordIndex() > 0) {
				fprintf(fid,"				texCoordIndex [ \n");
				int face[3];
				for (p = 0; p < (int) GetNTexCoordIndex(); p++) {
					GetTexCoordIndex(p, face);
					fprintf(fid,"						%i, %i, %i, -1,\n", face[0], face[1], face[2]);
				}	
				fprintf(fid,"				]\n");
			}

			if (GetNNormal() > 0) {
				fprintf(fid,"				normal DEF no Normal {\n");
				fprintf(fid,"					vector [\n");
				float Normal[3];
				for (p = 0; p < GetNNormal() ; p++) {
					GetNormal(p, Normal);
					fprintf(fid,"						%f %f %f,\n", Normal[0], Normal[1], Normal[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (GetNNormalIndex() > 0) {
				fprintf(fid,"				normalIndex [ \n");
				int face[3];
				for (p = 0; p < (int) GetNNormalIndex(); p++) {
					GetCoordIndex(p, face);
					if ( GetNormalPerVertex()) {
						fprintf(fid,"						%i, %i, %i, -1,\n", face[0], face[1], face[2]);
					}
					else {
						fprintf(fid,"						%i,\n", face[0]);
					}
				}	
				fprintf(fid,"				]\n");
			}
			if (GetNColor() > 0) {
				fprintf(fid,"				color DEF cro Color {\n");
				fprintf(fid,"					vector [\n");
				float Color[3];
				for (p = 0; p < GetNColor() ; p++) {
					GetColor(p, Color);
					fprintf(fid,"						%f %f %f,\n", Color[0], Color[1], Color[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (GetNColorIndex() > 0) {
				fprintf(fid,"				colorIndex [ \n");
				int face[3];
				for (p = 0; p < (int) GetNColorIndex(); p++) {
					GetCoordIndex(p, face);
					if ( GetColorPerVertex()) {
						fprintf(fid,"						%i, %i, %i, -1,\n", face[0], face[1], face[2]);
					}
					else {
						fprintf(fid,"						%i,\n", face[0]);
					}
				}	
				fprintf(fid,"				]\n");
			}

			fprintf(fid,"			}\n");
			fprintf(fid,"		}\n");
			fprintf(fid,"	]\n");
			fprintf(fid,"}\n");	
			fclose(fid);
			logger.toc();
		}
		else {
			logger.error( "Saving error: file can't be created \n");
		}
	return true;
}

bool IndexedFaceSet::AllocateMem(){
	// coord
	if (nCoord_>0) {
		coord_ = new float [3 * nCoord_];
	}
	if (nNormal_>0) {
		normal_ = new float [3 * nNormal_];
	}
	if (nColor_>0) {
		color_ = new float [3 * nColor_];
	}
	if ( nTexCoord_>0) {
		texCoord_ = new float [2 * nTexCoord_];
	}
	if (nCoordIndex_ >0) {
		coordIndex_ = new int * [nCoordIndex_];
		for (int p = 0; p < nCoordIndex_; p++) {
			coordIndex_[p] = new int[3];
		}
	}	
	if (nNormalIndex_>0) {
		if (normalPerVertex_){
			for (int p = 0; p < nNormalIndex_; p++) {
				normalIndex_[p] = new int[3];
			}
		}
		else {
			for (int p = 0; p < nNormalIndex_; p++) {
				normalIndex_[p] = new int[1];
			}	
		}
	}
	if (nColorIndex_>0) {
		if (colorPerVertex_){
			for (int p = 0; p < nColorIndex_; p++) {
				colorIndex_[p] = new int[3];
			}
		}
		else {
			for (int p = 0; p < nColorIndex_; p++) {
				colorIndex_[p] = new int[1];
			}	
		}
	}
	if ( nTexCoordIndex_>0) {
		texCoordIndex_ = new int * [nTexCoordIndex_];
		for (int p = 0; p < nTexCoordIndex_; p++) {
			texCoordIndex_[p] = new int[3];
		}
	}	
	return true;
}
void IndexedFaceSet::ComputeVerticesTraversal(std::vector<int> * partition) {
	int V = GetNCoord();
	if (traversal_!=NULL) delete [] traversal_;
		// Compute vertices traversal
	bool * isVisited = new bool[V];
	traversal_ = new int[V];
	ComputeVertex2Vertex();
	
	for (int v = 0; v < V; v++) {
		isVisited[v] = false;
	}
	int pos = 0;
	for (int v = 0; v < V; v++) {
		if ( !isVisited[v]) {
			int cluster = (*partition)[v];
			std::vector<int> myFIFO;
			myFIFO.push_back(v);
			int currentVertex = -1;
			int a = -1;
			isVisited[v] = true;
			while(myFIFO.size() != 0) {
				currentVertex = myFIFO[0];
				myFIFO.erase(myFIFO.begin());
				traversal_[pos++] = currentVertex;
				for (int n = 0; n < (int)vertex2Vertex_[currentVertex].size();n++) {
					a = vertex2Vertex_[currentVertex][n];
					if (((*partition)[a] == cluster) && (!isVisited[a])) {
						myFIFO.push_back(a);
						isVisited[a] = true;
					}
				}
			}
		}
	}
	delete [] isVisited;
}