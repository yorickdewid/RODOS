/**
 * @file ObjImporter.h
 * @date 2012/06/25
 * @author Julius Peinelt
 *
 * Copyright Julius Peinelt
 *
 * OjbImporter is used to parse and import a 3D model from a a wavefront .obj file.
 * The file to import is specified in the constructor.
 *
 */

#ifndef OBJIMPORTER_H
#define	OBJIMPORTER_H

#include <GL/freeglut.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cmath>
#include "../Visualisation/Polyhedron.h"
#include "TextureFromPPM.h"


struct Face{
	int v[3], t[3], n[3];
};



using namespace std;

class ObjImporter {
public:

    /**
     * The Konstruktor parses a wavefront .obj and the corresponding .mtl file.
     * @param oFilename Wavefront .obj file to parse.
     */
    ObjImporter(string oFilename);
    
    virtual ~ObjImporter();
    
    /**
     * Creates a Polyhedron out of the parsed .obj file.
     */
    Polyhedron createPolyhedron();
   
    
private:
    string mFilename;
    string tFilename;
    
    vector<Vector3f> vertices;
    vector<Vector3f> normals;
    vector<Vector2f> textureV;
    vector<Face> faces;
    GLuint tex_id;
    
    Vector4<GLfloat> ambient;
    Vector4<GLfloat> diffuse;
    Vector4<GLfloat> specular;
    GLfloat shininess;
    
    /**
     * Loads all Material properties from the specified .mtl file.
     * @param matFilename a .mtl File
     */
    void loadMaterial(string matFilename);
    
    /**
     * Loads a Texture by use of a TextureFromPPM from the specified .ppm file.
     * @param texFilename a .ppm image
     */
    void loadTexture(string texFilename);
    
    /**
     * Helper method which sorts parsed vertices by parsed faces.
     */
    void sortVertices();
    
    /**
     * Helper method which splits strings by all delims.
     * @param s String to be splitted
     * @param delims a String of delimiter
     */
    vector<string> splitString(string s, string delims);

};

#endif	/* OBJIMPORTER_H */

