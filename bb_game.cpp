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
#include "bb_game.h"

namespace Baseball {
    namespace Game {

        const Instance Instance::STARTER = Instance(BaseOut(State::S___0), 0, 0);

        Instance::Instance(const BaseOut& bo, uint i, int r) :
            baseOut(bo),
            inning(i),
            runs(r)
        {

        }


        Instance::Instance() :
            baseOut(State::SNULL),
            inning(0),
            runs(0)
        {
        }


        Instance& Instance::operator=(const Instance& rhs)
        {
            if (this != &rhs) {
                baseOut = rhs.baseOut;
                inning = rhs.inning;
                runs = rhs.runs;
            }

            return *this;
        }


        bool Instance::operator==(const Instance& rhs) const
        {
            return ((baseOut == rhs.baseOut) &&
                    (inning == rhs.inning) &&
                    (runs == rhs.runs));
        }


        bool Instance::operator!=(const Instance& rhs) const
        {
            return (false == operator==(rhs));
        }


        bool Instance::operator<(const Instance& rhs) const
        {
            return (false == operator>=(rhs));
        }


        bool Instance::operator<=(const Instance& rhs) const
        {
            return (false == operator>(rhs));
        }


        bool Instance::operator>(const Instance& rhs) const
        {
            if (inning > rhs.inning) return true;
            else if (inning < rhs.inning) return false;
            else if (runs > rhs.runs) return true;
            else if (runs < rhs.runs) return false;
            else if (baseOut.outs > rhs.baseOut.outs) return true;
            else if (baseOut.outs < rhs.baseOut.outs) return false;
            else return (baseOut.runners() > rhs.baseOut.runners());
        }


        bool Instance::operator>=(const Instance& rhs) const
        {
            return ((true == operator>(rhs)) ||
                    (true == operator==(rhs)));
        }

        ///////////////////////////////////////////////////////////////////////

        bool Lineup::sub(
            const player_tag& pt,
            const Instance& inst,
            const Position& p,
            const uint order,
            bool visitor)
        {
            bool ret = false;
            LineupList::iterator it = m_lineup.find(pt);
            Card c;

            c.order = order;
            c.visiting = visitor;
            c.position = p;
            c.instance = inst;

            if (it == m_lineup.end()) {
//                it = findOrder(order, visitor);
                std::pair<LineupList::iterator, bool> r = m_lineup.insert(std::make_pair(pt, c));

                ret = r.second;
            } else {
                // TODO:
                // we'll ultimately need to find a better way to do this without
                // trashing our previous data
                it->second.order = order;
                it->second.position = p;
            }

            return ret;
        }


        Lineup::Card Lineup::card(const player_tag& t) const
        {
            LineupList::const_iterator it = m_lineup.find(t);
            Card c;

            memset(&c, 0, sizeof(Card));

            if (it != m_lineup.end()) {
                c = it->second;
            }

            return c;
        }


        player_tag Lineup::find(const Instance& inst) const
        {
            LineupList::const_iterator it = m_lineup.begin();

            while (it != m_lineup.end()) {
                if (it->second.instance == inst) {
                    return it->first;
                }

                it++;
            }

            return player_tag();
        }


        player_tag Lineup::find(const Position& p,
                                bool visitor,
                                const Instance& after) const
        {
            LineupList::const_iterator it = m_lineup.begin();
            LineupList::const_iterator best = m_lineup.end();

            while (it != m_lineup.end()) {
                Card c = it->second;

                // is this a match?
                if ((c.position == p) &&
                    (c.visiting == visitor)) {
                    // is this the first match?
                    if (best == m_lineup.end()) {
                        best = it;
                    // otherwise see if this match is better
                    } else {
                        // if this iterator's instance less than after
                        if (c.instance < after) {
                            // this player was subbed in before after, if they
                            // were subbed in after the current best, they are
                            // the new best
                            if (c.instance > best->second.instance) {
                                best = it;
                            }
                        } else if (c.instance > after) {
                            // this player was subbed in after after, if they
                            // were subbed in before the current best, they are
                            // the new best
                            if (c.instance < best->second.instance) {
                                best = it;
                            }
                        } else {
                            // this player was subbed in exactly at after, this
                            // player is the new best only if the current best
                            // doesn't share the same instance.
                            if (c.instance != best->second.instance) {
                                best = it;
                            }
                        }
                    }
                }

                it++;
            }

            if (best == m_lineup.end()) return player_tag();

            return best->first;
        }


        player_tag Lineup::find(const uint order,
                                bool visitor,
                                const Instance& after) const
        {
            LineupList::const_iterator it = m_lineup.begin();
            LineupList::const_iterator best = m_lineup.end();

            while (it != m_lineup.end()) {
                Card c = it->second;

                // is this a match?
                if ((c.order == order) &&
                    (c.visiting == visitor)) {
                    if (best == m_lineup.end()) {
                        best = it;
                    } else {
                        if (c.instance < after) {
                            if (c.instance > best->second.instance) {
                                best = it;
                            }
                        } else if (c.instance > after) {
                            if (c.instance < best->second.instance) {
                                best = it;
                            }
                        } else {
                            if (c.instance != best->second.instance) {
                                best = it;
                            }
                        }
                    }
                }

                it++;
            }

            if (best == m_lineup.end()) return player_tag();

            return best->first;
        }


        Lineup::LineupList::iterator
        Lineup::findOrder(uint o, bool visitor)
        {
            LineupList::iterator it = m_lineup.begin();

            while (it != m_lineup.end()) {
                Card c = it->second;

                if ((c.order == o) && (c.visiting == visitor)) return it;

                it++;
            }

            return it;
        }
    }
}
