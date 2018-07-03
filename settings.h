#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private slots:
    void on_dialogButtons_accepted();

    void on_controllerFlip_stateChanged(int arg1);

private:
    Ui::Settings *ui;
    QSettings settings;
};

#endif // SETTINGS_H
