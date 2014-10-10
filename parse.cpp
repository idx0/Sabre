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
#include "parse.h"
#include "baseball.h"

#include <QFile>
#include <QDir>
#include <QThread>


static QStringList splitString(const QString& sz)
{
    QStringList l;
    int start = 0;
    bool insz = false;

    for (int i = 0; i < sz.length(); i++) {
        char c = sz.at(i).toLatin1();

        if (c == '"') {
            insz = !insz;
        } else if ((c == ',') && (!insz)) {
            int len = (i - start);
            if (len > 0) {
                l.push_back(sz.mid(start, len));
            } else {
                l.push_back(QString());
            }
            start = i + 1;
        }
    }

    if (start < sz.length()) {
        l.push_back(sz.right(sz.length() - start));
    }

    if (insz) {
        // we have unclosed double quotes
    }
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void Parser::parse()
{
    parseBallparks();

    parseRetroIds();

    parseYearlyData();

    emit finished();
}

bool Parser::parseBallparks()
{
    bool ret = true;

    QStringList filters;
    filters << "parks.dat";

    QStringList files = m_dbPath.entryList(filters, QDir::Files | QDir::NoSymLinks);
    QFile f(m_dbPath.absolutePath() + QDir::separator() + files.at(0));

    m_output->log("Processing file %s...", f.fileName().toStdString().c_str());

    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!f.atEnd()) {
            QString line = f.readLine();
            QStringList chunks = line.split(",");

            if (chunks.size() >= 9) {
                Baseball::ballpark_tag b(chunks.at(0).toStdString());

                Baseball::Ballpark::Record *r = Baseball::Ballpark::Table::createRecord(b);

                r->name = chunks.at(1).toStdString();
                r->nickname = chunks.at(2).toStdString();
                r->city = chunks.at(3).toStdString();
                r->state = chunks.at(4).toStdString();

                r->opened = QDate::fromString(chunks.at(5), "MM/dd/yyyy");
                r->closed = QDate::fromString(chunks.at(6), "MM/dd/yyyy");

                r->league = Baseball::Parse<Baseball::League>(chunks.at(7).toStdString());

                r->notes = line.section(',', 8).toStdString();
            }
        }
    } else {
        ret = false;
    }

    m_output->log("Processed %d records", Baseball::Ballpark::Table::count());

    return ret;
}


bool Parser::parseRetroIds()
{
    bool ret = true;

    QStringList filters;
    filters << "retroid.dat";

    QStringList files = m_dbPath.entryList(filters, QDir::Files | QDir::NoSymLinks);
    QFile f(m_dbPath.absolutePath() + QDir::separator() + files.at(0));

    m_output->log("Processing file %s...", f.fileName().toStdString().c_str());

    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!f.atEnd()) {
            QString line = f.readLine();
            QStringList chunks = line.split(",");

            if (chunks.size() == 4) {
                // surname, first name, retroid, debut
                // retro id: llllfxnn
                //   llll - first 4 letters of last name
                //      f - first letter of first name
                //      x - 0 if career ended after 1984, 1 if before, 8 if manager (never player), 9 if umpire
                //     nn - duplicate record count starting at 01

                // create a player tag from the retrosheet id
                Baseball::player_tag p(chunks.at(2).toStdString());

                char x = chunks.at(2).toStdString().at(5);

                // TODO: managers/coaches/umpires
                if ((x == '8') || (x == '9')) continue;

                // create a player record
                Baseball::Player::Record* r = Baseball::Player::Table::createRecord(p);

                r->surName = chunks.at(0).toStdString();
                r->firstName = chunks.at(1).toStdString();

                r->debut = QDate::fromString(chunks.at(3), "MM/dd/yyyy");
            }
        }
    } else {
        ret = false;
    }

    m_output->log("Processed %d records", Baseball::Player::Table::count());

    return ret;
}


bool Parser::yearRestricted(int y)
{
    if (m_years.empty()) return true;

    for (int i = 0; i < m_years.size(); i++) {
        int z = m_years.at(i);
        if (z == y) return true;
        // as long as we are less than y keep looking
        if (z > y) return false;
    }

    return false;
}


