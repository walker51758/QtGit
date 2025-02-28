#ifndef ADDNEWWORD_H
#define ADDNEWWORD_H

#include <QWidget>

namespace Ui {
class addNewWord;
}

class addNewWord : public QWidget
{
    Q_OBJECT

public:
    explicit addNewWord(QWidget *parent = nullptr);
    ~addNewWord();
    void swiftEditMode(QString editWord, QString editMeaning, int editTableId, int editId, QString editTable);

private slots:
    void on_backPushButton_clicked();

    void on_savePushButton_clicked();

public:
    Ui::addNewWord *ui;
    int editTableId;
    int editId;
    QString editTable;
    bool editMode = false;
};

#endif // ADDNEWWORD_H
