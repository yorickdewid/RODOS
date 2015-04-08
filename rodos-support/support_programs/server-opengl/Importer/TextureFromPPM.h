/**
 * @file TextureFromPPM.h
 * @date 2012/06/25
 * @author Julius Peinelt
 *
 * Copyright Julius Peinelt
 *
 * TextureFromPPM loads a texture from a .ppm file and stores the data in the video memory.
 */

#ifndef TEXTUREFROMPPM_H
#define	TEXTUREFROMPPM_H

#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

class TextureFromPPM {
public:
    
    /**
     * Loads data from a .ppm image in a unsigned char array.
     * @param tFilename Texture image
     */
    TextureFromPPM(string tFilename);
    
    TextureFromPPM(const TextureFromPPM& orig);
    
    virtual ~TextureFromPPM();
    
    /**
     * Returns the loaded texture data as array.
     */
    unsigned char* getTexture() const;
    
    /**
     * Returns the width of the texture.
     */
    int getWidth() const;
    
    /**
     * Returns the height of the texture.
     */
    int getHeight() const;
    
    /**
     * Returns the maximal value for a color component of the image. Usually
     * this is 255.
     */
    int getMaxColorValue() const;
    
private:
    int width;
    int height;
    int maxColorValue;  //standard is 255 (1byte but there is a format with 2bytes, which we ignore)
    unsigned char* texture;    

};

#endif	/* TEXTUREFROMPPM_H */

