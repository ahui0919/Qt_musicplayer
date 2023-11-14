#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);

}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    delete ui;
}

void MainWindow::on_btn_cal_clicked()
{
    //对话框不存在就创建对话框并初始化
    if(!caldlg) {
        caldlg = new CalculationDialog(nullptr);
        caldlg->show();
        connect(caldlg, SIGNAL(show_ui()), this, SLOT(on_mainwindow_show()));
        connect(caldlg, SIGNAL(hide_ui()), this, SLOT(on_mainwindow_hide()));
    }

    qDebug() << "caldlg->show() is ok";
    this->hide();
}

void MainWindow::on_btn_player_clicked()
{
    //对话框不存在就创建对话框并初始化
    if(!mplayerdlg) {
        mplayerdlg = new musicplayerDialog(nullptr);
        mplayerdlg->show();
        connect(mplayerdlg, SIGNAL(show_ui()), this, SLOT(on_mainwindow_show()));
        connect(mplayerdlg, SIGNAL(hide_ui()), this, SLOT(on_mainwindow_hide()));
    }

    qDebug() << "mplayerdlg->show() is ok";
    this->hide();
}


void MainWindow::on_btn_close_clicked()
{
    this->close();
}

void MainWindow::on_mainwindow_show()
{
    //关闭对话框的时候，删除对话框对象，触发析构函数
    if(caldlg) {
        disconnect(caldlg, SIGNAL(show_ui()), this, SLOT(on_mainwindow_show()));
        disconnect(caldlg, SIGNAL(hide_ui()), this, SLOT(on_mainwindow_hide()));
        delete caldlg;
        caldlg = nullptr;
    } else if(mplayerdlg) {
        disconnect(mplayerdlg, SIGNAL(show_ui()), this, SLOT(on_mainwindow_show()));
        disconnect(mplayerdlg, SIGNAL(hide_ui()), this, SLOT(on_mainwindow_hide()));
        delete mplayerdlg;
        mplayerdlg = nullptr;
    }

    qDebug() << "prepare show mainwindow ui";
    this->show();
}

void MainWindow::on_mainwindow_hide()
{
    qDebug() << "prepare hide mainwindow ui";
    this->hide();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        //记录鼠标放下的初始位置
        dragPosition = event->globalPos()-frameGeometry().topLeft();
        event->accept();
        qDebug() << "position = " << dragPosition;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton) {
        //移动窗口
        move(event->globalPos() - dragPosition);
        event->accept();
        qDebug() << " move mainwindowui position = " << dragPosition;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    dragPosition = QPoint();
    event->accept();
}

