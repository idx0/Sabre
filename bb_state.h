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

#include <vector>
#include "bb_defs.h"

namespace Baseball {

    class State;

	// This is the base event type
	class Event
	{
	public:
        enum Type {
			NP = 0,	// no play
            O, // batted ball out
            E, FLE, // error, error on foul fly ball
            B, BDP, // bunt, bunt double play
            BR, // runner hit by batted ball
			K, KC, // strike out
			FL,	// foul
			FO,	// force out
			INT, // interference
			IW, W, // intentional walk, walk
			SF, SH,	// sac fly, sac hit (bunt)
            DP, // double play
            TP, // triple play
            SB, // stolen base
            POCS, PO, // pick off caught stealing, pick off
            CS, // caught stealing
			BK, // balk
			DI, // defensive indifference
			OA, // runner advance unspecified
			PB, WP, // passed ball/wild pitch
			HBP, // hit by pitch
            H1B, H2B, H3B, HR, // single, double, triple, hr
			FC, // fielder's choice
			DGR, // ground rule double
		};

	public:

        Type type;

        Outs outs;

        Advance advance;

        unsigned int runsScored;
	};

    typedef State* StateLink;

	class State
	{
        friend class StateManager;

	public:
		enum Type {
            SNULL = 0,
            // S(first)(second)(third)(outs)
            S___0 = 0x10, S__X0, S_X_0, S_XX0, SX__0, SX_X0, SXX_0, SXXX0,
            S___1 = 0x20, S__X1, S_X_1, S_XX1, SX__1, SX_X1, SXX_1, SXXX1,
            S___2 = 0x30, S__X2, S_X_2, S_XX2, SX__2, SX_X2, SXX_2, SXXX2,
            SENDHALF = 0x40,
            SENDINNING,
            SENDGAME,
		};

        State() : type(SNULL) {}
        State(const Type& t) : type(t) {}

    public:
        Type type;

        Event event;

        PositionRef batter;
        Pitches pitches;

        PositionRefList baseRunners;

        uint inning;

        Count count;

        // true if the visiting team is at bat
        bool visiting;

        int runsHome;
        int runsVisiting;

        // tag for game associated with this state
        game_tag game;

        // link to batter's next state
        StateLink playerLink;

        // link to game's next state
        StateLink gameLink;

    public:

        int runsScored() const { return runsHome + runsVisiting; }
        bool endInning() const;
        unsigned int index() const { return m_index; }

    protected:

        unsigned int m_index;
	};


    class BaseOut
    {
    public:
        BaseOut(bool f = false,
                bool s = false,
                bool t = false,
                uint o = 0) :
            first(f),
            second(s),
            third(t),
            outs(o) {}
        BaseOut(const State::Type& t);

        bool first;
        bool second;
        bool third;

        uint outs;

    public:

        // adds a runner at base, or if out is true, removes
        // the runner
        void runner(const Base& base, bool out = false);

        // applies the given advance (assuming it is legal in this
        // state).  advances must be applied in descending order starting
        // with the lead runner.
        void advance(const Advance& adv);

        // resets the base out to the default state
        void reset();

    public: // various utility functions

        // returns true if runners are in scoring position
        bool scoringPosition() const;

        // returns the number of base runners
        int runners() const;

        // returns true if there is a force at the given base
        bool force(const Base& b) const;

        // returns the base at which an advancing runner can be
        // forced out.  note: the farthest base is returned, so
        // if the bases are loaded, even though there is a force at
        // any base, this function will return Home
        Base forced() const;

        // returns the state type for this base out
        State::Type state() const;

        // returns a stylized represenation of the state, assuming
        // the given inning
        std::string toString(int inning) const;

    public:

        BaseOut& operator=(const BaseOut& rhs);

        bool operator==(const BaseOut& rhs) const;

        bool operator!=(const BaseOut& rhs) const;

    };

    class StateManager : public Singleton<StateManager>
    {
    public:

        StateManager() {}
        ~StateManager();

        static StateLink createState(const State::Type& t = State::SNULL);

        static StateLink at(unsigned int idx);
        static unsigned int count() { return getInstance()->m_states.size(); }

        static void removeState(StateLink state);
        static void removeState(unsigned int idx);

    protected:

        std::vector<StateLink> m_states;
    };

}
