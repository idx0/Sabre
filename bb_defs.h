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

#include <string>
#include <list>
#include <map>
#include <set>
#include <assert.h>

#define TAGLEN  13

#define NOUTS           3
#define NINNINGS        9
#define OUTS_PER_GAME   27

typedef unsigned int uint;

static bool tag_cmp(const char *ref1, const char *ref2)
{
    for (int i = 0; i < TAGLEN; i++) {
        if (ref1[i] != ref2[i]) return false;
    }
    return true;
}

static void tag_cpy(char *ref_to, const char *ref_from)
{
    memset(ref_to, ' ', TAGLEN);
    for (int i = 0; i < TAGLEN; i++) {
        ref_to[i] = ref_from[i];
    }
    ref_to[TAGLEN-1] = 0;
}

static void tag_cpy_s(char *ref_to, const char *ref_from, size_t len_from)
{
    memset(ref_to, ' ', TAGLEN);
    for (size_t i = 0; i < std::min((size_t)TAGLEN, len_from); i++) {
        ref_to[i] = ref_from[i];
    }
    ref_to[TAGLEN-1] = 0;
}


namespace Baseball {

	enum Sky
	{
		SkyUnknown = 0,
		SkySunny,
		SkyCloudy,
		SkyOvercast,
		SkyNight,
		SkyDome
	};

	enum FieldCondition
	{
		FieldUnknown = 0,
		FieldSoaked,
		FieldWet,
		FieldDamp,
		FieldDry
	};

	enum Precipitation
	{
		PrecipUnknown = 0,
		PrecipNone,
		PrecipDrizzle,
		PrecipShowers,
		PrecipRain,
		PrecipSnow
	};

	enum WindDirection
	{
        DirUnknown = 0,
		DirToLeft,
		DirToCenter,
		DirToRight,
		DirLeftToRight,
		DirFromLeft,
		DirFromCenter,
		DirFromRight,
		DirRightToLeft
	};

	enum Base
	{
        NoBase = 0,
        Home,
		Batter,
		First,
		Second,
		Third
	};

	enum Position
	{
        NoPosition = 0,
        Pitcher,
		Catcher,
		FirstBase,
		SecondBase,
		ThirdBase,
		ShortStop,
		LeftField,
		CenterField,
		RightField,
		DesignatedHitter,
		PinchHitter,
		PinchRunner
	};

    enum League
    {
        NA, // National Association
        NL, // National League
        AA, // American Association
        UA, // Union Association
        PL, // Players League
        AL, // American League
        FL  // Federal League
    };

    typedef std::list<Position> PositionList;

    typedef std::list<std::string> StringList;

	typedef std::string commment;

    class tag {
    public:
		enum {
			Unknown = 0,
			Player,
			Umpire,
			Manager,
			Team,
			Game,
			Season,
			BallPark,
			Custom = 128,
		} type;

        char ref[TAGLEN];

        tag() : type(Unknown) {}

		tag& operator=(const tag& rhs) {
			if (this != &rhs) {
				type = rhs.type;
				tag_cpy(ref, rhs.ref);
			}

			return *this;
		}

        std::string toString() const {
            std::string ret;
            for (int i = 0; i < TAGLEN; i++) {
                if (ref[i] != ' ') { ret += ref[i]; }
            }
            return ret;
        }

        bool operator<(const tag& rhs) const
        {
            if (type < rhs.type) { return true; }
            else if (type > rhs.type) { return false; }
            else {
                int i = 0;
                while ((i < (TAGLEN - 1)) &&
                       (tolower(ref[i]) == tolower(rhs.ref[i])))
                {
                    i++;
                }
                return (tolower(ref[i]) < tolower(rhs.ref[i]));
            }
        }

		bool operator==(const tag& rhs) const {
			return ((type == rhs.type) &&
					(tag_cmp(ref, rhs.ref)));
		}

		bool operator!=(const tag& rhs) const {
			return (false == operator==(rhs));
		}
	};

    class player_tag : public tag {
    public:
        player_tag() { type = Player; }
        player_tag(const std::string& r) {
            type = Player;
            tag_cpy_s(ref, r.c_str(), r.length());
        }
    };

    class umpire_tag : public tag {
    public:
        umpire_tag() { type = Umpire; }
        umpire_tag(const std::string& r) {
            type = Umpire;
            tag_cpy_s(ref, r.c_str(), r.length());
        }
    };

    class manager_tag : public tag {
    public:
        manager_tag() { type = Manager; }
        manager_tag(const std::string& r) {
            type = Manager;
            tag_cpy_s(ref, r.c_str(), r.length());
        }
    };

    class team_tag : public tag {
    public:
        team_tag() { type = Team; }
        team_tag(const std::string& r) {
            type = Team;
            tag_cpy_s(ref, r.c_str(), r.length());
        }
    };

