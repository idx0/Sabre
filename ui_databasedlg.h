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
#ifndef UI_DATABASEDLG_H
#define UI_DATABASEDLG_H

#include <QDialog>
#include <QList>
#include <QDir>

QT_BEGIN_NAMESPACE
class QWidget;
class QFormLayout;
class QVBoxLayout;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QDir;
QT_END_NAMESPACE

class DatabaseDlg : public QDialog
{
    Q_OBJECT
public:
    explicit DatabaseDlg(const QDir& dir, QWidget *parent = 0);

    QList<int> getSelectedYears() const;

signals:

public slots:

    void onLoad();

protected:

    void parseYears();

    struct YearItem
    {
        QListWidgetItem* m_widget;
        int m_year;

        YearItem& operator=(const YearItem& rhs) {
            if (this != &rhs) {
                m_widget = rhs.m_widget;
                m_year = rhs.m_year;
            }

            return *this;
        }
    };

    QListWidget* m_listBox;
    QPushButton* m_btnLoad;

    QDir m_dir;
    QList<YearItem> m_yearItems;
};

#endif // UI_DATABASEDLG_H
