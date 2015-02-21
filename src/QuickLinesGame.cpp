/*
  Copyright (C) 2015 Jolla Ltd.
  Contact: Slava Monich <slava.monich@jolla.com>

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "QuickLinesGame.h"
#include "LinesDebug.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

//===========================================================================
// Utilities
//===========================================================================

static bool saveJson(QString aPath, const QJsonObject& aJson)
{
    QFileInfo file(aPath);
    QDir dir(file.dir());
    if (dir.mkpath(dir.absolutePath())) {
        QFile f(file.absoluteFilePath());
        if (!aJson.isEmpty()) {
            if (f.open(QIODevice::WriteOnly)) {
                if (f.write(QJsonDocument(aJson).toJson()) >= 0) {
                    return true;
                } else {
                    qWarning() << "Error writing" << aPath << f.errorString();
                }
            } else {
                qWarning() << "Error opening" << aPath << f.errorString();
            }
        } else if (!f.remove()) {
            qWarning() << "Error removing" << aPath << f.errorString();
        }
    } else {
        qWarning() << "Failed to create" << dir.absolutePath();
    }
    return false;
}

static bool loadJson(QString aPath, QJsonObject& root)
{
    QFile f(aPath);
    if (f.exists()) {
        if (f.open(QIODevice::ReadOnly)) {
            QDEBUG("reading" << aPath);
            root = QJsonDocument::fromJson(f.readAll()).object();
            return true;
        } else {
            QDEBUG("can't open" << aPath << f.errorString());
        }
    }
    return false;
}

//===========================================================================
// QuickLinesGame
//===========================================================================

#define SUPER QObject

QuickLinesGame::QuickLinesGame(QObject* aParent) :
    SUPER(aParent),
    iPrefs(NULL),
    iState(loadState()),
    iScores(loadScores())
{
    QDEBUG((void*)this << "- created");
    if (!iScores) iScores = new LinesScores(NULL);
    if (iState) {
        if (!iState->hasEmptyCells()) {
            QDEBUG("the last game was over, starting new one");
            restart();
        }
    } else {
        iState = new LinesState(NULL);
        saveState();
    }
}

QuickLinesGame::~QuickLinesGame()
{
    QDEBUG((void*)this << "- destroyed");
    delete iState;
    delete iScores;
}

QString QuickLinesGame::defaultStateFile()
{
    return linesDataDir() + QStringLiteral("state.json");
}

QString QuickLinesGame::defaultScoresFile()
{
    return linesDataDir() + QStringLiteral("scores.json");
}

int QuickLinesGame::highScore() const
{
    return iScores->highScore();
}

bool QuickLinesGame::newRecord() const
{
    return score() > iScores->highScore();
}

void QuickLinesGame::setPrefs(LinesPrefs* aPrefs)
{
    QDEBUG("prefs" << (void*)aPrefs);
    if (iPrefs != aPrefs) {
        if (iPrefs) iPrefs->disconnect(this);
        iPrefs = aPrefs;
        if (iPrefs) {
            onShowNextBallsChanged();
            connect(iPrefs,
                SIGNAL(showNextBallsChanged()),
                SLOT(onShowNextBallsChanged()));
        }
        Q_EMIT prefsChanged();
    }
}

bool QuickLinesGame::showBallPath() const
{
    return iPrefs ? iPrefs->showBallPath() : DEFAULT_SHOW_BALL_PATH;
}

void QuickLinesGame::onShowNextBallsChanged()
{
    if (iPrefs->showNextBalls() && iState->nextColorsShown()) {
        saveState();
    }
}

bool QuickLinesGame::handleClick(LinesPoint aPoint)
{
    if (iState && iState->hasEmptyCells()) {
        Q_EMIT clicked(aPoint);
        LinesPoint selection = iState->selection();
        if (selection.equal(aPoint)) {
            iState->unselect();
            saveState();
            Q_EMIT selectionChanged(selection, LinesPoint());
        } else if (iState->select(aPoint)) {
            saveState();
            Q_EMIT selectionChanged(selection, aPoint);
        } else if (selection.isValid()) {
            LinesPoints path = iState->findPath(selection, aPoint);
            if (path.isEmpty()) {
                QDEBUG("can't move ball from" <<
                    qPrintable(selection.toString()) << "to" <<
                    qPrintable(aPoint.toString()));
            } else {
                iState->unselect();
                saveState();
                Q_EMIT selectionChanged(selection, LinesPoint());
                Q_EMIT move(path);
                return showBallPath();
            }
        }
    }
    return false;
}

bool QuickLinesGame::click(int aX, int aY)
{
    return handleClick(LinesPoint(aX, aY));
}

void QuickLinesGame::stepDone(int aX, int aY)
{
    Q_EMIT moveStepDone(LinesPoint(aX, aY));
}

LinesPoints QuickLinesGame::moveBall(LinesPoint aFrom, LinesPoint aTo)
{
    LinesPoints diff;
    if (iState) {
        LinesState* state = iState->move(aFrom, aTo);
        if (state) {
            diff = state->diff(*iState);
            setState(state);
        }
    }
    return diff;
}

void QuickLinesGame::restart()
{
    setState(NULL);
    setState(new LinesState(NULL));
}

void QuickLinesGame::setState(LinesState* aState)
{
    if (iState != aState) {
        const int prevHighScore = highScore();
        const int prevScore = score();
        const bool wasOver = over();
        const bool wasNewRecord = newRecord();

        LinesState* prevState = iState;
        iState = aState;
        saveState();
        Q_EMIT stateChanged(prevState, iState);

        const int newScore = score();
        if (prevScore != newScore) {
            QDEBUG("score" << newScore);
            Q_EMIT scoreChanged();
            if (prevScore > newScore && iScores->addHighScore(prevScore)) {
                // Record the last score
                saveScores();
            }
        }

        if (wasNewRecord != newRecord()) {
            Q_EMIT newRecordChanged();
        }

        if (prevHighScore != highScore()) {
            Q_EMIT highScoreChanged();
        }

        if (wasOver != over()) {
            Q_EMIT overChanged();
        }

        delete prevState;
    }
}

void QuickLinesGame::saveState() const
{
    QJsonObject json;
    if (iState) json = iState->toJson();
    saveJson(defaultStateFile(), json);
}

void QuickLinesGame::saveScores() const
{
    QDEBUG("saving" << qPrintable(defaultScoresFile()));
    saveJson(defaultScoresFile(), iScores->toJson());
}

LinesState* QuickLinesGame::loadState()
{
    QJsonObject json;
    return loadJson(defaultStateFile(), json) ? new LinesState(&json) : NULL;
}

LinesScores* QuickLinesGame::loadScores()
{
    QJsonObject json;
    return loadJson(defaultScoresFile(), json) ? new LinesScores(&json) : NULL;
}
