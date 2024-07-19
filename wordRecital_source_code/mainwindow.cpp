#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createconnection.h"
#include "addnewword.h"
#include "skipto.h"

#include <QFile>
#include <QTextStream>
#include <fstream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <random>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createConnection(this);
    connect(ui->addNewWordAction, &QAction::triggered, this, &MainWindow::openAddNewWord);
    connect(ui->skipToAction, &QAction::triggered, this, &MainWindow::openSkipTo);
    ifstream infile("setting.txt");
    if(!infile)
        qDebug() << "can't open setting.txt when trying to read";
    infile >> wordNum;
    infile >> totalNum;
    for(int i = 0; i < totalNum; ++i)
        infile >> order[i];
    infile.close();
    /*QString cmd = QString("select count(*) from word_list");
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if(query.exec(cmd)){
        if(query.next()){
            totalNum = query.value(0).toInt();
        }
    }
    for(int i = 1; i <= totalNum; ++i)
        order[i] = i;*/
}

MainWindow::~MainWindow()
{
    ofstream outfile("setting.txt");
    if(!outfile)
        qDebug() << "can't open setting.txt when trying to write";
    outfile << wordNum << endl;
    outfile << totalNum << endl;
    for(int i = 0; i < totalNum; ++i)
        outfile << order[i] << " ";
    outfile << endl;
    outfile.close();
    delete ui;
}


void MainWindow::on_nextPushButton_clicked()
{
    if(wordNum == totalNum){
        QMessageBox::information(this, "提示", "已完成本轮复习", QMessageBox::Ok);
        wordNum = 1;
    }
    else ++wordNum;
    QString cmd = QString("select word, meaning from word_list where id = %1").arg(order[wordNum]);
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if(query.exec(cmd)){
        if(query.next()){
            displayedWord = query.value(0).toString();
            displayedMeaning = query.value(1).toString();
        }
    }
    else{
        qDebug() << "Error:" << query.lastError().text();
    }
    ui->wordLabel->setText(displayedWord);
    ui->meaningLabel->clear();
    QString numDisplayString = QString("%0 / %1").arg(wordNum).arg(totalNum);
    ui->numDisplayLabel->setText(numDisplayString);
}

void MainWindow::openAddNewWord(){
    addNewWord* page = new addNewWord;
    page->realTotalNum = totalNum;
    page->setWindowTitle("添加单词");
    page->show();
}
void MainWindow::on_meaningPushButton_clicked()
{
    ui->meaningLabel->setText(displayedMeaning);
}

void MainWindow::on_randomPushButton_clicked()
{
    random_device rd;
    mt19937 g(rd());
    shuffle(order, order + totalNum, g);
    QSqlDatabase db = QSqlDatabase::database();
    QMessageBox::information(this, "提示", "已完成随机排序", QMessageBox::Ok);
    wordNum = 0;
    ui->wordLabel->setText("hello");
    ui->meaningLabel->setText("你好");
    ui->numDisplayLabel->clear();
}

void MainWindow::on_renewPushButton_clicked()
{
    QString cmd = QString("select count(*) from word_list");
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if(query.exec(cmd)){
        if(query.next()){
            totalNum = query.value(0).toInt();
        }
    }
    for(int i = 1; i <= totalNum; ++i)
        order[i] = i;
    wordNum = 0;
    QMessageBox::information(this, "提示", "已获取词库最新更新，请点击“下一个”开始复习", QMessageBox::Ok);
    ui->wordLabel->setText("hello");
    ui->meaningLabel->setText("你好");
    ui->numDisplayLabel->clear();
}

void MainWindow::on_editPushButton_clicked()
{
    addNewWord* page = new addNewWord;
    page->setWindowTitle("编辑单词");
    page->swiftEditMode(displayedWord, displayedMeaning, order[wordNum]);
    if(ui->wordLabel->text() == "hello")
        return;
    else
        page->show();
}

void MainWindow::skipToSlot(int newWordNum){
    wordNum = newWordNum;
    QString cmd = QString("select word, meaning from word_list where id = %1").arg(order[wordNum]);
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if(query.exec(cmd)){
        if(query.next()){
            displayedWord = query.value(0).toString();
            displayedMeaning = query.value(1).toString();
        }
    }
    else{
        qDebug() << "Error:" << query.lastError().text();
    }
    ui->wordLabel->setText(displayedWord);
    ui->meaningLabel->clear();
    QString numDisplayString = QString("%0 / %1").arg(wordNum).arg(totalNum);
    ui->numDisplayLabel->setText(numDisplayString);
}

void MainWindow::openSkipTo(){
    skipTo* page = new skipTo;
    page->limit = totalNum;
    connect(page, &skipTo::skip_to, this, &MainWindow::skipToSlot);
    page->show();
}
