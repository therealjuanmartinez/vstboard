/******************************************************************************
#    Copyright 2010 Rapha�l Fran�ois
#    Contact : ctrlbrk76@gmail.com
#
#    This file is part of VstBoard.
#
#    VstBoard is free software: you can redistribute it and/or modify
#    it under the terms of the under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    VstBoard is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    under the terms of the GNU Lesser General Public License for more details.
#
#    You should have received a copy of the under the terms of the GNU Lesser General Public License
#    along with VstBoard.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "../precomp.h"

namespace Ui {
    class ConfigDialog;
}

class ConfigDialog : public QDialog {
    Q_OBJECT
public:
    ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();

    static const QString defaultSetupFile();
    static const QString defaultProjectFile();
    static const QString defaultVstPath();
    static void AddRecentSetupFile(const QString &file);
    static void AddRecentProjectFile(const QString &file);

    static const float defaultSampleRate();
    static const int defaultBufferSize();
    static const int defaultSamplePrecision();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ConfigDialog *ui;

public slots:
    void accept();

private slots:
    void on_browseProject_clicked();
    void on_browseSetup_clicked();
    void on_browseVst_clicked();
    void onVstPathIndexChanged(int index);
    void onSetupIndexChanged(int index);
    void onProjectIndexChanged(int index);
};

#endif // CONFIGDIALOG_H
