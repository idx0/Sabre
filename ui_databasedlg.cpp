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
#include "ui_databasedlg.h"

#include <QtWidgets>
#include <QVariant>

DatabaseDlg::DatabaseDlg(const QDir& dir, QWidget *parent) :
    QDialog(parent),
    m_dir(dir)
{
    QVBoxLayout* vbox = new QVBoxLayout;

    m_listBox = new QListWidget(this);
    m_btnLoad = new QPushButton(tr("Load"), this);

    m_listBox->setSelectionMode(QListWidget::MultiSelection);

    parseYears();

    m_listBox->selectAll();

    connect(m_btnLoad, SIGNAL(clicked()), this, SLOT(onLoad()));

    vbox->addWidget(m_listBox);
    vbox->addWidget(m_btnLoad);

    setLayout(vbox);
}


void DatabaseDlg::parseYears()
{
    QFileInfoList fil = m_dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    for (int i = 0; i < fil.size(); i++) {
        QDir d(fil.at(i).absoluteFilePath());
        QDate y = QDate::fromString(d.dirName(), "yyyy");

        if (y.isValid()) {
            YearItem yi;

            yi.m_widget = new QListWidgetItem(tr("%1").arg(y.year()));
            yi.m_widget->setSelected(true);
            yi.m_widget->setData(Qt::UserRole + 1, QVariant(y.year()));
            yi.m_year = y.year();

            m_yearItems.push_back(yi);
            m_listBox->addItem(yi.m_widget);

        } else {
            // skip
        }
    }
}


QList<int> DatabaseDlg::getSelectedYears() const
{
    QList<QListWidgetItem*> sel = m_listBox->selectedItems();
    QList<int> ret;

    for (int i = 0; i < sel.size(); i++) {
        QVariant v = sel.at(i)->data(Qt::UserRole + 1);

        ret.push_back(v.toInt());
    }

    return ret;
}


void DatabaseDlg::onLoad()
{
    accept();
}
