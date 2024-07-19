#include "createconnection.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>

createConnection::createConnection(QWidget *parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName("word_list");
        db.setUserName("root");
        db.setPassword("hmr123123");
        bool ok = db.open();
        if(!ok){
            QMessageBox::information(parent, "错误", "无法连接至数据库", QMessageBox::Ok);
            qDebug() << "error open database because" << db.lastError().text();
        }
        else{
            qDebug() << "success";
        }
}
