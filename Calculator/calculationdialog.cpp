#include "calculationdialog.h"
#include "ui_calculationdialog.h"
#include <QDebug>
#include "mainwindow.h"

CalculationDialog::CalculationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalculationDialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onButtonClickd(int)));
}

CalculationDialog::~CalculationDialog()
{
    qDebug() << "CalculationDialog::~CalculationDialog";
    delete ui;
}

//判断是否为操作数
bool CalculationDialog::isDigit(QChar c)
{
    if((c >= '0' && c <= '9') || c == '.' ) {
        return true;
    }
    return false;
}

//判断是否为运算符
bool CalculationDialog::isOperator(QChar c)
{
    if(c == '+' || c == '-' || c == '*' || c == '/') {
        return true;
    }
    return false;
}

bool CalculationDialog::isOperator(QString c)
{
    if(c == '+' || c == '-' || c == '*' || c == '/') {
        return true;
    }
    return false;
}

//获取运算符的优先级
int CalculationDialog::getOpPriority(QString op)
{
    if(op == '+' || op == '-') {
        return 1;
    } else if(op == '*' || op == '/') {
        return 2;
    }
}

QString CalculationDialog::arithimetic(QString first, QString op, QString second)
{
    double result;
    if(op == '+') {
        result = first.toDouble() + second.toDouble();
    } else if(op == '-') {
        result = first.toDouble() - second.toDouble();
    } else if(op == '*') {
        result = first.toDouble() * second.toDouble();
    } else if(op == '/') {

    }

    QString answer = QString("%1").arg(result);
    return answer;
}

//运算符级别大于符号栈栈顶直接入栈
//级别小于或者等于的出栈然后入队
void CalculationDialog::operatortostack(QString op)
{
    while(!stack.isEmpty()) {
        if(this->getOpPriority(op) > this->getOpPriority(stack.top())) {
            break;
        } else {
            queue.enqueue(stack.pop());
        }
    }
    stack.push(op);
}



//中缀表达式通过转化为后缀表达式入队
void CalculationDialog::midtohind()
{
    QString number;
    //遍历表达式，将操作数放到队列中，运算符放在栈中
    //遍历完成后，将符号栈中的运算符依次入队
    for(int i = 0; i < express.size(); i++) {
        if(this->isDigit(express.at(i))) {
            //数字入放入数字字符串中
            number += express.at(i);
        } else if(this->isOperator(express.at(i))) {
            //将数字字符串入队
            queue.enqueue(number);
            //清楚数字字符串
            number.clear();
            //操作符入栈
            operatortostack(express.at(i));
        }
    }

    queue.enqueue(number);
    while(!stack.isEmpty()) {
        queue.enqueue(stack.pop());
    }
    qDebug() << "queue = " << queue;
}

//计算队列的表达式结果
QString CalculationDialog::calhind()
{
    //遍历后缀表达式队列，如果是数字直接入栈，如果是运算符则弹出两个数字进行运算
    //运算的结果再存放到栈中，重复此过程，直到队列为空，最后把栈中的结果返回即可
    QString str;
    stack.clear();
    while(!queue.isEmpty()) {
        str = queue.dequeue();
        if(this->isOperator(str)) {
            QString res = arithimetic(stack.pop(), str, stack.pop());
            stack.push(res);
        } else {
            stack.push(str);
        }
    }
    return stack.pop();
}


QString CalculationDialog::calculate()
{

    //将表达式的中缀形式转换为后缀形式入队
    midtohind();
    //计算后缀表达式结果，返回栈顶值
    QString res = calhind();
    return res;
}

//按键组里的按键点击响应
void CalculationDialog::onButtonClickd(int id)
{
    QString text = ui->buttonGroup->button(id)->text();
    if(text == "Clear") {
        ui->lineEdit->clear();
        express.clear();
        stack.clear();
        queue.clear();
    } else if(text == "<-") {
        express.chop(1);
    } else if(text == "=") {
        //1.计算表达式结果
        QString result = calculate();
        //2.追加结果
        express += text;
        express += result;
    }  else {
        express += text;
    }

    ui->lineEdit->setText(express);

    if(text == "close") {
            express.clear();
            ui->lineEdit->clear();
            emit show_ui();         //隐藏对话框、显示主界面
        }

}

void CalculationDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        //记录鼠标放下的初始位置
        dragPosition = event->globalPos()-frameGeometry().topLeft();
        event->accept();
        qDebug() << "position = " << dragPosition;
    }
}

void CalculationDialog::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton) {
        //移动窗口
        move(event->globalPos() - dragPosition);
        event->accept();
        qDebug() << "move calculation position = " << dragPosition;
    }
}

void CalculationDialog::mouseReleaseEvent(QMouseEvent *event)
{
    dragPosition = QPoint();
    event->accept();
}
