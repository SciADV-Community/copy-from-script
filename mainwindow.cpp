#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "controller.h"

#include <QThread>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QSettings>
#include <QClipboard>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->setLayout(ui->mainLayout);
    this->initializeSettings();
    this->loadSave();

    QThread* controllerThread = new QThread(parent);
    Controller* controller = new Controller();
    controller->moveToThread(controllerThread);

    connect(controllerThread, SIGNAL(finished()), controller, SLOT(deleteLater()));
    connect(controllerThread, SIGNAL(started()), controller, SLOT(readInput()));
    connect(controller, SIGNAL(ButtonPressed()), this, SLOT(onButtonPressed()));
    connect(controller, SIGNAL(Paused()), this, SLOT(onPaused()));

    controllerThread->start();
}

MainWindow::~MainWindow()
{
    if(scriptName != "") {
        this->save();
    }
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    // Getting a script from the user
    QFileInfo openFileInfo(QFileDialog::getOpenFileName(this, "Open script"));
    // Set the script
    this->setScript(openFileInfo);
}

void MainWindow::on_nextButton_clicked()
{
    this->nextLine();
}

void MainWindow::setLine()
{
    ui->lineNumber->setText(QString::number(line));
    ui->currentText->setText(lines[line]);
    clipboard->setText(lines[line]);
}

void MainWindow::nextLine()
{
    if(reading && !paused) {
        line++;
        if(line < lines.size()){
            this->setLine();
        } else {
            this->nextScript();
        }
    }
}

void MainWindow::setScript(QFileInfo file)
{
    // Populating the appropriate fields
    scriptName = file.baseName();
    this->setScriptDir(file.absoluteDir());
    // Reading the file into a string list
    QFile script(file.absoluteFilePath());
    if(!script.open(QIODevice::ReadOnly | QIODevice::Text)) { // Attempt to open and if it fails
        QMessageBox::warning(this, "Warning", "Cannot open script : " + script.errorString()); // Show a warning
    }
    // Put the lines in memory
    lines.clear();
    while(!script.atEnd()) {
        QString text = script.readLine();
        lines.append(text);
    }
    script.close();
    // Updating the User Interface
    ui->currentScript->setText("Current Script: " + scriptName + " (" + QString::number(lines.size()) + " lines)");
    // Updating variables
    reading = true;
    line = 0;
    // Updating script position in directory
    scriptNumber = scripts.indexOf(file);
    // Starting the script
    this->setLine();
}

void MainWindow::setScriptDir(QDir dir)
{
    scriptDir = dir;
    QStringList scriptFilters;
    scriptFilters << settings.value("scriptPrefix").toString() + "*"; // TODO Global definition ?
    scripts = scriptDir.entryInfoList(scriptFilters);
}

void MainWindow::nextScript()
{
    scriptNumber++;
    if(scriptNumber < scripts.size()) {
        this->setScript(scripts[scriptNumber]);
    } else {
        ui->currentScript->setText("Current Script: None. Please select a new script.");
        ui->lineNumber->setText("Open script");
    }
}

void MainWindow::on_lineNumber_returnPressed()
{
    if(reading) {
        bool ok;
        int newLine = ui->lineNumber->text().toInt(&ok);
        if(ok) {
            if(newLine < lines.size()) {
                line = newLine;
                this->setLine();
            } else {
                ui->lineNumber->setText("Invalid Number");
            }
        } else {
            ui->lineNumber->setText("Invalid Number");
        }
    }
}

void MainWindow::loadSave()
{
    // Set up the save file path
    QString savePath = QCoreApplication::applicationDirPath();
    savePath.append("/"+settings.value("saveFileName").toString());
    // Check the save file
    QFileInfo saveFileInfo(savePath);
    if(saveFileInfo.exists() && saveFileInfo.isFile()) {
        // If it exists, open it and save its info to a line
        QFile saveFile(savePath);
        if(!saveFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Warning", "Cannot open save : " + saveFile.errorString());
        }
        QString saveInfo = saveFile.readLine();
        saveFile.close();
        // Parse the save info into a list
        QStringList saveArgs = saveInfo.split(",");
        // Set the script
        QFileInfo savedScript(saveArgs[0]);
        this->setScript(savedScript);
        // Set the line
        bool ok;
        line = saveArgs[1].toInt(&ok);
        if(ok) {
            this->setLine();
        }
    }
}

void MainWindow::save()
{
    // Get save path
    QString savePath = QCoreApplication::applicationDirPath();
    savePath.append("/"+settings.value("saveFileName").toString());
    // Attempt to open file
    QFile saveFile(savePath);
    if(!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot write save : " + saveFile.errorString());
    }
    // If it opened fine create a save string
    QString saveArgs = scripts.at(scriptNumber).absoluteFilePath();
    saveArgs.append(",");
    saveArgs.append(QString::number(line));
    // Write the save string to the file
    QTextStream saveStream(&saveFile);
    saveStream.setCodec("UTF-8");
    saveStream.setGenerateByteOrderMark(false);
    saveStream << saveArgs;
    saveFile.close();
}

void MainWindow::on_actionSave_triggered()
{
    this->save();
}

void MainWindow::on_actionLoad_triggered()
{
    this->loadSave();
}

void MainWindow::on_actionOptions_triggered()
{
    Settings settingsDialog;
    settingsDialog.setModal(true);
    settingsDialog.exec();
}

void MainWindow::initializeSettings()
{
    if(!settings.contains("scriptPrefix")) {
        settings.setValue("scriptPrefix", defaultScriptPrefix);
    }
    if(!settings.contains("saveFileName")) {
        settings.setValue("saveFileName", defaultSaveFileName);
    }
    if(!settings.contains("controllerFlipped")) {
        settings.setValue("controllerFlipped", defaultFlip);
    }
}

void MainWindow::onButtonPressed()
{
    this->nextLine();
}

void MainWindow::onPaused()
{
    paused = !paused;
    if(paused) {
        ui->lineNumber->setText("PAUSED");
    } else {
        ui->lineNumber->setText(QString::number(line));
    }
}