bool Parser::parseYearlyData()
{
    bool ret = true;

    // this operation looks for yyyy labeled directories off the database root and parses
    // them as yearly roster and play-by-play data
    QFileInfoList fil = m_dbPath.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    for (int i = 0; i < fil.size(); i++) {
        QDir d(fil.at(i).absoluteFilePath());
        QDate y = QDate::fromString(d.dirName(), "yyyy");

        if ((y.isValid()) && (yearRestricted(y.year()))) {
            m_output->log("Processing data files for year %s...", y.toString("yyyy").toStdString().c_str());

            // parse teams
            ret |= parseTeams(d);

            // parse rosters
            ret |= parseRosters(d);

            // parse play-by-play
            ret |= parseGameData(d, y);
        } else {
            ret = false;
        }
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

bool Parser::parseTeams(const QDir& path)
{
    bool ret = true;

    QString file = QString("TEAM%1").arg(path.dirName());
    QFile f(path.absolutePath() + QDir::separator() + file);
    QDate y = QDate::fromString(path.dirName(), "yyyy");

//    m_output->log("Processing file %s...", f.fileName().toStdString().c_str());

    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // retrosheet 8 character id,last name, first name,bats,throws,3 letter team id,position

        int yr = y.year();

        m_output->raw("Parsing Teams:");

        while (!f.atEnd()) {
            QString line = f.readLine();
            QStringList chunks = line.split(",");

            if (chunks.size() == 4) {
                QString tsz = chunks.at(0);
                tsz.remove(QRegExp("[\\r\\n]"));

                Baseball::team_tag t(tsz.toStdString());

                Baseball::Team::Record *r = Baseball::Team::Table::createRecord(t);

                char l = chunks.at(1).toStdString().at(0);

                r->year(yr).validate();
                r->year(yr).league = ((l == 'A') ? Baseball::AL : Baseball::NL);
                r->year(yr).location = chunks.at(2).toStdString();
                r->year(yr).name = chunks.at(3).toStdString();

                m_output->raw(" %s", QString(t.ref).trimmed().toStdString().c_str());
            }
        }

        m_output->flush();
    }

    return ret;
}


bool Parser::parseRosters(const QDir& path)
{
    bool ret = true;

    QStringList filters;
    filters << "*.ROS" << "*.ros";

    QStringList files = path.entryList(filters, QDir::Files | QDir::NoSymLinks | QDir::CaseSensitive);
    QDate y = QDate::fromString(path.dirName(), "yyyy");

    m_output->raw("Processing rosters");

    for (int i = 0; i < files.size(); i++) {
        QFile f(path.absolutePath() + QDir::separator() + files.at(i));

        m_output->raw(".");

        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // retrosheet 8 character id,last name, first name,bats,throws,3 letter team id,position

            while (!f.atEnd()) {
                QString line = f.readLine();
                QStringList chunks = line.split(",");

                if (chunks.size() == 7) {
                    // create a player tag from the retrosheet id
                    Baseball::player_tag p(chunks.at(0).toStdString());

                    // create a player record
                    Baseball::Player::Record* r = Baseball::Player::Table::get(p);

                    // roster setup should create each team/year and set the handedness for that team/year
                    QString tsz = chunks.at(5);
                    tsz.remove(QRegExp("[\\r\\n]"));

                    Baseball::Player::Record::TeamYear t(y.year(), Baseball::team_tag(tsz.toStdString()));

                    r->year(t).validate();

                    r->year(t).bats = Baseball::Parse<Baseball::Player::Handedness>(chunks.at(3).toStdString());
                    r->year(t).throws = Baseball::Parse<Baseball::Player::Handedness>(chunks.at(4).toStdString());

                    //mccua001
                }
            }

        } else {
            // TODO: error: could not open %1
            qWarning("Error: could not open %s", f.fileName().toStdString().c_str());

            ret = false;
        }
    }

    m_output->flush();

    return ret;
}


bool Parser::parseGameData(const QDir& d, const QDate& y)
{
    bool ret = true;

    QStringList filters;
    filters << "*.EVA" << "*.EVN";

    QStringList files = d.entryList(filters, QDir::Files | QDir::NoSymLinks);

    m_output->raw("Processing games");

    for (int i = 0; i < files.size(); i++) {
        QString f(d.absolutePath() + QDir::separator() + files.at(i));

        m_output->raw(".");

        ret |= parseFile(f, y.year());
    }

    m_output->flush();

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

bool Parser::parseFile(const QString& fileName, int year)
{
    bool ret = false;
    QFile file(fileName);
    m_lineNumber = 1;

    m_fileName = fileName;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        m_output->log("Processing file %s...", file.fileName().toStdString().c_str());
        size_t g = Baseball::Game::Table::count();

        while (!file.atEnd()) {
            QString line = file.readLine();
            line.remove(QRegExp("[\\r\\n]"));

            QStringList chunks = line.split(",");

            if (chunks.at(0).compare("id") == 0) {
                Baseball::game_tag g(chunks.at(1).toStdString());

                m_curGame = Baseball::Game::Table::createRecord(g);
                m_curGame->year = year;

                m_curInstance = Baseball::Game::Instance::STARTER;
            } else if (chunks.at(0).compare("info") == 0) {
                if (parseInfo(chunks) == false) {
//                    qWarning("Parse error in file %s line %d",
//                             file.fileName().toStdString().c_str(),
//                             m_lineNumber);
                }
            } else if (chunks.at(0).compare("data") == 0) {
            } else if (chunks.at(0).compare("com") == 0) {
            } else if (chunks.at(0).compare("badj") == 0) {
            } else if ((chunks.at(0).compare("start") == 0) ||
                       (chunks.at(0).compare("sub") == 0)) {
                parseSub(line);
            } else if (chunks.at(0).compare("play") == 0) {
                if (parsePlay(line) == false) {
                    qWarning("Parse error in file %s line %d",
                             file.fileName().toStdString().c_str(),
                             m_lineNumber);
                }
            }

            m_lineNumber++;
        }
    }

    return ret;
}


