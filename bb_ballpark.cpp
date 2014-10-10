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
#include "bb_ballpark.h"

#include <sstream>

namespace Baseball {
    namespace Ballpark {
        std::string Record::print() const
        {
            std::ostringstream oss;
            static const size_t MIN_NAME_LEN = 25 - TAGLEN;

            int i, tlen = std::max(MIN_NAME_LEN, name.length()) + TAGLEN + 3 + 4;
            int imax = tlen - 4;

            // top
            oss << '+';
            for (i = 2; i < tlen; i++) oss << '-';
            oss << "+\n";

            // title
            oss << "| " << m_tag.ref << "    " << truncate(name, std::max(MIN_NAME_LEN, name.length()), TruncateFill) << " |\n";
            if (!nickname.empty()) {
                oss << "|  " << truncate(nickname, imax - 1, TruncateFill | TruncateEnquote | TruncateElipsis) << " |\n";
            }

            // sep
            oss << '+';
            for (i = 2; i < tlen; i++) oss << '-';
            oss << "+\n";

            // data
            std::string location = city;
            location.append(", ");
            location.append(state);

            oss << "| " << truncate(location, imax, TruncateFill | TruncateElipsis) << " |\n";

            oss << "| opened: " << truncate(opened.toString("MM/dd/yyyy").toStdString(), imax - 8, TruncateFill) << " |\n";
            if (closed.isValid()) {
                oss << "| closed: " << truncate(closed.toString("MM/dd/yyyy").toStdString(), imax - 8, TruncateFill) << " |\n";
            } else {
                oss << "| " << truncate("        active", imax, TruncateFill) << " |\n";
            }

//            oss << "| " << truncate(notes, imax, true, true) << " |\n";

            // bottom
            oss << '+';
            for (i = 2; i < tlen; i++) oss << '-';
            oss << "+\n";

            return oss.str();
        }
    }
}
