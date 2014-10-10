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
#include "bb_stat.h"

#include <sstream>
#include <iomanip>

namespace Baseball {
    namespace Stat {

        Bin& Bin::operator++()
        {
            value++;
            return *this;
        }

        Bin& Bin::operator++(int)
        {
            Bin r(*this);
            ++(*this);
            return r;
        }

        Bin& Bin::operator+=(const Bin& b)
        {
            if (this != &b) {
                value += b.value;
            }

            return *this;
        }

        Bin& Bin::operator+=(uint v)
        {
            value += v;

            return *this;
        }

        Bin& Bin::operator--()
        {
            value--;
            return *this;
        }

        Bin& Bin::operator--(int)
        {
            Bin r(*this);
            --(*this);
            return r;
        }

        Bin& Bin::operator-=(const Bin& b)
        {
            if (this != &b) {
                value -= b.value;
            }

            return *this;
        }

        Bin& Bin::operator-=(uint v)
        {
            value -= v;

            return *this;
        }

        Bin& Bin::operator=(const Bin& b)
        {
            if (this != &b) {
                value = b.value;
            }

            return *this;
        }

        Bin& Bin::operator=(uint v)
        {
            value = v;

            return *this;
        }

        bool Bin::operator==(const Bin& b) const
        {
            return (value == b.value);
        }

        bool Bin::operator==(uint v) const
        {
            return (value == v);
        }

        bool Bin::operator!=(const Bin& b) const
        {
            return (false == operator==(b));
        }

        bool Bin::operator!=(uint v) const
        {
            return (false == operator==(v));
        }

        bool Bin::operator<(const Bin& b) const
        {
            return (value < b.value);
        }

        bool Bin::operator<(uint v) const
        {
            return (value < v);
        }

        bool Bin::operator<=(const Bin& b) const
        {
            return (value <= b.value);
        }

        bool Bin::operator<=(uint v) const
        {
            return (value <= v);
        }

        bool Bin::operator>(const Bin& b) const
        {
            return (value > b.value);
        }

        bool Bin::operator>(uint v) const
        {
            return (value > v);
        }

        bool Bin::operator>=(const Bin& b) const
        {
            return (value >= b.value);
        }

        bool Bin::operator>=(uint v) const
        {
            return (value >= v);
        }

        Bin Bin::operator+(const Bin& b) const
        {
            return (value + b.value);
        }

        Bin Bin::operator-(const Bin& b) const
        {
            return (value - b.value);
        }

        Bin Bin::operator*(const Bin& b) const
        {
            return (value * b.value);
        }

        Bin Bin::operator/(const Bin& b) const
        {
            return (value / b.value);
        }

        Bin Bin::operator+(uint v) const
        {
            return (value + v);
        }

        Bin Bin::operator-(uint v) const
        {
            return (value - v);
        }

        Bin Bin::operator*(uint v) const
        {
            return (value * v);
        }

        Bin Bin::operator/(uint v) const
        {
            return (value / v);
        }


        std::ostream& operator<<(std::ostream& os, const Bin& b)
        {
            os.operator <<(b.value);

            return os;
        }

        ///////////////////////////////////////////////////////////////////////
        //                                                                   //
        ///////////////////////////////////////////////////////////////////////

        Batting::Batting() :
            H1B(0), H2B(0), GDR(0), H3B(0), HR(0), RBI(0),
            HBP(0), K(0), BB(0), IBB(0),
            SF(0), SH(0), FC(0), DP(0), RBOE(0), INT(0),
            AB(0), PA(0)
        {
        }


        Bin Batting::H() const
        {
            return H1B + H2B + H3B + HR + GDR;
        }


        Metric Batting::OBP() const
        {
            Bin n = H() + BB + IBB + HBP;
            Bin d = AB + BB + IBB + HBP + SF;

            return MTR(n) / MTR(d);
        }


        Metric Batting::SLG() const
        {
            Metric n = MTR(H1B) + MTR(2 * H2B) + MTR(3 + H3B) + MTR(4 + HR);
            Bin d = AB;

            return n / MTR(d);
        }


        Fielding::Fielding() :
            A(0), E(0), PO(0)
        {
        }


        Pitching::Pitching() :
            IP(0), H(0), R(0), ER(0), BB(0), SO(0), WP(0),
            W(0), L(0), SV(0)
        {
        }


        BaseRunning::BaseRunning() :
            SB(0), CS(0)
        {
        }


        General::General() :
            GS(0), GP(0)
        {
        }

        ///////////////////////////////////////////////////////////////////////
        //                                                                   //
        ///////////////////////////////////////////////////////////////////////

        Print::Print(int rows) : m_rows(rows)
        {

        }


        Print::~Print()
        {
            for (int i = 0; i < m_columns.size(); i++) {
                delete m_columns[i];
            }
        }


