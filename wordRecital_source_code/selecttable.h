#ifndef SELECTTABLE_H
#define SELECTTABLE_H

#include <QWidget>

namespace Ui {
class selectTable;
}

class selectTable : public QWidget
{
    Q_OBJECT

public:
    explicit selectTable(QWidget *parent = nullptr);
    ~selectTable();

private slots:
    void on_addTablePushButton_clicked();

    //void checkChangedSlot(int state);

private:
    Ui::selectTable *ui;
    //bool selectAll = false;

signals:
    void renew();
};

#endif // SELECTTABLE_H
