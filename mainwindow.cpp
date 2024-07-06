#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolBar>
#include <QAction>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), drawingArea(new DrawingArea(this)) {
    setWindowTitle("Drawing Application");
    setGeometry(100, 100, 800, 600);

    drawingArea = new DrawingArea(this);
    setCentralWidget(drawingArea);

    QToolBar *toolbar = addToolBar("Drawing Tools");

    QAction *rectangleAction = new QAction("Прямоугольник", this);
    connect(rectangleAction, &QAction::triggered, [this]() { drawingArea->setShape("rectangle"); });
    toolbar->addAction(rectangleAction);

    QAction *triangleAction = new QAction("Треугольник", this);
    connect(triangleAction, &QAction::triggered, [this]() { drawingArea->setShape("triangle"); });
    toolbar->addAction(triangleAction);

    QAction *ellipseAction = new QAction("Эллипс", this);
    connect(ellipseAction, &QAction::triggered, [this]() { drawingArea->setShape("ellipse"); });
    toolbar->addAction(ellipseAction);

    QAction *connectionAction = new QAction("Связь", this);
    connect(connectionAction, &QAction::triggered, [this]() { drawingArea->setShape("connection"); });
    toolbar->addAction(connectionAction);

    QAction *moveAction = new QAction("Переместить", this);
    connect(moveAction, &QAction::triggered, [this]() { drawingArea->setShape("move"); });
    toolbar->addAction(moveAction);

   QAction *deleteAction = new QAction("Удалить", this);
    connect(deleteAction, &QAction::triggered, [this]() { drawingArea->setShape("delete"); });
    toolbar->addAction(deleteAction);


    QAction *saveAction = new QAction("Сохранить", this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveToFile); // подключаем сигнал сохранения
    toolbar->addAction(saveAction);

    QAction *loadAction = new QAction("Загрузить", this);
    connect(loadAction, &QAction::triggered, this, &MainWindow::loadFromFile); // подключаем сигнал загрузки
    toolbar->addAction(loadAction);

    drawingArea->setFocusPolicy(Qt::StrongFocus);
    drawingArea->setFocus();
}


MainWindow::~MainWindow() {
     delete drawingArea;
}

void MainWindow::saveToFile() {
    QString filename = QFileDialog::getSaveFileName(this, "Сохранить файл", QDir::homePath(), "Text files (*.txt)");
    if (!filename.isEmpty()) {
        drawingArea->saveToFile(filename);
    }
}

void MainWindow::loadFromFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Загрузить файл", QDir::homePath(), "Text files (*.txt)");
    if (!filename.isEmpty()) {
        drawingArea->loadFromFile(filename);
    }
}





/*
MainWindow::~MainWindow()
{
    delete ui;
}
*/

