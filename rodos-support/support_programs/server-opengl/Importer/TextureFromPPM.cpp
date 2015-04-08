/* 
 * File:   TextureFromPPM.cpp
 * Author: julius
 * 
 * Created on January 28, 2012, 5:15 PM
 */

#include "TextureFromPPM.h"

TextureFromPPM::TextureFromPPM(string tFilename) {
    
    ifstream file;
    
    file.open(tFilename.c_str());
    if (!file) {
        //file couldn't be read
        cout<<"Texture: '"<< tFilename <<"' File couldn't be read" << endl;
    }
    char header[100];
    file.getline(header, 100, '\n');
    if ( (header[0]!=80) || (header[1]!=54) ) {   
        //PPM has 'P6' as magic number in header but this header NOT -> no PPM
      //end with texture = null or something like that
        cout<<"Texture: No PPM file"<< endl;
    }
    
    file.getline(header,100,'\n');
    while(header[0]=='#') {
        file.getline(header,100,'\n');
    }
  
    char *ptr;
    width = strtol(header ,&ptr , 0);
    height = atoi(ptr);
 
    file.getline(header,100,'\n');
    maxColorValue = strtol(header,&ptr,0); //should be 255
    
    texture = (unsigned char*) new unsigned char[3 * width * height];
    
    file.read( reinterpret_cast<char *>(texture), (3*width*height)*sizeof(unsigned char));
    
    if (file.fail()) {
        //file size is wrong
        cout<<"Texture: File size is wrong" <<endl;
    }

    file.close();  
}


unsigned char* TextureFromPPM::getTexture() const {
    return texture;
}

int TextureFromPPM::getWidth() const {
    return width;
}

int TextureFromPPM::getHeight() const {
    return height;
}

int TextureFromPPM::getMaxColorValue() const {
    return maxColorValue;
}



TextureFromPPM::TextureFromPPM(const TextureFromPPM& orig) {
    width = orig.getWidth();
    height = orig.getHeight();
    maxColorValue = 255;  //standard is 255 (1byte but there is a format with 2bytes, which we ignore)
    texture = (unsigned char*) new unsigned char[3 * width * height];
	for (int i = 0; i < (3*width*height); i++) {
		texture[i] = orig.getTexture()[i];
	}
}

TextureFromPPM::~TextureFromPPM() {
    delete [] texture;
}

