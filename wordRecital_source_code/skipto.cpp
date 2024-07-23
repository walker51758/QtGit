#include "skipto.h"
#include "ui_skipto.h"

#include <QMessageBox>

skipTo::skipTo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::skipTo)
{
    ui->setupUi(this);
    connect(ui->wordNumLineEdit, SIGNAL(returnPressed()), this, SLOT(on_pushButton_clicked()));
}

skipTo::~skipTo()
{
    delete ui;
}

void skipTo::on_pushButton_clicked()
{
    int newWordNum = ui->wordNumLineEdit->text().toInt();
    if(newWordNum > limit || newWordNum <= 0){
        QMessageBox::information(this, "错误", "单词编号无效", QMessageBox::Ok);
        ui->wordNumLineEdit->clear();
    }
    else{
        emit skip_to(newWordNum);
        this->close();
    }
}