    class game_tag : public tag {
    public:
        game_tag() { type = Game; }
        game_tag(const std::string& r) {
            type = Game;
            tag_cpy_s(ref, r.c_str(), r.length());
        }
    };

    class season_tag : public tag {
    public:
        season_tag() { type = Season; }
        season_tag(const std::string& r) {
            type = Season;
            tag_cpy_s(ref, r.c_str(), r.length());
        }
    };

    class ballpark_tag : public tag {
    public:
        ballpark_tag() { type = BallPark; }
        ballpark_tag(const std::string& r) {
            type = BallPark;
            tag_cpy_s(ref, r.c_str(), r.length());
        }
    };

	typedef std::list<player_tag> player_tag_list;
	typedef std::list<umpire_tag> umpire_tag_list;
	typedef std::list<manager_tag> manager_tag_list;
	typedef std::list<team_tag> team_tag_list;
	typedef std::list<game_tag> game_tag_list;
	typedef std::list<season_tag> season_tag_list;
	typedef std::list<ballpark_tag> ballpark_tag_list;

    struct PositionRef
    {
        Position position;
        player_tag tag;

        PositionRef& operator=(const PositionRef& rhs) {
            if (this != &rhs) {
                position = rhs.position;
                tag = rhs.tag;
            }
            return *this;
        }
        bool operator==(const PositionRef& rhs) const {
            return ((position == rhs.position) &&
                    (tag == rhs.tag));
        }
        bool operator!=(const PositionRef& rhs) const {
            return (false == operator==(rhs));
        }
    };

    typedef std::list<PositionRef> PositionRefList;

	class Pitch
	{
	public:
		enum Type {
			// U  unknown or missed pitch
			Unknown = 0,
			// B  ball
			Ball,
			// I  intentional ball
			BallIntentional,
			// V  called ball because pitcher went to his mouth
			BallCalled,
			// K  strike (unknown type)
			Strike,
			// S  swinging strike
			StrikeSwinging,
			// C  called strike
			StrikeCalled,
			// F  foul
			Foul,
			// T  foul tip
			FoulTip,
			// H  hit batter
			HitBatter,
			// L  foul bunt
			BuntFoul,
			// O  foul tip on bunt
			BuntFoulTip,
			// M  missed bunt attempt
			BuntMissed,
			// N  no pitch (on balks and interference calls)
			NoPitch,
			// P  pitchout
			Pitchout,
			// Q  swinging on pitchout
			PitchoutSwinging,
			// R  foul ball on pitchout
			PitchoutFoul,
			// Y  ball put into play on pitchout
			PitchoutInPlay,
			// X  ball put into play by batter
            InPlay
		};

		enum PickOff
		{
			NoPickoff = 0,
			PickoffFirst,
			PickoffSecond,
			PickoffThird,
            CatcherFirst,
            CatcherSecond,
            CatcherThird,
		};

        Type type;

        PickOff pickoff;

		// runner going on pitch
        bool runnerGoing;

		// pitch blocked by catcher
        bool blocked;

    public:
        Pitch(const Type& t = Unknown,
              const PickOff& p = NoPickoff,
              bool g = false,
              bool b = false) :
            type(t),
            pickoff(p),
            runnerGoing(g),
            blocked(b) {}
	};

    typedef std::list<Pitch> Pitches;

    // class representing information about batted balls in play
    class BattedBall
    {
    public:
        BattedBall() : type(None) {}

        static const uint sFIELD_DESIGNATION_LEN = 6;

        enum Type {
            // none means that for this event, there was no ball put in
            // play
            None = 0,
            Unknown,
            Bunt,
            Liner,
            Popup,
            Grounder,
            Fly,
            Foul    // for foul balls caught
        };

        char fieldDesignation[sFIELD_DESIGNATION_LEN];

        Type type;
    };

    // runner advances
    class Advance
    {
    public:
        Advance() { for (int i = 0; i < 4; i++) m_adv[i] = NoBase; }
        ~Advance() {}

        // true if this advancement was on error (otherwise the advancement
        // was caused as a result of the event)
        bool error;

        bool stolen;

        Advance& operator=(const Advance& rhs) {
            if (this != &rhs) {
                error = rhs.error;
                stolen = rhs.stolen;
                m_adv[0] = rhs.m_adv[0];
                m_adv[1] = rhs.m_adv[1];
                m_adv[2] = rhs.m_adv[2];
                m_adv[3] = rhs.m_adv[3];
            }

            return *this;
        }

        // returns the number of runs scored by these advances
        int runs() const;

        Base operator[](const Base& b) const;
        Base& operator[](const Base& b);

        // merge
        Advance& operator|=(const Advance& rhs);

        static const Base INAVLID_BASE;