bool Parser::parseSub(const QString& line)
{
    // add starting roster info to game
    // make sure we create team/years for all players
    // if they don't already have them
    QStringList parts = line.split(",");
    bool ok;
    bool np = false;

    if (!m_curGame) return false;

    // remove start/sub
    parts.pop_front();

    // parse tag
    Baseball::player_tag t(parts.at(0).toStdString());
    parts.pop_front();

    // parse name
    parts.pop_front();

    // parse home/visiting team
    int v = parts.at(0).toInt(&ok);
    if (!ok) return false;
    parts.pop_front();

    // parse order
    uint o = parts.at(0).toUInt(&ok);
    if (!ok) return false;
    parts.pop_front();

    // parse position
    Baseball::Position p = Baseball::Parse<Baseball::Position>(parts.at(0).toStdString());

    // add to the lineup
    np = m_curGame->lineup.sub(t, m_curInstance, p, o, (v == 0));

    // hopefully, at this point all is well, however we want to check for
    // team/years for this player.  If this player does not have a team year
    // for this game, then one should be created here.
    Baseball::Player::Record* r = Baseball::Player::Table::get(t);

    if (r) {
        Baseball::team_tag tt;

        if (v == 0) {
            tt = m_curGame->teamVisiting;
        } else {
            tt = m_curGame->teamHome;
        }

        Baseball::Player::Record::TeamYear t(m_curGame->year, tt);

        r->year(t).validate();

        if (np) {
            r->year(t).general.GP++;

            if (m_curInstance == Baseball::Game::Instance::STARTER) {
                r->year(t).general.GS++;
            }
        }
    }

    return true;
}


bool Parser::parseInfo(const QStringList& info)
{
    bool ret = true;

    if (!m_curGame) return false;

    if (info.size() >= 3) {
        QString var = info.at(1);

        if (var.compare("visteam") == 0) {
            m_curGame->teamVisiting = Baseball::team_tag(info.at(2).toStdString());
        } else if (var.compare("hometeam") == 0) {
            m_curGame->teamHome = Baseball::team_tag(info.at(2).toStdString());
        } else if (var.compare("date") == 0) {
        } else if (var.compare("number") == 0) {
            bool ok;
            int num = info.at(2).toInt(&ok);

            if (ok) {
                switch (num) {
                default:
                case 0:
                    m_curGame->type = Baseball::Game::Record::SingleGame;
                    break;
                case 1:
                    m_curGame->type = Baseball::Game::Record::DoubleHeaderFirst;
                    break;
                case 2:
                    m_curGame->type = Baseball::Game::Record::DoubleHeaderSecond;
                    break;
                }
            } else {
                m_curGame->type = Baseball::Game::Record::Unknown;
            }
        } else if (var.compare("starttime") == 0) {
            // TODO
        } else if (var.compare("daynight") == 0) {
            if (info.at(2).compare("night") == 0) {
                m_curGame->night = true;
            } else {
                m_curGame->night = false;
            }
        } else if (var.compare("usedh") == 0) {
            m_curGame->useDH = Baseball::Parse<bool>(info.at(2).toStdString());
        } else {
            ret = false;
        }
    }

    return ret;
}


