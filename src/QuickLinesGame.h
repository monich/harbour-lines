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

#ifndef QUICK_LINES_GAME_H
#define QUICK_LINES_GAME_H

#include "LinesPrefs.h"
#include "LinesState.h"
#include "LinesScores.h"
#include <QtQml>

class QuickLinesGame: public QObject
{
    Q_OBJECT
    Q_PROPERTY(LinesPrefs* prefs READ prefs WRITE setPrefs NOTIFY prefsChanged)
    Q_PROPERTY(int score READ score NOTIFY scoreChanged)
    Q_PROPERTY(bool over READ over NOTIFY overChanged)
    Q_PROPERTY(int highScore READ highScore NOTIFY highScoreChanged)
    Q_PROPERTY(bool newRecord READ newRecord NOTIFY newRecordChanged)

public:
    explicit QuickLinesGame(QObject* aParent = NULL);
    ~QuickLinesGame();

    bool showBallPath() const;
    const LinesState* state() const { return iState; }
    LinesPrefs* prefs() const { return iPrefs; }
    int score() const { return iState ? iState->score() : 0; }
    bool over() const { return iState && !iState->hasEmptyCells(); }
    int highScore() const;
    bool newRecord() const;

    void setPrefs(LinesPrefs* aPrefs);

    Q_INVOKABLE void restart();
    Q_INVOKABLE bool click(int aX, int aY);
    Q_INVOKABLE void stepDone(int aX, int aY);

    LinesPoints moveBall(LinesPoint aFrom, LinesPoint aTo);

    static QString defaultStateFile();
    static QString defaultScoresFile();

private:
    bool handleClick(LinesPoint aPoint);
    void setState(LinesState* aState);
    void saveState() const;
    void saveScores() const;
    static LinesState* loadState();
    static LinesScores* loadScores();

Q_SIGNALS:
    void prefsChanged();
    void scoreChanged();
    void overChanged();
    void highScoreChanged();
    void newRecordChanged();
    void clicked(LinesPoint aPoint);
    void move(LinesPoints aPath);
    void moveStepDone(LinesPoint aPoint);
    void stateChanged(LinesState* aPrev, LinesState* aCurrent);
    void selectionChanged(LinesPoint aPrev, LinesPoint aCurrent);

private Q_SLOTS:
    void onShowNextBallsChanged();

private:
    LinesPrefs* iPrefs;
    LinesState* iState;
    LinesScores* iScores;
};

QML_DECLARE_TYPE(QuickLinesGame)

#endif // QUICK_LINES_GAME_H
