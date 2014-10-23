//-----------------------------------------------------------------------------
// File:        famc.cpp
// Description: Support for MPEG-4 FAMC, compression of directories of mesh files.
//-----------------------------------------------------------------------------
// Copyright (c) 2014- Rufael Mekuria, Centrum Wiskunde en Informatica
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//     1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software. If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//     2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//     3. This notice may not be removed or altered from any source
//     distribution.
//-----------------------------------------------------------------------------
#include <String.h>
#include "famc.h"
#include "Console.h"
#include "IndexedFaceSet.h"
#include "Animation.h"
#include "FAMCEncoder.h"
#include "FAMCDecoder.h"
#include "meshio.h"
#include <string>
#include "FAMCVertexInfoEncoder.h"
#include "FAMCVertexInfoDecoder.h"
#include "FAMCSimplificationModeEncoder.h"
#include "FAMCSimplificationModeDecoder.h"

#include <iostream>
#include <fstream>

#ifdef _WIN32
#include "dirent_win32.h"
#else
#include "dirent.h"
#endif

/* 
struct for loading the animation in a way that makes it easier to convert it to the FAMC structure
*/
struct simpleTempAnimation
{
	IndexedFaceSet famc_key_frame;

	unsigned int number_of_frames; 
		
	/* fixed connectibity and coordinate count */
	unsigned int coord_count;
	unsigned int tr_count;

	std::vector<float> m_keys;
	std::vector<float> m_coords; 
	std::vector<float> m_normals; 
	std::vector<float> m_colors; 

	std::vector<unsigned int> m_conn;

	simpleTempAnimation()
		: number_of_frames(0), coord_count(0), tr_count(0)
	{}

	/**/
	void appendAMesh(Mesh * aMesh)
	{
		if(!aMesh->mVertices.size())
			return;
		//! append the mesh to the tempAnim
		if(number_of_frames == 0)
		{ // key mesh
			coord_count = (unsigned int) aMesh->mVertices.size();
			tr_count = (unsigned int) aMesh->mIndices.size()/3;

			m_coords.resize(coord_count * 3);

			for(int i=0; i<aMesh->mVertices.size();i++)
			{
				m_coords[3*i] = aMesh->mVertices[i].x;
				m_coords[3*i+1] = aMesh->mVertices[i].y;
				m_coords[3*i+2] = aMesh->mVertices[i].z;
			}

			m_normals.resize(coord_count * 3);

			for(int i=0; i<aMesh->mNormals.size();i++)
			{
				m_normals[3*i] = aMesh->mNormals[i].x;
				m_normals[3*i+1] = aMesh->mNormals[i].y;
				m_normals[3*i+2] = aMesh->mNormals[i].z;
			}

			m_colors.resize(coord_count * 3);

			for(int i=0; i<aMesh->mColors.size();i++)
			{
				m_colors[3*i] =   aMesh->mColors[i].x;
				m_colors[3*i+1] = aMesh->mColors[i].y;
				m_colors[3*i+2] = aMesh->mColors[i].z;
			}

			m_conn.resize(aMesh->mIndices.size());

			for(int i=0; i<aMesh->mIndices.size();i++)
			{
				m_conn[i] = (unsigned int) aMesh->mIndices[i];
			}

			printf ("frame number %d appended to mesh sequence \n",number_of_frames );

			m_keys.push_back(1.0);
			number_of_frames++;
		}
		else
		{
			// check for consistency only via vertex and face count
			if(aMesh->mIndices.size()/3 !=  tr_count) {
				return;
			}
			if(aMesh->mVertices.size() !=  coord_count) {
				return;
			}
			//! add to the coordinates
			m_coords.reserve(coord_count * 3 * (number_of_frames+1) );

			for(int i=0; i<aMesh->mVertices.size();i++)
			{
				m_coords.push_back(aMesh->mVertices[i].x);
				m_coords.push_back(aMesh->mVertices[i].y);
				m_coords.push_back(aMesh->mVertices[i].z);
			}

			m_normals.reserve(coord_count * 3 * (number_of_frames+1) );

			for(int i=0; i<aMesh->mNormals.size();i++)
			{
				m_normals.push_back(aMesh->mNormals[i].x);
				m_normals.push_back(aMesh->mNormals[i].y);
				m_normals.push_back(aMesh->mNormals[i].z);
			}

			m_colors.reserve(coord_count * 3 * (number_of_frames+1) );

			for(int i=0; i<aMesh->mColors.size();i++)
			{
				m_colors.push_back(aMesh->mColors[i].x);
				m_colors.push_back(aMesh->mColors[i].y);
				m_colors.push_back(aMesh->mColors[i].z);
			}

			printf ("frame number %d appended to mesh sequence \n",number_of_frames );

			m_keys.push_back(1.0);
			number_of_frames++;
		}
	};

