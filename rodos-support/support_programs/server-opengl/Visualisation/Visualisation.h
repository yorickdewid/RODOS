/**
 * @file Visualisation.h
 * @date 2012/06/25
 * @author Julius Peinelt
 *
 * Copyright Julius Peinelt
 *
 * The Visualisation is responsible for creating Windows and displaying and
 * sending update and creation requests to/for 3D-objects. 
 */

#ifndef VISUALISATION_H

#include <GL/freeglut.h>
#include "Polyhedron.h"
#include <string>
#include <vector>
#include "../Importer/ObjImporter.h"
#include <cstdio>

#define	VISUALISATION_H

using namespace std;

class Visualisation {
private:
    int _id;
    string _name;
    vector<Polyhedron> _polyeders;
    
    
public:

    /**
     * Creates a Window with the specified width and hight on the given position and title.
     * It also initializes the OpenGL context.
     * @param name Window title
     * @param x_pos x-Position of the upper left corner of the window
     * @param y_pos y-Position of the upper left corner of the window
     * @param width width of the window
     * @param height height of the window.
     */
    Visualisation(string name, GLfloat x_pos, GLfloat y_pos, int width, int height);
    
    Visualisation(const Visualisation& orig);
    
    virtual ~Visualisation();

    /**
     * Clears the view and initiates the displaying of all Polyhedrons.
     */
    void display();
    
    /**
     * Sets the OpenGL context to "this" and calls display().
     */
    void refresh();
    
    /**
     * Calls display().
     */
    void idle();
    
    /**
     * Creates a Polyhedron based on the specified filename.
     * @param filename Path to a wavefront .obj file
     */
    int createPolyhedron(string& filename);
    
    /**
     * Relocates and rotates a Polyhedron, specified by the id.
     * @param id Id of the Polyhedron
     * @param x_pos new absolute x-Position
     * @param y_pos new absolute y-Position
     * @param z_pos new absolute z-Position
     * @param x_rot new absolute rotation around the local object x-Axis
     * @param y_rot new absolute rotation around the local object y-Axis
     * @param z_rot new absolute rotation around the local object z-Axis
     */
    void updatePolyhedron(size_t id, GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, GLfloat x_rot, GLfloat y_rot, GLfloat z_rot);
	
};

#endif	/* VISUALISATION_H */

