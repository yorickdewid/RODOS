/* 
 * File:   Visualisation.cpp
 * Author: julius
 * 
 * Created on April 19, 2012, 2:18 PM
 */



#include "Visualisation.h"

Visualisation::Visualisation(string name, GLfloat x_pos, GLfloat y_pos, int width, int height) {    
    glutInitWindowSize(width, height);
    glutInitWindowPosition(x_pos, y_pos);
    _id = glutCreateWindow(name.c_str());
    
    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    //glFrontFace(GL_CCW);
	glEnable(GL_DEPTH_TEST);									
    glEnable(GL_CULL_FACE);
	
	float ambient[] = {0.8, 0.8, 0.8, 1.0};
	float diffuseAndSpecular[] = {1.0, 1.0, 1.0, 1.0};
	float position[] = {-1.0, 2.0, 0.0, 0.0};
    	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseAndSpecular);
    glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseAndSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
    
    //View
    glViewport( 0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0, width / height, 1.0, 40.0 );
	glMatrixMode( GL_MODELVIEW );
}

Visualisation::Visualisation(const Visualisation& orig) {
}

Visualisation::~Visualisation() {
}

void Visualisation::display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (size_t i = 0; i < _polyeders.size(); i++) {
        _polyeders.at(i).display();
    }
    glutSwapBuffers();
}


void Visualisation::refresh() {
    glutSetWindow(_id);
	glutPostRedisplay();
}

void Visualisation::idle() {
    display(); 
}

int Visualisation::createPolyhedron(string& filename) {
    ObjImporter importer(filename);
    Polyhedron poly = importer.createPolyhedron();
    _polyeders.push_back(poly);
    return _polyeders.size();
}


void Visualisation::updatePolyhedron(size_t id, GLfloat x_pos, GLfloat y_pos, GLfloat z_pos, GLfloat x_rot, GLfloat y_rot, GLfloat z_rot) {
    if (id > 0 && id <= _polyeders.size()) {
	_polyeders.at(id - 1).update_pos(x_pos, y_pos, z_pos, x_rot, y_rot, z_rot);
    }
}
