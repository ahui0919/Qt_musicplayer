#ifndef CALCULATIONDIALOG_H
#define CALCULATIONDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QQueue>
#include <QStack>
namespace Ui {
class CalculationDialog;
}

class CalculationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalculationDialog(QWidget *parent = 0);
    ~CalculationDialog();
    QString calculate();
    void midtohind();
    QString calhind();
    bool isDigit(QChar c);
    bool isOperator(QChar c);
    bool isOperator(QString c);
    void operatortostack(QString op);
    int getOpPriority(QString op);
    QString arithimetic(QString first, QString op, QString second);

signals:
    void show_ui();
    void hide_ui();

private slots:
    void onButtonClickd(int id);

private:
    Ui::CalculationDialog *ui;
    QString express;//输入的表达式
    QStack<QString> stack;
    QQueue<QString> queue;
    QPoint dragPosition;//左键点击窗口的起始位置

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // CALCULATIONDIALOG_H