bool Parser::parsePlay(const QString& line)
{
    bool ret = true;
    QStringList parts = line.split(",");

    if (!m_curGame) return false;

    // pop the front, which is "play"
    parts.pop_front();

    // get/create the link to the new state
    // if the last state is invalid, or an endgame state, then create a
    // brand new state variable and set the game state
    if ((Baseball::isValid(m_lastState) == false) ||
        (m_lastState->type == Baseball::State::SNULL) ||
        (m_lastState->type == Baseball::State::SENDGAME)) {

        m_currentState = Baseball::StateManager::createState(Baseball::State::S___0);
        m_curGame->plays = m_currentState;

    // if the current state is an inning end, we will allocate a new state
    // and attach it to the chain
    } else if (m_lastState->endInning()) {
        m_currentState = Baseball::StateManager::createState(Baseball::State::S___0);

        m_lastState->gameLink = m_currentState;
    // otherwise, the current state was created on the last play, so use it's link
    } else {
        m_currentState = m_lastState->gameLink;
    }

    if (!m_currentState) return false;

    bool ok;
    Baseball::Game::Instance curInst = m_curInstance;


    // set the inning
    m_currentState->inning = parts.at(0).toUInt(&ok);
    m_curInstance.inning = m_currentState->inning;
    if (!ok) return false;
    parts.pop_front();

    // the second field in the line specifies whether this is from a home
    // or visiting team, since we are storing this for both the game and the
    // batter (as well as fielders and baserunners), this field is not required.
    parts.pop_front();

    // parse the batter
    m_currentState->batter.tag = Baseball::player_tag(parts.at(0).toStdString());
    Baseball::Game::Lineup::Card c = m_curGame->lineup.card(m_currentState->batter.tag);

    m_currentBatter = Baseball::Player::Table::get(m_currentState->batter.tag);

    m_currentState->batter.position = c.position;
    m_currentState->visiting = c.visiting;
    parts.pop_front();


    // parse count
    if (parts.at(0).compare("??")) {
        m_currentState->count = Baseball::Count::INVALID;
    } else {
        m_currentState->count.balls = parts.at(0).at(0).toLatin1() - '0';
        m_currentState->count.strikes = parts.at(0).at(1).toLatin1() - '0';
    }
    parts.pop_front();

    // parse pitches
    parsePlayPitches(parts.at(0));
    parts.pop_front();

    // setup
    Baseball::PositionRef pitcher;
    pitcher.position = Baseball::Pitcher;
    pitcher.tag = m_curGame->lineup.find(pitcher.position,
                                         !c.visiting,
                                         curInst);

    m_currentPitcher = Baseball::Player::Table::get(pitcher.tag);

    // parse the event of the play
    parseEvent(parts.join(","));



//    qDebug("%s", m_curInstance.baseOut.toString(m_currentState->inning).c_str());
    m_currentState->gameLink = Baseball::StateManager::createState(m_curInstance.baseOut.state());

    // if the last play was an end of inning, reset our state type
    m_lastState = m_currentState;

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void Parser::parsePlayPitches(const QString& pitches)
{
    bool runnerGoing = false;
    bool catcherPickoff = false;
    bool blocked = false;

    m_currentState->pitches.clear();

    for (int i = 0; i < pitches.length(); i++) {
        Baseball::Pitch p;
        char c = pitches.at(0).toLatin1();
        bool dontadd = false;

        // runner going
        if (runnerGoing) {
            p.runnerGoing = true;
            runnerGoing = false;
        }

        // catcher blocked
        if (blocked) {
            p.blocked = true;
            blocked = false;
        }

        // catcher pickoff
        if (c == '1') {
            if (catcherPickoff) {
                p.pickoff = Baseball::Pitch::CatcherFirst;
                catcherPickoff = false;
            } else {
                p.pickoff = Baseball::Pitch::PickoffFirst;
            }
        } else if (c == '2') {
            if (catcherPickoff) {
                p.pickoff = Baseball::Pitch::CatcherSecond;
                catcherPickoff = false;
            } else {
                p.pickoff = Baseball::Pitch::PickoffSecond;
            }
        } else if (c == '3') {
            if (catcherPickoff) {
                p.pickoff = Baseball::Pitch::CatcherThird;
                catcherPickoff = false;
            } else {
                p.pickoff = Baseball::Pitch::PickoffThird;
            }
        } else {

            switch (c) {
            case '+': catcherPickoff = true; dontadd = true; break;
            case '*': blocked = true; dontadd = true; break;
            case '.': dontadd = true; break;
            case '>': runnerGoing = true; dontadd = true; break;
            case 'B': p.type = Baseball::Pitch::Ball; break;
            case 'C': p.type = Baseball::Pitch::StrikeCalled; break;
            case 'F': p.type = Baseball::Pitch::Foul; break;
            case 'H': p.type = Baseball::Pitch::HitBatter; break;
            case 'I': p.type = Baseball::Pitch::BallIntentional; break;
            case 'K': p.type = Baseball::Pitch::Strike; break;
            case 'L': p.type = Baseball::Pitch::BuntFoul; break;
            case 'M': p.type = Baseball::Pitch::BuntMissed; break;
            case 'N': p.type = Baseball::Pitch::NoPitch; break;
            case 'O': p.type = Baseball::Pitch::BuntFoulTip; break;
            case 'P': p.type = Baseball::Pitch::Pitchout; break;
            case 'Q': p.type = Baseball::Pitch::PitchoutSwinging; break;
            case 'R': p.type = Baseball::Pitch::PitchoutFoul; break;
            case 'S': p.type = Baseball::Pitch::StrikeSwinging; break;
            case 'T': p.type = Baseball::Pitch::FoulTip; break;
            case 'U': p.type = Baseball::Pitch::Unknown; break;
            case 'V': p.type = Baseball::Pitch::BallCalled; break;
            case 'X': p.type = Baseball::Pitch::InPlay; break;
            case 'Y': p.type = Baseball::Pitch::PitchoutInPlay; break;
            default: dontadd = true; break;
            }
        }

        if (!dontadd) {
            m_currentState->pitches.push_back(p);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void Parser::parseEvent(const QString& eventString)
{
    QStringList al = eventString.split(".");
    QStringList dl = al.at(0).split("/");


    QStringList desc;

    QString adv;
    if (al.size() > 1) {
        adv = al.at(1);
        parseEventAdv(adv);
    }

    QString ev = eventString;//dl.at(0);
    parseEventEv(ev.remove(QRegExp("[#!]")));

    if (dl.size() > 1) {
        dl.pop_front();
        desc = dl;

        parseEventDesc(desc);
    }
}

#define CALL_MEMBER_FN(obj, pf) ((obj)->*(pf))

int Parser::parseEventEv(const QString& evString)
{
    // events are parsed via regex.
    //  batting/fielding
    static const unsigned int PE_SIZE = 13;

    static const ParseEvent pe[PE_SIZE] = {
    //   outs                   ([1-9?E]{0,8}[1-9](\([123B]\))?){1,3}
        { QRegExp("([1-9]{0,8}[1-9](\\([123B]\\))?){1,3}"), &Parser::parseEvOut },
    //   hit                    ([SDT][1-9])|H[^P]R?(\([1-9]\))?
    //   ground rule double     DGR
        { QRegExp("DGR([1-9])?|([SDT][1-9?]*)|H[^P]R?(\\([1-9]\\))?"), &Parser::parseEvHit },
    //   fielder's choice       FC([1-9?])?
        { QRegExp("FC([1-9?])?"), &Parser::parseEvFC },
    //   error                  E[1-9]|FLE[1-9]
        { QRegExp("[1-9]{0,8}E[1-9]|FLE[1-9]"), &Parser::parseEvError },
    //   hit batter             HP
    //   interference           C/E[123]
        { QRegExp("HP|C"), &Parser::parseEvBatter },
    //   strikeout              K(23)?(\+(SB[23H]|CS[23H]|OA|PO[123H]|E[1-9]|WP|PB))?
        { QRegExp("^K(.*)?"), &Parser::parseEvStrikeout },
    //   walk                   (IW?|W)(\+(SB[23H]|CS[23H]|OA|PO[123H]|E[1-9]|WP|PB))?
        { QRegExp("^(IW?|W)(.*)?"), &Parser::parseEvWalk },
    //   no play                NP
        { QRegExp("NP"), &Parser::parseEvIgnore },
    //  base running
    //   caught stealing        CS[23H]\([1-9]E?[1-9]\)
        { QRegExp("(CS[23H](\\([1-9]{0,8}((E[1-9](/TH)?)|[1-9])\\))?(\\(UR\\))?;?)+"), &Parser::parseEvBaseRunning },
    //   defensive indifference DI
    //   other                  OA
    //   passed ball            PB
    //   wild pitch             WP
    //   balk                   BK
        { QRegExp("BK|DI|OA|PB|WP"), &Parser::parseEvBaseRunning },
    //   pickoff                PO[123]\([1-9E]?[1-9]\)
    //   pickoff with CS        POCS[123]\([1-9]{2,4}\)
        { QRegExp("PO[123]\\([1-9]{0,8}((E[1-9](/TH)?)|[1-9])\\)"), &Parser::parseEvBaseRunning },
        { QRegExp("POCS[123H]\\([1-9]{0,8}((E[1-9](/TH)?)|[1-9])\\)"), &Parser::parseEvBaseRunning },
    //   stolen base            SB[23H]
        { QRegExp("(((SB[23])|(SBH(\\(UR\\))?));?)+"), &Parser::parseEvBaseRunning }
    };

    // player assist/out/error string [1-9]{0,8}((E[1-9](/TH)?)|[1-9])

    // as we parse the play, we will determine the next state given our
    // current state and the event which occured
    int matched = 0;

    for (unsigned int i = 0; i < PE_SIZE; i++) {
        if (pe[i].r.indexIn(evString) == 0) {
            matched = pe[i].r.matchedLength();
            CALL_MEMBER_FN(this, pe[i].pf)(evString.left(matched));
            break;
        }
    }

    if (!matched) {
        qDebug("%s [%d]: unmatched string `%s'",
               m_fileName.toStdString().c_str(),
               m_lineNumber,
               evString.toStdString().c_str());
    }

    return matched;
}


void Parser::parseEventDesc(const QStringList& descList)
{

}


void Parser::parseEventAdv(const QString& advString)
{
    // the advance field can be parsed with:
    //    [B123][-X][123H](\(([1-9]{0,8}((E[1-9](/TH[123H]?)?)|[1-9])|TH[1-9]?|NR|UR|NORBI|RBI|WP|[1-9]/INT)\))*
    QStringList advList = advString.split(";");

    Baseball::Player::Record::TeamYear tybat(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamVisiting : m_curGame->teamHome));
    Baseball::Player::Record::TeamYear tyfield(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamHome : m_curGame->teamVisiting));

    static int cnt = 0;

    for (int i = 0; i < advList.size(); i++) {
        QString sz = advList.at(i);

        // some flags relavent to this advance
        bool ur = false;
        bool norbi = false;
        bool rbi = false;
        bool wp = false;
        bool fint = false;
        bool th = false;
        bool err = false;

        QRegExp rx("[B123][-X][123H]");
        Baseball::Advance advance;

        // the first part of each advance is the actual base advancement
        // an '-' indicates that base was successfully taken.  An 'X' means
        // the runner is out at that base.
        if (rx.indexIn(sz) == 0) {
            std::string s = sz.left(rx.matchedLength()).toStdString();
            Baseball::Out o;

            Baseball::Base from = Baseball::Parse<Baseball::Base>(std::string(1, s.at(0)));
            Baseball::Base to = Baseball::Parse<Baseball::Base>(std::string(1, s.at(2)));
            bool out = (s.at(1) == 'X' ? true : false);

            if (out) {
                m_curInstance.baseOut.runner(from, true);

                // create an Out for this state

                o.tagOut = true;
                o.base = to;

            } else {
                // advance the runner
                advance[from] = to;

                // update our instance
                m_curInstance.baseOut.advance(advance);
            }

            if (sz.length() > rx.matchedLength()) {
                QString p = sz.right(sz.length() - rx.matchedLength());

                // parse extra fields
                ur = p.contains("(UR)");
                norbi = (p.contains("(NORBI)") || p.contains("(NR)"));
                rbi = p.contains("(RBI)");
                wp = p.contains("(WP)");
            }

            if ((to == Baseball::Home) && (!out))
            {
                if ((rbi) ||
                    ((!err) && (!norbi))) {
                    if (m_currentBatter) {
                        m_currentBatter->year(tybat).batting.RBI++;
                    }
                }

                if (!ur) {
                    if (m_currentPitcher) {
                        m_currentPitcher->year(tyfield).pitching.ER++;
                    }
                }

                // give player a R

                m_currentState->event.runsScored++;
            }

            if (wp) {
                if (m_currentPitcher) {
                    m_currentPitcher->year(tyfield).pitching.WP++;
                }
            }

            // push out/advance
            if (out) {
                incrementOuts();
                m_currentState->event.outs.push_back(o);
            }

            m_currentState->event.advance |= advance;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// returns true if the regex given is matched at the beginning of sz
static bool regexMatch(const QString& regex, const QString& sz)
{
    QRegExp rx(regex);

    if (rx.indexIn(sz) == 0) {
        return true;
    }

    return false;
}

void Parser::parseEvOut(const QString& ev)
{
    // the basic form for outs is as follows
    //  [1-9]{1,4}(\([123B]\))?
    // events in this function may contain up to 3 outs in this format
    //
    // the first part is a sequence of between 1 and 4 numbers representing
    // baseball positions in standard notation.  Each of these numbers represent
    // a player with an assist for that out with the exception of the right
    // most number in the sequence which represents the player making the out.
    //
    // The second part of the sequence is an operational base number surrounded
    // by parantheses.  This number represents the base of the baserunner being
    // put out.
    QRegExp rx("([1-9]{0,8}[1-9](\\([123B]\\))?)");
    int rc = 0;

    Baseball::Game::Instance preEventInst(Baseball::BaseOut(m_currentState->type),
                                          m_currentState->inning,
                                          m_currentState->runsScored());

    Baseball::Player::Record::TeamYear tyfield(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamHome : m_curGame->teamVisiting));

    if (m_currentPitcher) {
        m_currentPitcher->year(tyfield).pitching.BFP++;
    }

    // set the event here to out
    m_currentState->event.type = Baseball::Event::O;

    while ((rc = rx.indexIn(ev, rc)) != -1) {
        QString s = ev.mid(rc, rx.matchedLength());
        rc += rx.matchedLength();

        bool exBase = false;
        bool error = false;
        Baseball::Base b = Baseball::NoBase;
        Baseball::Out o;

        o.unassisted = true;
        o.tagOut = false;

        for (int i = 0; i < s.length(); i++) {
            char c = s.at(i).toLatin1();

            if ((exBase) &&
                ((c == '1') || (c == '2') ||
                 (c == '3') || (c == 'B'))) {
                b = Baseball::Parse<Baseball::Base>(std::string(1, c));
            } else if (c == 'E') {
                error = true;
            } else if ((c >= '1') && (c <= '9')) {
                // is this the last character of the string or is the next
                // character a '('
                if (error) {
                    // error
                    Baseball::PositionRef p;
                    p.position = Baseball::Parse<Baseball::Position>(std::string(1, c));
                    p.tag = m_curGame->lineup.find(p.position,
                                                   !m_currentState->visiting,
                                                   preEventInst);

                    o.out.position = Baseball::NoPosition;
                    Baseball::Player::Record *rec = Baseball::Player::Table::get(p.tag);

                    // update event type
                    m_currentState->event.type = Baseball::Event::E;

                    if (rec) {
                        rec->year(tyfield).fielding.E++;
                    }
                } else if (((i + 1) >= s.length()) || (s.at(i + 1).toLatin1() == '(')) {
                    // out
                    o.out.position = Baseball::Parse<Baseball::Position>(std::string(1, c));
                    o.out.tag = m_curGame->lineup.find(o.out.position,
                                                       !m_currentState->visiting,
                                                       preEventInst);

                    Baseball::Player::Record *rec = Baseball::Player::Table::get(o.out.tag);

                    if (rec) {
                        // record the statistical put out
                        rec->year(tyfield).fielding.PO++;
                    }

                    incrementOuts();

                    if (m_currentPitcher) {
                        m_currentPitcher->year(tyfield).pitching.IP++;
                    }
                } else {
                    // assist
                    Baseball::PositionRef r;
                    r.position = Baseball::Parse<Baseball::Position>(std::string(1, c));
                    r.tag = m_curGame->lineup.find(r.position,
                                                   !m_currentState->visiting,
                                                   preEventInst);
                    o.assists.push_back(r);
                    o.unassisted = false;

                    Baseball::Player::Record *rec = Baseball::Player::Table::get(r.tag);

                    if (rec) {
                        rec->year(tyfield).fielding.A++;
                    }
                }

                error = false;
            } else if (c == '(') {
                exBase = true;
            } else if (c == ')') {
                exBase = false;
            }
        }

        o.base = b;

        m_currentState->event.outs.push_back(o);
    }
}


void Parser::parseEvHit(const QString& ev)
{
    Baseball::Player::Record::TeamYear tyfield(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamHome : m_curGame->teamVisiting));

    Baseball::Player::Record::TeamYear tybat(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamVisiting : m_curGame->teamHome));

    if (m_currentPitcher) {
        m_currentPitcher->year(tyfield).pitching.BFP++;
        m_currentPitcher->year(tyfield).pitching.H++;
    }

    if (regexMatch("H[^P]R?(\\([1-9]\\))?", ev)) {
        m_currentBatter->year(tybat).batting.HR++;
        m_currentBatter->year(tybat).batting.RBI++;

        Baseball::Advance adv;
        adv[Baseball::Batter] = Baseball::Home;

        m_currentState->event.advance |= adv;
    }
}


void Parser::parseEvFC(const QString& ev)
{
    Baseball::Game::Instance preEventInst(Baseball::BaseOut(m_currentState->type),
                                          m_currentState->inning,
                                          m_currentState->runsScored());

    Baseball::Player::Record::TeamYear tyfield(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamHome : m_curGame->teamVisiting));

    incrementOuts();

    if (m_currentPitcher) {
        m_currentPitcher->year(tyfield).pitching.IP++;
        m_currentPitcher->year(tyfield).pitching.BFP++;
    }

    m_currentState->event.type = Baseball::Event::FC;

    QRegExp rx("[1-9]{0,8}((E[1-9](/TH[1-9]?)?)|[1-9])");
    int rc = 0;

    if ((rc = rx.indexIn(ev)) != -1) {
        QString s = ev.mid(rc, rx.matchedLength());
        Baseball::PositionRef errPos;

        parseOutString(s, &errPos);
    }
}


void Parser::parseEvError(const QString& ev)
{

}


void Parser::parseEvBatter(const QString& ev)
{

}


void Parser::parseEvWalk(const QString& ev)
{
    Baseball::Player::Record::TeamYear tybat(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamVisiting : m_curGame->teamHome));

    if (m_currentBatter) {
        m_currentBatter->year(tybat).batting.BB++;
    }

    m_currentState->event.type = Baseball::Event::W;

    // check for a batter advance, if we have one, ignore the walk advance
    if (m_currentState->event.advance[Baseball::Batter] == Baseball::NoBase) {
        Baseball::Advance a;
        a[Baseball::Batter] = Baseball::First;

        m_currentState->event.advance |= a;
        m_curInstance.baseOut.advance(a);
    }
}


