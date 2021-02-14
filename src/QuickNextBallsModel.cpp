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

#include "QuickNextBallsModel.h"

#include "HarbourDebug.h"

#define SUPER QAbstractListModel

enum QuickNextBallsModelRole {
    BallColor = Qt::UserRole
};

QuickNextBallsModel::QuickNextBallsModel(QObject* aParent) :
    SUPER(aParent),
    iGame(NULL)
{
    HDEBUG("created");
    updateState();
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

QuickNextBallsModel::~QuickNextBallsModel()
{
    HDEBUG("destroyed");
}

void QuickNextBallsModel::emitDataChanged(int aFirstRow, int aLastRow)
{
    Q_EMIT dataChanged(createIndex(aFirstRow, 0), createIndex(aLastRow, 0));
}

void QuickNextBallsModel::updateState()
{
    int i;
    const LinesState* gameState = iGame ? iGame->state() : NULL;
    if (gameState) {
        const int n = gameState->nextBallCount();
        const LinesColor* nextColors = gameState->nextBallColors();
        for (i=0; i<n; i++) iState.color[i] = nextColors[i];
        iState.index = gameState->nextBallsStateIndex();
    } else {
        i = 0;
        iState.index = -1;
    }
    while (i < LinesNextBalls) {
        iState.color[i++] = LColorNone;
    }
}

void QuickNextBallsModel::emitStateChangeSignals(
    const QuickNextBallsModel::State* aPrevState)
{
    if (aPrevState->index != iState.index) {
        // This signal has to be emitted first to start the animation
        Q_EMIT stateIndexChanged();
    }
    int firstChangedRow = -1;
    for (int i=0; i<LinesNextBalls; i++) {
        if (iState.color[i] != aPrevState->color[i]) {
            if (firstChangedRow < 0) firstChangedRow = i;
        } else if (firstChangedRow >= 0) {
            emitDataChanged(firstChangedRow, i-1);
            firstChangedRow = -1;
        }
    }
    if (firstChangedRow >= 0) {
        emitDataChanged(firstChangedRow, LinesNextBalls-1);
    }
}

void QuickNextBallsModel::setGame(QuickLinesGame* aGame)
{
    HDEBUG("game" << (void*)aGame);
    if (iGame != aGame) {
        State state = iState;
        if (iGame) iGame->disconnect(this);
        iGame = aGame;
        connect(iGame,
            SIGNAL(stateChanged(LinesState*,LinesState*)),
            SLOT(onStateChanged(LinesState*,LinesState*)));
        updateState();
        emitStateChangeSignals(&state);
        Q_EMIT gameChanged();
    }
}

QHash<int,QByteArray> QuickNextBallsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[BallColor] = "color";
    return roles;
}

int QuickNextBallsModel::rowCount(const QModelIndex&) const
{
    return LinesNextBalls;
}

QVariant QuickNextBallsModel::data(const QModelIndex& aIndex, int aRole) const
{
    const int i = aIndex.row();
    if (i >= 0 && i < LinesNextBalls && aRole == BallColor) {
        return QVariant::fromValue(iColors.colorName(iState.color[i]));
    }
    return QVariant();
}

void QuickNextBallsModel::onStateChanged(LinesState* aPrev, LinesState* aNew)
{
    State state = iState;
    updateState();
    emitStateChangeSignals(&state);
}
