#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_1_Core>
#include <QBasicTimer>
#include <QTime>

class QOpenGLContext;
class Shader;
class MainWindow;

class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow();
    QOpenGLContext *context() { return mContext; }
    void exposeEvent(QExposeEvent *) override;
    void changeTimer(int newTime);

signals:
    void ready();
    void error(const QString &msg);

private slots:
    void render();

private:
    void init();

    QOpenGLContext *mContext;
    bool mInitialized;
    Shader *mShaderProgram;
    GLint  mMatrixUniform;
    GLuint containerVAO;

    GLuint vertexBufferObject;
    GLuint colorbuffer;
    float mMove;

    QBasicTimer mTimer;     //timer that drives the gameloop
    QTime mTimeStart;       //time variable that reads the actual FPS

    MainWindow *mMainWindow;


protected:
//    void mousePressEvent(QMouseEvent *event) override{}
//    void mouseMoveEvent(QMouseEvent *event) override{}
    void keyPressEvent(QKeyEvent *event) override;
//    void keyReleaseEvent(QKeyEvent *event) override{}
//    void wheelEvent(QWheelEvent *event) override{}

    void timerEvent(QTimerEvent *) override;
};

#endif // RENDERWINDOW_H