	void getFAMCAnimationAndIfs(Animation &anim, IndexedFaceSet &ifs){
		anim.CreateAnimation(m_keys,m_coords,m_normals, m_colors);
		// set the ifs
		ifs.FreeMem();
		ifs.SetNColor( coord_count);
		ifs.SetNCoord( coord_count);
		ifs.SetNNormal( coord_count);
		ifs.SetNCoordIndex( tr_count);
		ifs.AllocateMem();
		for(int i=0; i< coord_count; i++){
			ifs.SetColor(i, (float *) &m_colors[3*i]);
			ifs.SetNormal(i, (float *) &m_normals[3*i]);
			ifs.SetCoord(i, (float *) &m_coords[3*i]);
		}
		for(int i=0; i< tr_count; i++){
			int tr[3] = { (int) m_conn[3*i], (int)m_conn[3*i +1] , (int) m_conn[3*i +2]};
			ifs.SetCoordIndex(i, &tr[0]);
		}

		printf(" loaded indexed face set with %u vertices and %u faces\n", coord_count,tr_count );
	};
} temp_anim;

Animation tempFamcAnim;

/*
 Load files from directory into a temporary structure that can be converted to an FAMC compatible animation
*/
void load_from_directory(
    const char *dirname)
{
    DIR *dir;
    struct dirent *ent;
                
    /* Open directory stream */
    dir = opendir (dirname);
    if (dir != NULL) {

        /* Print all files and directories within the directory */
        while ((ent = readdir (dir)) != NULL) {
            switch (ent->d_type) {
            case DT_REG:
                printf ("loading... %s\n", ent->d_name);
				{
					Mesh aMesh; 
					std::string rel_path = std::string(dirname) + std::string("//") +  std::string(ent->d_name);
					ImportMesh(rel_path.c_str(), &aMesh);
					if(aMesh.mVertices.size() > 0){
						//add the mesh to the animation
						temp_anim.appendAMesh(&aMesh);
					}
				}
                break;

            case DT_DIR:
                printf ("%s/\n", ent->d_name);
                break;

            case DT_LNK:
                printf ("%s@\n", ent->d_name);
                break;

            default:
                printf ("%s*\n", ent->d_name);
            }
        }

        closedir (dir);

		// create famc animation

    } else {
        /* Could not open directory */
        printf ("Cannot open directory %s\n", dirname);
        exit (EXIT_FAILURE);
    }
}

