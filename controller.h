#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <windows.h>
#include <QTimer>
#include <QSettings>

class Controller : public QObject
{
    Q_OBJECT
public:
    QTimer* timer;
    Controller();

public slots:
    void readInput();

private slots:
    void pollControllers();

signals:
    void ButtonPressed();
    void Paused();

private:
    QSettings settings;
    DWORD currentPacket;
};

#endif // CONTROLLER_H
