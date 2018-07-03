#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"

#include <QMainWindow>
#include <QDir>
#include <QSettings>
#include <QApplication>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onButtonPressed();
    void onPaused();

private slots:
    void on_actionOpen_triggered();

    void on_nextButton_clicked();

    void on_lineNumber_returnPressed();

    void on_actionSave_triggered();

    void on_actionOptions_triggered();

    void on_actionLoad_triggered();

private:
    Ui::MainWindow *ui;
    // Variables to check
    bool paused = false;
    bool reading = false;
    // Preferences
    QString defaultScriptPrefix = "RN";
    QString defaultSaveFileName = "current_script.txt";
    bool defaultFlip = false;
    QSettings settings;
    // Fields related to the current script environment
    QString scriptName = "";
    QDir scriptDir;
    QFileInfoList scripts;
    int scriptNumber = 0;
    int line = 0;
    QStringList lines;
    QClipboard *clipboard = QApplication::clipboard();
    // Functions
    void setScript(QFileInfo file);
    void setScriptDir(QDir dir);
    void nextScript();
    void setLine();
    void nextLine();
    void loadSave();
    void save();
    void initializeSettings();
};

#endif // MAINWINDOW_H
