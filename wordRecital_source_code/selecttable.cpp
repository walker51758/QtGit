#include "selecttable.h"
#include "ui_selecttable.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCheckBox>

selectTable::selectTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::selectTable)
{
    ui->setupUi(this);
    ui->tableListWidget->setSelectionMode(QListWidget::MultiSelection);
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (!query.exec("SHOW TABLES")) {
            qDebug() << "database error: " << query.lastError().text();
        }
    while (query.next()) {
        QString tableName = query.value(0).toString();
        if (tableName != "word_list") {
            ui->tableListWidget->addItem(tableName);
        }
    }
    //connect(ui->selectAllCheckBox, SIGNAL(stateChanged(int)), this, SLOT(checkChangedSlot(int)));
}

selectTable::~selectTable()
{
    delete ui;
}

void selectTable::on_addTablePushButton_clicked()
{
    /*if(selectAll){
        for (int i = 0; i < ui->tableListWidget->count(); ++i) {
            QListWidgetItem *item = ui->tableListWidget->item(i);
            item->setSelected(true);
        }
    }*/
    QList<QListWidgetItem*> selectedItems = ui->tableListWidget->selectedItems();
    if (selectedItems.isEmpty()) {
         QMessageBox::information(this, "提示", "还没选择任何一个表格!", QMessageBox::Ok);
         return;
    }
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    if (!query.exec("DELETE FROM word_list")) {
         qDebug() << "database error: " << query.lastError().text();
         return;
    }
    foreach (QListWidgetItem* item, selectedItems) {
        QString table = item->text();
        if (!query.exec(QString("INSERT INTO word_list (word, meaning, source_table, table_id) select word, meaning, '%0', tableId from %1").arg(table, table))) {
            qDebug() << "database error: " << query.lastError().text();
            return;
        }
    }
    if (!query.exec("SET @new_id = 0")) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }
    if (!query.exec("UPDATE word_list SET id = (@new_id := @new_id + 1) ORDER BY id")) {
        QMessageBox::critical(this, "Database Error", query.lastError().text());
        return;
    }
    QMessageBox::information(this, "提示", "已添加到词库!");
    emit renew();
    this->close();
}

/*void selectTable::checkChangedSlot(int state){
    if(state == Qt::Checked)
        selectAll = true;
    else
        selectAll = false;
}*/