    protected:
        Base m_adv[4];
    };

    typedef std::list<Advance> Advances;

    // out
    // each out denotes a single out and the assist(s) for that out.  A double
    // play would thus need two out class objects to identify.
    class Out
    {
    public:
        Out();
        ~Out() {}

        // true if a tag out was applied, this should never be true if
        // base is set to Batter.  Additionally, if this is false, any
        // out on a base is assumed to be a force out.
        bool tagOut;

        // true if the out was unassisted
        bool unassisted;

        // if tag is true, this should be the base the runner is
        // advancing toward when tagged.  If this was an out from
        // a fly ball, m_unassisted should be true and this should be
        // Batter.  Otherwise, m_unassisted is intended to mean that
        // the fielder played the ball then stepped on the bag denoted
        // by this variable for a force out.
        Base base;

        // the player making the out
        PositionRef out;

        // The player(s) making the assist
        PositionRefList assists;

        // returns true if this was a fly ball
        bool isFly() const {
            return ((base == Batter) && (unassisted));
        }

        Out& operator=(const Out& rhs) {
            if (this != &rhs) {
                tagOut     = rhs.tagOut;
                unassisted = rhs.unassisted;
                base       = rhs.base;
                out        = rhs.out;
                assists    = rhs.assists;
            }

            return *this;
        }
    };

    typedef std::list<Out> Outs;

    /*
     * Reconstructing Plays from state & event data
     *  - Each event contains up to 3 outs and some number of
     *    runner advancements.  It may also contain a valid batted
     *    ball value if the ball was put in play during this play.
     *
     * Examples:
     *   A grounder hit to the short stop who throws out the runner advancing from first,
     *   but the second baseman coving the bag can't make the throw to first in time, FC.
     *
     * BattedBall.type = Grounder;
     * Out.base = Second;
     * Out.tagOut = false;
     * Out.unassisted = false;
     * Out.out = [xxxxxxxx, 4];
     * Out.assists = [[xxxxxxxx, 6]];
     * Event.type = FC;
     *
     */

    struct Count
    {
        uint strikes;
        uint balls;

        Count& operator=(const Count& rhs);
        bool operator==(const Count& rhs) const;
        bool operator!=(const Count& rhs) const;

        static const Count INVALID;
    };


    class Date
    {
    public:
        Date();
        Date(int m, int d, int y);

        int month;
        int day;
        int year;

        enum DateFormat {
            MMDDYYYY = 0,
            YYYYMMDD
        };

        static Date fromString(const std::string& sz,
                               const DateFormat& format = MMDDYYYY);

        Date& operator=(const Date& rhs);

        bool operator==(const Date& rhs) const;
        bool operator!=(const Date& rhs) const;
        bool operator<(const Date& rhs) const;
    };


    template<typename T>
    bool isValid(T* obj) {
        return (obj != static_cast<T*>(0));
    }

    template<typename T>
    class Singleton
    {
    public:
        static T* getInstance() {
            if (!isValid(m_instance)) {
                m_instance = new (std::nothrow) T;
            }

            assert(m_instance);

            return m_instance;
        }
    private:
        static T* m_instance;
    };

    template<typename T>
    T* Singleton<T>::m_instance = static_cast<T*>(0);

    template<typename K, typename V>
    std::list<K> keys(const std::map<K, V> &m)
    {
        std::list<K> ret;
        typename std::map<K, V>::iterator it = m.begin();

        while (it != m.end()) {
            ret.push_back(it->first);
            it++;
        }

        return ret;
    }

    // template function definition for parse overrides
    template<typename T> T Parse(const std::string& sz) { T i; return i; }

    template<> Sky Parse<Sky>(const std::string& sz);
    template<> FieldCondition Parse<FieldCondition>(const std::string& sz);
    template<> Precipitation Parse<Precipitation>(const std::string& sz);
    template<> WindDirection Parse<WindDirection>(const std::string& sz);
    template<> Base Parse<Base>(const std::string& sz);
    template<> Position Parse<Position>(const std::string& sz);
    template<> Pitch::Type Parse<Pitch::Type>(const std::string& sz);
    template<> BattedBall::Type Parse<BattedBall::Type>(const std::string& sz);
    template<> League Parse<League>(const std::string& sz);

    template<> bool Parse<bool>(const std::string& sz); // true, false

    // to string functions
    std::string toString(const Sky& enumValue);
    std::string toString(const FieldCondition& enumValue);
    std::string toString(const Precipitation& enumValue);
    std::string toString(const WindDirection& enumValue);
    std::string toString(const Base& enumValue);
    std::string toString(const Position& enumValue);
    std::string toString(const Pitch::Type& enumValue);
    std::string toString(const BattedBall::Type& enumValue);
    std::string toString(const League& enumValue);
}
