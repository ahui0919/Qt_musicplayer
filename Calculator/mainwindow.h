#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calculationdialog.h"
#include "musicplayerdialog.h"
#include <QMouseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void hide_or_show_emit(bool flag);

private slots:
    void on_btn_cal_clicked();
    void on_btn_close_clicked();
    void on_mainwindow_show();
    void on_mainwindow_hide();

    void on_btn_player_clicked();

private:
    Ui::MainWindow *ui;
    CalculationDialog *caldlg = nullptr;
    musicplayerDialog *mplayerdlg = nullptr;
    QPoint dragPosition;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

};

#endif // MAINWINDOW_H
