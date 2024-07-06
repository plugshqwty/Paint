
#include "DrawingArea.h"
#include <QFile>
#include <QTextStream>

DrawingArea::DrawingArea(QWidget *parent) : QWidget(parent), drawing(false), moving(false), selectedShape(nullptr), selectedShapeIndex(-1) {}

void DrawingArea::setShape(const QString &shape) {
    currentShape = shape;
}
int DrawingArea::getSelectedShapeIndex() const {
    return selectedShapeIndex;
}
void DrawingArea::saveToFile(const QString &filename) {

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Could not open file for writing:" << file.errorString();
        return;
    }

    QTextStream out(&file);

    // Сохранение фигур
    for (const Shape &shape : shapes) {
        out << shape.type << " " << shape.rect.x() << " " << shape.rect.y() << " " << shape.rect.width() << " " << shape.rect.height() << "\n";
    }

    // Сохранение связей
    for (const Connection &connection : connections) {
        out << connection.start.x() << " " << connection.start.y() << " " << connection.end.x() << " " << connection.end.y() << "\n";
    }

    file.close();
}

void DrawingArea::loadFromFile(const QString &filename) {

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Could not open file for reading:" << file.errorString();
        return;
    }

    QTextStream in(&file);
    shapes.clear();
    connections.clear();

    QString line;
    while (!in.atEnd()) {
        line = in.readLine().trimmed();

            qDebug() << "READ SHAPES";
                QStringList parts = line.split(" ");
                if (parts.size() == 5) {
                    QString type = parts[0];
                    int x = parts[1].toInt();
                    int y = parts[2].toInt();
                    int width = parts[3].toInt();
                    int height = parts[4].toInt();
                    shapes.append({type, QRect(x, y, width, height)});
                }

                QStringList parts1 = line.split(" ");
                if (parts.size() == 4) {
                    int startX = parts1[0].toInt();
                    int startY = parts1[1].toInt();
                    int endX = parts1[2].toInt();
                    int endY = parts1[3].toInt();
                    Connection connection;
                    connection.start = QPoint(startX, startY);
                    connection.end = QPoint(endX, endY);
                    connections.append(connection);
                    connections.append({QPoint(startX, startY), QPoint(endX, endY)});
            // }
          // }
       // }
                }}

    file.close();
    update();
}

void DrawingArea::deleteSelectedShape(int selectedShapeIndex) {


    qDebug() << "Deleting shape at index:" << selectedShapeIndex;
    if (selectedShapeIndex != -1 && selectedShapeIndex < shapes.size()) {
        qDebug() << "Deleting shape:" << shapes[selectedShapeIndex].type;

        // Удаление связей, связанных с удаляемой фигурой
        QVector<Connection> updatedConnections;
        for (const Connection &connection : connections) {
            if (!(shapes[selectedShapeIndex].rect.contains(connection.start) || shapes[selectedShapeIndex].rect.contains(connection.end))) {
                updatedConnections.append(connection);
            }
        }
        connections = updatedConnections;
        // Удаление выбранной фигуры
        shapes.remove(selectedShapeIndex);

        selectedShapeIndex = -1;
        selectedShape = nullptr; // Сброс выбранной фигуры
        update();
    } else {
        qDebug() << "No shape selected to delete.";
    }
}

void DrawingArea::mousePressEvent(QMouseEvent *event) {
    if (currentShape == "connection" && event->button() == Qt::LeftButton) {
        Shape *shape = getShapeAt(event->pos());
        if (shape) {
            if (!drawing) {
                drawing = true;
                startPoint = shape->rect.center();
                qDebug() << "Connection started at:" << startPoint;
            } else {
                if (shape->rect.contains(startPoint)) {
                    drawing = false;
                } else {
                    endPoint = shape->rect.center();
                    connections.append({startPoint, endPoint});
                    qDebug() << "Connection ended at:" << endPoint;
                    drawing = false;
                }
            }
            update();
        } else {
            drawing = false;
        }
    } else if (currentShape == "move" && event->button() == Qt::LeftButton) {
        Shape *shape = getShapeAt(event->pos());
        if (shape) {
            moving = true;
            moveStartPoint = event->pos();
            selectShape(shape); // Выбор фигуры
          //  deleteSelectedShape(selectedShapeIndex);
            setCursor(Qt::ClosedHandCursor);
            qDebug() << "Shape selected:" << shape->type << "at" << shape->rect;
        } else {
            qDebug() << "No shape selected.";
        }
    }
 else if (currentShape == "delete" && event->button() == Qt::LeftButton) {
        Shape *shape= getShapeAt(event->pos());
        if (shape) {
         //   moving = true;
           // moveStartPoint = event->pos();
            selectShape(shape); // Выбор фигуры
            deleteSelectedShape(selectedShapeIndex);
          //  setCursor(Qt::ClosedHandCursor);
            qDebug() << "Shape selected:" << shape->type << "at" << shape->rect;
        } else {
            qDebug() << "No shape selected.";
        }
    }

    else if (event->button() == Qt::LeftButton) {

        drawing = true;
        startPoint = event->pos();
    } else if (event->button() == Qt::RightButton) {
        drawing = false;
        moving = false;
        setCursor(Qt::ArrowCursor);
        update();
    }
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event) {
    if (drawing) {
        endPoint = event->pos();
        update();
    } else if (moving && selectedShape) {
        QPoint offset = event->pos() - moveStartPoint;
        moveStartPoint = event->pos();
        moveShape(offset);
        update();
    }
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event) {

    if (event->button() == Qt::LeftButton && drawing) {
        if (currentShape != "connection") {
            drawing = false;
            endPoint = event->pos();

            QRect rect = getRectFromPoints(startPoint, endPoint);
            shapes.append({currentShape, rect});
            update();
        } else {
            // Проверка, попадает ли конец связи на фигуру
            endPoint = event->pos();
            Shape *endShape = getShapeAt(endPoint);
            if (!endShape) {
                // Если не попадает на фигуру, удалить связь
                if (!connections.isEmpty()) {
                 //   connections.removeLast();
                    qDebug() << "Connection removed, end point not on shape";
                }
            } else {
                // Добавление связи
                connections.append({startPoint, endShape->rect.center()});
            }
            drawing = false;
            update();
        }
    } else if (event->button() == Qt::LeftButton && moving) {
        moving = false;
        selectedShapeIndex = -1;
        selectedShape = nullptr; // Сброс выбранной фигуры
        setCursor(Qt::ArrowCursor);
    }

}

