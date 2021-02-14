/*
  Copyright (C) 2015-2021 Jolla Ltd.
  Copyright (C) 2015-2021 Slava Monich <slava.monich@jolla.com>

  You may use this file under the terms of the BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer
       in the documentation and/or other materials provided with the
       distribution.
    3. Neither the names of the copyright holders nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "QuickLinesGame.h"

#include "HarbourDebug.h"
#include "HarbourJson.h"

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
    HDEBUG((void*)this << "- created");
    if (!iScores) iScores = new LinesScores(NULL);
    if (iState) {
        if (!iState->hasEmptyCells()) {
            HDEBUG("the last game was over, starting new one");
            restart();
        }
    } else {
        iState = new LinesState(NULL);
        saveState();
    }
}

QuickLinesGame::~QuickLinesGame()
{
    HDEBUG((void*)this << "- destroyed");
    if (iState && !iState->hasEmptyCells()) {
        HDEBUG("ending the game");
        if (iScores->addScore(score())) {
            saveScores();
        }
        setState(NULL);
    }
    delete iState;
    delete iScores;
}

QString QuickLinesGame::stateFile()
{
    return linesDataDir() + QStringLiteral("state.json");
}

QString QuickLinesGame::scoreFile()
{
    return linesDataDir() + QStringLiteral("scores.json");
}

int QuickLinesGame::highScore() const
{
    return iScores->highScore();
}

bool QuickLinesGame::newRecord() const
{
    return score() >= iScores->highScore();
}

void QuickLinesGame::setPrefs(LinesPrefs* aPrefs)
{
    HDEBUG("prefs" << (void*)aPrefs);
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
                HDEBUG("can't move ball from" <<
                    qPrintable(selection.toString()) << "to" <<
                    qPrintable(aPoint.toString()));
                Q_EMIT oops(aPoint);
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
    const int prevHighScore = highScore();
    if (iScores->addScore(score())) {
        saveScores();
    }
    if (prevHighScore != highScore()) {
        Q_EMIT highScoreChanged();
    }
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
        if (iState && iPrefs && iPrefs->showNextBalls()) {
            iState->nextColorsShown();
        }

        saveState();

        Q_EMIT stateChanged(prevState, iState);

        if (prevScore != score()) {
            HDEBUG("score" << score());
            Q_EMIT scoreChanged();
        }

        if (wasNewRecord != newRecord()) {
            Q_EMIT newRecordChanged();
        }

        if (prevHighScore != highScore()) {
            Q_EMIT highScoreChanged();
        }

        if (wasOver != over()) {
#if HARBOUR_DEBUG
            if (!wasOver) {
                HDEBUG("Game is over");
            }
#endif
            Q_EMIT overChanged();
        }

        delete prevState;
    }
}

void QuickLinesGame::saveState() const
{
    QVariantMap map;
    if (iState) map = iState->toVariantMap();
    HarbourJson::save(stateFile(), map);
}

void QuickLinesGame::saveScores() const
{
    HDEBUG("saving" << qPrintable(scoreFile()));
    HarbourJson::save(scoreFile(), iScores->toVariantMap());
}

LinesState* QuickLinesGame::loadState()
{
    QVariantMap map;
    return HarbourJson::load(stateFile(), map) ? new LinesState(&map) : NULL;
}

LinesScores* QuickLinesGame::loadScores()
{
    QVariantMap map;
    return HarbourJson::load(scoreFile(), map) ? new LinesScores(&map) : NULL;
}
