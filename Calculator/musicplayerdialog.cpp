#include "musicplayerdialog.h"
#include "ui_musicplayerdialog.h"
#include <QDir>
#include <QDateTime>
#include <QList>
#include <QListWidgetItem>

musicplayerDialog::musicplayerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::musicplayerDialog), music_path("../Calculator/music"), song_index(0)
{
    ui->setupUi(this);
    ui->btn_play->setCheckable(true);   //激活按钮的checked状态
    ui->btn_sound->setCheckable(true);

    this->setWindowFlags(Qt::FramelessWindowHint);

    onSystimeout();//初次打开音乐播放器时初始化时间

    getSongNames();
    initProcess();
    initTime();
    gui_init();
}

musicplayerDialog::~musicplayerDialog()
{
    process->close();
    qDebug() << "musicplayerDialog::~musicplayerDialog()";
    delete ui;
}

//获取目录下的歌曲
void musicplayerDialog::getSongNames()
{
    //创建一个目录对象，并初始化
    QDir dir(music_path);
    //设置目录过滤器，只获取过滤器制定格式的文件名
    QStringList filters;
    filters << "*.flac" << "*.mp3";
    dir.setNameFilters(filters);

    //获取目录下的mp3文件，返回字符串列表
    song_names = dir.entryList();
    qDebug() << "song_names_list: " << song_names;
}

void musicplayerDialog::initProcess()
{
    process = new QProcess(this);
    //    "F:/Mplayer/mplayer/MPlayer_Windows/MPlayer_core.exe"
#ifdef Q_OS_WIN
    process->setProgram("F:/Mplayer/mplayer/MPlayer_Windows/MPlayer_core.exe");
#else
    process->setProgram("/usr/bin/mplayer");
#endif


    connect(process, SIGNAL(started()), this, SLOT(onStarted()));
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(readyRead()), this, SLOT(onReadData()));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onError(QProcess::ProcessError)));

}

//音乐播放指定索引的歌曲
void musicplayerDialog::play_music()
{
    if(song_names.isEmpty()) {
        qDebug() << "song_names is empty";
        return;
    }

    if(process->state() == QProcess::Running) {
        process->close();
    }

    QStringList args; //程序运行时的参数列表
    qDebug() << "song_index = " << song_index;
    QString path_name = QString("%1/%2").arg(music_path).arg(song_names.at(song_index));
    args<< "-slave" << "-quiet" << path_name;
    process->setArguments(args);
    qDebug() << "play argument: " <<process->arguments();
    process->open();

    display_musiclrc();
}

void musicplayerDialog::initTime()
{
    sys_timer = new QTimer(this);
    sys_timer->setInterval(1000);
    connect(sys_timer, &QTimer::timeout, this, &musicplayerDialog::onSystimeout);
    sys_timer->start();

    pos_timer = new QTimer(this);
    pos_timer->setInterval(50);
    connect(pos_timer, &QTimer::timeout, [&]{
       this->process->write("get_time_pos\n");
       this->process->write("get_percent_pos\n");
    });
}

void musicplayerDialog::gui_init()
{
   ui->slider_sound->setSliderPosition(60);
}

//读取歌词文件，保存每行歌词的时间，及显示歌词到界面的列表控件
void musicplayerDialog::display_musiclrc()
{
    ui->listWidget_lrc->clear();
    time_list.clear();
    ui->listWidget_lrc->verticalScrollBar()->setSliderPosition(0);
    QString songName = song_names.at(song_index);
    QString songWordName = songName.replace(QRegularExpression("(mp3|flac)"), "lrc");

    qDebug() << "songWordName: " << songWordName;

    QFile file(music_path + "/" + songWordName);
    if(!file.exists()) {
        qDebug() << "file is not exist";
        return ;
    }

    if(!file.open(QFile::ReadOnly)) {
        qDebug() << "open" << songWordName << "error: " << file.errorString();
        return;
    }

    //使用文本流读文件
    QTextStream in(&file);
    in.setCodec("UTF-8"); //设置文件的编码格式
    qDebug() << "in.atEnd;" << in.atEnd();
    while(!in.atEnd()) {
        QString line = in.readLine();
        //保存每一行歌词的时间，到容器中
        double time_pos = line.mid(1, 2).toDouble() * 60 + line.mid(4,4).toDouble();
        time_list.append(time_pos);
        line.remove(0, 10);
        QListWidgetItem *item = new QListWidgetItem(line);
        item->setTextAlignment(Qt::AlignHCenter);
        ui->listWidget_lrc->addItem(item);
    }
    file.close();
    qDebug() << "time list: " << time_list;
}

void musicplayerDialog::onStarted()
{
    qDebug() << "process started success!";
    //QString cmd("get_time_length\n");
    //process->write(cmd.toUtf8());
    process->write("get_time_length\n");
    pos_timer->start();
}

void musicplayerDialog::onFinished(int code,QProcess::ExitStatus status)
{
    qDebug() << "process finished ...." << code << ", "<< status << endl;
    if(process->state() == status && song_index != song_names.size() -1 ) {
        song_index++;
        qDebug() << "finished song played";
        this->play_music();
    }
}

