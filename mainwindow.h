#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QDateTime>
#include <QXmlStreamWriter>
#include <QFile>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QDebug>
#include <QPointF>
#include <QPen>
#include <QBrush>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void load();
    void save();
    void draw();
    void generate();
    void graph();
    bool intersects(QVector<QPainterPath> v,QPainterPath p);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QPointF> points;//массив точек
    QGraphicsScene *scene;//сцена, на которой рисуем

};

#endif // MAINWINDOW_H
