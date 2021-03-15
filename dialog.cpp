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
 * \file dialog.cpp
 * @licence end@
 */

#include <QSerialPortInfo>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QSettings>

#include "dialog.h"
#include "ui_dialog.h"
#include "settingsdialog.h"
#include "version.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
    , dltMiniServer(this)
    , dltMultimeter(this)
{
    ui->setupUi(this);

    on_pushButtonDefaultSettings_clicked();

    setWindowTitle(QString("DLTMultimeter %1").arg(DLT_MULTIMETER_VERSION));

    ui->pushButtonStop->setDisabled(true);

    connect(&dltMultimeter, SIGNAL(status(QString)), this, SLOT(statusMultimeter(QString)));
    connect(&dltMiniServer, SIGNAL(status(QString)), this, SLOT(statusDlt(QString)));

    connect(&dltMultimeter, SIGNAL(valueMultimeter(QString,QString)), this, SLOT(valueMultimeter(QString,QString)));

    /*  load global settings */
    QSettings settings;
    QString filename = settings.value("autoload/filename").toString();
    bool autoload = settings.value("autoload/checked").toBool();
    bool autostart = settings.value("autostart/checked").toBool();

    /* autoload settings */
    if(autoload)
    {
        dltMultimeter.readSettings(filename);
        dltMiniServer.readSettings(filename);
        restoreSettings();
    }

    /* autostart */
    if(autostart)
    {
        on_pushButtonStart_clicked();
    }

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::restoreSettings()
{
}

void Dialog::updateSettings()
{
}

void Dialog::on_pushButtonStart_clicked()
{
    updateSettings();

    dltMultimeter.start();
    dltMiniServer.start();

    ui->pushButtonStart->setDisabled(true);
    ui->pushButtonStop->setDisabled(false);
    ui->pushButtonDefaultSettings->setDisabled(true);
    ui->pushButtonLoadSettings->setDisabled(true);
    ui->pushButtonSettings->setDisabled(true);
}

void Dialog::on_pushButtonStop_clicked()
{
    dltMultimeter.stop();
    dltMiniServer.stop();

    ui->pushButtonStart->setDisabled(false);
    ui->pushButtonStop->setDisabled(true);
    ui->pushButtonDefaultSettings->setDisabled(false);
    ui->pushButtonLoadSettings->setDisabled(false);
    ui->pushButtonSettings->setDisabled(false);
}

void Dialog::statusMultimeter(QString text)
{
    ui->lineEditStatusMultimeter->setText(text);

    if(text == "" || text == "stopped")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatusMultimeter->setPalette(palette);
    }
    if(text == "started")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatusMultimeter->setPalette(palette);
    }
    if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatusMultimeter->setPalette(palette);
    }
}

void Dialog::statusDlt(QString text)
{
    ui->lineEditStatusDLT->setText(text);

    if(text == "" || text == "stopped")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::white);
        ui->lineEditStatusDLT->setPalette(palette);
    }
    if(text == "listening")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::yellow);
        ui->lineEditStatusDLT->setPalette(palette);
    }
    if(text == "connected")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::green);
        ui->lineEditStatusDLT->setPalette(palette);
    }
    if(text == "error")
    {
        QPalette palette;
        palette.setColor(QPalette::Base,Qt::red);
        ui->lineEditStatusDLT->setPalette(palette);
    }
}

void Dialog::on_pushButtonSettings_clicked()
{
    SettingsDialog dlg(this);

    dlg.restoreSettings(&dltMultimeter, &dltMiniServer);

    if(dlg.exec()==QDialog::Accepted)
    {
        dlg.backupSettings(&dltMultimeter, &dltMiniServer);
        restoreSettings();
    }
}

void Dialog::on_pushButtonDefaultSettings_clicked()
{
    dltMultimeter.clearSettings();
    dltMiniServer.clearSettings();

    restoreSettings();
}

void Dialog::on_pushButtonLoadSettings_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Settings"), "", tr("DLTMultimeter Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        return;
    }

    dltMultimeter.readSettings(fileName);
    dltMiniServer.readSettings(fileName);

    restoreSettings();
}

void Dialog::on_pushButtonSaveSettings_clicked()
{
    updateSettings();

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Settings"), "", tr("DLTMultimeter Settings (*.xml);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
             return;

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    //xml.writeStartDocument();

    xml.writeStartElement("DLTMultimeterSettings");
        dltMultimeter.writeSettings(xml);
        dltMiniServer.writeSettings(xml);
    xml.writeEndElement(); // DLTMultimeterSettings

    //xml.writeEndDocument();
    file.close();
}

void Dialog::on_pushButtonInfo_clicked()
{
    QMessageBox msgBox(this);

    msgBox.setWindowTitle("Info DLTMultimeter");
    msgBox.setTextFormat(Qt::RichText);

    QString text;
    text += QString("Version: %1<br>").arg(DLT_MULTIMETER_VERSION);
    text += "<br>";
    text += "Information and Documentation can be found here:<br>";
    text += "<br>";
    text += "<a href='https://github.com/alexmucde/DLTMultimeter'>Github DLTMultimeter</a><br>";
    text += "<br>";
    text += "This SW is licensed under GPLv3.<br>";
    text += "<br>";
    text += "(C) 2021 Alexander Wenzel <alex@eli2.de>";

    msgBox.setText(text);

    msgBox.setStandardButtons(QMessageBox::Ok);

    msgBox.exec();
}

void Dialog::valueMultimeter(QString value,QString unit)
{
    ui->lineEditUnit->setText(unit);
    ui->lineEditValue->setText(value);

    dltMiniServer.sendValue2(value,unit);
}

