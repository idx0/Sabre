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
#include "sabre_output.h"

namespace Sabre {

    const int Output::m_sMAX_OUTPUT_LEN = 8192;

    void Output::log(const QString& sz)
    {
        m_mutex.lock();
        m_text.append(sz);
        m_text.append("\n");
        m_mutex.unlock();

        emit textChanged(m_text.right(m_sMAX_OUTPUT_LEN));
    }


    void Output::log(const QStringList& sl)
    {
        m_mutex.lock();

        m_text.append("[ ");
        m_text.append(sl.join(", "));
        m_text.append(" ]\n");

        m_mutex.unlock();

        emit textChanged(m_text.right(m_sMAX_OUTPUT_LEN));
    }


    void Output::log(const std::string& sz)
    {
        m_mutex.lock();
        m_text.append(sz.c_str());
        m_text.append("\n");
        m_mutex.unlock();

        emit textChanged(m_text.right(m_sMAX_OUTPUT_LEN));
    }


    void Output::log(const char* fmt, ...)
    {
        static const unsigned int BUFLEN = 1024;
        char buf[BUFLEN];
        va_list args;

        m_mutex.lock();

        va_start(args, fmt);
        vsnprintf(buf, BUFLEN, fmt, args);
        va_end(args);

        m_text.append(buf);
        m_text.append("\n");

        m_mutex.unlock();

        emit textChanged(m_text.right(m_sMAX_OUTPUT_LEN));
    }


    void Output::raw(const char* fmt, ...)
    {
        static const unsigned int BUFLEN = 1024;
        char buf[BUFLEN];
        va_list args;

        m_mutex.lock();

        va_start(args, fmt);
        vsnprintf(buf, BUFLEN, fmt, args);
        va_end(args);

        m_text.append(buf);

        m_mutex.unlock();

        emit textChanged(m_text.right(m_sMAX_OUTPUT_LEN));
    }


    void Output::flush()
    {
        m_text.append("\n");

        emit textChanged(m_text.right(m_sMAX_OUTPUT_LEN));
    }

}
