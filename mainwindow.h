#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "DrawingArea.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void saveToFile(); // изменяем сигнатуру методов для сохранения и загрузки
    void loadFromFile();
private:
    DrawingArea *drawingArea;
     Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H