void Parser::parseEvStrikeout(const QString& ev)
{
    Baseball::Game::Instance preEventInst(Baseball::BaseOut(m_currentState->type),
                                          m_currentState->inning,
                                          m_currentState->runsScored());

    Baseball::Player::Record::TeamYear tyfield(m_curGame->year,
        (m_currentState->visiting ? m_curGame->teamHome : m_curGame->teamVisiting));

    bool matched = false;

    incrementOuts();
    m_currentState->event.type = Baseball::Event::K;

    // K([1-9E][1-9])?(\+(DI|OA|PB|WP|BK|stolen base|caught stealing|pickoffs)?
    if (m_currentPitcher) {
        m_currentPitcher->year(tyfield).pitching.SO++;
        m_currentPitcher->year(tyfield).pitching.IP++;
        m_currentPitcher->year(tyfield).pitching.BFP++;
    }

    // if we have a ($$) value, we should attribute the assist and PO to the given numbers,
    // otherwise, the catcher gets the PO

#if 0
    // if we have a '+', parse baserunning event
    static const unsigned int BR_SIZE = 5;

    static const ParseEvent pe[BR_SIZE] = {
        { QRegExp("BK"), &Parser::parseEvBaseRunning },
        { QRegExp("DI|OA|PB|WP"), &Parser::parseEvBaseRunning },
        { QRegExp("(((SB[23])|(SBH(\\(UR\\))?));?)+"), &Parser::parseEvBaseRunning },

        { QRegExp("(CS[23H](\\([1-9E]{0,8}[1-9]\\))?(\\(UR\\))?;?)+"), &Parser::parseEvBaseRunning },
        { QRegExp("PO[123]\\([1-9E]{0,8}[1-9]\\)|POCS[123H]\\([1-9E]{1,8}[1-9]\\)"), &Parser::parseEvBaseRunning }
    };

    for (unsigned int i = 0; i < BR_SIZE; i++) {
        if (pe[i].r.exactMatch(ev)) {
            CALL_MEMBER_FN(this, pe[i].pf)(ev);
            matched = true;
            break;
        }
    }

    if (!matched) {
        qDebug("unmatched string `%s'", ev.toStdString().c_str());
    }
#endif
}


