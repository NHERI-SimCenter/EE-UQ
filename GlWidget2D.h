#ifndef GL_WIDGET_2D_H
#define GL_WIDGET_2D_H

/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);

//#include <QGLWidget>
#include <QVector2D>
#include <QTimer>
#include <QMatrix4x4>

class Controller2D;

typedef struct _vertexStruct
{
    GLfloat position[3];
    GLfloat color[3];
} vertexStruct;


class GlWidget2D : public QOpenGLWidget, protected QOpenGLFunctions {


    Q_OBJECT // must include this if you use Qt signals/slots

public:
  GlWidget2D(QWidget *parent = NULL);
  ~GlWidget2D();
  void setController(Controller2D *);
  
  void update();
  void reset();
  void drawLine(int tag, float x1, float y1, float x2, float y2, float thick, float r, float g, float b);
  void drawPoint(int tag, float x1, float y1, int numPixels, float r, float g, float b);
  void drawText(int tag, float x1, float y1, char *text, float r, float g, float b);
  
  public slots:
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *);
  void mouseSingleClickEvent(void);
  
public:

  void drawBuffers();

 protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    
    /*
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    */
    void keyPressEvent(QKeyEvent *event);

    int selectMode;

    Controller2D *theController; // pointer to class that has the draw command

    QPointF mousePressPosition;
    QPointF mouseReleasePosition;
    int doubleClicked;
    QTimer timer;
    bool clickedLeft;
    
    int *pointIDs;
    int numPoint, maxNumPoint;
    GLfloat *pointVertices;
    GLfloat *pointColors;
    // GLfloat *pointTagColors;
    
    int *lineIDs;
    int numLine, maxNumLine;
    GLfloat *lineVertices;
    GLfloat *lineColors;
    // GLfloat *lineTagColors;

    // new GL stuff
    QOpenGLShaderProgram *program;
    GLuint h;
    GLuint w;
    GLuint vPosition;
    GLuint vColor;
    GLuint mvpMatrix;
    GLuint pointBuffer;
    GLuint lineBuffer;
    vertexStruct *thePointVertices;
    vertexStruct *theLineVertices;
    QMatrix4x4   orthoProjectionMatrix;
};




#endif // GL_WIDGET_2D_H