void DrawingArea::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        drawing = false;
        moving = false;
        selectedShapeIndex = -1;
        selectedShape = nullptr; // Сброс выбранной фигуры
        setCursor(Qt::ArrowCursor);
        update();
    } else if (event->key() == Qt::Key_Delete) {

        qDebug() << "Shape selected at index:" << selectedShapeIndex;
        deleteSelectedShape(selectedShapeIndex);
    }
}

void DrawingArea::paintEvent(QPaintEvent *event) {

    QWidget::paintEvent(event);
    QPainter painter(this);

    // Перерисовка всех сохраненных фигур
    for (const Shape &shape : shapes) {
        if (shape.type == "rectangle") {
            painter.drawRect(shape.rect);
        } else if (shape.type == "triangle") {
            QPoint points[3] = {
                QPoint((shape.rect.left() + shape.rect.right()) / 2, shape.rect.top()),
                QPoint(shape.rect.left(), shape.rect.bottom()),
                QPoint(shape.rect.right(), shape.rect.bottom())
            };
            painter.drawPolygon(points, 3);
        } else if (shape.type == "ellipse") {
            painter.drawEllipse(shape.rect);
        }
    }

    // Перерисовка всех сохраненных связей
    QVector<Connection> validConnections;
    for (const Connection &connection : connections) {
        Shape *startShape = getShapeAt(connection.start);
        Shape *endShape = getShapeAt(connection.end);
        if (startShape && endShape) {
            painter.drawLine(connection.start, connection.end);
            validConnections.append(connection);
        }
    }
    connections = validConnections;

    // Отрисовка текущей фигуры
    if (drawing) {
        if (currentShape == "connection") {
            painter.drawLine(startPoint, endPoint);
        } else {
            QRect rect = getRectFromPoints(startPoint, endPoint);
            if (currentShape == "rectangle") {
                painter.drawRect(rect);
            } else if (currentShape == "triangle") {
                QPoint points[3] = {
                    QPoint((startPoint.x() + endPoint.x()) / 2, startPoint.y()),
                    QPoint(startPoint.x(), endPoint.y()),
                    QPoint(endPoint.x(), endPoint.y())
                };
                painter.drawPolygon(points, 3);
            } else if (currentShape == "ellipse") {
                painter.drawEllipse(rect);
            }
        }
    }
}

QRect DrawingArea::getRectFromPoints(const QPoint &p1, const QPoint &p2) {
    return QRect(p1, p2).normalized();
}

void DrawingArea::moveShape(const QPoint &offset) {
    if (selectedShape) {
        selectedShape->rect.translate(offset);

        // Обновление связей
        for (Connection &connection : connections) {
            if (connection.start == selectedShape->rect.center() - offset) {
                connection.start = selectedShape->rect.center();
            }
            if (connection.end == selectedShape->rect.center() - offset) {
                connection.end = selectedShape->rect.center();
            }
        }
    }
}

void DrawingArea::selectShape(Shape *shape) {
    selectedShape = shape;
    for (int i = 0; i < shapes.size(); ++i) {
        if (&shapes[i] == shape) {
            selectedShapeIndex = i;
            qDebug() << "Shape selected at index:" << selectedShapeIndex;
            return;
        }
    }
    //selectedShapeIndex = -1;
}

DrawingArea::Shape* DrawingArea::getShapeAt(const QPoint &point) {
 /*   for (Shape &shape : shapes) {
        if (shape.rect.contains(point)) {
            return &shape;
        }
    }
    return nullptr;*/
    for (Shape &shape : shapes) {
        if (shape.rect.contains(point)) {
            return &shape;
        }
    }
    return nullptr;
}
