#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "renderwindow.h"
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QTextEdit>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mOutput = new QTextEdit;
    mOutput->setReadOnly(true);
    start();
}

MainWindow::~MainWindow()
{
    delete mRenderWindow;
    delete ui;
}

void MainWindow::start()
{
    QSurfaceFormat fmt;

    fmt.setVersion(4, 1);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setRenderableType(QSurfaceFormat::OpenGL);

    // The example rendering will need a depth buffer.
    fmt.setDepthBufferSize(16);

    mOutput->clear();
    qDebug() << "Requesting surface format" << fmt;

    mRenderWindow = new RenderWindow(fmt, this);
    if (!mRenderWindow->context()) {
        mOutput->append(tr("Failed to create context"));
        delete mRenderWindow;
        mRenderWindowContainer = new QWidget;
        addRenderWindow();
        return;
    }
    mSurface = mRenderWindow;

    connect(mRenderWindow, &RenderWindow::ready, this, &MainWindow::renderWindowReady);
    connect(mRenderWindow, &RenderWindow::error, this, &MainWindow::renderWindowError);

    mRenderWindowContainer = QWidget::createWindowContainer(mRenderWindow);
    addRenderWindow();
}

void MainWindow::renderWindowReady()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);
}

void MainWindow::renderWindowError(const QString &msg)
{
    mOutput->append(tr("An error has occurred:\n%1").arg(msg));
}

void MainWindow::addRenderWindow()
{
    ui->renderLayout->addWidget(mRenderWindowContainer);
}
