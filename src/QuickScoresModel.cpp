/*
  Copyright (C) 2020 Jolla Ltd.
  Copyright (C) 2020 Slava Monich <slava.monich@jolla.com>

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the names of the copyright holders nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

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

#include "QuickScoresModel.h"

#include "HarbourDebug.h"

enum QuickScoresModelRole {
    ScoreRole = Qt::UserRole,
    TimeStampRole,
    CurrentScoreRole
};

QuickScoresModel::QuickScoresModel(QObject* aParent) :
    QAbstractListModel(aParent),
    iGame(NULL),
    iCurrentScoreIndex(-1)
{
    HDEBUG("created");
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

QuickScoresModel::~QuickScoresModel()
{
    HDEBUG("destroyed");
}

void QuickScoresModel::setGame(QuickLinesGame* aGame)
{
    HDEBUG("game" << (void*)aGame);
    if (iGame != aGame) {
        if (iGame) iGame->disconnect(this);
        iGame = aGame;
        connect(iGame, SIGNAL(overChanged()), SLOT(updateState()));
        connect(iGame, SIGNAL(scoreChanged()), SLOT(updateState()));
        connect(iGame, SIGNAL(destroyed(QObject*)), SLOT(onGameDestroyed()));
        beginResetModel();
        iCurrentScoreIndex = currentScoreIndex();
        HDEBUG("current score index" << iCurrentScoreIndex);
        Q_EMIT gameChanged();
        endResetModel();
    }
}

int QuickScoresModel::currentScoreIndex() const
{
    if (iGame) {
        const int score = iGame->score();
        if (score > 0) {
            const LinesScores* scores = iGame->scores();
            const int n = scores->count();
            for (int i = 0; i < n; i++) {
                if (score >= scores->scoreAt(i)->iValue) {
                    return i;
                }
            }
            if (n <= LinesScores::MAX_COUNT) {
                return n;
            }
        }
    }
    return -1;
}

void QuickScoresModel::onGameDestroyed()
{
    setGame(NULL);
}

void QuickScoresModel::updateState()
{
    const int idx = currentScoreIndex();
    if (iCurrentScoreIndex != idx) {
        HDEBUG("current score index" << iCurrentScoreIndex << "=>" << idx);
        beginResetModel();
        iCurrentScoreIndex = idx;
        endResetModel();
    } else {
        HDEBUG("current score" << iGame->score());
        const QModelIndex modelIndex(createIndex(idx, 0));
        Q_EMIT dataChanged(modelIndex, modelIndex);
    }
}

QHash<int,QByteArray> QuickScoresModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(ScoreRole, "score");
    roles.insert(TimeStampRole, "timestamp");
    roles.insert(CurrentScoreRole, "currentScore");
    return roles;
}

int QuickScoresModel::rowCount(const QModelIndex& aParent) const
{
    if (iGame) {
        const LinesScores* scores = iGame->scores();
        return (iCurrentScoreIndex >= 0) ?
            qMin(scores->count() + 1, LinesScores::MAX_COUNT) :
            scores->count();
    }
    return 0;
}

QVariant QuickScoresModel::data(const QModelIndex& aIndex, int aRole) const
{
    if (iGame) {
        const int row = aIndex.row();
        if (row == iCurrentScoreIndex) {
            switch (aRole) {
            case ScoreRole: return QVariant(iGame->score());
            case TimeStampRole: return QVariant(QDateTime::currentDateTime());
            case CurrentScoreRole: return QVariant(true);
            }
        } else {
            const LinesScores* scores = iGame->scores();
            const LinesScore* score = scores->scoreAt((iCurrentScoreIndex < 0 ||
                iCurrentScoreIndex > row) ? row : (row - 1));
            if (score) {
                switch (aRole) {
                case ScoreRole: return QVariant(score->iValue);
                case TimeStampRole: return QVariant(score->iTimestamp);
                case CurrentScoreRole: return QVariant(false);
                }
            }
        }
    }
    return QVariant();
}
