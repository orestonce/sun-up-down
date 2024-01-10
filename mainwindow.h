#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QPointF getSunPosByValue(int value);
private:
    Ui::MainWindow *ui;

    QPointF m_sunPos;
    bool m_followX = false;
    QRect m_rect;

    quint32 m_value = 0;    // [0, 100]
    QTimer m_timer;
    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
private slots:
    void on_pushButton_clicked();
};

#endif // MAINWINDOW_H
