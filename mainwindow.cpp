#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "math.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srand(QDateTime::currentMSecsSinceEpoch());//случайные числа

    scene = new QGraphicsScene(this);//создаем сцену
    ui->graphicsView->setScene(scene);//задаем сцену для GraphicsView
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::load()//загрузка из XML
{
    points.clear();//очищаем точки, если есть
    QXmlStreamReader xmlReader;//входной поток
    QFile file("data.xml");//файл

    if (!file.open(QIODevice::ReadOnly))//если не удалось открыть файл
    {
        QMessageBox::warning(0, "Error!", "Error opening file");
    }
    else
    {
        xmlReader.setDevice(&file);//привязываем файл к потоку
        while (!xmlReader.atEnd() && !xmlReader.hasError()){//пока есть поля, читаем
            QXmlStreamReader::TokenType token = xmlReader.readNext();//поле
            if(token == QXmlStreamReader::StartElement && xmlReader.name().toString() == "point"){ //если это начало элемента и одновременно  имя равно point (исходя из структуры xml , чтобы он поле data не проверял, там все равно нет еще значений.)
                int x = xmlReader.attributes().value("x").toInt();//читаем аттрибуты
                int y = xmlReader.attributes().value("y").toInt();
                QPointF p(x,y);//создаем точку
                points.append(p);//добавляем ее в список
            }
        }
    }

    ui->label->setText("Всего точек: " + QString::number(points.length()));//выводим кол-во
}

void MainWindow::graph() //строим кривую
{

    double length = 0;
    double AllLength = 0;
    double min = 0;
    int minindex = 0;
        QVector<QPointF> p(points);
        if (p.empty()) return; //выходим из метода, чтобы наша программа принудительно не закрылась из-за отсутствия загруженных точек.
        QPainterPath path;//путь
        path.moveTo(p[0]); // перемещаемся к 1 нашей точке
        for(int i = 0 ; i < p.length(); i++)//проходим все точки(начало отрезка)
        {
            if (i == p.length() - 1)  // соединим последнюю точку с первой
            {
                path.lineTo(p[0]);
                scene->addPath(path);
                break;
            }
            //  min и length ищем одинаково (Исходя из теоремы пифагора , расстояние между точками можно посчитать как корень из суммы квадратов двух катетов)
            min =  sqrt((fabs(p[i].x() - p[i + 1].x()) * fabs(p[i].x() - p[i + 1].x())) + (fabs(p[i].y() - p[i + 1].y()) * fabs(p[i].y() - p[i + 1].y())));
            for (int j =  i + 1; j < p.length(); j++)
            {
                length = sqrt((fabs(p[j].x() - p[i].x()) * fabs(p[j].x() - p[i].x())) + (fabs(p[j].y() - p[i].y()) * fabs(p[j].y() - p[i].y())));
                if ( length < min) //если находим точку ближе , то ей и присваиваем статус минимальной
                {
                    min = length;
                    minindex = j;
                }
            }
               path.lineTo(p[minindex]); // двигаемся к самой близкой точке
               AllLength +=length; // к общей длине прибавляем длину текущего отрезка

        }



    ui->label_2->setText("Длина: " + QString::number(AllLength));//выводим длину (в пикселях).
}

void MainWindow::save()
{
    QXmlStreamWriter xmlWriter;
    QFile file("data.xml");//наш файл

    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0, "Error!", "Error opening file");
    }
    else
    {
        xmlWriter.setDevice(&file);
        xmlWriter.writeStartDocument();//выводим начало документа
        xmlWriter.writeStartElement("data");//контейнер для точек
        for(int i = 0; i < points.length(); i++){//выводим все точки из массива
            xmlWriter.writeStartElement("point");//начало элемента
            xmlWriter.writeAttribute("x",QString::number((int)points[i].x()));//в аттрибуты выводим точки
            xmlWriter.writeAttribute("y",QString::number((int)points[i].y()));
            xmlWriter.writeEndElement();//конец элемента
        }
        xmlWriter.writeEndElement();//конец элемента data
        xmlWriter.writeEndDocument();//конец документа
    }
}

void MainWindow::draw()
{
    scene->clear();
    QVector<QPointF> p(points);
    QVector<QPainterPath> paths;//массив отрезков

    float total = p.length();

    for(int i = 0; i < p.length(); i++)//проходим все точки(начало отрезка)
    {
        for(int j = i + 1; j < p.length(); j++)//конец отрезка
        {
            QApplication::processEvents();

            QPainterPath path;//путь
            path.moveTo(p[i]);//задаем начало и конец
            path.lineTo(p[j]);
            if(!intersects(paths,path)){//если не пересекает ничего

                float percent = (i / total) * 100.0f;
                ui->pushButton_4->setText(QString::number(percent,'f',1) + "%");

                paths.append(path);//добавим
                scene->addPath(paths.last());//добавим в сцену
                break; //нет смысла дальше проверять (так как с этой точкой (i = N уже нашли конец отрезка в j цикле)).
            }
        }
    }
    ui->pushButton_4->setText("Построить");
}

void MainWindow::generate()
{
    int n = this->ui->spinBox->value();//читаем значение спиннера
    int maxY = this->ui->graphicsView->height() - 20;//задаем границы для генерации точек
    int maxX = this->ui->graphicsView->width() - 20;
    int x,y;
    while(n > 0){//пока не пройдем все точки
        x=  rand()%maxX + 10;//генерируем
        y = rand()%maxY + 10;
        QPointF p(x,y);
        if(!points.contains(p)){//если такой точки нет то
            points.append(p);//добавим ее
            n--;
        }
    }
    ui->label->setText("Всего точек: " + QString::number(points.length()));//выведем кол-во
}

bool MainWindow::intersects(QVector<QPainterPath> v, QPainterPath p)//проверка пересечений
{

    for(int i = 0; i < v.length(); i++)//проверяем пересечения путей
        if(p.intersects(v[i]))
            return true;
    return false;

}




void MainWindow::on_pushButton_clicked()
{
    load();
}

void MainWindow::on_pushButton_2_clicked()
{
    save();
}

void MainWindow::on_pushButton_3_clicked()
{
    generate();
}

void MainWindow::on_pushButton_4_clicked()
{
    draw();
}

void MainWindow::on_pushButton_5_clicked()
{
    scene->clear();
    points.clear();
    ui->label->setText("Всего точек: " + QString::number(points.length()));//выведем кол-во
}

void MainWindow::on_pushButton_6_clicked()
{
    graph();
}