//处理实时时间，显示时间到界面，并高亮显示相应的歌词
void musicplayerDialog::handTimePosition(QString line)
{
    line.remove("\n");
    line.remove("ANS_TIME_POSITION=");
    ui->label_time_pos->setText(line);
    double time_pos = line.toDouble();
    //qDebug() << "time pos: " << time_pos;
    int index = time_list.indexOf(time_pos);
    if(index != -1) {
        ui->listWidget_lrc->item(index)->setTextColor(QColor(225,80,30));
        if(index - 1 != -1) {
            ui->listWidget_lrc->item(index-1)->setTextColor(QColor(0,0,0));
        }

        if(index >= 7) {
            ui->listWidget_lrc->verticalScrollBar()->setSliderPosition(index-7);
        }
    }

}

//读外部程序的输出数据
void musicplayerDialog::onReadData()
{
    while(process->canReadLine()) {
        QString line = process->readLine();
        //qDebug() << "line: " << line;
        if(line.contains("ANS_LENGTH")) {   //只要ANS_LENGTH那一行的数据
            //读取时间总长度
            line.remove("\n");
            line.remove("ANS_LENGTH=");
            ui->label_time_length->setText(line);
        }

        if(line.contains("ANS_TIME_POSITION")) {
            //读取实时播放时间
            handTimePosition(line);
        }

        if(line.contains("ANS_PERCENT_POSITION")) {
            //读取播放百分比
            line.remove("\n");
            line.remove("ANS_PERCENT_POSITION=");
            ui->slider_process->setValue(line.toInt());

        }
    }

    //qDebug() << "process read data: " ;
}

void musicplayerDialog::onError(QProcess::ProcessError err)
{
    qDebug() << "process error: " << err << ", " << process->errorString();
}

//定时器更新当前播放时长位置
void musicplayerDialog::onUpdatepostime()
{

    //ui->label_time_pos->setText();
}

//定时器超时，自动执行获取系统时间
void musicplayerDialog::onSystimeout()
{
    //获取系统时间
    QString curtime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss ddd");
    //显示时间到界面
    ui->label_time_sys->setText(curtime);

}

void musicplayerDialog::on_btn_close_clicked()
{

    emit show_ui();

}

void musicplayerDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        //记录鼠标放下的初始位置
        dragPosition = event->globalPos()-frameGeometry().topLeft();
        event->accept();
        qDebug() << "position = " << dragPosition;
    }
}

void musicplayerDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton) {
        //移动窗口
        move(event->globalPos() - dragPosition);
        event->accept();
        qDebug() << " move musicplayerDialog position = " << dragPosition;
    }
}

void musicplayerDialog::mouseReleaseEvent(QMouseEvent *event)
{
    dragPosition = QPoint();
    event->accept();
}

void musicplayerDialog::on_btn_last_clicked()
{
    if(song_index > 0) {
        song_index --;
    } else {
        song_index = song_names.size() -1;
    }

    qDebug() << "The " << song_index << "song";
    this->play_music();
}

void musicplayerDialog::on_btn_next_clicked()
{
    if(song_index < song_names.size() -1) {
        song_index ++;

    } else {
        song_index = 0;
    }


    qDebug() << "The " << song_index << "song";
    this->play_music();
}

void musicplayerDialog::on_btn_song_clicked()
{

}

void musicplayerDialog::on_btn_quick_last_clicked()
{
    qDebug() << "quick seek -10s played!";
    process->write("seek -10\n");
}

void musicplayerDialog::on_btn_quick_next_clicked()
{
    qDebug() << "quick seek +10s played!";
    process->write("seek 10\n");
}

void musicplayerDialog::on_btn_module_clicked()
{

}

//点击播放或暂停按钮
void musicplayerDialog::on_btn_play_clicked(bool checked)
{
    qDebug() << "checked: " << checked;
    if(checked) {   //true 表示播放
        //如果进程还没有运行，则启动进程，播放音乐
        if(process->state() != QProcess::Running) {
            this->play_music();
        }
        pos_timer->start();
        //如果进程启动了，则发暂停指令
        ui->btn_play->setText("暂停");
        process->write("pause\n");
    } else {        //false 表示暂停
        ui->btn_play->setText("播放");
        pos_timer->stop();
        process->write("pause\n");
    }

}

//点击喇叭设置静音或者打开声音
void musicplayerDialog::on_btn_sound_clicked(bool checked)
{
    qDebug() << "btn_sound: " << checked;
    if(checked) {
        ui->btn_sound->setText("静音");
    } else {
        ui->btn_sound->setText("音量");

    }
    process->write("mute\n");
}

//调节音量大小
void musicplayerDialog::on_slider_sound_valueChanged(int value)
{
    qDebug() << "slide  volume progress: " << value;
    QString val = QString::number(value);
    QString cmd = "volume " + val + " 1\n";
    process->write(cmd.toUtf8());
}


void musicplayerDialog::on_slider_process_sliderMoved(int position)
{
    qDebug() << "slide song progress: " << position;
    QString val = QString::number(position);
    QString cmd = "seek " + val + " 1\n";
    process->write(cmd.toUtf8());
}
