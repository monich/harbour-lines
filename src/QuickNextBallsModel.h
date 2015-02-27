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

    virtual QHash<int,QByteArray> roleNames() const;
    virtual int rowCount(const QModelIndex& aParent) const;
    virtual QVariant data(const QModelIndex& aIndex, int aRole) const;

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
