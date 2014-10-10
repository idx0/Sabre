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

#include <vector>
#include <ostream>

namespace Baseball {
    namespace Stat {

        enum Category
        {
            CatBatting = 0,
            CatFielding,
            CatPitching,
            CatBaseRunning,
            CatGeneral
        };

        typedef double Metric;

#       define MTR(x) ((Metric)(x.value))

        class Bin
        {
        public:
            Bin() : value(0) {}
            Bin(uint v) : value(v) {}
            ~Bin() {}

            Bin& operator++();
            Bin& operator++(int);
            Bin& operator+=(const Bin& b);
            Bin& operator+=(uint v);

            Bin& operator--();
            Bin& operator--(int);
            Bin& operator-=(const Bin& b);
            Bin& operator-=(uint v);

            Bin& operator=(const Bin& b);
            Bin& operator=(uint v);

            bool operator==(const Bin& b) const;
            bool operator==(uint v) const;
            bool operator!=(const Bin& b) const;
            bool operator!=(uint v) const;
            bool operator<(const Bin& b) const;
            bool operator<(uint v) const;
            bool operator<=(const Bin& b) const;
            bool operator<=(uint v) const;
            bool operator>(const Bin& b) const;
            bool operator>(uint v) const;
            bool operator>=(const Bin& b) const;
            bool operator>=(uint v) const;

            Bin operator+(const Bin& b) const;
            Bin operator-(const Bin& b) const;
            Bin operator*(const Bin& b) const;
            Bin operator/(const Bin& b) const;
            Bin operator+(uint v) const;
            Bin operator-(uint v) const;
            Bin operator*(uint v) const;
            Bin operator/(uint v) const;

            uint value;
        };

        typedef const Bin* BinRef;

        std::ostream& operator<<(std::ostream& os, const Bin& b);

        struct Column
        {
            Column(int idx, const std::string l, unsigned int len);
            ~Column();

            int index;
            std::string label;

            BinRef min;
            BinRef max;

            unsigned int count;
            Bin total;

            BinRef* bins;

            void set(int idx, BinRef b);
            Bin get(int idx) const;

            Column& operator=(const Column& rhs);
        };


        class Print
        {
        public:
            Print(int rows);
            ~Print();

            void set(BinRef b, int r = 0, int c = 0);
            void setRowLabels(const StringList& lst);
            void addColumn(int idx, const std::string l, unsigned int len);

            std::string print() const;

        protected:

            std::string printLine(int llen, std::vector<int>& colLens) const;
            std::string printBin(Bin stat, int min = -1) const;
            unsigned int numLen(Bin b) const;

        private:
            int m_rows;
            std::vector<Column*> m_columns;

            StringList m_rowLabels;
        };


        struct Batting
        {
            Batting();

            Bin H1B;   // single
            Bin H2B;   // double
            Bin GDR;   // ground rule double
            Bin H3B;   // triple
            Bin HR;    // HR
            Bin RBI;   // RBIs

            Bin HBP;   // hit by pitch
            Bin K;     // strikeout
            Bin BB;    // walk
            Bin IBB;   // intentional walk

            Bin SF;    // sacrifice fly
            Bin SH;    // sacrifice hit
            Bin FC;    // fielder's choice
            Bin DP;    // double play
            Bin RBOE;  // reached base on error
            Bin INT;   // interference

            Bin AB;    // at bats
            Bin PA;    // plate appearences

            // computed stats
            Bin H() const;

            // raw values
            Metric OBP() const;
            Metric SLG() const;

        };

        struct Fielding
        {
            Fielding();

            Bin A;     // assists
            Bin E;     // errors
            Bin PO;    // put outs
        };

        struct Pitching
        {
            Pitching();

            Bin IP;    // innings pitched (x3 outs)
            Bin H;     // hits
            Bin R;     // runs
            Bin ER;    // earned runs
            Bin BB;    // base on balls
            Bin SO;    // strike outs
            Bin WP;    // wild pitch

            Bin W;     // wins
            Bin L;     // losses
            Bin SV;    // saves

            Bin BFP;   // batters faced by pitcher
        };

        struct BaseRunning
        {
            BaseRunning();

            Bin SB;    // stolen bases
            Bin CS;    // caught stealing
        };

        struct General
        {
            General();

            Bin GS;    // games started
            Bin GP;    // games played
        };

    }
}
