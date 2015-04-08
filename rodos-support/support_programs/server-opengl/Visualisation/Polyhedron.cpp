



#include "Polyhedron.h"

Polyhedron::Polyhedron(const vector<Vector3f> vertices, const vector<Vector3f> normals, 
        const vector<Vector2f> textureV, GLuint tex_id, 
            const Vector4<GLfloat> ambient, const Vector4<GLfloat> diffuse,
            const Vector4<GLfloat> specular, GLfloat shininess) {
    
    isVisible = true;
    
    Matrix<GLfloat> stateMatrix;   //is init with identity matrix
    
    _vertices = vertices;
    _normals = normals;
    _textureV = textureV;
    _texId = tex_id;
    
    _ambient = ambient;
    _diffuse = diffuse;
    _specular = specular;
    _shininess = shininess;
}

void Polyhedron::update_pos(GLfloat x, GLfloat y, GLfloat z, GLfloat xDeg, GLfloat yDeg, GLfloat zDeg) {

    _stateMatrix.setTranslation(Vector3<GLfloat>(x, y, z));
    Matrix<GLfloat> tempM = Matrix<GLfloat>::createRotationAroundAxis(xDeg, yDeg, zDeg);
    _stateMatrix.setRotation(tempM);
}


void Polyhedron::display() {
    if (!isVisible) {
        return;
    }
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, _ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, _diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, _specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _shininess);
	
    glLoadMatrixf(_stateMatrix);
    glPushMatrix();
     
    glBindTexture(GL_TEXTURE_2D, _texId);
    
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_TRIANGLES);
    for (size_t x = 0; x < _vertices.size(); x++) {

        glNormal3fv((GLfloat *) &_normals.at(x));
        glTexCoord2fv((GLfloat *) &_textureV.at(x));
        glVertex3fv((GLfloat *) &_vertices.at(x));
        
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
    
    glPopMatrix();
}

void Polyhedron::setIsVisible(bool b) {
    isVisible = b;
}
