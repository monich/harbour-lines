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

#include "QuickLinesModel.h"
#include "LinesTypes.h"

#include "HarbourDebug.h"

#define SUPER QAbstractListModel

enum QuickLinesModelRole {
    CellRow = Qt::UserRole,
    CellColumn,
    CellColor,
    CellState
};

QuickLinesModel::QuickLinesModel(QObject* aParent) :
    SUPER(aParent),
    iMoveStep(-1),
    iGame(NULL)
{
    HDEBUG("created");
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

QuickLinesModel::~QuickLinesModel()
{
    HDEBUG("destroyed");
}

void QuickLinesModel::setGame(QuickLinesGame* aGame)
{
    HDEBUG("game" << (void*)aGame);
    if (iGame != aGame) {
        if (iGame) iGame->disconnect(this);
        iGame = aGame;
        Q_EMIT gameChanged();
        connect(iGame,
            SIGNAL(move(LinesPoints)),
            SLOT(onMove(LinesPoints)));
        connect(iGame,
            SIGNAL(moveStepDone(LinesPoint)),
            SLOT(onMoveStepDone(LinesPoint)));
        connect(iGame,
            SIGNAL(selectionChanged(LinesPoint,LinesPoint)),
            SLOT(onSelectionChanged(LinesPoint,LinesPoint)));
        connect(iGame,
            SIGNAL(stateChanged(LinesState*,LinesState*)),
            SLOT(onStateChanged(LinesState*,LinesState*)));
        connect(iGame,
            SIGNAL(clicked(LinesPoint)),
            SLOT(onClicked(LinesPoint)));
    }
}

QHash<int,QByteArray> QuickLinesModel::roleNames() const
{
    HDEBUG("roleNames");
    QHash<int, QByteArray> roles;
    roles[CellRow] = "row";
    roles[CellColumn] = "column";
    roles[CellColor] = "color";
    roles[CellState] = "state";
    return roles;
}

int QuickLinesModel::rowCount(const QModelIndex&) const
{
    return LinesGridSize*LinesGridSize;
}

QVariant QuickLinesModel::data(const QModelIndex& aIndex, int aRole) const
{
    // Pre-allocate state strings
    static const QString START_MOVE("START_MOVE");
    static const QString END_MOVE("END_MOVE");
    static const QString MOVING("MOVING");
    static const QString SELECTED("SELECTED");
    static const QString STATIC("STATIC");
    static const QString EMPTY("EMPTY");
    static const QString NONE("NONE");

    const LinesPoint point(pointFromIndex(aIndex));
    switch (aRole) {
    case CellColumn:
        return QVariant::fromValue(point.x);
    case CellRow:
        return QVariant::fromValue(point.y);
    case CellColor:
        if (iGame) {
            const LinesState* state = iGame->state();
            if (state) {
                if (state->hasBallAt(point)) {
                    return QVariant::fromValue(colorAt(point));
                } else if (iMoveStep >= 0) {
                    int index = iMove.indexOf(point);
                    if (index >= 0 && index <= iMoveStep) {
                        return QVariant::fromValue(colorAt(iMove.at(0)));
                    }
                }
            }
        }
        return QVariant::fromValue(QString());
    case CellState:
        if (iGame) {
            const LinesState* state = iGame->state();
            if (state) {
                int index = iMove.indexOf(point);
                if (index >= 0 && index <= iMoveStep) {
                    if (index == 0) {
                        return QVariant::fromValue(START_MOVE);
                    } else if (index == (iMove.size()-1)) {
                        return QVariant::fromValue(END_MOVE);
                    } else {
                        return QVariant::fromValue(MOVING);
                    }
                }
                if (state->hasBallAt(point)) {
                    if (state->selection().equal(point)) {
                        return QVariant::fromValue(SELECTED);
                    } else {
                        return QVariant::fromValue(STATIC);
                    }
                }
                return QVariant::fromValue(EMPTY);
            }
        }
        return QVariant::fromValue(NONE);
    }
    return QVariant();
}

QModelIndex QuickLinesModel::indexFromPoint(LinesPoint aPoint) const
{
    if (aPoint.isValid()) {
        return createIndex(aPoint.x + aPoint.y * LinesGridSize, 0);
    } else {
        return QModelIndex();
    }
}

LinesPoint QuickLinesModel::pointFromIndex(QModelIndex aIndex)
{
    const int row = aIndex.row();
    return LinesPoint(row % LinesGridSize, row / LinesGridSize);
}

QString QuickLinesModel::colorAt(LinesPoint aPoint) const
{
    if (iGame) {
        const LinesState* state = iGame->state();
        if (state) {
            return iColors.colorName(state->colorAt(aPoint));
        }
    }
    return QString();
}

LinesPoint QuickLinesModel::selection() const
{
    if (iGame) {
        const LinesState* state = iGame->state();
        if (state) {
            return state->selection();
        }
    }
    return LinesPoint();
}

void QuickLinesModel::onSelectionChanged(LinesPoint aPrev, LinesPoint aCurrent)
{
    HDEBUG("selection changed" <<
        qPrintable(aPrev.toString()) << "->" <<
        qPrintable(aCurrent.toString()));
    if (aPrev.isValid()) {
        const QModelIndex index(indexFromPoint(aPrev));
        Q_EMIT dataChanged(index, index);
    }
    if (aCurrent.isValid()) {
        const QModelIndex index(indexFromPoint(aCurrent));
        Q_EMIT dataChanged(index, index);
    }
}

void QuickLinesModel::onMove(LinesPoints aPath)
{
    if (iMoveStep >= 0) {
        const LinesPoint p(iMove.at(iMoveStep));
        iMoveStep = -1;
        iMove.clear();
        const QModelIndex index(indexFromPoint(p));
        Q_EMIT dataChanged(index, index);
    }
    if (!aPath.isEmpty()) {
        iMove = aPath;
        iMoveStep = 0;
        const QModelIndex index(indexFromPoint(iMove.at(iMoveStep)));
        Q_EMIT dataChanged(index, index);
    }
}

void QuickLinesModel::onMoveStepDone(LinesPoint aPoint)
{
    if (iMoveStep >= 0 && iMove.at(iMoveStep).equal(aPoint)) {
        if (iMoveStep+1 < iMove.size()) {
            // Moving the ball
            if (iGame->showBallPath()) {
                // Show next step
                iMoveStep++;
            } else {
                // Jump straight to the end
                iMoveStep = iMove.size()-1;
            }
            const QModelIndex index(indexFromPoint(iMove.at(iMoveStep)));
            Q_EMIT dataChanged(index, index);
        } else {
            // Arrived to the destination
            const LinesPoints path(iMove);
            iMoveStep = -1;
            iMove.clear();
            HDEBUG("arrived at" << qPrintable(path.last().toString()));
            const LinesPoints change(iGame->moveBall(path.first(), path.last()));
            const int n = path.size();
            HDEBUG(n << "points changed");
            for (int i=0; i<n; i++) {
                const LinesPoint p(path.at(i));
                if (!change.contains(p)) {
                    const QModelIndex index(indexFromPoint(p));
                    Q_EMIT dataChanged(index, index);
                }
            }
        }
    } else {
        HDEBUG("unexpected move at" << qPrintable(aPoint.toString()));
    }
}

void QuickLinesModel::onStateChanged(LinesState* aPrev, LinesState* aCurrent)
{
    if (aPrev && aCurrent) {
        const LinesPoints diff = aPrev->diff(*aCurrent);
        const int n = diff.size();
        HDEBUG(n << "cells changed");
        for (int i=0; i<n; i++) {
            const QModelIndex index(indexFromPoint(diff.at(i)));
            Q_EMIT dataChanged(index, index);
        }
    } else {
        Q_EMIT dataChanged(createIndex(0, 0),
            createIndex(LinesGridSize*LinesGridSize, 0));
    }
}

void QuickLinesModel::onClicked(LinesPoint aPoint)
{
    if (iMoveStep >= 0 && iMoveStep+1 < iMove.size()) {
        iMoveStep = iMove.size()-1;
        const QModelIndex index(indexFromPoint(iMove.at(iMoveStep)));
        Q_EMIT dataChanged(index, index);
    }
}
