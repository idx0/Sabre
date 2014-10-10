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
#include "sabre_search.h"
#include "baseball.h"

#include <QStringList>
#include <sstream>

namespace Sabre {

    void searchPlayer(Output* output, const QString& search)
    {
        QStringList sl = search.split(QRegExp("\\s+"));

        Baseball::Player::Table::Reference r = Baseball::Player::Table::begin();
        Baseball::Player::Table::RecordList rl;

        while (r != Baseball::Player::Table::end()) {
            Baseball::Player::Record* p = r.record();
            bool matched = false;

            if (p) {
                QString ln(p->surName.c_str());

                for (int j = 0; j < sl.size(); j++) {
                    if (ln.contains(sl.at(j), Qt::CaseInsensitive)) {
                        rl.push_back(p);
                        matched = true;
                        break;
                    }
                }


                if (!matched) {
                    QString fn(p->firstName.c_str());

                    for (int i = 0; i < sl.size(); i++) {
                        if (fn.contains(sl.at(i), Qt::CaseInsensitive)) {
                            rl.push_back(p);
                            break;
                        }
                    }
                }
            }

            r.next();
        }

        Baseball::Player::Table::RecordList::iterator it;
        std::ostringstream oss;

        it = rl.begin();

        while (it != rl.end()) {
            Baseball::Player::Record* p = (*it);

            oss << p->id().toString().c_str() << "  " << p->firstName << ' ' << p->surName << std::endl;

            it++;
        }

        output->log(oss.str());

    }

}