/* load the Famc Parameter from a txt file, one parameter per line */
void loadFamcParamFromTxtFile(FAMCParams  &famcParam, std::string &FamcParamFile)
{
	std::ifstream ifile(FamcParamFile);

	if(!ifile.good()){
		printf( "Could not load FAMC parameter file %s \n", FamcParamFile.c_str() );
		return;
	}

	else{
		while(ifile.good()){
			std::string arg_name; 
			std::string arg_val; 
			std::string comment;
			std::getline(ifile, arg_name,' ');
			std::getline(ifile, arg_val,' ');
			std::getline(ifile, comment);

			if(arg_name.compare("FileName:")== 0){
				//FileName: outFamc   input/output file name (.mp4)
				strcpy(famcParam.file_, arg_val.c_str());
				printf( " parameter %s is set to: %s \n",arg_name.c_str() ,arg_val.c_str() );
			}
			else if(arg_name.compare("sizeAnimSeg:")== 0){
				famcParam.sizeAnimSeg_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %s \n",arg_name.c_str() ,arg_val.c_str() );
			}
			else if(arg_name.compare("transform:")== 0){
				famcParam.transformType_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %s \n",arg_name.c_str() ,arg_val.c_str() );
			}
			else if(arg_name.compare("globalRMSE:")== 0){
				famcParam.globalRMSE_ = (float) atof(arg_val.c_str());
				printf( " parameter %s is set to: %s \n",arg_name.c_str() ,arg_val.c_str() );
			}
			else if(arg_name.compare("ratioWeights:")== 0){
				famcParam.ratioWeights_ = (float) atof(arg_val.c_str());
				printf( " parameter %s is set to: %f \n",arg_name.c_str() ,famcParam.ratioWeights_ );
			}
			else if(arg_name.compare("qAT:")== 0){
				famcParam.qAT_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %s \n",arg_name.c_str() ,arg_val.c_str() );
			}
			else if(arg_name.compare("qCoord:")== 0){
				famcParam.qCoord_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %s \n",arg_name.c_str() ,arg_val.c_str() );
			}
			else if(arg_name.compare("qNormal:")== 0){
				famcParam.qNormal_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %s \n",arg_name.c_str() ,arg_val.c_str() );
			}
			else if(arg_name.compare("qColor:")== 0){
				famcParam.qColor_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.qColor_ );
			}
			else if(arg_name.compare("qOther:")== 0){
				famcParam.qOther_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() , famcParam.qOther_);
			}
			else if(arg_name.compare("LD_auto:")== 0){
				famcParam.LD_auto_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() , famcParam.LD_auto_ );
			}
			else if(arg_name.compare("LD_L:")== 0){
				famcParam.LD_L_ = atoi(arg_val.c_str());\
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.LD_L_ );
			}
			else if(arg_name.compare("LD_l:")== 0){
				famcParam.LD_l_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.LD_l_  );
			}
			else if(arg_name.compare("LD_P:")== 0){
				famcParam.LD_P_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.LD_P_ );
			}
			else if(arg_name.compare("fr_per_seg:")== 0){
				//famcParam. = atoi(arg_val.c_str());
			}
			else if(arg_name.compare("LD_B:")== 0){
				famcParam.LD_B_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.LD_B_ );
			}
			else if(arg_name.compare("LD_Pred:")== 0){
				famcParam.LD_Pred_= atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.LD_Pred_ );
			}
			else if(arg_name.compare("SNR_L:")== 0){
				famcParam.SNR_L_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.SNR_L_ );
			}
			else if(arg_name.compare("SNR_l:")== 0){
				famcParam.SNR_l_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.SNR_l_ );
			}
			else if(arg_name.compare("Normal_Pred:")== 0){
				famcParam.Normal_Pred_ = atoi(arg_val.c_str());
				printf( " parameter %s is set to: %d \n",arg_name.c_str() ,famcParam.Normal_Pred_ );
			}
			else if(arg_name.compare("statFile:") == 0){
				printf( " parameter %s is ( not) set to: %s \n",arg_name.c_str(), arg_val.c_str() );
			}
			else{
				printf(" incorrect parameter argument detected %s \n", arg_name.c_str());
			}
		}
	}
}

CConsole logger("C:\\log.txt");
void Import_DIR(const char * aFileName, Mesh * aMesh)
{
	load_from_directory(aFileName);
};

void Import_MP4(const char * aFileName, Mesh * aMesh)
{
	std::string file_name (aFileName);

	std::string file_name_no_ext(file_name.begin(),file_name.begin() + (file_name.size() - 4) );

	printf( " the bare file_name to be loaded is %s \n ", file_name_no_ext.c_str());

	FAMCDecoder *decoder = new FAMCDecoder;

	char * fname = strdup(file_name_no_ext.c_str());
	decoder->DecodeStream( fname, 1 , tempFamcAnim);
	delete fname;
 };

