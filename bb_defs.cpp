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
#include "bb_defs.h"

namespace Baseball {

    const Count Count::INVALID = { 5, 5 };

    ///////////////////////////////////////////////////////////////////////////
    //// Class Member Functions                                            ////
    ///////////////////////////////////////////////////////////////////////////

    Date::Date() : month(0), day(0), year(0)
    {

    }

    Date::Date(int m, int d, int y) : month(m), day(d), year(y)
    {

    }

    Date& Date::operator=(const Date& rhs)
    {
        if (this != &rhs) {
            day = rhs.day;
            month = rhs.month;
            year = rhs.year;
        }

        return *this;
    }

    bool Date::operator==(const Date& rhs) const
    {
        return ((day == rhs.day) &&
                (month == rhs.month) &&
                (year == rhs.year));
    }

    bool Date::operator!=(const Date& rhs) const
    {
        return (false == operator==(rhs));
    }

    bool Date::operator<(const Date& rhs) const
    {
        if (year < rhs.year) { return true; }
        else if (year > rhs.year) { return false; }
        else if (month < rhs.month) { return true; }
        else if (month > rhs.month) { return false; }
        else {
            return (day < rhs.day);
        }
    }

    Date Date::fromString(const std::string& sz, const DateFormat& format)
    {
        int m = 0, d = 0, y = 0;

        switch (format)
        {
        default:
        case MMDDYYYY:
            sscanf(sz.c_str(), "%d/%d/%d", m, d, y);
            break;
        case YYYYMMDD:
            sscanf(sz.c_str(), "%d/%d/%d", y, m, d);
            break;
        }

        return Date(m, d, y);
    }



    Count& Count::operator=(const Count& rhs)
    {
        if (this != &rhs) {
            strikes = rhs.strikes;
            balls   = rhs.balls;
        }

        return *this;
    }

    bool Count::operator==(const Count& rhs) const
    {
        return ((strikes == rhs.strikes) &&
                (balls == rhs.balls));
    }

    bool Count::operator!=(const Count& rhs) const
    {
        return (false == operator==(rhs));
    }


    ///////////////////////////////////////////////////////////////////////////
    //// toString Functions                                                ////
    ///////////////////////////////////////////////////////////////////////////

    std::string toString(const Sky& enumValue)
    {
        return std::string();
    }


    std::string toString(const FieldCondition& enumValue)
    {
        return std::string();
    }


    std::string toString(const Precipitation& enumValue)
    {
        return std::string();
    }


    std::string toString(const WindDirection& enumValue)
    {
        return std::string();
    }


    std::string toString(const Base& enumValue)
    {
        return std::string();
    }


    std::string toString(const Position& enumValue)
    {
        static const std::string names[] = {
            "?", "P", "C", "1B",
            "2B", "3B", "SS", "LF",
            "CF", "RF", "DH", "PH",
            "PR"
        };

        return names[enumValue];
    }


    std::string toString(const Pitch::Type& enumValue)
    {
        return std::string();
    }


    std::string toString(const BattedBall::Type& enumValue)
    {
        return std::string();
    }


    std::string toString(const League& enumValue)
    {
        static const std::string names[] = {
            "NA", // National Association
            "NL", // National League
            "AA", // American Association
            "UA", // Union Association
            "PL", // Players League
            "AL", // American League
            "FL"  // Federal League
        };

        // TODO: add checks here

        return names[enumValue];
    }

    ///////////////////////////////////////////////////////////////////////////
    //// Parse<> Functions                                                 ////
    ///////////////////////////////////////////////////////////////////////////

    template<> Sky Parse<Sky>(const std::string& sz)
    {
        if (sz.compare("cloudy") == 0) {
            return SkyCloudy;
        } else if (sz.compare("dome") == 0) {
            return SkyDome;
        } else if (sz.compare("night") == 0) {
            return SkyNight;
        } else if (sz.compare("overcast") == 0) {
            return SkyOvercast;
        } else if (sz.compare("sunny") == 0) {
            return SkySunny;
        }

        return SkyUnknown;
    }


