/* 
 * File:   ObjImporter.cpp
 * Author: julius
 * 
 * Created on March 16, 2012, 3:41 PM
 */

#include "ObjImporter.h"

ObjImporter::ObjImporter(string oFilename) {
    
    ifstream input;
    
    input.open(oFilename.c_str());
    if (!input) {
        //file couldn't be read
        cout<<"Object: File couldn't be read" << endl;
    }
    
    string fileline;

    vector<string> linecomp;
    
    while (getline(input, fileline)) {
        
        linecomp = splitString(fileline, " \n");
        
        if (linecomp.at(0) == "v") {
            vertices.push_back(Vector3<GLfloat>(atof(linecomp.at(1).c_str()), atof(linecomp.at(2).c_str()), atof(linecomp.at(3).c_str())));
        } else if (linecomp.at(0) == "vt") {
            textureV.push_back(Vector2<GLfloat>(atof(linecomp.at(1).c_str()), atof(linecomp.at(2).c_str())));
        } else if (linecomp.at(0) == "vn") {
            normals.push_back(Vector3<GLfloat>(atof(linecomp.at(1).c_str()), atof(linecomp.at(2).c_str()), atof(linecomp.at(3).c_str())));
        } else if (linecomp.at(0) == "f") {
            vector<string> fCoords;
            Face tempface;
            for (int i = 0; i < 3; i++) {
                fCoords = splitString(linecomp.at(i+1), "/");
                
                tempface.v[i] = atoi(fCoords.at(0).c_str());
                tempface.t[i] = atoi(fCoords.at(1).c_str());
                tempface.n[i] = atoi(fCoords.at(2).c_str());
            }
            faces.push_back(tempface);
        } else if (linecomp.at(0) == "mtllib") {
            //load mtllib and load texture
            mFilename = linecomp.at(1);
        }
    }
    input.close();
    
    if (!mFilename.empty()) {
        loadMaterial(mFilename);
    }
    sortVertices();
}

void ObjImporter::loadMaterial(string matFilename) {
    ifstream matfile;
    
    matfile.open(matFilename.c_str());
    if (!matfile) {
        //file couldn't be read
        cout << "Material: File '"<< matFilename <<"' couldn't be read" << endl;
    }
    string fileline;
    vector<string> linecomp;
    while(getline(matfile, fileline)) {
        linecomp = splitString(fileline, " \n");
        if (linecomp.size() == 0) {
            continue;
        }
        
        //ambient component
        if (linecomp.at(0) == "Ka") {
            ambient = Vector4<GLfloat>(atof(linecomp.at(1).c_str()), atof(linecomp.at(2).c_str()), atof(linecomp.at(3).c_str()), 1.0);
        //diffuse component
        } else if (linecomp.at(0) == "Kd") {
            diffuse = Vector4<GLfloat>(atof(linecomp.at(1).c_str()), atof(linecomp.at(2).c_str()), atof(linecomp.at(3).c_str()), 1.0);
        //specular component
        } else if (linecomp.at(0) == "Ks") {
            specular = Vector4<GLfloat>(atof(linecomp.at(1).c_str()), atof(linecomp.at(2).c_str()), atof(linecomp.at(3).c_str()), 1.0);
        //shininess
        //since Ns ranges 0 - 1000 but OpenGL expects a 0-128 range we have to adjust it.
        } else if (linecomp.at(0) == "Ns") {
            shininess = (GLfloat)atof(linecomp.at(1).c_str());
            shininess /= 7.8125;
        //texture map, it doesn't matter which you take if there's only one for us...
        } else if (linecomp.at(0) == "map_Kd" || linecomp.at(0) == "map_Ka" || linecomp.at(0) == "map_Ks") {
            vector<string> filePath = splitString(linecomp.at(1), "/\n");
            tFilename = filePath.at(filePath.size() - 1);
        }
    }
    matfile.close();
    loadTexture(tFilename);
}

void ObjImporter::loadTexture(string texFilename) {
    TextureFromPPM texture(texFilename);

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, tex_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    const unsigned char* imageData = texture.getTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.getWidth(), texture.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
}


Polyhedron ObjImporter::createPolyhedron() {
   Polyhedron retPol(vertices, normals, textureV, tex_id, ambient, diffuse, specular, shininess);
    return retPol;
}


void ObjImporter::sortVertices() {
    vector<Vector3f> orderedVertices;
    vector<Vector3f> orderedNormals;
    vector<Vector2f> orderedTextureV;
    
    for (size_t x = 0; x < faces.size(); x++) {
        orderedVertices.push_back(Vector3f(vertices.at((faces.at(x).v[0]) -1)));
        orderedVertices.push_back( Vector3f(vertices.at((faces.at(x).v[1]) -1)) );
        orderedVertices.push_back(Vector3f(vertices.at((faces.at(x).v[2]) -1)));
        
        orderedNormals.push_back(Vector3f(normals.at((faces.at(x).n[0]) -1)));
        orderedNormals.push_back(Vector3f(normals.at((faces.at(x).n[1]) -1)));
        orderedNormals.push_back(Vector3f(normals.at((faces.at(x).n[2]) -1)));
        
        orderedTextureV.push_back(Vector2f(textureV.at((faces.at(x).t[0]) -1)));
        orderedTextureV.push_back(Vector2f(textureV.at((faces.at(x).t[1]) -1)));
        orderedTextureV.push_back(Vector2f(textureV.at((faces.at(x).t[2]) -1)));
    }
    
    vertices = orderedVertices;
    normals = orderedNormals;
    textureV = orderedTextureV;
}


ObjImporter::~ObjImporter() {}


vector<string> ObjImporter::splitString(string s, string delims) {
    stringstream temp(stringstream::in | stringstream::out);
    vector<string> elems(0);
    bool found;
    if (s.size() == 0) {
        return elems;
    }
    if (delims.size() == 0) {
       elems.push_back(s);
       return elems;
    }
    //look at every letter of s
    for (size_t i = 0; i < s.size(); i++) {
        found = false;
        //look for delimiter
        for (size_t j = 0; j < delims.size(); j++) {
            if(s.at(i) == delims.at(j)) {
                //only write stream to string in vector when stream not empty
                if(temp.str().size() > 0) {
                    elems.push_back(temp.str());
                }
                temp.str("");
                found = true;
                break;
            }
        }
        // if s.at(i) is no delimiter write it in the stream
        if(!found)
            temp << s.at(i);
    }
    if(temp.str().size() > 0)
        elems.push_back(temp.str());
    return elems;
}
