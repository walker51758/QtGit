#include "addnewword.h"
#include "ui_addnewword.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>

addNewWord::addNewWord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addNewWord)
{
    ui->setupUi(this);
    connect(ui->wordLineEdit, SIGNAL(returnPressed()), this, SLOT(on_savePushButton_clicked()));
    connect(ui->meaningLineEdit, SIGNAL(returnPressed()), this, SLOT(on_savePushButton_clicked()));
}

addNewWord::~addNewWord()
{
    delete ui;
}

void addNewWord::on_backPushButton_clicked()
{
    this->close();
}

void addNewWord::on_savePushButton_clicked()
{
    if(editMode){
        QString wordString = ui->wordLineEdit->text();
        QString meaningString = ui->meaningLineEdit->text();
        if(wordString == "")
            QMessageBox::information(this, "错误", "单词不能为空", QMessageBox::Ok);
        else{
            QString cmd = QString("update word_list set word = '%0', meaning = '%1' where id = '%2'")
                    .arg(wordString).arg(meaningString).arg(editId);
            QSqlDatabase db = QSqlDatabase::database();
            QSqlQuery query(db);
            if(query.exec(cmd)){
                 QMessageBox::information(this, "提示", "单词修改成功！", QMessageBox::Ok);
                 this->close();
            }
        }
    }
    else{
        QString wordString = ui->wordLineEdit->text();
        QString meaningString = ui->meaningLineEdit->text();
        if(wordString == "")
            QMessageBox::information(this, "错误", "单词不能为空", QMessageBox::Ok);
        else{
            if(meaningString == "")
                ui->meaningLineEdit->setFocus();
            else{
                realTotalNum++;
                QSqlDatabase db = QSqlDatabase::database();
                QSqlQuery query(db);
                QString cmd = QString("insert into word_list values('%0', '%1', '%2');")
                        .arg(wordString).arg(meaningString).arg(realTotalNum);
                if(query.exec(cmd)){
                     QMessageBox::information(this, "提示", "单词添加成功！", QMessageBox::Ok);
                }
                else realTotalNum--;
                ui->wordLineEdit->clear();
                ui->meaningLineEdit->clear();
                ui->wordLineEdit->setFocus();
            }
        }
    }
}

void addNewWord::swiftEditMode(QString editWord, QString editMeaning, int id){
    ui->wordLineEdit->setText(editWord);
    ui->meaningLineEdit->setText(editMeaning);
    editId = id;
    editMode = true;
}
