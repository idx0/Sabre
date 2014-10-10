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
#include "bb_player.h"

#include <stdio.h>
#include <sstream>

namespace Baseball {
    namespace Player {

        const Record::Year Record::NULL_YEAR;

        Record::Year& Record::operator[](TeamYear yr)
        {
            return year(yr);
        }

        const Record::Year& Record::operator[](TeamYear yr) const
        {
            return year(yr);
        }

        Record::Year& Record::year(TeamYear yr)
        {
            return m_years[yr];
        }

        const Record::Year& Record::year(TeamYear yr) const
        {
            Years::const_iterator it = m_years.find(yr);

            if (it != m_years.end()) {
                return it->second;
            }

            return NULL_YEAR;
        }

        bool Record::TeamYear::operator<(const Record::TeamYear& rhs) const
        {
            if (type < rhs.type) { return true; }
            else if (type > rhs.type) { return false; }
            else {
                switch (type) {
                case TEAM:
                    return (tm < rhs.tm);
                    break;
                case TEAMYEAR:
                    if (tm < rhs.tm) { return true; }
                    else if (rhs.tm < tm) { return false; }
                    else {
                        return (yr < rhs.yr);
                    }
                    break;
                case YEAR:
                    return (yr < rhs.yr);
                    break;
                }
            }
        }


        std::string Record::TeamYear::toString() const
        {
            static const int BUFLEN = 32;
            char buf[BUFLEN];

            _snprintf(buf, BUFLEN, "%s %d", tm.toString().c_str(), yr);

            return std::string(buf);
        }


        std::string Record::print() const
        {
           std::ostringstream oss;

           oss << firstName << ' ' << surName << "\n\n";
           oss << "+------+------+--------------+\n"
               << "| Year | Team | Handedness   |\n"
               << "+------+------+--------------+\n";

           Years::const_iterator it = m_years.begin();
           for (; it != m_years.end(); it++) {
               oss << "| " << it->first.yr << " | ";
               oss << truncate(it->first.tm.ref, 4, TruncateFill) << " | ";

               oss << toString(it->second.bats) << " ";
               oss << toString(it->second.throws) << "          |\n";
           }

           oss << "+------+------+--------------+\n";

           return oss.str();
        }


        Record::YearList Record::filter(filterFunc func) const
        {
            YearList list;
            Years::const_iterator it = m_years.begin();

            if (func) {
                for (; it != m_years.end(); it++) {
                    if (func(it->second)) {
                        list.push_back(it->second);
                    }
                }
            } else {
                for (; it != m_years.end(); it++) {
                    list.push_back(it->second);
                }
            }

            return list;
        }

        std::string Record::printCategory(const Stat::Category& cat) const
        {
            switch (cat) {
            default:
            case Stat::CatGeneral:
                return printGeneral();
            case Stat::CatFielding:
                return printFielding();
            case Stat::CatBaseRunning:
                return printBaseRunning();
            case Stat::CatBatting:
                return printBatting();
            case Stat::CatPitching:
                return printPitching();
            }
        }


        std::string Record::printBatting() const
        {
            static const int nStats = 18;
            static const std::string coll[nStats] = {
                "PA", "AB", "1B", "2B", "GDR", "3B", "HR", "RBI",
                "HBP", "K", "BB", "IBB", "SF", "SH",
                "FC", "DP", "RBOE", "INT"
            };

            Stat::Print printer(m_years.size());
            StringList szl;

            // create columns
            for (int i = 0; i < nStats; i++) {
                printer.addColumn(i, coll[i], m_years.size());
            }

            // enumerate years
            Years::const_iterator it = m_years.begin();
            int row = 0;

            while (it != m_years.end()) {
                printer.set(&(it->second.batting.PA), row, 0);
                printer.set(&(it->second.batting.AB), row, 1);
                printer.set(&(it->second.batting.H1B), row, 2);
                printer.set(&(it->second.batting.H2B), row, 3);
                printer.set(&(it->second.batting.GDR), row, 4);
                printer.set(&(it->second.batting.H3B), row, 5);
                printer.set(&(it->second.batting.HR), row, 6);
                printer.set(&(it->second.batting.RBI), row, 7);
                printer.set(&(it->second.batting.HBP), row, 8);
                printer.set(&(it->second.batting.K), row, 9);
                printer.set(&(it->second.batting.BB), row, 10);
                printer.set(&(it->second.batting.IBB), row, 11);
                printer.set(&(it->second.batting.SF), row, 12);
                printer.set(&(it->second.batting.SH), row, 13);
                printer.set(&(it->second.batting.FC), row, 14);
                printer.set(&(it->second.batting.DP), row, 15);
                printer.set(&(it->second.batting.RBOE), row, 16);
                printer.set(&(it->second.batting.INT), row, 17);

                szl.push_back(it->first.toString());

                row++;
                it++;
            }

            printer.setRowLabels(szl);

            return printer.print();
        }


        std::string Record::printFielding() const
        {
            static const int nStats = 3;
            static const std::string coll[nStats] = {
                "PO",
                "A",
                "E"
            };

            Stat::Print printer(m_years.size());
            StringList szl;

            // create columns
            for (int i = 0; i < nStats; i++) {
                printer.addColumn(i, coll[i], m_years.size());
            }

            // enumerate years
            Years::const_iterator it = m_years.begin();
            int row = 0;

            while (it != m_years.end()) {
                printer.set(&(it->second.fielding.PO), row, 0);
                printer.set(&(it->second.fielding.A), row, 1);
                printer.set(&(it->second.fielding.E), row, 2);

                szl.push_back(it->first.toString());

                row++;
                it++;
            }

            printer.setRowLabels(szl);

            return printer.print();
        }


        std::string Record::printPitching() const
        {
            return std::string();
        }


        std::string Record::printBaseRunning() const
        {
            return std::string();
        }


        std::string Record::printGeneral() const
        {
            static const int nStats = 2;
            static const std::string coll[nStats] = {
                "GS",
                "GP"
            };

            Stat::Print printer(m_years.size());
            StringList szl;

            // create columns
            for (int i = 0; i < nStats; i++) {
                printer.addColumn(i, coll[i], m_years.size());
            }

            // enumerate years
            Years::const_iterator it = m_years.begin();
            int row = 0;

            while (it != m_years.end()) {
                printer.set(&(it->second.general.GS), row, 0);
                printer.set(&(it->second.general.GP), row, 1);

                szl.push_back(it->first.toString());

                row++;
                it++;
            }

            printer.setRowLabels(szl);

            return printer.print();
        }

        // approx.
        unsigned long Record::weight() const
        {
            return (firstName.length() + surName.length() + sizeof(Year) * m_years.size());
        }
    }

    template<> Player::Handedness Parse<Player::Handedness>(const std::string& sz)
    {
        char h = sz.at(0);

        if ((h == 'B') || (h == 'S')) {
            return Player::Switch;
        } else if (h == 'L') {
            return Player::Left;
        } else if (h == 'R') {
            return Player::Right;
        }

        return Player::UnknownHandedness;
    }

    std::string toString(const Player::Handedness& enumValue)
    {
        static const char names[] = {
            '?',
            'R',
            'L',
            'S'
        };

        return std::string(1, names[enumValue]);
    }
}
