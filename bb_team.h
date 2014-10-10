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
#pragma once

#include "bb_defs.h"
#include "bb_record.h"

#include <vector>
#include <QDate>

namespace Baseball {
    namespace Team {
        class Record : public CoreRecord
        {
        public:
            Record(const tag& p) : CoreRecord(p) {}

            // year this team debuted
            QDate debut;

            struct Year : public RecordYear {
                std::string location;
                std::string name;

                League league;
            };

            static const Year NULL_YEAR;

            // adds a year if not already in the record, or returns
            // the given year
            Year& year(int yr);
            Year& operator[](int yr);

            // returns the NULL_YEAR if this year is not on record, or
            // returns the given year
            const Year& year(int yr) const;
            const Year& operator[](int yr) const;

        protected:

            typedef std::map<int, Year> Years;

            Years m_years;
        };


        class Table : public CoreTable<Record>
        {

        };
    }
}