void Parser::parseEvBaseRunning(const QString& ev)
{

}


void Parser::parseEvIgnore(const QString& ev)
{
    Q_UNUSED(ev);
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

Baseball::Out Parser::parseOutString(const QString& sz, Baseball::PositionRef* error)
{
    Baseball::Out ret;

    Baseball::Game::Instance preEventInst(Baseball::BaseOut(m_currentState->type),
                                          m_currentState->inning,
                                          m_currentState->runsScored());

    // [1-9]{0,8}((E[1-9](/TH[123H]?)?)|[1-9])
    bool err = false;
    int tcheck = 0;

    if (error) {
        error->position = Baseball::NoPosition;
    }

    ret.unassisted = true;
    ret.tagOut = false;

    for (int i = 0; i < sz.length(); i++) {
        char c = sz.at(i).toLatin1();

        if (err) {
            // [1-9](/TH[123H]?)?
            if ((c >= '1') && (c <= '9')) {
                if ((tcheck < 3) && (error)) {
                    error->position = Baseball::Parse<Baseball::Position>(std::string(1, c));
                    error->tag = m_curGame->lineup.find(error->position,
                                                        !m_currentState->visiting,
                                                        preEventInst);
                }
            } else if (((c == '/') && (tcheck == 0)) ||
                       ((c == 'T') && (tcheck == 1))) {
                tcheck++;
            } else if ((c == 'H') && (tcheck == 2)) {
                tcheck++;
                // throwing error
            }
        } else if (c == 'E') {
            err = true;
        } else if ((c >= '1') && (c <= '9')) {
            if ((i + 1) == sz.length()) {
                // out
                ret.out.position = Baseball::Parse<Baseball::Position>(std::string(1, c));
                ret.out.tag = m_curGame->lineup.find(ret.out.position,
                                                     !m_currentState->visiting,
                                                     preEventInst);

                ret.base = Baseball::Batter;
                Baseball::BaseOut bo(m_currentState->type);

                if (!ret.unassisted) {
                    switch (ret.out.position) {
                    default: break;
                    case Baseball::FirstBase:
                        ret.base = Baseball::First;
                        if (!bo.force(Baseball::First)) { ret.tagOut = true; }
                        break;
                    case Baseball::SecondBase:
                        ret.base = Baseball::Second;
                        if (!bo.force(Baseball::Second)) { ret.tagOut = true; }
                        break;
                    case Baseball::ThirdBase:
                        ret.base = Baseball::Third;
                        if (!bo.force(Baseball::Third)) { ret.tagOut = true; }
                        break;
                    }
                }

            } else {
                // assist
                Baseball::PositionRef a;
                a.position = Baseball::Parse<Baseball::Position>(std::string(1, c));
                a.tag = m_curGame->lineup.find(a.position,
                                               !m_currentState->visiting,
                                               preEventInst);
                ret.assists.push_back(a);
                ret.unassisted = false;
            }
        }
    }

    return ret;
}

void Parser::incrementOuts()
{
    m_curInstance.baseOut.outs++;

    if (m_curInstance.baseOut.outs >= 3) {
        m_curInstance.baseOut.reset();
    }
}
