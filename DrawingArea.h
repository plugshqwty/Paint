
#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QString>
#include <QVector>
#include <QDebug>
#include <QFile>
#include <QTextStream>
class DrawingArea : public QWidget {
    Q_OBJECT

public:
    DrawingArea(QWidget *parent = nullptr);
    void setShape(const QString &shape);
    int selectedShapeIndex;
    int getSelectedShapeIndex() const;



public slots:
    void deleteSelectedShape(int selectedShapeIndex);
    void saveToFile(const QString &filename);
    void loadFromFile(const QString &filename);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    struct Shape {
        QString type;
        QRect rect;
        bool operator==(const Shape &other) const {
            return type == other.type && rect == other.rect;
        }
    };

    struct Connection {
        QPoint start;
        QPoint end;
    };

    QString currentShape;
    bool drawing;
    bool moving;
    QPoint startPoint;
    QPoint endPoint;
    QPoint moveStartPoint;
    Shape* selectedShape;

    QVector<Shape> shapes;
    QVector<Connection> connections;

    QRect getRectFromPoints(const QPoint &p1, const QPoint &p2);
    Shape* getShapeAt(const QPoint &point);
    void moveShape(const QPoint &offset);
    void selectShape(Shape *shape);
};

#endif // DRAWINGAREA_H
