/**
 * @licence app begin@
 * Copyright (C) 2021 Alexander Wenzel
 *
 * This file is part of the DLT Multimeter project.
 *
 * \copyright This code is licensed under GPLv3.
 *
 * \author Alexander Wenzel <alex@eli2.de>
 *
 * \file dialog.h
 * @licence end@
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QTcpServer>
#include <QTcpSocket>

#include "dltminiserver.h"
#include "dltmultimeter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(bool autostart,QString configuration,QWidget *parent = nullptr);
    ~Dialog();

private slots:

    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();

    void statusMultimeter(QString text);
    void statusDlt(QString text);

    void valueMultimeter(QString value,QString unit);

    void on_pushButtonDefaultSettings_clicked();
    void on_pushButtonLoadSettings_clicked();
    void on_pushButtonSaveSettings_clicked();
    void on_pushButtonInfo_clicked();
    void on_pushButtonSettings_clicked();

private:
    Ui::Dialog *ui;

    void restoreSettings();
    void updateSettings();

    DLTMiniServer dltMiniServer;
    DLTMultimeter dltMultimeter;

};
#endif // DIALOG_H
