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

#ifndef QUICK_NEXT_BALLS_MODEL_H
#define QUICK_NEXT_BALLS_MODEL_H

#include "QuickLinesGame.h"
#include "QuickLinesColors.h"
#include <QAbstractListModel>

class QuickNextBallsModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QuickLinesGame* game READ game WRITE setGame NOTIFY gameChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int stateIndex READ stateIndex NOTIFY stateIndexChanged)

    struct State {
        LinesColor color[LinesNextBalls];
        int index;
    };

public:
    explicit QuickNextBallsModel(QObject* aParent = NULL);
    ~QuickNextBallsModel();

    QuickLinesGame* game() const { return iGame; }
    void setGame(QuickLinesGame* aGame);

    int count() const { return LinesNextBalls; }
    int stateIndex() const { return iState.index; }

    QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex& aParent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& aIndex, int aRole) const Q_DECL_OVERRIDE;

private:
    void updateState();
    void emitStateChangeSignals(const State* aPrevState);
    void emitDataChanged(int firstRow, int lastRow);

private Q_SLOTS:
    void onStateChanged(LinesState* aPrev, LinesState* aCurrent);

Q_SIGNALS:
    void gameChanged();
    void countChanged();
    void stateIndexChanged();

private:
    QuickLinesGame* iGame;
    QuickLinesColors iColors;
    State iState;
};

QML_DECLARE_TYPE(QuickNextBallsModel)

#endif // QUICK_NEXT_BALLS_MODEL_H
