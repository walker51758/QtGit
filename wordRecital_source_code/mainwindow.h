#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void openAddNewWord();
    void openSkipTo();
    void switchMode();

private slots:
    void on_nextPushButton_clicked();

    void on_meaningPushButton_clicked();

    void on_randomPushButton_clicked();

    void on_renewPushButton_clicked();

    void openEdit();

    void skipToSlot(int newWordNum);

    void openSelectTable();

    void on_formerPushButton_clicked();

private:
    Ui::MainWindow *ui;
    int wordNum = 0;
    int order[10000] = {0};
    int totalNum;
    QString displayedWord;
    QString displayedMeaning;
    int mode = 1;
    bool meaningDisplayed = false;
};
#endif // MAINWINDOW_H
