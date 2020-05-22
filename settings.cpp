#include "settings.h"
#include "ui_settings.h"

#include <QSettings>
#include <QDir>
#include <QDebug>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->scriptPrefix->setText(settings.value("scriptPrefix").toString());
    ui->saveFileName->setText(settings.value("saveFileName").toString());
    ui->controllerFlip->setChecked(settings.value("controllerFlipped").toBool());
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_dialogButtons_accepted()
{
    settings.setValue("scriptPrefix", ui->scriptPrefix->text());
    settings.setValue("saveFileName", ui->saveFileName->text());
}

void Settings::on_controllerFlip_stateChanged(int arg1)
{
    qDebug() << arg1;
    if(arg1 == 2) {
        qDebug() << "flipped";
        settings.setValue("controllerFlipped", true);
    } else {
        settings.setValue("controllerFlipped", false);
    }
}
