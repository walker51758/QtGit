#ifndef SKIPTO_H
#define SKIPTO_H

#include <QWidget>

namespace Ui {
class skipTo;
}

class skipTo : public QWidget
{
    Q_OBJECT

public:
    explicit skipTo(QWidget *parent = nullptr);
    ~skipTo();
    int limit;

private:
    Ui::skipTo *ui;

private slots:
    void on_pushButton_clicked();

signals:
    void skip_to(int wordNum);
};

#endif // SKIPTO_H
