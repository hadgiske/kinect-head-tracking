#include "KHeigthMap.h"

KHeigthMap::KHeigthMap(void)
{
	vcounter = 0;
	vectors = 0;
	dcounter = 0;
	data = 0;
	metadata = 0;
}

KHeigthMap::~KHeigthMap(void)
{
	if(vcounter > 0){
		delete[] vectors;
	}

	for(int i = 0 ; i < dcounter ; i++){
		delete data[i];
		delete metadata[i];
	}
}

void KHeigthMap::feedDepthmap(XnDepthPixel* DataFeed, xn::DepthMetaData MetaData){
	XnDepthPixel** temp = new XnDepthPixel*[dcounter+1];
	KMetaData**     tempm= new KMetaData*[dcounter+1];

	for(int i = 0 ; i < dcounter ; i++) {
		temp[i] = data[i];
		tempm[i]= metadata[i];
	}

	temp[dcounter] = DataFeed;

	tempm[dcounter] = new KMetaData();
	tempm[dcounter]->XRes = MetaData.XRes();
	tempm[dcounter]->YRes = MetaData.YRes();

	dcounter++;

	data = temp;
	metadata = tempm;
}

void KHeigthMap::normalize(){
	for (int i = 0 ; i < dcounter ; i++){
		for (int k = 0 ; k < metadata[i]->YRes ; k++){
			for (int l = 0 ; l < metadata[i]->XRes ; l++){
				data[i][k+l+k*(metadata[i]->YRes - 1)] = data[i][k+l+k*(metadata[i]->YRes - 1)] / 65535 * 255;
			}
		}
	}
}

void KHeigthMap::compute(){
	if (vcounter > 0){
		delete[] vectors;
	}
	vcounter = 0;


	int tmpcounter = 0;
	for(int i = 0 ; i < dcounter ; i++){
		tmpcounter += metadata[i]->XRes * metadata[i]->YRes;
	}

	vectors = new KVector[tmpcounter];
	vcounter = tmpcounter;
	
	int currentcounter = 0;
	vectors[currentcounter].x = 0;
	vectors[currentcounter].y = 0;
	vectors[currentcounter].z = data[0][0];

	currentcounter++;

	for	(int i = 0 ; i < dcounter ; i++)	{
		for (int y = 0 ; y < metadata[i]->YRes ; y+=2){

			for(int x = 0 ; x < metadata[i]->XRes ; x++){
				vectors[currentcounter].x = x;
				vectors[currentcounter].y = y+1;
				vectors[currentcounter].z = data[i][(y+1)*(metadata[i]->YRes)+x];
				currentcounter++;

				vectors[currentcounter].x = x+1;
				vectors[currentcounter].y = y;
				vectors[currentcounter].z = data[i][(y)*(metadata[i]->YRes)+x+1];
				currentcounter++;
			}

		}
	}	
}

void KHeigthMap::render(){
}
