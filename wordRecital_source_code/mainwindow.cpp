#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "createconnection.h"
#include "addnewword.h"
#include "skipto.h"
#include <selecttable.h>

#include <QFile>
#include <QTextStream>
#include <fstream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <random>
#include <QShortcut>
#include <QKeySequence>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createConnection(this);
    QShortcut* nextShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut* formerShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    QShortcut* collectShortcut = new QShortcut(QKeySequence("Ctrl+C"), this);
    connect(ui->addNewWordAction, &QAction::triggered, this, &MainWindow::openAddNewWord);
    connect(ui->skipToAction, &QAction::triggered, this, &MainWindow::openSkipTo);
    connect(ui->updateAction, &QAction::triggered, this, &MainWindow::openSelectTable);
    connect(ui->editAction, &QAction::triggered, this, &MainWindow::openEdit);
    connect(ui->switchModeAction, &QAction::triggered, this, &MainWindow::switchMode);
    connect(ui->clearAction, &QAction::triggered, this, &MainWindow::clearTheReview);
    connect(nextShortcut, &QShortcut::activated, this, &MainWindow::on_nextPushButton_clicked);
    connect(formerShortcut, &QShortcut::activated, this, &MainWindow::on_formerPushButton_clicked);
    connect(collectShortcut, &QShortcut::activated, this, &MainWindow::on_collectPushButton_clicked);
    ifstream infile("setting.txt");
    if(infile){
        infile >> wordNum;
        infile >> totalNum;
        infile >> mode;
        for(int i = 0; i < totalNum; ++i)
            infile >> order[i];
        infile.close();
    }
    else{
        QString cmd = QString("select count(*) from word_list");
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery query(db);
        if(query.exec(cmd)){
            if(query.next()){
                totalNum = query.value(0).toInt();
            }
        }
        if(totalNum == 0)
            QMessageBox::information(this, "提示", "未添加任何单词到词库", QMessageBox::Ok);
        else{
            for(int i = 0; i < totalNum; ++i)
                order[i] = i + 1;
        }
        wordNum = 0;
        mode = 1;
    }
    switch(mode){
    case 1: ui->modeDisplayLabel->setText("测试模式");
        break;
    case 2: ui->modeDisplayLabel->setText("记忆模式");
        break;
    default: break;
    }
}

MainWindow::~MainWindow()
{
    ofstream outfile("setting.txt");
    if(!outfile)
        qDebug() << "can't open setting.txt when trying to write";
    outfile << wordNum << endl;
    outfile << totalNum << endl;
    outfile << mode << endl;
    for(int i = 0; i < totalNum; ++i)
        outfile << order[i] << " ";
    outfile << endl;
    outfile.close();
    delete ui;
}


void MainWindow::on_nextPushButton_clicked()
{
    if(totalNum == 0){
        QMessageBox::information(this, "提示", "列表中无单词", QMessageBox::Ok);
        return;
    }
    if(wordNum == totalNum){
        if(meaningDisplayed){
            QMessageBox::information(this, "提示", "已完成本轮复习", QMessageBox::Ok);
            wordNum = 1;
        }
        else{
            on_meaningPushButton_clicked();
            return;
        }
    }
    else{
        if(mode == 2){
            if(displayedWord == "hello") meaningDisplayed = true;
            if(meaningDisplayed){
                wordNum++;
                meaningDisplayed = false;
            }
            else{
                on_meaningPushButton_clicked();
                return;
            }
        }
        else
        ++wordNum;
    }
    QString cmd = QString("select word, meaning from word_list where id = %1").arg(order[wordNum - 1]);
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if(query.exec(cmd)){
        if(query.next()){
            displayedWord = query.value(0).toString();
            displayedMeaning = query.value(1).toString();
        }
        else{
            qDebug() << "Error:" << query.lastError().text();
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
    page->setWindowTitle("添加单词");
    page->show();
}
void MainWindow::on_meaningPushButton_clicked()
{
    ui->meaningLabel->setText(displayedMeaning);
    meaningDisplayed = true;
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
    for(int i = 0; i < totalNum; ++i)
        order[i] = i + 1;
    wordNum = 0;
    QMessageBox::information(this, "提示", "已获取词库最新更新，请点击“下一个”开始复习", QMessageBox::Ok);
    ui->wordLabel->setText("hello");
    ui->meaningLabel->setText("你好");
    ui->numDisplayLabel->clear();
    meaningDisplayed = true;
}

void MainWindow::openEdit()
{
    addNewWord* page = new addNewWord;
    page->setWindowTitle("编辑单词");
    QString cmd = QString("select source_table, table_id from word_list where id = %0").arg(order[wordNum - 1]);
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if(query.exec(cmd)){
        if(query.next()){
            int editTableId = query.value(1).toInt();
            QString editTable = query.value(0).toString();
            page->swiftEditMode(displayedWord, displayedMeaning, editTableId, order[wordNum - 1], editTable);
        }
    }
    if(ui->wordLabel->text() == "hello")
        return;
    else
        page->show();
}

void MainWindow::skipToSlot(int newWordNum){
    wordNum = newWordNum;
    QString cmd = QString("select word, meaning from word_list where id = %1").arg(order[wordNum - 1]);
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

void MainWindow::openSelectTable(){
    selectTable* page = new selectTable;
    page->setWindowTitle("选择表格");
    connect(page, &selectTable::renew, this, &MainWindow::on_renewPushButton_clicked);
    page->show();
}

void MainWindow::on_formerPushButton_clicked()
{
    if(displayedWord == "Hello") return;
    if(wordNum == 1){
        QMessageBox::information(this, "提示", "已回到第一个单词", QMessageBox::Ok);
    }
    else
        --wordNum;
    meaningDisplayed = false;
    QString cmd = QString("select word, meaning from word_list where id = %1").arg(order[wordNum - 1]);
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if(query.exec(cmd)){
        if(query.next()){
            displayedWord = query.value(0).toString();
            displayedMeaning = query.value(1).toString();
        }
        else{
            qDebug() << "Error:" << query.lastError().text();
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

void MainWindow::switchMode(){
    if(mode == 1){
        mode = 2;
        ui->modeDisplayLabel->setText("记忆模式");
        if(ui->meaningLabel->text() == "") meaningDisplayed = false;
    }
    else{
        mode = 1;
        ui->modeDisplayLabel->setText("测试模式");
    }
}


void MainWindow::on_collectPushButton_clicked()
{
    QString wordString = displayedWord;
    QString meaningString = displayedMeaning;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    QString cmd = QString("SELECT COUNT(*) FROM review WHERE word = '%1'").arg(wordString);
    if(query.exec(cmd)){
        if(query.next()){
            if(query.value(0).toInt() == 1){
                QMessageBox::information(this, "提示", "已收藏！", QMessageBox::Ok);
                return;
            }
        }
    }
    int tableId = 0;
    QString cmd2 = QString("select count(*) from review");
    if(query.exec(cmd2)){
        if(query.next()){
            tableId = query.value(0).toInt() + 1;
        }
    }
    QString cmd3 = QString("insert into review values('%1', '%2', '%3');")
            .arg(wordString).arg(meaningString).arg(tableId);
    if(query.exec(cmd3)){
         QMessageBox::information(this, "提示", "单词添加成功！", QMessageBox::Ok);
    }
    else
        QMessageBox::information(this, "错误", "新单词插入失败", QMessageBox::Ok);
}

void MainWindow::clearTheReview(){
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认操作", "你确定要执行此操作吗？",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (!query.exec("DELETE FROM review")) {
             qDebug() << "database error: " << query.lastError().text();
        }
    }
}
