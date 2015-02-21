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

#ifndef LINES_STATE_H
#define LINES_STATE_H

#include "LinesTypes.h"
#include "LinesSet.h"

class QJsonObject;

class LinesState
{
    class PathSearch;

public:
    explicit LinesState(const QJsonObject* aJson);
    explicit LinesState(const LinesState& aState);

    LinesColor colorAt(LinesPoint aPoint) const;
    LinesColor nextColor(int aIndex) const;
    LinesPoint selection() const;
    bool isEmpty() const;
    bool hasEmptyCells() const;
    bool hasBallAt(LinesPoint aPoint) const;
    bool hasSelection() const;
    bool nextColorsShown();
    const LinesColor* nextColors() const;
    QJsonObject toJson() const;
    LinesPoints findPath(LinesPoint aFrom, LinesPoint aTo) const;
    LinesPoints diff(const LinesState& aState) const;

    LinesState* move(LinesPoint aFrom, LinesPoint aTo) const;

    bool select(LinesPoint aPoint);
    void unselect();
    int score() const;

private:
    static QByteArray byteArray(const QJsonObject& aJson, QString aKey);

private:
    void generateNextColors();
    LinesPoint dropBall(LinesColor aColor);
    LinesPoints dropNextBalls();
    LinesSet lineFromPoint(LinesPoint aPoint);
    void removeBall(LinesPoint aPoint);
    void removeBalls(LinesPoints aPoints);
    void buildLine(LinesSet aLine, LinesPoint aPoint);
    const LinesScoreSystem* scoreSystem() const;

private:
    QList<LinesPoint> iAvailable;
    LinesRandom iRandom;

    LinesColor iNextColor[LinesNextBalls];
    LinesColor iGrid[LinesGridSize][LinesGridSize];
    LinesPoint iSelection;
    bool iHaveSeenNextColors;
    bool iEasyPlay;
    int iScore;
    int iSubsequentLines;
};

inline LinesColor LinesState::colorAt(LinesPoint aPoint) const
    { return aPoint.isValid() ? iGrid[aPoint.x][aPoint.y] : LColorNone; }
inline bool LinesState::isEmpty() const
    { return iAvailable.size() == LinesGridSize*LinesGridSize; }
inline bool LinesState::hasEmptyCells() const
    { return !iAvailable.isEmpty(); }
inline bool LinesState::hasBallAt(LinesPoint aPoint) const
    { return colorAt(aPoint) != LColorNone; }
inline LinesPoint LinesState::selection() const
    { return iSelection.isValid() ? iSelection : LinesPoint(); }
inline bool LinesState::hasSelection() const
    { return iSelection.isValid(); }
inline const LinesColor* LinesState::nextColors() const
    { return iNextColor; }
inline void LinesState::unselect()
    { iSelection.invalidate(); }
inline int LinesState::score() const
    { return iScore; }


#endif // LINES_STATE_H
