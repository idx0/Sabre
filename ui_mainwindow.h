/**
 *
 * Sabre - A sabermetrically designed database for baseball statistics
 * Copyright (C) 2014  Stephen Schweizer (code@theindexzero.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "sabre_output.h"

#include "parse.h"

QT_BEGIN_NAMESPACE
class QPushButton;
class QTextEdit;
class QLineEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

protected slots:

    void onChangeDataDirectory();
    void onInput();
    void onParseFinished();

    void onOutputChanged(const QString& output);

signals:

    void parse();

protected:

    void saveSettings();
    void restoreSettings();

protected:
    Sabre::Output* m_output;

    QThread m_thread;

    QPushButton* m_btnDataDirectory;
    QTextEdit* m_result;
    QLineEdit* m_input;
};

#endif // MAINWINDOW_H
