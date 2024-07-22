#include "addnewword.h"
#include "ui_addnewword.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

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
            QString cmd = QString("update %0 set word = '%1', meaning = '%2' where tableId = '%3'")
                    .arg(editTable).arg(wordString).arg(meaningString).arg(editTableId);
            QString cmd1 = QString("update word_list set word = '%0', meaning = '%1' where id = '%2'")
                    .arg(wordString).arg(meaningString).arg(editId);
            QSqlDatabase db = QSqlDatabase::database();
            QSqlQuery query(db);
            if(query.exec(cmd) && query.exec(cmd1)){
                 QMessageBox::information(this, "提示", "单词修改成功！", QMessageBox::Ok);
                 this->close();
            }
        }
    }
    else{
        QString wordString = ui->wordLineEdit->text();
        QString meaningString = ui->meaningLineEdit->text();
        QString tableString = ui->tabelLineEdit->text();
        if(wordString == "")
            QMessageBox::information(this, "错误", "单词不能为空", QMessageBox::Ok);
        else if(tableString == "")
            QMessageBox::information(this, "错误", "表格不能为空", QMessageBox::Ok);
        else{
            if(meaningString == "")
                ui->meaningLineEdit->setFocus();
            else{
                QSqlDatabase db = QSqlDatabase::database();
                QSqlQuery query(db);
                QString cmd = QString("select table_name from information_schema.tables where table_schema = '%0' and table_name = '%1'")
                        .arg(db.databaseName()).arg(tableString);
                if (query.exec(cmd)) {
                    if (!query.next()) {
                        QString cmd1 = QString("CREATE TABLE %1 (word VARCHAR(100), meaning VARCHAR(100), tableId INT PRIMARY KEY AUTO_INCREMENT)").arg(tableString);
                        if(!query.exec(cmd1))
                             qDebug() << "Failed to create table:" << query.lastError().text();
                    }
                }
                int tableId = 0;
                QString cmd2 = QString("select count(*) from %0").arg(tableString);
                if(query.exec(cmd2)){
                    if(query.next()){
                        tableId = query.value(0).toInt() + 1;
                    }
                }
                QString cmd3 = QString("insert into %0 values('%1', '%2', '%3');")
                        .arg(tableString).arg(wordString).arg(meaningString).arg(tableId);
                if(query.exec(cmd3)){
                     QMessageBox::information(this, "提示", "单词添加成功！", QMessageBox::Ok);
                }
                else
                    QMessageBox::information(this, "错误", "新单词插入失败", QMessageBox::Ok);
                ui->wordLineEdit->clear();
                ui->meaningLineEdit->clear();
                ui->wordLineEdit->setFocus();
            }
        }
    }
}

void addNewWord::swiftEditMode(QString editWord, QString editMeaning, int tableId, int id, QString table){
    ui->wordLineEdit->setText(editWord);
    ui->meaningLineEdit->setText(editMeaning);
    editId = id;
    editTableId = tableId;
    editTable = table;
    editMode = true;
}
