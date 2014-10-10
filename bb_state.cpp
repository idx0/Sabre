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
#include "bb_state.h"

namespace Baseball {

    bool State::endInning() const
    {
        return ((type == SENDHALF) ||
                (type == SENDINNING));
    }

    ///////////////////////////////////////////////////////////////////////////

    StateManager::~StateManager()
    {
        for (unsigned int i = 0; i < m_states.size(); i++) {
            delete m_states[i];
        }

        m_states.clear();
    }

    StateLink StateManager::createState(const State::Type& t)
    {
        StateLink s = new State(t);

        getInstance()->m_states.push_back(s);

        s->m_index = (getInstance()->m_states.size() - 1);

        return s;
    }

    void StateManager::removeState(unsigned int idx)
    {
        if (idx < getInstance()->m_states.size()) {
            getInstance()->m_states.erase(
                getInstance()->m_states.begin() + idx);
        }
    }

    void StateManager::removeState(StateLink state)
    {
        removeState(state->index());
    }

    StateLink StateManager::at(unsigned int idx)
    {
        if (idx < getInstance()->m_states.size()) {
            return getInstance()->m_states.at(idx);
        }

        return NULL;
    }

    ///////////////////////////////////////////////////////////////////////////

    void BaseOut::runner(const Base& base, bool out)
    {
        switch (base) {
        default: break;
        case First: first = !out; break;
        case Second: second = !out; break;
        case Third: third = !out; break;
        }
    }

    void BaseOut::advance(const Advance& adv)
    {
        first = false;
        second = false;
        third = false;

        for (int b = Batter; b <= Third; b++) {
            if (adv[Base(b)] == First) {
                first = true;
            }

            if (adv[Base(b)] == Second) {
                second = true;
            }

            if (adv[Base(b)] == Third) {
                third = true;
            }
        }
    }

    State::Type BaseOut::state() const
    {
        if ((first) && (second) && (third)) {   //xxx
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::SXXX0; break;
            case 1: return State::SXXX1; break;
            case 2: return State::SXXX2; break;
            }
        } else if ((first) && (second) && (!third)) {   //xx-
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::SXX_0; break;
            case 1: return State::SXX_1; break;
            case 2: return State::SXX_2; break;
            }
        } else if ((first) && (!second) && (!third)) {  //x--
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::SX__0; break;
            case 1: return State::SX__1; break;
            case 2: return State::SX__2; break;
            }
        } else if ((!first) && (!second) && (!third)) { //---
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::S___0; break;
            case 1: return State::S___1; break;
            case 2: return State::S___2; break;
            }
        } else if ((!first) && (second) && (!third)) {  //-x-
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::S_X_0; break;
            case 1: return State::S_X_1; break;
            case 2: return State::S_X_2; break;
            }
        } else if ((!first) && (!second) && (third)) {  //--x
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::S__X0; break;
            case 1: return State::S__X1; break;
            case 2: return State::S__X2; break;
            }
        } else if ((first) && (!second) && (third)) {   //x-x
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::SX_X0; break;
            case 1: return State::SX_X1; break;
            case 2: return State::SX_X2; break;
            }
        } else if ((!first) && (second) && (third)) {   //-xx
            switch (outs) {
            default: return State::SNULL; break;
            case 0: return State::S_XX0; break;
            case 1: return State::S_XX1; break;
            case 2: return State::S_XX2; break;
            }
        }

        return State::SNULL;
    }

    BaseOut::BaseOut(const State::Type& t)
    {
        unsigned int c = static_cast<unsigned int>(t);

        if ((t == State::SNULL) || (t == State::SENDHALF) ||
            (t == State::SENDINNING) || (t == State::SENDGAME)) {
            first = false;
            second = false;
            third = false;
            outs = 0;
        } else {
            third = (c & 0x01);
            second = (c & 0x02);
            first = (c & 0x04);
            outs = ((c >> 4) - 1);
        }
    }


    bool BaseOut::scoringPosition() const
    {
        return ((second) || (third));
    }


    bool BaseOut::force(const Base& b) const
    {
        switch (b) {
        case Home:
            return ((first) && (second) && (third));
        default: return false;
        case First:
            return true;
        case Second:
            return first;
        case Third:
            return ((first) && (second));
        }
    }


    Base BaseOut::forced() const
    {
        if ((first) && (second) && (third)) {
            return Home;
        } else if ((first) && (second)) {
            return Third;
        } else if (first) {
            return Second;
        }

        return First;
    }


    int BaseOut::runners() const
    {
        if ((first) && (second) && (third)) {             //xxx
            return 3;
        } else if (((first) && (second) && (!third)) ||   //xx-
                   ((first) && (!second) && (third)) ||   //x-x
                   ((!first) && (second) && (third))) {   //-xx
            return 2;
        } else if (((first) && (!second) && (!third)) ||  //x--
                   ((!first) && (second) && (!third)) ||  //-x-
                   ((!first) && (!second) && (third))) {  //--x
            return 1;
        }

        return 0;
    }


    BaseOut& BaseOut::operator=(const BaseOut& rhs)
    {
        if (this != &rhs) {
            first = rhs.first;
            second = rhs.second;
            third = rhs.third;
            outs = rhs.outs;
        }

        return *this;
    }


    bool BaseOut::operator==(const BaseOut& rhs) const
    {
        return ((first == rhs.first) &&
                (second == rhs.second) &&
                (third == rhs.third) &&
                (outs == rhs.outs));
    }


    bool BaseOut::operator!=(const BaseOut& rhs) const
    {
        return (false == operator==(rhs));
    }


    std::string BaseOut::toString(int inning) const
    {
        static const int BUFLEN = 256;
        char buf[BUFLEN];
        /*
         * 01   @   0      First inning, no outs, runner on second
         *    /   \
         *   .  o  .
         *    \   /
         *      ^
         */

        _snprintf(buf, BUFLEN,
                  "%02d   %c   %1d \n   /   \\   \n  %c  o  %c  \n   \\   /   \n     ^     \n",
                  inning, (second ? '@' : '.'), outs,
                  (third ? '@' : '.'), (first ? '@' : '.'));

        return std::string(buf);

    }


    void BaseOut::reset()
    {
        outs = 0;
        first = false;
        second = false;
        third = false;
    }

}