    template<> FieldCondition Parse<FieldCondition>(const std::string& sz)
    {
        if (sz.compare("dry") == 0) {
            return FieldDry;
        } else if (sz.compare("soaked") == 0) {
            return FieldSoaked;
        } else if (sz.compare("wet") == 0) {
            return FieldWet;
        }

        return FieldUnknown;
    }


    template<> Precipitation Parse<Precipitation>(const std::string& sz)
    {
        if (sz.compare("drizzle") == 0) {
            return PrecipDrizzle;
        } else if (sz.compare("none") == 0) {
            return PrecipNone;
        } else if (sz.compare("rain") == 0) {
            return PrecipRain;
        } else if (sz.compare("showers") == 0) {
            return PrecipShowers;
        } else if (sz.compare("snow") == 0) {
            return PrecipSnow;
        }

        return PrecipUnknown;
    }


    template<> WindDirection Parse<WindDirection>(const std::string& sz)
    {
        if (sz.compare("fromcf") == 0) {
            return DirFromCenter;
        } else if (sz.compare("fromlf") == 0) {
            return DirFromLeft;
        } else if (sz.compare("fromrf") == 0) {
            return DirFromRight;
        } else if (sz.compare("ltor") == 0) {
            return DirLeftToRight;
        } else if (sz.compare("rtol") == 0) {
            return DirRightToLeft;
        } else if (sz.compare("tocf") == 0) {
            return DirToCenter;
        } else if (sz.compare("tolf") == 0) {
            return DirToLeft;
        } else if (sz.compare("torf") == 0) {
            return DirToRight;
        }

        return DirUnknown;
    }


    template<> Base Parse<Base>(const std::string& sz)
    {
        char c = sz.at(0);

        if (c == '1') {
            return First;
        } else if (c == '2') {
            return Second;
        } else if (c == '3') {
            return Third;
        } else if (c == 'B') {
            return Batter;
        } else if (c == 'H') {
            return Home;
        }

        return NoBase;
    }


    template<> Position Parse<Position>(const std::string& sz)
    {
        char c = sz.at(0);

        if (isdigit(c)) {
            return Position(c - '0');
        }

        return NoPosition;
    }


    template<> Pitch::Type Parse<Pitch::Type>(const std::string& sz)
    {
        return Pitch::Unknown;
    }


    template<> BattedBall::Type Parse<BattedBall::Type>(const std::string& sz)
    {
        return BattedBall::Unknown;
    }


    template<> League Parse<League>(const std::string& sz)
    {
        if (sz.compare("NA") == 0) {
            return NA;
        } else if (sz.compare("AA") == 0) {
            return AA;
        } else if (sz.compare("UA") == 0) {
            return UA;
        } else if (sz.compare("PL") == 0) {
            return PL;
        } else if (sz.compare("AL") == 0) {
            return AL;
        } else if (sz.compare("FL") == 0) {
            return FL;
        }

        return NL;
    }

    template<> bool Parse<bool>(const std::string& sz)
    {
        if (sz.compare("true")) return true;

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////

    Out::Out() :
        tagOut(false),
        unassisted(false),
        base(NoBase)
    {

    }


    const Base Advance::INAVLID_BASE = NoBase;

    int Advance::runs() const
    {
        int r = 0;

        for (int i = 0; i < 4; i++) {
            if (m_adv[i] == Home) {
                r++;
            }
        }

        return r;
    }

    Base Advance::operator[](const Base& b) const {
        if ((b >= Batter) && (b <= Third)) {
            return m_adv[b - Batter];
        }

        return const_cast<Base>(INAVLID_BASE);
    }

    Base& Advance::operator[](const Base& b) {
        if ((b >= Batter) && (b <= Third)) {
            return m_adv[b - Batter];
        }

        return const_cast<Base>(INAVLID_BASE);
    }


    Advance& Advance::operator|=(const Advance& rhs)
    {
        if (this != &rhs) {
            for (int i = 0; i < 4; i++) {
                if (rhs.m_adv[i] != NoBase) {
                    m_adv[i] = rhs.m_adv[i];
                }
            }
        }

        return *this;
    }
}
