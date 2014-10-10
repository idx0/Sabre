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
#include "bb_record.h"

#include <sstream>

namespace Baseball {

    template<typename E>
    Filter<E>::Filter(const E &enumValue)
    {
        m_set.insert(enumValue);
    }


    template<typename E>
    Filter<E>::Filter(const ValueSet &enumList)
    {
        m_set.insert(enumList.begin(), enumList.end());
    }


    template<typename E>
    bool Filter<E>::add(const E& enumValue)
    {
        std::pair<ValueSet::iterator, bool> ret;

        ret = m_set.insert(enumValue);

        return ret.second;
    }


    template<typename E>
    bool Filter<E>::remove(const E& enumValue)
    {
        return (m_set.erase(enumValue) > 0);
    }


    template<typename E>
    bool Filter<E>::test(const E& enumValue, const Operation& op) const
    {
        ValueSet::const_iterator it = m_set.begin();
        bool ret = false;

        if (m_set.size() == 0) return true;

        while (it != m_set.end()) {
            // if they are equal, set ret to true
            if (enumValue == *it) {
                ret = true;
            } else if (op == And) {
                // if they are unequal with AND set, return false
                return false;
            }

            it++;
        }

        return ret;
    }


    CoreRecord::CoreRecord(const tag& t) : m_tag(t)
    {
        // nothing to do
    }

    std::string CoreRecord::print() const
    {
        return std::string();
    }


    unsigned long CoreRecord::weight() const
    {
        return sizeof(CoreRecord);
    }

    ///////////////////////////////////////////////////////////////////////////
    //                                                                       //
    ///////////////////////////////////////////////////////////////////////////



    ///////////////////////////////////////////////////////////////////////////
    //                                                                       //
    ///////////////////////////////////////////////////////////////////////////

    std::string truncate(const std::string& sz, size_t len, unsigned int format)
    {
        std::ostringstream oss;
        std::string ret;
        unsigned int resLen = 0;
        bool elipsis = false;

        if ((format & TruncateElipsis) &&
            (sz.length() > len)) {
            resLen += 3;
            elipsis = true;
        }

        if (format & TruncateEnquote) {
            resLen += 2;
            oss << '"';
        }

        oss << sz.substr(0, len - resLen);

        if (elipsis) {
            oss << "...";
        }

        if (format & TruncateEnquote) oss << '"';

        ret = oss.str();

        if ((format & TruncateFill) &&
            (ret.size() < len)) {
            ret.append(len - ret.size(), ' ');
        }

        return ret;
    }
}
