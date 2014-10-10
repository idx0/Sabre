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

#include <QObject>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QList>
#include <QDir>

#include "sabre_output.h"

#include "baseball.h"

class Parser : public QObject
{
    Q_OBJECT

public:
    Parser(const QString& dbPath, Sabre::Output* out) :
        m_output(out),
        m_dbPath(dbPath),
        m_curGame(NULL),
        m_lastState(NULL),
        m_currentState(NULL) {}

    void restrictYears(const QList<int>& years) { m_years = years; }

public slots:

    void parse();

signals:

    void finished();

protected:

    bool yearRestricted(int y);

    Sabre::Output* m_output;

    // this operation reads the list of ballparks and initializes ballpark data
    bool parseBallparks();

    // this operation reads the retroid file and initializes all player data
    bool parseRetroIds();

    bool parseYearlyData();

    // parse files
    bool parseRosters(const QDir& path);
    bool parseTeams(const QDir& path);
    bool parseGameData(const QDir& d, const QDate& y);

    // parse lines
    bool parseFile(const QString& fileName, int year);

    bool parseInfo(const QStringList& info);
    bool parsePlay(const QString& line);
    bool parseSub(const QString& line);

    void parsePlayPitches(const QString& pitches);

    void parseEvent(const QString& eventString);

    int parseEventEv(const QString& evString);
    void parseEventDesc(const QStringList& descList);
    void parseEventAdv(const QString& advString);

private:

    typedef void (Parser::*parseEvTypeFunc)(const QString&);

    struct ParseEvent {
        QRegExp r;
        parseEvTypeFunc pf;
    };

    void parseEvOut(const QString& ev);
    void parseEvHit(const QString& ev);
    void parseEvFC(const QString& ev);
    void parseEvError(const QString& ev);
    void parseEvBatter(const QString& ev);
    void parseEvWalk(const QString& ev);
    void parseEvStrikeout(const QString& ev);
    void parseEvBaseRunning(const QString& ev);

    // This operation parses a string containing a single out.  If this
    // string contains an error, and the error variable is set to a valid
    // address, then that variable is set to the player making the error.
    //
    // This operation is not intended to be used with strings containing
    // multiple outs separted by (%) strings.  This operation will *not*
    // update player record statistics.
    //
    // If the out was unassisted, or the out was not made by the first, second
    // or third baseman, then it is assumed to be Batter (and should be properly
    // set by the caller).  Otherwise, it is assumed to be the base which made
    // the out.
    //
    // If the base could be determined based on the above criterion, then
    // tagOut will be set if there is no force at that base.
    Baseball::Out parseOutString(const QString& sz,
                                 Baseball::PositionRef* error = NULL);

    void parseEvIgnore(const QString& ev);

    void incrementOuts();

    QList<int> m_years;
    QDir m_dbPath;

    QString m_fileName;
    unsigned int m_lineNumber;

    Baseball::Game::Record* m_curGame;
    Baseball::Game::Instance m_curInstance;

    Baseball::StateLink m_lastState;
    Baseball::StateLink m_currentState;

    Baseball::Player::Record* m_currentBatter;
    Baseball::Player::Record* m_currentPitcher;
};
