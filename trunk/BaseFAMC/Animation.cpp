/*!
 ************************************************************************
 *  \file
 *     Animation.cpp
 *  \brief
 *     Animation class.
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
#include "Animation.h"
#include <stdio.h>
#include "Console.h"
#include "FileIO.h"
#include <vector>
#include <math.h>
extern CConsole logger;

Animation::Animation(void)
{
	coord_ = NULL;			
	normal_ = NULL;
	color_ = NULL;
	attributes_ = NULL;
	coordKey_ = NULL;			
	normalKey_ = NULL;
	colorKey_ = NULL;
	attributesKey_ = NULL;
	nCoord_ = 0;	
	nNormal_ = 0;
	nColor_ = 0;	
	nAttributes_ = 0;
	nKeyCoord_ = 0;	
	nKeyNormal_ = 0;
	nKeyColor_ = 0;	
	nKeyAttributes_ = 0;
	duration_ = 1.0;
}

Animation::~Animation(void)
{
	FreeMem();
}

bool Animation::GetCoord(int frame, int vertex, float * coord)  {
	if ((frame < nKeyCoord_) && (vertex < nCoord_)) {
		for(int h = 0; h < 3; h++) {
			coord[h] = coord_[frame][vertex*3+h];
		}
		return true;
	}
	return false;
}
bool Animation::SetCoord(int frame, int vertex, float * coord)  {
	if ((frame < nKeyCoord_) && (vertex < nCoord_)) {
		for(int h = 0; h < 3; h++) {
			coord_[frame][vertex*3+h] = coord[h];
		}
		return true;
	}
	return false;
}
bool Animation::GetColor(int frame, int vertex, float * color){
	if ((frame < nKeyColor_) && (vertex < nColor_)) {
		for(int h = 0; h < 3; h++) {
			color[h] = color_[frame][vertex*3+h];
		}
		return true;
	}
	return false;
}
bool Animation::SetColor(int frame, int vertex, float * color){
	if ((frame < nKeyColor_) && (vertex < nColor_)) {
		for(int h = 0; h < 3; h++) {
			color_[frame][vertex*3+h]= color[h];
		}
		return true;
	}
	return false;
}
bool Animation::GetNormal(int frame, int vertex, float * normal){
	if ((frame < nKeyNormal_) && (vertex < nNormal_)) {
		for(int h = 0; h < 3; h++) {
			normal[h] = normal_[frame][vertex*3+h];
		}
		return true;
	}
	return false;
}
bool Animation::SetNormal(int frame, int vertex, float * normal){
	if ((frame < nKeyNormal_) && (vertex < nNormal_)) {
		for(int h = 0; h < 3; h++) {
			normal_[frame][vertex*3+h] = normal[h];
		}
		return true;
	}
	return false;
}

void Animation::FreeMem()
{
	if (coordKey_) delete [] coordKey_;
	if (colorKey_) delete [] colorKey_;
	if (normalKey_) delete [] normalKey_;
	if (attributesKey_) delete [] attributesKey_;
	

	FreeFloat(coord_, nKeyCoord_);
	FreeFloat(normal_, nKeyNormal_);
	FreeFloat(color_, nKeyColor_);
	FreeFloat(attributes_, nKeyAttributes_);
	nCoord_ = 0;
	nColor_ = 0;
	nNormal_ = 0;
	nAttributes_ = 0;
	nKeyCoord_ = 0;	
	nKeyNormal_ = 0;
	nKeyColor_ = 0;	
	nKeyAttributes_ = 0;
}
void Animation::FreeFloat(float ** tab, int dim)
{
	if (tab) {
		for (int i = 0; i < dim; i++) {
			delete [] tab[i];
		}
		delete [] tab;
	}
}

void Animation::AllocateMem(){
	// we allocate coord
	if ( nKeyCoord_ > 0) {
		coord_ = new float * [nKeyCoord_];
		coordKey_ = new float [nKeyCoord_];
		for (int i = 0; i < nKeyCoord_; i++) {
			coord_[i] = new float [3*nCoord_];
		}
	}
	// we allocate normal
	if ( nKeyNormal_ > 0) {
		normal_ = new float * [nKeyNormal_];
		normalKey_ = new float [nKeyNormal_];
		for (int i = 0; i < nKeyNormal_; i++) {
			normal_[i] = new float [3*nNormal_];
		}
	}
	if ( nKeyColor_ > 0) {
		color_ = new float * [nKeyColor_];
		colorKey_ = new float [nKeyColor_];
		for (int i = 0; i < nKeyColor_; i++) {
			color_[i] = new float [3*nColor_];
		}
	}
	if ( nKeyAttributes_ > 0) {
		attributes_ = new float * [nKeyAttributes_];
		attributesKey_ = new float [nKeyAttributes_];
		for (int i = 0; i < nKeyAttributes_; i++) {
			attributes_[i] = new float [3*nAttributes_];
		}
	}

}
bool Animation::LoadInterpolatorsVRML2(char * fileName) {
	FreeMem();

	std::vector<float> coord;
	std::vector<float> color;
	std::vector<float> normal;
	std::vector<float> attributes;
	std::vector<float> coordKey;
	std::vector<float> colorKey;
	std::vector<float> normalKey;
	std::vector<float> attributesKey;

	FileIO vrml(fileName, "r");
	if (!vrml.IsOpned()) return false;

	logger.write_2_log("Parsing ");
	logger.tic();

	char word[1024];
	int t = 0;
	vrml.ReadLine();

	int notEOF = 1;
	int readWord = 1;
	while(notEOF) {
		readWord = 1;

		// we get the coordinate interpolations
		if (!strcmp(word, "CoordinateInterpolator")) {
			readWord = 0;
			vrml.Goto("key", word);			
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					float f = 0.0f;
					bool isFloat = vrml.GetFloat(word,f);
					if (isFloat) {
						coordKey.push_back(f);
					}
					else {
						break;
					}
				}
			}
			if (strcmp(word, "keyValue")) vrml.Goto("keyValue", word);
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
						break;
					}
				}
			}
		}
		// we get the coordinate interpolations
		if (!strcmp(word, "NormalInterpolator")) {
			readWord = 0;
			vrml.Goto("key", word);			
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					float f = 0.0f;
					bool isFloat = vrml.GetFloat(word,f);
					if (isFloat) {
						normalKey.push_back(f);
					}
					else {
						break;
					}
				}
			}
			if (strcmp(word, "keyValue")) vrml.Goto("keyValue", word);
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
						break;
					}
				}
			}
		}
		// we get the coordinate interpolations
		if (!strcmp(word, "ColorInterpolator")) {
			readWord = 0;
			vrml.Goto("key", word);			
			while (	vrml.GetWordAll(word) ) {
				if (word[0] == '#') {// we skip commentaries
					vrml.GotoEOL(word);
				}
				else {
					float f = 0.0f;
					bool isFloat = vrml.GetFloat(word,f);
					if (isFloat) {
						colorKey.push_back(f);
					}
					else {
						break;
					}
				}
			}
			if (strcmp(word, "keyValue")) vrml.Goto("keyValue", word);
			while (	vrml.GetWordAll(word) ) {
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
						break;
					}
				}
			}
		}

		// we get the animation duration
		if (!strcmp(word, "TimeSensor")) {
			readWord = 0;
			vrml.Goto("cycleInterval", word);
			float f = 0.0f;
			do {
				vrml.GetWordAll(word);
			}while (!vrml.GetFloat(word,f));
			duration_ = (double)f;
		}

		if (readWord) {
			notEOF = vrml.GetWordAll(word);
		}
	}


	logger.toc();

	logger.write_2_log("Filling the data structures\n");

	nKeyCoord_ = (int) coordKey.size();
	nKeyColor_ = (int) colorKey.size();
	nKeyNormal_ = (int) normalKey.size();
	nKeyAttributes_ = (int) attributes.size();

	if (nKeyCoord_ != 0) nCoord_ = (int) coord.size()/(3 * nKeyCoord_);
	if (nKeyNormal_ != 0) nNormal_ = (int) normal.size()/(3 * nKeyNormal_);
	if (nKeyColor_ != 0) nColor_ = (int) color.size()/(3 * nKeyColor_);
	if (nKeyAttributes_ != 0) nAttributes_ =	(int) attributes.size()/nKeyAttributes_;

	// we fill coord
	if ( nKeyCoord_ > 0) {
		logger.write_2_log("Filling coord\t");
		logger.tic();
		coord_ = new float * [nKeyCoord_];
		coordKey_ = new float [nKeyCoord_];
		int k=0;
		int i=0;
		for (i = 0; i < nKeyCoord_; i++) {
			coord_[i] = new float [3*nCoord_];
			coordKey_[i] = coordKey[i];
			for (int p = 0; p < nCoord_; p++) {
				for(int h = 0; h < 3; h++) {
					coord_[i][p*3+h] = coord[k];
					k++;
				}
			}
		}
		logger.toc();
	}
	// we fill normal
	if ( nKeyNormal_ > 0) {
		logger.write_2_log("Filling normal\t");
		logger.tic();
		normal_ = new float * [nKeyNormal_];
		normalKey_ = new float [nKeyNormal_];
		int k=0;
		int i=0;
		for (i = 0; i < nKeyNormal_; i++) {
			normal_[i] = new float [3*nNormal_];
			normalKey_[i] = normalKey[i];
			for (int p = 0; p < nNormal_; p++) {
				for(int h = 0; h < 3; h++) {
					normal_[i][p*3+h] = normal[k];
					k++;
				}
			}
		}
		logger.toc();
	}
	if ( nKeyColor_ > 0) {
		logger.write_2_log("Filling color\t");
		logger.tic();
		color_ = new float * [nKeyColor_];
		colorKey_ = new float [nKeyColor_];
		int k=0;
		int i=0;
		for (i = 0; i < nKeyColor_; i++) {
			color_[i] = new float [3*nColor_];
			colorKey_[i] = colorKey[i];
			for (int p = 0; p < nColor_; p++) {
				for(int h = 0; h < 3; h++) {
					color_[i][p*3+h] = color[k];
					k++;
				}
			}
		}
		logger.toc();
	}
	if ( nKeyAttributes_ > 0) {
		logger.write_2_log("Filling attributes\t");
		logger.tic();
		attributes_ = new float * [nKeyAttributes_];
		attributesKey_ = new float [nKeyAttributes_];
		int k=0;
		int i=0;
		for (i = 0; i < nKeyAttributes_; i++) {
			attributes_[i] = new float [3*nAttributes_];
			attributesKey_[i] = attributesKey[i];
			for (int p = 0; p < nAttributes_; p++) {
				for(int h = 0; h < 3; h++) {
					attributes_[i][p*3+h] = attributes[k];
					k++;
				}
			}
		}
		logger.toc();
	}
	logger.write_2_log("Filling done!\n");

	return true;
}

bool Animation::SaveInterpolatorVRML2(char * fileName, IndexedFaceSet & ifs) {

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
			fprintf(fid,"# NCoord =%i , NNormal = %i, NColor = %i, NTexCoord = %i\n",
				ifs.GetNCoord(), ifs.GetNNormal(), ifs.GetNColor(), ifs.GetNTexCoord());
			fprintf(fid,"# NCoordIndex =%i , NNormalIndex = %i, NColorIndex = %i, NTexCoordIndex = %i\n",
				ifs.GetNCoordIndex(), ifs.GetNNormalIndex(), ifs.GetNColorIndex(), ifs.GetNTexCoordIndex());
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
			fprintf(fid,"				texture ImageTexture {\n");
			fprintf(fid,"					url \"texture.jpg\"\n");
			fprintf(fid,"				}\n");
			fprintf(fid,"			}\n");
			fprintf(fid,"			geometry IndexedFaceSet {\n");
			if ( !ifs.GetCCW() ) {
				fprintf(fid,"				ccw FALSE\n");
			}
			if ( !ifs.GetSolid() ) {
				fprintf(fid,"				solid FALSE\n");
			}
			if ( !ifs.GetConvex() ) {
				fprintf(fid,"				convex FALSE\n");
			}
			if ( !ifs.GetColorPerVertex() ) {
				fprintf(fid,"				colorPerVertex FALSE\n");
			}
			if ( !ifs.GetNormalPerVertex() ) {
				fprintf(fid,"				normalPerVertex FALSE\n");
			}

			if (ifs.GetNCoord() > 0) {
				fprintf(fid,"				coord DEF co Coordinate {\n");
				fprintf(fid,"					point [\n");
				float coord[3];
				for (p = 0; p < ifs.GetNCoord() ; p++) {
					ifs.GetCoord(p, coord);
					fprintf(fid,"						%f %f %f,\n", coord[0], coord[1], coord[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNCoordIndex() > 0) {
				fprintf(fid,"				coordIndex [ \n");
				int coordIndex[3];
				for (p = 0; p < ifs.GetNCoordIndex(); p++) {
					ifs.GetCoordIndex(p, coordIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", coordIndex[0], coordIndex[1], coordIndex[2]);
				}	
				fprintf(fid,"				]\n");
			}
			if (ifs.GetNTexCoord()) {
				fprintf(fid,"				texCoord TextureCoordinate {\n");
				fprintf(fid,"					point [\n");
				float texCoord[2];
				for (p = 0; p < ifs.GetNTexCoord() ; p++) {
					ifs.GetTexCoord(p, texCoord);
					fprintf(fid,"						%f %f,\n", texCoord[0], texCoord[1]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNTexCoordIndex()>0) {
				fprintf(fid,"				texCoordIndex [ \n");
				int texCoordIndex[3];
				for (p = 0; p < ifs.GetNTexCoordIndex(); p++) {
					ifs.GetTexCoordIndex(p, texCoordIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", texCoordIndex[0], texCoordIndex[1], texCoordIndex[2]);
				}	
				fprintf(fid,"				]\n");
			}
			if (ifs.GetNNormal()> 0) {
				fprintf(fid,"				normal DEF no Normal {\n");
				fprintf(fid,"					vector [\n");
				float normal[3];
				for (p = 0; p < ifs.GetNNormal() ; p++) {
					ifs.GetNormal(p, normal);
					fprintf(fid,"						%f %f %f,\n", normal[0], normal[1], normal[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNNormalIndex() > 0) {
				fprintf(fid,"				normalIndex [ \n");
				int normalIndex[3];
				for (p = 0; p < ifs.GetNNormalIndex(); p++) {
					ifs.GetNormalIndex(p, normalIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", normalIndex[0], normalIndex[1], normalIndex[2]);
				}	
				fprintf(fid,"				]\n");
			}
			if (ifs.GetNColor()> 0) {
				fprintf(fid,"				color DEF cro Color {\n");
				fprintf(fid,"					color [\n");
				float color[3];
				for (p = 0; p < ifs.GetNColor() ; p++) {
					ifs.GetColor(p, color);
					fprintf(fid,"						%f %f %f,\n", color[0], color[1], color[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNColorIndex() > 0) {
				fprintf(fid,"				colorIndex [ \n");
				int colorIndex[3];
				for (p = 0; p < ifs.GetNColorIndex(); p++) {
					ifs.GetNormalIndex(p, colorIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", colorIndex[0], colorIndex[1], colorIndex[2]);
				}	
				fprintf(fid,"				]\n");
			}

			fprintf(fid,"			}\n");
			fprintf(fid,"		}\n");
			if ( nCoord_ > 0) {
				fprintf(fid,"		DEF ci CoordinateInterpolator {\n");
				fprintf(fid,"			key [ ");
				for (i = 0; i < nKeyCoord_; i++) {
					fprintf(fid,"%f\n", coordKey_[i]);
				}
				fprintf(fid,"			]\n");

				fprintf(fid,"			keyValue [\n");
				for (i = 0; i < nKeyCoord_; i++) {
					for	(p = 0; p < nCoord_ ; p++) {
						fprintf(fid,"				%f %f %f,\n", coord_[i][p*3], coord_[i][p*3+1], coord_[i][p*3+2]);
					}
				}
				fprintf(fid,"			]\n");
				fprintf(fid,"		}\n");
			}

			if ( nNormal_ > 0) {
				fprintf(fid,"		DEF ni NormalInterpolator {\n");
				fprintf(fid,"			key [ ");
				for (i = 0; i < nKeyNormal_; i++) {
					fprintf(fid,"%f\n", normalKey_[i]);
				}
				fprintf(fid,"			]\n");

				fprintf(fid,"			keyValue [\n");
				for (i = 0; i < nKeyNormal_; i++) {
					for	(p = 0; p < nNormal_ ; p++) {
						fprintf(fid,"				%f %f %f,\n", normal_[i][p*3], normal_[i][p*3+1], normal_[i][p*3+2]);
					}
				}
				fprintf(fid,"			]\n");
				fprintf(fid,"		}\n");
			}

			if ( nColor_ > 0) {
				fprintf(fid,"		DEF cri ColorInterpolator {\n");
				fprintf(fid,"			key [ ");
				for (i = 0; i < nKeyColor_; i++) {
					fprintf(fid,"%f ", colorKey_[i]);
				}
				fprintf(fid,"]\n");

				fprintf(fid,"			keyValue [\n");
				for (i = 0; i < nKeyColor_; i++) {
					for	(p = 0; p < nColor_ ; p++) {
						fprintf(fid,"				%f %f %f,\n", color_[i][p*3], color_[i][p*3+1], color_[i][p*3+2]);
					}
				}
				fprintf(fid,"			]\n");
				fprintf(fid,"		}\n");
			}

			fprintf(fid,"		DEF ts TimeSensor {\n");
			fprintf(fid,"			cycleInterval %f\n", duration_);
			fprintf(fid,"			loop TRUE\n");
			fprintf(fid,"		}\n");
			fprintf(fid,"	]\n");
			fprintf(fid,"}\n");	
			if (nCoord_> 0 ) {
				fprintf(fid,"ROUTE ts.fraction_changed TO ci.set_fraction\n");
				fprintf(fid,"ROUTE ci.value_changed TO co.point\n");
			}

			if (nNormal_> 0 ) {
				fprintf(fid,"ROUTE ts.fraction_changed TO ni.set_fraction\n");
				fprintf(fid,"ROUTE ni.value_changed TO no.vector\n");
			}

			if (nColor_> 0 ) {
				fprintf(fid,"ROUTE ts.fraction_changed TO cri.set_fraction\n");
				fprintf(fid,"ROUTE cri.value_changed TO cro.vector\n");
			}

			fclose(fid);
			logger.toc();
		}
		else {
			logger.error( "Saving error: file can't be created \n");
		}
	return true;
}
int Animation::SaveFrame(char * fileName, IndexedFaceSet & ifs, int frame, float ** color) {
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
			fprintf(fid,"# NCoord =%i , NNormal = %i, NColor = %i, NTexCoord = %i\n",
				ifs.GetNCoord(), ifs.GetNNormal(), ifs.GetNColor(), ifs.GetNTexCoord());
			fprintf(fid,"# NCoordIndex =%i , NNormalIndex = %i, NColorIndex = %i, NTexCoordIndex = %i\n",
				ifs.GetNCoordIndex(), ifs.GetNNormalIndex(), ifs.GetNColorIndex(), ifs.GetNTexCoordIndex());
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
			fprintf(fid,"				texture ImageTexture {\n");
			fprintf(fid,"					url \"texture.jpg\"\n");
			fprintf(fid,"				}\n");
			fprintf(fid,"			}\n");
			fprintf(fid,"			geometry IndexedFaceSet {\n");
			if ( !ifs.GetCCW() ) {
				fprintf(fid,"				ccw FALSE\n");
			}
			if ( !ifs.GetSolid() ) {
				fprintf(fid,"				solid FALSE\n");
			}
			if ( !ifs.GetConvex() ) {
				fprintf(fid,"				convex FALSE\n");
			}
			if ( (!ifs.GetColorPerVertex()) && (color == NULL) ) {
				fprintf(fid,"				colorPerVertex FALSE\n");
			}
			if ( !ifs.GetNormalPerVertex() ) {
				fprintf(fid,"				normalPerVertex FALSE\n");
			}

			if (ifs.GetNCoord() > 0) {
				fprintf(fid,"				coord DEF co Coordinate {\n");
				fprintf(fid,"					point [\n");
				float coord[3];
				for (p = 0; p < ifs.GetNCoord() ; p++) {
					GetCoord(frame, p, coord);
					fprintf(fid,"						%f %f %f,\n", coord[0], coord[1], coord[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNCoordIndex() > 0) {
				fprintf(fid,"				coordIndex [ \n");
				int coordIndex[3];
				for (p = 0; p < ifs.GetNCoordIndex(); p++) {
					ifs.GetCoordIndex(p, coordIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", coordIndex[0], coordIndex[1], coordIndex[2]);
				}	
				fprintf(fid,"				]\n");
			}
			if (ifs.GetNTexCoord()) {
				fprintf(fid,"				texCoord TextureCoordinate {\n");
				fprintf(fid,"					point [\n");
				float texCoord[2];
				for (p = 0; p < ifs.GetNTexCoord() ; p++) {
					ifs.GetTexCoord(p, texCoord);
					fprintf(fid,"						%f %f,\n", texCoord[0], texCoord[1]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNTexCoordIndex()>0) {
				fprintf(fid,"				texCoordIndex [ \n");
				int texCoordIndex[3];
				for (p = 0; p < ifs.GetNTexCoordIndex(); p++) {
					ifs.GetTexCoordIndex(p, texCoordIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", texCoordIndex[0], texCoordIndex[1], texCoordIndex[2]);
				}	
				fprintf(fid,"				]\n");
			}
			if (ifs.GetNNormal()> 0) {
				fprintf(fid,"				normal DEF no Normal {\n");
				fprintf(fid,"					vector [\n");
				float normal[3];
				for (p = 0; p < ifs.GetNNormal() ; p++) {
					GetNormal(frame, p, normal);
					fprintf(fid,"						%f %f %f,\n", normal[0], normal[1], normal[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNNormalIndex() > 0) {
				fprintf(fid,"				normalIndex [ \n");
				int normalIndex[3];
				for (p = 0; p < ifs.GetNNormalIndex(); p++) {
					ifs.GetNormalIndex(p, normalIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", normalIndex[0], normalIndex[1], normalIndex[2]);
				}	
				fprintf(fid,"				]\n");
			}
			if ((ifs.GetNColor()> 0) && (color == NULL)) {
				fprintf(fid,"				color DEF cro Color {\n");
				fprintf(fid,"					color [\n");
				float color[3];
				for (p = 0; p < ifs.GetNColor() ; p++) {
					GetColor(frame, p, color);
					fprintf(fid,"						%f %f %f,\n", color[0], color[1], color[2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (color != NULL) {
				fprintf(fid,"				color DEF cro Color {\n");
				fprintf(fid,"					color [\n");
				for (p = 0; p < ifs.GetNCoord() ; p++) {				
					fprintf(fid,"						%f %f %f,\n", color[p][0], color[p][1], color[p][2]);
				}
				fprintf(fid,"					]\n");
				fprintf(fid,"				}\n");
			}
			if (ifs.GetNColorIndex() > 0) {
				fprintf(fid,"				colorIndex [ \n");
				int colorIndex[3];
				for (p = 0; p < ifs.GetNColorIndex(); p++) {
					ifs.GetNormalIndex(p, colorIndex);
					fprintf(fid,"						%i, %i, %i, -1, \n", colorIndex[0], colorIndex[1], colorIndex[2]);
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
int Animation::SavePartitionnedFirstFrame(char * file, IndexedFaceSet & ifs, std::vector<int> * partition, int k) {

	int n = k / 6 +1 ;
	float pas = 1.0f / n;

	std::vector<float> r;
	std::vector<float> g;
	std::vector<float> b;

	float min = 0.1f;
	int p = 0;
	while (p<k) {
		r.push_back(pas); g.push_back(min); b.push_back(min);
		r.push_back(min); g.push_back(pas); b.push_back(min);
		r.push_back(min); g.push_back(min); b.push_back(pas);
		r.push_back(pas); g.push_back(pas); b.push_back(min);
		r.push_back(pas); g.push_back(min); b.push_back(pas);
		r.push_back(min); g.push_back(pas); b.push_back(pas);
		r.push_back(pas); g.push_back(pas); b.push_back(pas);
		p+=7;
		pas += 1.0f / n;
	}


	float ** color = new float * [nCoord_];
	for (int v = 0; v < nCoord_; v++) {
		color[v] = new float[3];
		int cluster = (*partition)[v];
		color[v][0] = r[cluster];
		color[v][1] = g[cluster];
		color[v][2] = b[cluster];
	}
	
	SaveFrame(file, ifs, 0, color);

	// free memory
	for (int v = 0; v < nCoord_; v++) {
		delete [] color[v];
	}
	delete [] color;
	return 0;
}


double Animation::ComputeFirstFrameBBDiagonal() {
	float x = 0.0, y = 0.0, z = 0.0;
	float xmin = 0.0, xmax = 0.0; 
	float ymin = 0.0, ymax = 0.0; 
	float zmin = 0.0, zmax = 0.0; 

	xmin = xmax = 0.0;
	ymin = ymax = 0.0;
	zmin = zmax = 0.0;
	for (int v = 0; v < nCoord_ ; v++) {
		if (v == 0) {
			xmin = xmax = coord_[0][0];
			ymin = ymax = coord_[0][1];
			zmin = zmax = coord_[0][2];
		}
		x = coord_[0][v*3];
		y = coord_[0][v*3+1];
		z = coord_[0][v*3+2];
		if ( xmin > x) xmin = x;
		if ( xmax < x) xmax = x;
		if ( ymin > y) ymin = y;
		if ( ymax < y) ymax = y;
		if ( zmin > z) zmin = z;
		if ( zmax < z) zmax = z;
	}
	return pow( pow( (double) (xmax-xmin), 2.0) + pow((double) (ymax-ymin), 2.0) + pow((double) (zmax-zmin) , 2.0) , 0.5);
}

void Animation::ComputeClustersAdjacency(std::vector<int> * partition, int ** connec, int T){
	for (int p = 0; p < T; p++) {
		int i = (*partition)[connec[p][0]];
		int j = (*partition)[connec[p][1]];
		int k = (*partition)[connec[p][2]];
		AddNeighbor(i,i);
		AddNeighbor(j,j);
		AddNeighbor(k,k);
		AddNeighbor(i,j);AddNeighbor(j,i);
		AddNeighbor(i,k);AddNeighbor(k,i);
		AddNeighbor(k,j);AddNeighbor(j,k);
	}
}
void Animation::AddNeighbor(int v1, int v2)
{
	int found = 0;	
	for (IntVect::iterator posc = adj_[v1].begin(); posc != adj_[v1].end(); ++posc) {
		if ((*posc) == v2) {
			found = 1;
			break;
		}
	}
	if (found == 0)
		adj_[v1].push_back(v2);
}
void Animation::ComputeErrorDistribution(MotionModel &motionModel, std::vector<float> & errors, std::vector<int> * partition, float & errMM, float & errAM) {

	float diagonal = (float) ComputeFirstFrameBBDiagonal();

	double dist = 0.0;
	double dist2 = 0.0;
	double distGlobal = 0.0;
	double distGlobal2 = 0.0;

	float coord0[3]= {0.0f, 0.0f, 0.0f};
	float coord1[3]= {0.0f, 0.0f, 0.0f};
	float coordp[3]= {0.0f, 0.0f, 0.0f};
	float coordp2[3]= {0.0f, 0.0f, 0.0f};
	AffineTransform at;

	for (int v = 0; v < nCoord_; v++) {
		double distVertex = 0.0;

		for(int f = 0; f < nKeyCoord_; f++){
			GetCoord(f, v, coord1);
			GetCoord(0, v, coord0);
			motionModel.Get(coord0, v, f, coordp);
			dist =  pow( (double) (coord1[0]-coordp[0])/diagonal, 2.0) + 
				    pow( (double) (coord1[1]-coordp[1])/diagonal, 2.0) + 
					pow( (double) (coord1[2]-coordp[2])/diagonal, 2.0) ;
			distGlobal += dist;
			distVertex += dist;
			motionModel.GetAffineTransform(at, f, (*partition)[v]);
			at.Get(coord0, coordp2);
			dist2 =  pow( (double) (coord1[0]-coordp2[0])/diagonal, 2.0) + 
				     pow( (double) (coord1[1]-coordp2[1])/diagonal, 2.0) + 
					 pow( (double) (coord1[2]-coordp2[2])/diagonal, 2.0) ;
			distGlobal2 += dist2;
		}
		errors.push_back((float) pow(distVertex/nKeyCoord_, 0.5));
	}
	errAM = (float) pow(distGlobal2/(nKeyCoord_ * nCoord_), 0.5); 
	errMM = (float) pow(distGlobal/(nKeyCoord_ * nCoord_), 0.5); 
}

float Animation::ComputeVertexErrorsDiff(int v, MotionModel &motionModel, std::vector<int> * partition) {
	double dist = 0.0;
	double dist2 = 0.0;

	float coord0[3];
	float coord1[3];
	float coordp[3];
	float coordp2[3];
	AffineTransform at;


	double errMM = 0.0;
	double errAM = 0.0;
	for(int f = 0; f < nKeyCoord_; f++){
		GetCoord(f, v, coord1);
		GetCoord(0, v, coord0);
		motionModel.Get(coord0, v, f, coordp);
		dist =  pow( (double) (coord1[0]-coordp[0]), 2.0) + 
			    pow( (double) (coord1[1]-coordp[1]), 2.0) + 
				pow( (double) (coord1[2]-coordp[2]), 2.0) ;
		errMM += dist;
		motionModel.GetAffineTransform(at, f, (*partition)[v]);
		at.Get(coord0, coordp2);
		dist2 =  pow( (double) (coord1[0]-coordp2[0]), 2.0) + 
			     pow( (double) (coord1[1]-coordp2[1]), 2.0) + 
				 pow( (double) (coord1[2]-coordp2[2]), 2.0) ;
		errAM += dist2;
	}
	return (float) (pow(errAM,0.5) - pow(errMM,0.5)); 

}

bool Animation::FindNearestCoordKey(float t, int & frame){
	if (nKeyCoord_ == 0)  return false;

	frame = 0;
	float minD = fabs(coordKey_[frame] - t);
	float d = 0.0;
	for(int f = 1; f < nKeyCoord_; f++){
		d = fabs(coordKey_[f] - t);
		if ( d < minD ){
			minD = d;
			frame = f;
		}

	}
	return true;
}

void Animation::AnimToIFS(IndexedFaceSet &ifs, int frame){
	int p = 0;
	if (GetNCoord() > 0) {
		float coord[3];
		for (p = 0; p < ifs.GetNCoord() ; p++) {
			GetCoord(frame, p, coord);
			ifs.SetCoord(p, coord);
		}
	}
	if (GetNNormal()> 0) {
		float normal[3];
		for (p = 0; p < ifs.GetNNormal() ; p++) {
			GetNormal(frame, p, normal);
			ifs.SetNormal(p, normal);
		}
	}

	if (GetNColor()> 0) {
		float color[3];
		for (p = 0; p < ifs.GetNColor() ; p++) {
			GetColor(frame, p, color);
			ifs.SetColor(p, color);
		}
	}
}
void Animation::IFSToAnim(IndexedFaceSet &ifs, int frame){
	int p = 0;
	if (GetNCoord() > 0) {
		float coord[3];
		for (p = 0; p < ifs.GetNCoord() ; p++) {
			ifs.GetCoord(p, coord);
			SetCoord(frame, p, coord);
		}
	}
	if (GetNNormal()> 0) {
		float normal[3];
		for (p = 0; p < ifs.GetNNormal() ; p++) {
			ifs.GetNormal(p, normal);
			SetNormal(frame, p, normal);
		}
	}

	if (GetNColor()> 0) {
		float color[3];
		for (p = 0; p < ifs.GetNColor() ; p++) {
			ifs.GetColor(p, color);
			SetColor(frame, p, color);
		}
	}
}
void Animation::GetAnimationSegment(Animation & segment, int frameStart, int frameEnd){
	int nbrFrames = frameEnd - frameStart +1;
	if (GetNKeyCoord() > 0){
		segment.SetNKeyCoord(nbrFrames);
		segment.SetNCoord(GetNCoord());
	}
	if (GetNKeyNormal() > 0){
		segment.SetNKeyNormal(nbrFrames);
		segment.SetNNormal(GetNNormal());
	}
	if (GetNKeyColor() > 0){
		segment.SetNKeyColor(nbrFrames);
		segment.SetNColor(GetNColor());
	}

	segment.AllocateMem();

	int p = 0;
	for (int frame = frameStart; frame < frameEnd; frame++) {
		// we copy keys
		if (GetNKeyCoord() > 0)  segment.GetKeyCoord()[frame-frameStart]  = GetKeyCoord()[frame];
		if (GetNKeyNormal() > 0) segment.GetKeyNormal()[frame-frameStart] = GetKeyNormal()[frame];
		if (GetNKeyColor() > 0)  segment.GetKeyColor()[frame-frameStart]  = GetKeyColor()[frame];
		// we copy keyvalues
		if (GetNCoord() > 0) {
			float coord[3];
			for (p = 0; p < GetNCoord() ; p++) {
				GetCoord(frame, p, coord);
				segment.SetCoord(frame-frameStart, p, coord);
			}
		}
		if (GetNNormal() > 0) {
			float normal[3];
			for (p = 0; p < GetNNormal() ; p++) {
				GetNormal(frame, p, normal);
				segment.SetNormal(frame-frameStart, p, normal);
			}
		}
		if (GetNColor() > 0) {
			float color[3];
			for (p = 0; p < GetNColor() ; p++) {
				GetColor(frame, p, color);
				segment.SetColor(frame-frameStart, p, color);
			}
		}
	}
}
void Animation::GetAnimationSegment(Animation & segment, IndexedFaceSet &ifs, int frameStart, int frameEnd){
	segment.SetDuration(duration_);
	int nbrFrames = frameEnd - frameStart+2;
	if (GetNKeyCoord() > 0){
		segment.SetNKeyCoord(nbrFrames);
		segment.SetNCoord(GetNCoord());
	}
	if (GetNKeyNormal() > 0){
		segment.SetNKeyNormal(nbrFrames);
		segment.SetNNormal(GetNNormal());
	}
	if (GetNKeyColor() > 0){
		segment.SetNKeyColor(nbrFrames);
		segment.SetNColor(GetNColor());
	}

	segment.AllocateMem();

	int p = 0;
	// copy first frame
	if (GetNKeyCoord() > 0)  segment.GetKeyCoord()[0]  = -1.0;
	if (GetNKeyNormal() > 0) segment.GetKeyNormal()[0] = -1.0;
	if (GetNKeyColor() > 0)  segment.GetKeyColor()[0]  = -1.0;
	// we copy keyvalues
	if (GetNCoord() > 0) {
		float coord[3];
		for (p = 0; p < ifs.GetNCoord() ; p++) {
			ifs.GetCoord(p, coord);
			segment.SetCoord(0, p, coord);
		}
	}
	if (GetNNormal() > 0) {
		float normal[3];
		for (p = 0; p < ifs.GetNNormal() ; p++) {
			ifs.GetNormal(p, normal);
			segment.SetNormal(0, p, normal);
		}
	}
	if (GetNColor() > 0) {
		float color[3];
		for (p = 0; p < ifs.GetNColor() ; p++) {
			ifs.GetColor(p, color);
			segment.SetColor(0, p, color);
		}
	}


	for (int frame = frameStart; frame <= frameEnd; frame++) {
		// we copy keys
		if (GetNKeyCoord() > 0)  segment.GetKeyCoord()[frame-frameStart+1]  = GetKeyCoord()[frame];
		if (GetNKeyNormal() > 0) segment.GetKeyNormal()[frame-frameStart+1] = GetKeyNormal()[frame];
		if (GetNKeyColor() > 0)  segment.GetKeyColor()[frame-frameStart+1]  = GetKeyColor()[frame];
		// we copy keyvalues
		if (GetNCoord() > 0) {
			float coord[3];
			for (p = 0; p < GetNCoord() ; p++) {
				GetCoord(frame, p, coord);
				segment.SetCoord(frame-frameStart+1, p, coord);
			}
		}
		if (GetNNormal() > 0) {
			float normal[3];
			for (p = 0; p < GetNNormal() ; p++) {
				GetNormal(frame, p, normal);
				segment.SetNormal(frame-frameStart+1, p, normal);
			}
		}
		if (GetNColor() > 0) {
			float color[3];
			for (p = 0; p < GetNColor() ; p++) {
				GetColor(frame, p, color);
				segment.SetColor(frame-frameStart+1, p, color);
			}
		}
	}
}

int Animation::GetAnimationData(std::vector<float> &keys, std::vector<float> & coord, std::vector<float> &normal, std::vector<float> &color, int start){
	int numberOfFrame = 0;
	float * keysA = NULL;

	if (GetNKeyCoord() > 0) {
		keysA = GetKeyCoord();
		numberOfFrame = GetNKeyCoord();
	}
	if (GetNKeyNormal() > 0) {
		keysA = GetKeyNormal();
		numberOfFrame = GetNKeyNormal();
	}
	if (GetNKeyColor() > 0) {
		keysA = GetKeyColor();
		numberOfFrame = GetNKeyColor();
	}
	int p = 0;
	for (int frame = start; frame < numberOfFrame; frame++) {
		int done = 0;	
		keys.push_back(keysA[frame]);
		// we copy keyvalues
		if (GetNCoord() > 0) {
			float coordA[3];
			for (p = 0; p < GetNCoord() ; p++) {
				GetCoord(frame, p, coordA);
				coord.push_back(coordA[0]);
				coord.push_back(coordA[1]);
				coord.push_back(coordA[2]);
			}
		}
		if (GetNNormal() > 0) {
			float normalA[3];
			for (p = 0; p < GetNNormal() ; p++) {
				GetNormal(frame, p, normalA);
				normal.push_back(normalA[0]);
				normal.push_back(normalA[1]);
				normal.push_back(normalA[2]);
			}
		}
		if (GetNColor() > 0) {
			float colorA[3];
			for (p = 0; p < GetNColor() ; p++) {
				GetColor(frame, p, colorA);
				color.push_back(colorA[0]);
				color.push_back(colorA[1]);
				color.push_back(colorA[2]);
			}
		}
	}
	return numberOfFrame-start;
}
void Animation::CreateAnimation(std::vector<float> &keys, std::vector<float> & coord, std::vector<float> &normal, std::vector<float> &color){
	int nbrFrames = (int)keys.size();
	if (coord.size() > 0){
		SetNKeyCoord(nbrFrames);
		SetNCoord((int)coord.size()/(nbrFrames*3));
	}
	if (normal.size() > 0){
		SetNKeyNormal(nbrFrames);
		SetNNormal((int)normal.size()/(nbrFrames*3));
	}
	if (color.size() > 0){
		SetNKeyColor(nbrFrames);
		SetNColor((int)color.size()/(nbrFrames*3));
	}

	AllocateMem();

	int p = 0;
	int posCoord = 0;
	int posNormal = 0;
	int posColor = 0;
	for (int frame = 0; frame < nbrFrames; frame++) {
		// we copy keys
		if (GetNKeyCoord() > 0)  GetKeyCoord()[frame]  = keys[frame];
		if (GetNKeyNormal() > 0) GetKeyNormal()[frame] = keys[frame];
		if (GetNKeyColor() > 0)  GetKeyColor()[frame]  = keys[frame];
		// we copy keyvalues
		if (GetNCoord() > 0) {
			float coordA[3];
			for (p = 0; p < GetNCoord() ; p++) {
				coordA[0] = coord[posCoord++];
				coordA[1] = coord[posCoord++];
				coordA[2] = coord[posCoord++];
				SetCoord(frame, p, coordA);
			}
		}
		if (GetNNormal() > 0) {
			float normalA[3];
			for (p = 0; p < GetNNormal() ; p++) {
				normalA[0] = normal[posNormal++];
				normalA[1] = normal[posNormal++];
				normalA[2] = normal[posNormal++];
				SetNormal(frame, p, normalA);
			}
		}
		if (GetNColor() > 0) {
			float colorA[3];
			for (p = 0; p < GetNColor() ; p++) {
				colorA[0] = color[posColor++];
				colorA[1] = color[posColor++];
				colorA[2] = color[posColor++];
				SetColor(frame, p, colorA);
			}
		}
	}
}
