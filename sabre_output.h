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
#ifndef SABRE_OUTPUT_H
#define SABRE_OUTPUT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMutex>

#include <string>
#include <stdarg.h>

namespace Sabre {

    class Output : public QObject
    {
        Q_OBJECT
    public:
        Output() {}
        ~Output() {}

        static const int m_sMAX_OUTPUT_LEN;

        void log(const QString& sz);
        void log(const QStringList& sl);
        void log(const std::string& sz);
        void log(const char* fmt, ...);

        void raw(const char* fmt, ...);

        void flush();

    signals:

        void textChanged(const QString& sz);

    protected:

        QString m_text;
        QMutex m_mutex;
    };

}


#endif // SABRE_OUTPUT_H
