#ifndef MUSICPLAYERDIALOG_H
#define MUSICPLAYERDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QDebug>
#include <QProcess>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QScrollBar>

namespace Ui {
class musicplayerDialog;
}

class musicplayerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit musicplayerDialog(QWidget *parent = 0);
    ~musicplayerDialog();

    void getSongNames();
    void initProcess();
    void play_music();
    void initTime();
    void gui_init();    //初始化界面
    void display_musiclrc();    //播放文件显示相应的歌词文件
    void handTimePosition(QString line);


signals:
    void show_ui();
    void hide_ui();

private slots:
    void onStarted();
    void onFinished(int code, QProcess::ExitStatus status);
    void onReadData();
    void onError(QProcess::ProcessError err);
    void onUpdatepostime();
    void onSystimeout();

    void on_btn_close_clicked();

    void on_btn_last_clicked();

    void on_btn_next_clicked();

    void on_btn_song_clicked();

    void on_btn_quick_last_clicked();

    void on_btn_quick_next_clicked();

    void on_btn_module_clicked();

    void on_btn_play_clicked(bool checked);

    void on_btn_sound_clicked(bool checked);

    void on_slider_sound_valueChanged(int value);

    void on_slider_process_sliderMoved(int position);

private:
    Ui::musicplayerDialog *ui;
    QPoint dragPosition;
    QString music_path;
    QStringList song_names;
    int song_index;
    QProcess *process;
    QTimer *pos_timer;
    QTimer *sys_timer;

    QList<double> time_list;    //用于保存每一行歌词的时间，单位为秒

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

};

#endif // MUSICPLAYERDIALOG_H