void IfsToAMesh(Mesh *aMesh,  IndexedFaceSet & ifs  )
{
	if(ifs.GetNCoord()){
		aMesh->mVertices.resize(ifs.GetNCoord());
		for(int i=0; i < ifs.GetNCoord(); i++ )
		{
			float b[4];
			ifs.GetCoord(i,b);
			aMesh->mVertices[i].x = b[0];
			aMesh->mVertices[i].y = b[1];
			aMesh->mVertices[i].z = b[2];
		}
	}

	if(ifs.GetNNormal()){
		aMesh->mNormals.resize(ifs.GetNNormal());

		for(int i=0; i < ifs.GetNNormal(); i++ )
		{
			float b[4];
			ifs.GetNormal(i,b);
			aMesh->mNormals[i].x = b[0];
			aMesh->mNormals[i].y = b[1];
			aMesh->mNormals[i].z = b[2];
		}
	}

	if(ifs.GetNColor()){
		aMesh->mColors.resize(ifs.GetNColor());

		for(int i=0; i < ifs.GetNColor(); i++ )
		{
			float b[4] = {1.0,1.0,1.0,1.0};
			ifs.GetColor(i,b);
			aMesh->mColors[i].x = b[0];
			aMesh->mColors[i].y = b[1];
			aMesh->mColors[i].z = b[2];
			aMesh->mColors[i].w = b[3];
		}
	}

	if(ifs.GetNCoordIndex()){
		aMesh->mIndices.resize(3 * ifs.GetNCoordIndex());

		for(int i=0; i < ifs.GetNCoordIndex(); i++ )
		{
			int face[4] = {0,0,0,0};
			ifs.GetCoordIndex(i,face);
			aMesh->mIndices[3*i] = face[0];
			aMesh->mIndices[3*i+1] = face[1];
			aMesh->mIndices[3*i+2] = face[2];
		}
	}
};

void Export_DIR(const char * aFileName, Mesh * aMesh,Options &aOptions){

	// we will move the 

	if( tempFamcAnim.GetNKeyCoord() > 0) {
		
		printf( " found animation with %d frames OK\n", tempFamcAnim.GetNKeyCoord() );

		IndexedFaceSet ifs; 

		char firstFrameVRML[1024] = "    ";

		sprintf(firstFrameVRML,"%sFF.wrl", aFileName);

		if(ifs.LoadIFSVRML2(firstFrameVRML)){
			printf( " loaded initial key frame %s OK \n", aFileName );
		}
		else{
			printf( " could not load initial key frame %s , press key to exit \n", aFileName );
			exit(0);
		}
		for(int i=0; i< tempFamcAnim.GetNKeyCoord();i++){
			
			Mesh aMesh;

			tempFamcAnim.AnimToIFS(ifs,i);

			IfsToAMesh(&aMesh, ifs);

			char index_label[12]="0000";

			sprintf( index_label,"%05d",i );

			std::string out_path = std::string(aFileName) + std::string("__") + std::string(index_label) + aOptions.mExportExtension;

			ExportMesh(out_path.c_str(),&aMesh, aOptions);
		}
	}

	return;
};
void Export_MP4(const char * aFileName, Mesh * aMesh, Options &aOptions)
{
	// step1: read FAMC parameters from txt file
	FAMCParams famcParam; 

	// load the FamC parameters from a .txt file
	loadFamcParamFromTxtFile(famcParam, aOptions.mFamcParamFile);

	// step2: create encoder with the FamcParameters
	FAMCEncoder encoder(famcParam);

	// step3: load animation and ifs from tempAnimation
	IndexedFaceSet myIFS;
	Animation myAnim;

	temp_anim.getFAMCAnimationAndIfs(myAnim,myIFS);

	// step4: encode the stream (automatically stores the output file)
	char sfile[1024] = "log.txt"; 

	std::string file_name(aFileName);

	std::string file_name_no_ext(file_name.begin(),file_name.begin() + (file_name.size() - 4) );

	printf( " the bare file_name to be exported is %s \n ", file_name_no_ext.c_str());

	encoder.EncodeStream(myAnim , myIFS, file_name_no_ext.c_str(), sfile);
};