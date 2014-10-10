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
#include "bb_team.h"

namespace Baseball {
    namespace Team {

        const Record::Year Record::NULL_YEAR;

        Record::Year& Record::operator[](int yr)
        {
            return year(yr);
        }

        const Record::Year& Record::operator[](int yr) const
        {
            return year(yr);
        }

        Record::Year& Record::year(int yr)
        {
            return m_years[yr];
        }

        const Record::Year& Record::year(int yr) const
        {
            Years::const_iterator it = m_years.find(yr);

            if (it != m_years.end()) {
                return it->second;
            }

            return NULL_YEAR;
        }
    }
}
