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

#include <QDate>

namespace Baseball {
    namespace Ballpark {
        class Record : public CoreRecord
        {
        public:
            Record(const tag& p) : CoreRecord(p) {}

            //PARKID,NAME,AKA,CITY,STATE,START,END,LEAGUE,NOTES

            std::string name;
            std::string nickname;

            std::string city;
            std::string state;  // TODO: enum?

            QDate opened;
            QDate closed;

            League league;

            std::string notes;

            std::string print() const;

            bool isActive() const { return closed.isValid(); }
        };

        /* ballpark filters:
         *   name
         *   nickname
         *   city
         *   state
         *   league
         *   closed [date/duration]
         *   opened [date/duration]
         *   active [date/duration]
         */

        // the goal with processing text based input from the user was not to fully parse and understand natural
        // language, but rather to restrict the grammar of that language in such as way that it still feels
        // somewhat natural to the user, while at the same time providing a more concrete and syntacitally defined
        // structure.

        class Table : public CoreTable<Record>
        {

        };
    }
}
