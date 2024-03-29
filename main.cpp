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

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("index_zero");
    a.setOrganizationDomain("theindexzero.com/code");
    a.setApplicationName("Sabre");

    a.setStyle(QStyleFactory::create("fusion"));

    MainWindow w;
    w.show();

    return a.exec();
}
