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
    : m_context(0),
      m_initialized(false),
      m_angle(0.0f),
      mMainWindow(mainWindow)

{
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    m_context = new QOpenGLContext(this);
    m_context->setFormat(requestedFormat());
    if (!m_context->create()) {
        delete m_context;
        m_context = 0;
    }
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

    QSurfaceFormat format = m_context->format();

    //Compile shaders:
    m_program2 = new Shader("../QtOpenGL/plainvertex.vert", "../QtOpenGL/plainfragment.frag");

    //Setting up pointer into shader:
    m_colAttr = glGetUniformLocation( m_program2->Program, "colAttr" );
    m_posAttr = glGetUniformLocation( m_program2->Program, "posAttr" );
    //m_matrixUniform = glGetUniformLocation( m_program2->Program, "matrix" ); // enable in shader and in render() function

    //Vertex Array  Object
    GLuint containerVAO;
    glGenVertexArrays( 1, &containerVAO );
    glBindVertexArray( containerVAO );

    //Vertex Buffer Object to hold vertices
    glGenBuffers( 1, &VBO );
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    //Buffer to hold colors:
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
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

    int msSinceLastFrame = mTimeStart.restart();    //restart() returns ms since last restart.
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow)    //if no editor, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 60) //once pr 60 frames = once pr second
        {
            //showing some statistics in status bar
            //mMainWindow->statusBar()->showMessage(" FrameDraw in ms: " + QString::number(msSinceLastFrame));
            frameCount = 0;
        }
    }
}

void RenderWindow::render()
{
    if (!m_context->makeCurrent(this)) {
        emit error(tr("makeCurrent() failed"));
        return;
    }

    initializeOpenGLFunctions();
    //QOpenGLFunctions *f = m_context->functions();
    if (!m_initialized) {
        m_initialized = true;
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f,1.0f);
        init();
        emit ready();
    }

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram( m_program2->Program );

//    GLfloat modelviewmatrix[] = {
//        1.0f , 0.0f , 0.0f , m_angle,
//        0.0f, 1.0f, 0.0f , 0.0f ,
//        0.0f, 0.0f, 1.0f, 0.0f,
//        0.0f, 0.0f, 0.0f, 1.0f
//    };

//    glUniformMatrix4fv( m_matrixUniform, 1, GL_FALSE, modelviewmatrix);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(
                0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
                1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                3,                                // size
                GL_FLOAT,                         // type
                GL_FALSE,                         // normalized?
                0,                                // stride
                (void*)0                          // array buffer offset
                );

    glDrawArrays(GL_TRIANGLES, 0, 3);
    GLenum err = GL_NO_ERROR;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "glGetError returns " << err;
    }


    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    m_context->swapBuffers(this);

    std::cout << m_angle;

    m_angle += 0.01f;

    // Instead of 0 wait a few more milliseconds before rendering again. This is
    // only here to make the UI widgets more responsive on slower machines. We
    // can afford it since our rendering is so lightweight.
//    const int interval = 5;
//    QTimer::singleShot(interval, this, &RenderWindow::render);
}

