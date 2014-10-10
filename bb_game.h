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
#include "bb_state.h"

#include <map>
#include <QDateTime>

namespace Baseball {
    namespace Game {

        // A Instance is a unique game index denoted by a combination of data
        // including base/out state, inning, and number of runs scored.  The instance
        // is defined such that every state in a game has a unique instance
        // representing it.
        class Instance
        {
        public:
            Instance(const BaseOut& bo, uint i, int r);
            Instance();

            BaseOut baseOut;
            uint inning;
            int runs;

        public:
            Instance& operator=(const Instance& rhs);

            bool operator==(const Instance& rhs) const;
            bool operator!=(const Instance& rhs) const;

            bool operator<(const Instance& rhs) const;
            bool operator<=(const Instance& rhs) const;

            bool operator>(const Instance& rhs) const;
            bool operator>=(const Instance& rhs) const;

        public:
            static const Instance STARTER;
        };

        class Bucket
        {
        public:
            Bucket();
            virtual ~Bucket();

            uint& operator[](const uint rhs);

            virtual uint sum() const;

        protected:

            static const uint BUCKET_MAX;

            uint *m_buckets;
            uint m_sum;
        };

        class Lineup
        {
        public:
            Lineup() {}

            // sub a player into the game, this can be used
            // for starting lineups as well
            bool sub(const player_tag& pt,
                     const Instance& inst,
                     const Position& p,
                     const uint order,          // batting order in lineup
                     bool visitor = false);     // true if visiting team player

            struct Card
            {
                Position position;
                // can be zero for pitchers in the AL
                uint order;
                // the instance for this player
                Instance instance;
                // true if visiting team
                bool visiting;

                Card& operator=(const Card& rhs)
                {
                    if (this != &rhs) {
                        instance = rhs.instance;
                        position = rhs.position;
                        order    = rhs.order;
                        visiting = rhs.visiting;
                    }

                    return *this;
                }
            };

        public:

            Card card(const player_tag& t) const;

            // returns the first player tag found at inst
            player_tag find(const Instance& inst) const;

            // returns the player tag of the player at position p
            // - the player is returned from the visiting team's roster if visitor
            //   is set to true, otherwise the home team's roster is used.
            // - the after instance may be used to restrict rosters to after a
            //   certain instance has occured
            player_tag find(const Position& p,
                            bool visitor = false,
                            const Instance& after = Instance::STARTER) const;
            player_tag find(const uint order,
                            bool visitor = false,
                            const Instance& after = Instance::STARTER) const;

        private:

            typedef std::map<player_tag, Card> LineupList;

            LineupList m_lineup;

            LineupList::iterator findOrder(uint o, bool visitor);
        };


        class Record : public CoreRecord
        {
        public:
            Record(const tag& ref) : CoreRecord(ref) {}

            enum Type
            {
                Unknown = 0,
                SingleGame,
                // First game of a double header
                DoubleHeaderFirst,
                // Second game of a double header
                DoubleHeaderSecond,
            };

            // The date and start time of this game.  If the game does not have a start
            // time listed, 00:00pm is used.
            QDateTime startTime;

            int year;

            // The type of game
            Type type;

            // field conditions
            Sky sky;
            FieldCondition condition;
            Precipitation precipitation;
            float temperature;
            WindDirection windDirection;
            float windSpeed;

            // True if DH is used
            bool useDH;

            int attendance;
            int duration;

            // true if night game
            bool night;

            // reference to the ballpark
            ballpark_tag ballpark;

            // home/away team references
            team_tag teamHome;
            team_tag teamVisiting;

            // additional notes or comments
            commment com;

            // pitchers
            player_tag pitcherW;
            player_tag pitcherL;
            player_tag pitcherSave;

            // runs scored
            int runsHome;
            int runsVisited;

            // this serves essentially as a list of all players who played in this game
            Lineup lineup;

            // state lists
            StateLink plays;
        };

        class Table : public CoreTable<Record>
        {

        };
    }
}
