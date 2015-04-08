/**
 * @file Polyhedron.h
 * @date 2012/06/25
 * @author Julius Peinelt
 *
 * Copyright Julius Peinelt
 *
 * A Polyhedron represents a 3D-object with a texture and material properties.
 */

#ifndef POLYEDER_H
#define	POLYEDER_H

#include <GL/freeglut.h>
#include "../Helper/jMath.h"
#include "../Importer/TextureFromPPM.h"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>

using namespace std;

class Polyhedron {
private:

    Matrix<GLfloat> _stateMatrix;
    vector<Vector3f> _vertices;
    vector<Vector3f> _normals;
    vector<Vector2f> _textureV;
    GLuint _texId;
    
    Vector4<GLfloat> _ambient;
    Vector4<GLfloat> _diffuse;
    Vector4<GLfloat> _specular;
    GLfloat _shininess;
    
    bool isVisible;



public:
    
    /**
     * Creates a Polyhedron with given vertices, normals, etc.
     * @param vertices Vertices of the Polyhedron
     * @param normals Normals to compute the lightning
     * @param textureV Texture-coordinates
     * @param tex_id Texture-id which is saved in the video memory
     * @param ambient Ambient-component of the material
     * @param diffuse Diffuse-component of the material
     * @param specular Specular-component of the material
     * @param shininess Shininess factor of the material
     */
    Polyhedron(const vector<Vector3f> vertices, const vector<Vector3f> normals, 
        const vector<Vector2f> textureV, GLuint tex_id, 
            const Vector4<GLfloat> ambient, const Vector4<GLfloat> diffuse,
            const Vector4<GLfloat> specular, GLfloat shininess);
    
    
    

    virtual ~Polyhedron() {};
    
    /**
     * Relocates and rotates the Polyhedron. All parameters are absolute values.
     * @param x new x-Position
     * @param y new y-Position
     * @param z new z-Position
     * @param xDeg new absolute rotation around the local object x-Axis
     * @param yDeg new absolute rotation around the local object y-Axis
     * @param zDeg new absolute rotation around the local object z-Axis
     */
    void update_pos(GLfloat x, GLfloat y, GLfloat z, GLfloat xDeg, GLfloat yDeg, GLfloat zDeg);
    
    /**
     * Draws the Polyhedron.
     */
    void display();
    
    /**
     * Sets if the Polyhedron should be drawn
     */
    void setIsVisible(bool b);


};

#endif	/* POLYEDER_H */

