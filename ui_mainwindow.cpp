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
#include "ui_mainwindow.h"
#include "ui_databasedlg.h"
#include "parse.h"

#include "baseball.h"
#include "sabre.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QVBoxLayout* vbox = new QVBoxLayout;
    QWidget *widge = new QWidget;
    QFont ft("Courier New", 10);

    m_output = new Sabre::Output;

    m_btnDataDirectory = new QPushButton(tr("Choose Data Directory ..."), this);
    m_result = new QTextEdit(this);
    m_input = new QLineEdit(this);

    connect(m_btnDataDirectory, SIGNAL(clicked()), this, SLOT(onChangeDataDirectory()));

    m_result->setLineWrapMode(QTextEdit::NoWrap);
    m_result->setWordWrapMode(QTextOption::NoWrap);
    m_result->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_result->setReadOnly(true);
    m_result->setFont(ft);

    vbox->addWidget(m_btnDataDirectory);
    vbox->addWidget(m_input);
    vbox->addWidget(m_result);

    widge->setLayout(vbox);

    restoreSettings();

    connect(m_input, SIGNAL(returnPressed()), this, SLOT(onInput()));
    connect(m_output, SIGNAL(textChanged(QString)), this, SLOT(onOutputChanged(QString)));

    setCentralWidget(widge);
}

MainWindow::~MainWindow()
{

}


static bool filterSwitch(const Baseball::Player::Record::Year& y)
{
    if (y.isNull() == true) return false;

    return (y.bats == Baseball::Player::Switch);
}


static bool filterSwitchPlayer(const Baseball::Player::Record* r)
{
    if (r) {
        return (r->filter(filterSwitch).size() > 0);
    }

    return false;
}


void MainWindow::onChangeDataDirectory()
{
    QFileDialog dlg;

    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);

    if (dlg.exec()) {
        QString path = dlg.selectedFiles().at(0);
        DatabaseDlg dbdlg(QDir(path), this);

        if (dbdlg.exec()) {
            Parser *p = new Parser(path, m_output);

            p->restrictYears(dbdlg.getSelectedYears());

            activateWindow();
            raise();

            p->moveToThread(&m_thread);

            connect(&m_thread, SIGNAL(finished()), p, SLOT(deleteLater()));
            connect(p, SIGNAL(finished()), this, SLOT(onParseFinished()));
            connect(this, SIGNAL(parse()), p, SLOT(parse()));

            m_thread.start();

            emit parse();
        }
    }
}


void MainWindow::onParseFinished()
{
    m_thread.quit();
    m_thread.wait();

#if 0
    Baseball::Player::Record* r =
        Baseball::Player::Table::get("bondb001");   // barry bonds

    if (r) {
        m_result->setText(r->print().c_str());
    }
#endif
    Baseball::Player::Table::RecordList rl =
        Baseball::Player::Table::filter(filterSwitchPlayer);

    m_output->log(tr("Found %1 result(s).").arg(rl.size()));

#if 0
    Baseball::Player::Table::RecordList::iterator it;

    for (it = rl.begin(); it != rl.end(); it++) {
        m_result->setText((*it)->print().c_str());
    }
#endif
}


void MainWindow::onInput()
{
    QStringList l = m_input->text().split(QRegExp("\\s+"));

    if (l.size() > 1) {
        if ((l.at(0).compare("ballpark") == 0) ||
            (l.at(0).compare("park") == 0) ||
            (l.at(0).compare("stadium") == 0)) {
            Baseball::Ballpark::Record* r =
                Baseball::Ballpark::Table::get(l.at(1).toStdString());

            if (r) {
                m_output->log(r->print());
            } else {
                m_output->log(tr("Record `%1' was not found.").arg(l.at(1)));
            }

        } else if (l.at(0).compare("player") == 0) {
            if (l.size() < 2) { return; }

            Baseball::Player::Record *r = Baseball::Player::Table::get(l.at(1).toStdString());

            if (r) {
                if (l.size() >= 3) {
                    if (l.at(2).compare("fielding") == 0) {
                        m_output->log(r->printCategory(Baseball::Stat::CatFielding));
                    } else if (l.at(2).compare("general") == 0) {
                        m_output->log(r->printCategory(Baseball::Stat::CatGeneral));
                    } else if (l.at(2).compare("batting") == 0) {
                        m_output->log(r->printCategory(Baseball::Stat::CatBatting));
                    }
                } else {

                }
            }
        } else if (l.at(0).compare("search") == 0) {
            l.pop_front();

            Sabre::searchPlayer(m_output, l.join(QChar(' ')));
        }
    }

    m_input->clear();
}


void MainWindow::onOutputChanged(const QString& output)
{
    m_result->setPlainText(output);
    m_result->moveCursor(QTextCursor::End);
}


void MainWindow::restoreSettings()
{
    // TODO: restore stuff here

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}


void MainWindow::saveSettings()
{
    // TODO: save stuff here

    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();

    QMainWindow::closeEvent(event);
}
