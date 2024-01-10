#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPainter>
#include <qmath.h>
#include <QDebug>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->widget->installEventFilter(this);

    connect(&m_timer, &QTimer::timeout, [=](){
        if(m_value >= 100)
        {
            m_timer.stop();
            ui->pushButton->setText("开始");
            return;
        }
        m_value++;
        ui->widget->repaint();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

QPointF MainWindow::getSunPosByValue(int value)
{
    double x0 = m_rect.width()*1.0 * value / 100;

    double a = m_rect.width() * 1.0 / 2;
    double b = m_rect.height() *1.0 / 2;
    double y0 = qSqrt((1 - x0*x0 /(a * a)) * (b * b));

    //太阳的位置
    double x1 = x0 + m_rect.width()/2 + m_rect.x();
    double y1 = m_rect.height()/2 + m_rect.top() - y0;

    return QPointF(x1, y1);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widget && event->type() == QEvent::Paint)
    {
        QPainter painter(ui->widget);
        QSize sz = ui->widget->size();
        double wOffset = 1.0/7;
        double hOffset = 1.0/10;
        QRect rect(sz.width()*wOffset, sz.height()*hOffset,
                   sz.width() * (1 - 2*wOffset - 0.06),
                   sz.height() * (1 - hOffset - 0.06));
        m_rect = rect;
        painter.drawArc(rect, 0, 180*16);
//        painter.drawRect(rect);

        int value = m_value;
        value -= 50;
        value = -value;

        m_sunPos = getSunPosByValue(value);

        QImage img("://res/sun.png");
        img = img.scaled(60, 60);

        painter.drawImage(QPoint(m_sunPos.x() - img.width()/2, m_sunPos.y() - img.height()/2), img);

        QImage tree(":/res/tree.png");

        double h = rect.height()/4;

        double rate = tree.height() / h;
        double w = tree.width() / rate;

        tree = tree.scaled(w, h);

        painter.drawImage(QPoint(rect.left() + rect.width()/2 - tree.width()/2,
                                 rect.bottom() - tree.height()), tree);

        QMap<int, QString> data{
            {50, "8:00 (东方)"},
            {25, "10:00"},
            {0, "12:00"},
            {-25, "16:00"},
            {-50, "20:00 (西方)"},
        };
        for(auto it = data.begin(); it != data.end(); it++)
        {
            QPointF pos = getSunPosByValue(it.key());
            QPointF endPos = pos;
            if(pos.x() < m_rect.left() + m_rect.width()/2)
            {
                endPos.rx() -= 40;
            }
            else
            {
                endPos.rx() += 40;
            }

            if(it.key() != 0)
                painter.drawLine(pos, endPos);

            QPointF drawPos;
            if(endPos.x() > pos.x())
                drawPos = pos;
            else
                drawPos = endPos;

            drawPos.ry() -= 3;
            drawPos.rx() += 3;

            if(it.key() == -50)
                drawPos.rx() -= 40;
            painter.drawText(drawPos, it.value());
        }


        //画影子
        QRectF shadowRect;
        double shadowLength = tree.height() * 3 * (qAbs(value) / 100.0);

        if(shadowLength < 3)
            shadowLength = 3;

        if(value < 0)
        {
            shadowRect = QRectF(QPointF(rect.left() + rect.width()/2, rect.bottom() - 3),
                               QSizeF(shadowLength, 5));
        }
        else
        {
            shadowRect = QRectF(QPointF(rect.left() + rect.width()/2 - shadowLength, rect.bottom() - 3),
                                QSizeF(shadowLength, 5));
        }

        painter.setPen(Qt::black);
        painter.setBrush(Qt::black);
        painter.drawRect(shadowRect);
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPointF clickPos = event->pos();
    double xOffset = qAbs(clickPos.x() - m_sunPos.x());
    double yOffset = qAbs(clickPos.y() - m_sunPos.y());

    if(xOffset < 30 && yOffset < 30)
    {
        m_followX = true;
    }
    else
    {
        m_followX = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(m_followX)
    {
        double x = event->pos().x();
        double rate = 1;

        if(x >= m_rect.right())
        {
            rate = 0;
        }
        else if(m_rect.left() <= x)
        {
            double value = double(x - m_rect.left());
            if(value < m_rect.width())
            {
                value = m_rect.width() - value;
            }
            rate = value / m_rect.width();
        }

        if(rate > 1)
        {
            rate = 1;
        }

        m_value = qint32(rate * 100);
        ui->widget->repaint();
    }
}

void MainWindow::on_pushButton_clicked()
{
    if(m_timer.isActive())
    {
        m_timer.stop();
        ui->pushButton->setText("开始");
        return;
    }
    ui->pushButton->setText("停止");
    m_value = 0;
    m_timer.start(130);
}