        std::string Print::print() const
        {
            std::vector<int> colMins;
            unsigned int i;
            int row = 0;

            // get column widths
            for (i = 0; i < m_columns.size(); i++) {
                BinRef m = m_columns[i]->max;
                if (m) {
                    colMins.push_back(std::max(numLen(*m),
                                      (unsigned int)m_columns[i]->label.length()));
                } else {
                    colMins.push_back((unsigned int)m_columns[i]->label.length());
                }
            }

            // get width of first column (row labels)
            int rowmin = 0;
            StringList::const_iterator it = m_rowLabels.begin();
            while (it != m_rowLabels.end()) {
                if ((*it).length() > rowmin) { rowmin = (*it).length(); }
                it++;
            }

            std::ostringstream oss;

            // print columns
            oss << std::right;
            oss << std::setw(rowmin + 3) << ' ';
            oss << printLine(0, colMins);

            oss << std::right;
            oss << std::setw(rowmin + 5) << "| ";
            for (i = 0; i < m_columns.size(); i++) {
                oss << std::setw(colMins[i]);
                oss << m_columns[i]->label << " | ";
            }
            oss << std::endl;

            // print rows
            it = m_rowLabels.begin();
            while ((it != m_rowLabels.end()) &&
                   (row < m_rows)) {

                oss << printLine(rowmin, colMins);

                oss << "| ";
                oss << std::right;
                oss << std::setw(rowmin);
                oss << *(it) << " | ";

                for (int j = 0; j < m_columns.size(); j++) {
                    oss << printBin(*m_columns[j]->bins[row], colMins[j]);
                    oss << " | ";
                }

                oss << std::endl;

                it++;
                row++;
            }

            oss << printLine(rowmin, colMins);

            return oss.str();
        }


        std::string Print::printLine(int llen, std::vector<int>& colLens) const
        {
            std::ostringstream oss;

            oss << '+';

            if (llen > 0) {
                oss << std::setfill('-') << std::setw(llen + 3) << '+';
            }

            for (int i = 0; i < colLens.size(); i++) {
                oss << std::setfill('-') << std::setw(colLens[i] + 3) << '+';
            }

            oss << std::endl;

            return oss.str();
        }


        unsigned int Print::numLen(Bin b) const
        {
            std::string num = std::to_string(static_cast<unsigned long long>(b.value));

            return num.length();
        }


        std::string Print::printBin(Bin stat, int min) const
        {
            std::ostringstream oss;
            std::string num = std::to_string(static_cast<unsigned long long>(stat.value));

            int nl = num.length();

            oss << std::right;
            oss << std::setw(std::max(nl, min));
            oss << stat;

            return oss.str();
        }


        void Print::setRowLabels(const StringList& lst)
        {
            m_rowLabels = lst;
        }


        void Print::set(BinRef b, int r, int c)
        {
            if ((c >= 0) && (c < m_columns.size())) {
                if ((r >= 0) && (r < m_columns[c]->count)) {
                    m_columns[c]->set(r, b);
                }
            }
        }


        void Print::addColumn(int idx, const std::string l, unsigned int len)
        {
            Column *c = new Column(idx, l, len);

            m_columns.push_back(c);
        }

        ///////////////////////////////////////////////////////////////////////
        //                                                                   //
        ///////////////////////////////////////////////////////////////////////

        Column::Column(int idx,
                       const std::string l,
                       unsigned int len) :
            index(idx),
            label(l),
            min(NULL),
            max(NULL),
            count(len),
            total(0)
        {
            bins = new BinRef[len];
            for (int i = 0; i < len; i++) bins[i] = NULL;
        }


        Column::~Column()
        {
            delete[] bins;
        }


        void Column::set(int idx, BinRef b)
        {
            if (!count) return;

            if ((idx >= 0) && (idx < count)) {
                bins[idx] = b;
            }


            min = NULL;
            max = NULL;
            total = 0;

            for (int i = 0; i < count; i++) {
                if (bins[i]) {
                    if ((!max) || (*bins[i] > *max)) { max = bins[i]; }
                    if ((!min) || (*bins[i] < *min)) { min = bins[i]; }

                    total += *bins[i];
                }
            }
        }


        Bin Column::get(int idx) const
        {
            if ((idx >= 0) && (idx < count)) {
                if (bins[idx] != NULL) {
                    return *bins[idx];
                }
            }

            return 0;
        }


        Column& Column::operator=(const Column& rhs)
        {
            if (this != &rhs) {
                index   = rhs.index;
                label   = rhs.label;
                min     = rhs.min;
                max     = rhs.max;
                count   = rhs.count;
                total   = rhs.total;

                delete[] bins;
                bins = new BinRef[count];

                for (int i = 0; i < count; i++) {
                    bins[i] = rhs.bins[i];
                }
            }

            return *this;
        }
    }
}
