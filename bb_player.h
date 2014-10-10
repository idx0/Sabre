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

#include "bb_record.h"
#include "bb_stat.h"

#include <vector>
#include <QDate>

namespace Baseball {
    namespace Player {

        enum Handedness
        {
            UnknownHandedness = 0,
            Right,
            Left,
            Switch
        };

        class Record : public CoreRecord
        {
        public:
            Record(const tag& p) : CoreRecord(p) {}

            // first/given name
            std::string firstName;

            // last/family/surname
            std::string surName;

            // first year on record
            QDate debut;

            virtual std::string print() const;
            virtual unsigned long weight() const;

        public:

            struct TeamYear {
                enum {
                    TEAM,
                    TEAMYEAR,
                    YEAR
                } type;

                TeamYear(const team_tag& t) : type(TEAM), tm(t) {}
                TeamYear(int y, const team_tag& t) : type(TEAMYEAR), yr(y), tm(t) {}
                TeamYear(int y) : type(TEAM), yr(y) {}

                int yr;
                team_tag tm;

                bool operator<(const TeamYear& rhs) const;

                std::string toString() const;
            };

            // This struct holds "team year" data for this player.
            // A player has a team/year pair for each
            struct Year : public RecordYear
            {
                // tag containing the team for this year
                team_tag team;

                // jersey number worn this year
                unsigned int number;

                // positions player this year
                PositionList positions;

                // throwing hand for this year
                Handedness throws;

                // batting side (left/right/switch) this year
                Handedness bats;

                // stats
                Stat::Batting batting;
                Stat::Fielding fielding;
                Stat::Pitching pitching;
                Stat::BaseRunning baseRunning;
                Stat::General general;
            };

            typedef bool (*filterFunc)(const Year&);

            typedef std::list<Year> YearList;

            static const Year NULL_YEAR;

        public:

            std::string printCategory(const Stat::Category& cat) const;

            // adds a year if not already in the record, or returns
            // the given year
            Year& year(TeamYear yr);
            Year& operator[](TeamYear yr);

            // returns the NULL_YEAR if this year is not on record, or
            // returns the given year
            const Year& year(TeamYear yr) const;
            const Year& operator[](TeamYear yr) const;

            YearList filter(filterFunc func = NULL) const;

        private:

            std::string printBatting() const;
            std::string printFielding() const;
            std::string printPitching() const;
            std::string printBaseRunning() const;
            std::string printGeneral() const;

        protected:

            typedef std::map<TeamYear, Year> Years;

            Years m_years;
        };

        class Table : public CoreTable<Record>
        {

        };

    }   // namespace Player

    template<> Player::Handedness Parse<Player::Handedness>(const std::string& sz);

    std::string toString(const Player::Handedness& enumValue);

}   // namespace Baseball
