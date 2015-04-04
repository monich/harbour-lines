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

#include "QuickLinesModel.h"
#include "LinesTypes.h"

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
    QDEBUG("created");
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

QuickLinesModel::~QuickLinesModel()
{
    QDEBUG("destroyed");
}

void QuickLinesModel::setGame(QuickLinesGame* aGame)
{
    QDEBUG("game" << (void*)aGame);
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
    QDEBUG("QuickLinesModel::roleNames");
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
    const LinesPoint point(pointFromIndex(aIndex));
    switch (aRole) {
    case CellColumn:
        return QVariant(point.x);
    case CellRow:
        return QVariant(point.y);
    case CellColor:
        if (iGame) {
            const LinesState* state = iGame->state();
            if (state) {
                if (state->hasBallAt(point)) {
                    return QVariant(colorAt(point));
                } else if (iMoveStep >= 0) {
                    int index = iMove.indexOf(point);
                    if (index >= 0 && index <= iMoveStep) {
                        return QVariant(colorAt(iMove.at(0)));
                    }
                }
            }
        }
        return QVariant(QString());
    case CellState:
        if (iGame) {
            const LinesState* state = iGame->state();
            if (state) {
                int index = iMove.indexOf(point);
                if (index >= 0 && index <= iMoveStep) {
                    if (index == 0) {
                        return QVariant(QString("START_MOVE"));
                    } else if (index == (iMove.size()-1)) {
                        return QVariant(QString("END_MOVE"));
                    } else {
                        return QVariant(QString("MOVING"));
                    }
                }
                if (state->hasBallAt(point)) {
                    if (state->selection().equal(point)) {
                        return QVariant(QString("SELECTED"));
                    } else {
                        return QVariant(QString("STATIC"));
                    }
                }
                return QVariant(QString("EMPTY"));
            }
        }
        return QVariant(QString("NONE"));
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
    QDEBUG("selection changed" <<
        qPrintable(aPrev.toString()) << "->" <<
        qPrintable(aCurrent.toString()));
    if (aPrev.isValid()) {
        QModelIndex index(indexFromPoint(aPrev));
        Q_EMIT dataChanged(index, index);
    }
    if (aCurrent.isValid()) {
        QModelIndex index(indexFromPoint(aCurrent));
        Q_EMIT dataChanged(index, index);
    }
}

void QuickLinesModel::onMove(LinesPoints aPath)
{
    if (iMoveStep >= 0) {
        LinesPoint p(iMove.at(iMoveStep));
        iMoveStep = -1;
        iMove.clear();
        QModelIndex index(indexFromPoint(p));
        Q_EMIT dataChanged(index, index);
    }
    if (!aPath.isEmpty()) {
        iMove = aPath;
        iMoveStep = 0;
        QModelIndex index(indexFromPoint(iMove.at(iMoveStep)));
        Q_EMIT dataChanged(index, index);
    }
}

void QuickLinesModel::onMoveStepDone(LinesPoint aPoint)
{
    if (iMoveStep >= 0 && iMove.at(iMoveStep).equal(aPoint)) {
        if (iMoveStep+1 < iMove.size()) {
            if (iGame->showBallPath()) {
                iMoveStep++;
            } else {
                iMoveStep = iMove.size()-1;
            }
            QModelIndex index(indexFromPoint(iMove.at(iMoveStep)));
            Q_EMIT dataChanged(index, index);
        } else {
            LinesPoints path(iMove);
            iMoveStep = -1;
            iMove.clear();
            QDEBUG("arrived at" << qPrintable(path.last().toString()));
            LinesPoints change(iGame->moveBall(path.first(), path.last()));
            const int n = path.size();
            for (int i=0; i<n; i++) {
                LinesPoint p(path.at(i));
                if (!change.contains(p)) {
                    QModelIndex index(indexFromPoint(p));
                    Q_EMIT dataChanged(index, index);
                }
            }
        }
    } else {
        QDEBUG("unexpected move at" << qPrintable(aPoint.toString()));
    }
}

void QuickLinesModel::onStateChanged(LinesState* aPrev, LinesState* aCurrent)
{
    if (aPrev && aCurrent) {
        LinesPoints diff = aPrev->diff(*aCurrent);
        const int n = diff.size();
        QDEBUG(n << "cells changed");
        for (int i=0; i<n; i++) {
            QModelIndex index(indexFromPoint(diff.at(i)));
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
        QModelIndex index(indexFromPoint(iMove.at(iMoveStep)));
        Q_EMIT dataChanged(index, index);
    }
}
