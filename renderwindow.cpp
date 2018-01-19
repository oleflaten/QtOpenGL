#include "renderwindow.h"
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QStatusBar>

#include "shader.h"
#include "mainwindow.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(0),
      mInitialized(false),
      mMove(0.0f),
      mMainWindow(mainWindow)

{
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    mContext = new QOpenGLContext(this);
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = 0;
    }
}

RenderWindow::~RenderWindow()
{
    glDeleteVertexArrays( 1, &containerVAO );
    glDeleteBuffers( 1, &vertexBufferObject );
    glDeleteBuffers( 1, &colorbuffer );

}

//Simple global for vertices - should belong to a class
static GLfloat vertices[] = {
    0.0f, 0.707f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
};

//Simple global for colors - should belong to a class
static GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

void RenderWindow::init()
{
    initializeOpenGLFunctions();

    QSurfaceFormat format = mContext->format();

    //Compile shaders:
    mShaderProgram = new Shader("../QtOpenGL/plainvertex.vert", "../QtOpenGL/plainfragment.frag");


    //mMatrixUniform = glGetUniformLocation( mShaderProgram->Program, "matrix" ); // enable in shader and in render() function also to use matrix

    //Vertex Array  Object
    glGenVertexArrays( 1, &containerVAO );
    glBindVertexArray( containerVAO );

    //Vertex Buffer Object to hold vertices
    glGenBuffers( 1, &vertexBufferObject );
    glBindBuffer( GL_ARRAY_BUFFER, vertexBufferObject );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
                0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );

    //Buffer to hold colors:
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
                );

    glBindVertexArray( 0 );     //unbind VAO
}

void RenderWindow::render()
{
    if (!mContext->makeCurrent(this)) {
        emit error(tr("makeCurrent() failed"));
        return;
    }

    initializeOpenGLFunctions();
    //QOpenGLFunctions *f = m_context->functions(); //can do this instead of initializeOpenGLFunctions();,
                                                    //but then all gl functions must have a f-> prefix
    if (!mInitialized) {
        mInitialized = true;
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f,1.0f);
        init();
        emit ready();
    }

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram( mShaderProgram->Program );

/*    Uncomment this to use a matrix - also in vertex shader and in
//    GLfloat modelviewmatrix[] = {
//        1.0f , 0.0f , 0.0f , m_angle,
//        0.0f, 1.0f, 0.0f , 0.0f ,
//        0.0f, 0.0f, 1.0f, 0.0f,
//        0.0f, 0.0f, 0.0f, 1.0f
//    };

//    glUniformMatrix4fv( m_matrixUniform, 1, GL_FALSE, modelviewmatrix);
*/
    glBindVertexArray( containerVAO );
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray( 0 );
    GLenum err = GL_NO_ERROR;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "glGetError returns " << err;
    }


    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    mContext->swapBuffers(this);

    std::cout << mMove; //Just writing this out to see that this functions is called repeatedly

    mMove += 0.01f;


/*
 * Old timer - the new is calling timerEvent() and is started in exposeEvent()
 * this makes the render() function to be called again and again
 * Instead of 0 wait a few more milliseconds before rendering again. This is
 * only here to make the UI widgets more responsive on slower machines. We
 *  can afford it since our rendering is so lightweight.
    const int interval = 5;
    QTimer::singleShot(interval, this, &RenderWindow::render);
*/
}

//This function is called from Qt when window is exposed / shown
void RenderWindow::exposeEvent(QExposeEvent *)
{
    if (isExposed())
    {
        mTimer.start(16, this);
        mTimeStart.start();
    }
}

void RenderWindow::changeTimer(int newTime)
{
    mTimer.stop();
    mTimer.start(newTime, this);
}

void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_A)
    {
        mMainWindow->statusBar()->showMessage(" AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA ");

    }
    if(event->key() == Qt::Key_S)
    {
        mMainWindow->statusBar()->showMessage(" SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS");

    }
}

void RenderWindow::timerEvent(QTimerEvent *)
{
    render();

    //The rest here is just to show the frame rate:
    int msSinceLastFrame = mTimeStart.restart();    //restart() returns ms since last restart.
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow)    //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 60) //once pr 60 frames =  update the message once pr second
        {
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" FrameDraw: " + QString::number(msSinceLastFrame) + " ms");
            frameCount = 0;     //reset to show a new message in 60 frames
        }
    }
}



