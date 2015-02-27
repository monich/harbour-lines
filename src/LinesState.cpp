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

#include "LinesState.h"

#include <QJsonObject>
#include <QJsonArray>

/* JSON keys */
#define LINES_STATE_JSON_VERSION (1)
static QString kStateJsonVersion("version");
static QString kStateJsonRandomSeed("randomSeed");
static QString kStateJsonNextColors("nextColors");
static QString kStateJsonNextColorsKnown("nextColorsKnown");
static QString kStateJsonSelection("selection");
static QString kStateJsonBalls("balls");
static QString kStateJsonScore("score");
static QString kStateJsonSubsequentLines("subsequentLines");

static const LinesScoreSystem scoreNextBalls = {5, 5, 5};
static const LinesScoreSystem scoreNoNextBalls = {6, 5, 5};

class LinesState::PathSearch {
public:
    bool iUsed[LinesGridSize][LinesGridSize];
    QList <LinesPoints> iPathSet;
    int iFound;

public:
    PathSearch() : iFound(-1) {}
    bool step(LinesPoint aTo);
    inline bool canUse(LinesPoint p) { return !iUsed[p.x][p.y]; }
    inline bool markAsUsed(LinesPoint p) {
        if (canUse(p)) {
            iUsed[p.x][p.y] = true;
            return true;
        }
        return  false;
    }
};

bool LinesState::PathSearch::step(LinesPoint aTo)
{
    int stepsMade = 0;
    int n = iPathSet.size();
    for (int i=0; i<n; i++) {
        LinesPoints path(iPathSet.at(i));
        LinesPoint lastPoint(path.last());
        bool firstStep = true;
        for (int k=0; k<LDirMajorCount; k++) {
            LinesPoint next(lastPoint);
            if (linesMajorDirection[k]->step(&next) && markAsUsed(next)) {
                int pathIndex;
                LinesPoints newPath(path);
                newPath.append(next);
                stepsMade++;
                if (firstStep) {
                    pathIndex = i;
                    firstStep = false;
                    iPathSet.replace(i, newPath);
                } else {
                    pathIndex = iPathSet.size();
                    iPathSet.append(newPath);
                }
                if (next.equal(aTo)) {
                    iFound = pathIndex;
                    return true;
                }
            }
        }
        if (firstStep) {
            // Dead end, remove this path
            iPathSet.removeAt(i);
            i--;
            n--;
        }
    }
    return stepsMade > 0;
}

LinesState::LinesState(const QJsonObject* aJson) :
    iNextBallsStateIndex(0),
    iEasyPlay(false)
{
    QVariantMap map;
    if (aJson) {
        map = aJson->toVariantMap();
        QVariant version = map.value(kStateJsonVersion);
        if (version.toInt() != LINES_STATE_JSON_VERSION) {
            qWarning() << "Unexpected state JSON version:" << version;
            aJson = NULL;
            map.clear();
        }
    }

    bool ok = false;
    if (aJson) {
        QByteArray data = byteArray(*aJson, kStateJsonNextColors);
        if (data.length() == LinesNextBalls) {
            ok = true;
            for (int i=0; i<LinesNextBalls; i++) {
                const int color = (signed char)data.at(i);
                if (color >= LColorNone && color < LColorCount) {
                    iNextColor[i] = (LinesColor)color;
                } else {
                    ok = false;
                    break;
                }
            }
        }
    }

    if (!ok) {
        generateNextColors();
    }

    ok = false;
    if (aJson) {
        QByteArray data = byteArray(*aJson, kStateJsonBalls);
        if (data.length() == LinesGridSize*LinesGridSize) {
            ok = true;
            for (int x=0, i=0; x<LinesGridSize && ok; x++) {
                for (int y=0; y<LinesGridSize; y++, i++) {
                    const int color = (signed char)data.at(i);
                    if (color >= LColorNone && color < LColorCount) {
                        if ((iGrid[x][y] = (LinesColor)color) == LColorNone) {
                            iAvailable.append(LinesPoint(x,y));
                        }
                    } else {
                        ok = false;
                        break;
                    }
                }
            }
        }
    }

    if (!ok) {
        iAvailable.clear();
        for (int x=0; x<LinesGridSize; x++) {
            for (int y=0; y<LinesGridSize; y++) {
                iGrid[x][y] = LColorNone;
                iAvailable.append(LinesPoint(x,y));
            }
        }
    }

    if (aJson) {
        QByteArray data = byteArray(*aJson, kStateJsonSelection);
        if (data.length() == 2) {
            const int x = data.at(0);
            const int y = data.at(1);
            if (x >= 0 && x < LinesGridSize && y >= 0 && y < LinesGridSize) {
                iSelection.x = x;
                iSelection.y = y;
            }
        }

        data = byteArray(*aJson, kStateJsonRandomSeed);
        const int seedLen = 8;
        if (data.length() == seedLen) {
            int64_t seed = 0;
            for (int i=0; i<seedLen; i++) {
                seed <<= 8;
                seed |= (unsigned char)data.at(i);
            }
            iRandom.setSeed(seed);
        }

        iScore = map.value(kStateJsonScore).toInt();
        iSubsequentLines = map.value(kStateJsonSubsequentLines).toInt();
        iHaveSeenNextColors = map.value(kStateJsonNextColorsKnown).toBool();

        if (iScore < 0) iScore = 0;
        if (iSubsequentLines < 0) iSubsequentLines = 0;
    } else {
        iScore = 0;
        iSubsequentLines = 0;
        iHaveSeenNextColors = false;
    }

    if (isEmpty()) {
        dropNextBalls();
    }
}

LinesState::LinesState(const LinesState& aState) :
    iAvailable(aState.iAvailable),
    iRandom(aState.iRandom),
    iSelection(aState.iSelection),
    iNextBallsStateIndex(aState.iNextBallsStateIndex),
    iHaveSeenNextColors(false),
    iEasyPlay(aState.iEasyPlay),
    iScore(aState.iScore),
    iSubsequentLines(aState.iSubsequentLines)
{
    memcpy(iGrid, aState.iGrid, sizeof(iGrid));
    memcpy(iNextColor, aState.iNextColor, sizeof(iNextColor));
}

QByteArray LinesState::byteArray(const QJsonObject& aJson, QString aKey)
{
    return QByteArray::fromHex(aJson.value(aKey).toString().toLocal8Bit());
}

QJsonObject LinesState::toJson() const
{
    int i;
    QByteArray colors;
    colors.reserve(LinesNextBalls);
    for (i=0; i<LinesNextBalls; i++) colors.append((char)(iNextColor[i]));

    QByteArray balls;
    balls.reserve(LinesGridSize*LinesGridSize);
    for (int x=0; x<LinesGridSize; x++) {
        for (int y=0; y<LinesGridSize; y++) {
            balls.append((char)(iGrid[x][y]));
        }
    }

    QByteArray selection;
    selection.reserve(2);
    selection.append((char)iSelection.x);
    selection.append((char)iSelection.y);

    QByteArray seedData;
    seedData.reserve(8);
    const int64_t seed = iRandom.seed();
    for (i=0; i<8; i++) seedData.append((char)(seed >> 8*(7-i)));

    QJsonObject json;
    json[kStateJsonVersion] = LINES_STATE_JSON_VERSION;
    json[kStateJsonBalls] = QString(balls.toHex());
    json[kStateJsonSelection] = QString(selection.toHex());
    json[kStateJsonNextColors] = QString(colors.toHex());
    json[kStateJsonNextColorsKnown] = iHaveSeenNextColors;
    json[kStateJsonSubsequentLines] = iSubsequentLines;
    json[kStateJsonRandomSeed] = QString(seedData.toHex());
    json[kStateJsonScore] = iScore;
    return json;
}

bool LinesState::nextColorsShown()
{
    if (!iHaveSeenNextColors) {
        QDEBUG("cheating!");
        iHaveSeenNextColors = true;
        return true;
    }
    return false;
}

const LinesScoreSystem* LinesState::scoreSystem() const
{
    return iHaveSeenNextColors ? &scoreNextBalls : &scoreNoNextBalls;
}

LinesPoints LinesState::dropNextBalls()
{
    LinesPoints points;
    for (int i=0; i<LinesNextBalls; i++) {
        LinesPoint p(dropBall(iNextColor[i]));
        if (p.isValid()) {
            points.append(p);
        } else {
            break;
        }
    }
    generateNextColors();
    return points;
}

LinesSet LinesState::lineFromPoint(LinesPoint aPoint)
{
    LinesColor color = colorAt(aPoint);
    if (color != LColorNone) {
        LinesSet line(color);
        line.append(aPoint);
        buildLine(line, aPoint);
        if (!iEasyPlay) line.removeShortSegments();
        if (line.score(scoreSystem()) > 0) {
            return line;
        }
    }
    return LinesSet();
}

void LinesState::buildLine(LinesSet aLine, LinesPoint aPoint)
{
    for (int k=0; k<LDirCount; k++) {
        LinesPoint next(aPoint);
        if (linesDirection[k]->step(&next) &&
            colorAt(next) == aLine.color() &&
            aLine.append(next)) {
            buildLine(aLine, next);
        }
    }
}

LinesColor LinesState::nextColor(int aIndex) const
{
    QASSERT(aIndex >= 0 && aIndex < LinesNextBalls);
    if (aIndex >= 0 && aIndex < LinesNextBalls) {
        return iNextColor[aIndex];
    }
    return LColorNone;
}

bool LinesState::select(LinesPoint aPoint)
{
    if (aPoint.isValid()) {
        if (colorAt(aPoint) != LColorNone) {
            iSelection = aPoint;
            QDEBUG("selected" << qPrintable(aPoint.toString()));
            return true;
        } else {
            QDEBUG("nothing at" << qPrintable(aPoint.toString()));
            return false;
        }
    } else if (iSelection.isValid()) {
        QDEBUG("cleared selection");
        iSelection.invalidate();
        return true;
    } else {
        return false;
    }
}

int LinesState::nextBallCount() const
{
    const int available = iAvailable.size();
    return (available > LinesNextBalls) ? LinesNextBalls : available;
}

void LinesState::generateNextColors()
{
    iNextBallsStateIndex++;
    for (int i=0; i<LinesNextBalls; i++) {
        iNextColor[i] = iRandom.nextColor();
        QDEBUG("next color" << iNextColor[i]);
    }
}

LinesPoint LinesState::dropBall(LinesColor aColor)
{
    const int n = iAvailable.size();
    if (n > 0) {
        QASSERT(aColor != LColorNone);
        const int i = iRandom.next(n);
        LinesPoint p = iAvailable.takeAt(i);
        iGrid[p.x][p.y] = aColor;
        QDEBUG("dropped" << aColor << "at" << qPrintable(p.toString()));
        return p;
    }
    return LinesPoint();
}

void LinesState::removeBall(LinesPoint aPoint)
{
    if (hasBallAt(aPoint)) {
        iGrid[aPoint.x][aPoint.y] = LColorNone;
        iAvailable.append(aPoint);
    } else {
        QDEBUG("no ball at" << qPrintable(aPoint.toString()));
    }
}

void LinesState::removeBalls(LinesPoints aPoints)
{
    const int n = aPoints.size();
    QDEBUG("removing" << n << "balls");
    for (int i=0; i<n; i++) {
        removeBall(aPoints.at(i));
    }
}

LinesPoints LinesState::findPath(LinesPoint aFrom, LinesPoint aTo) const
{
    if (hasBallAt(aFrom) && aTo.isValid() && !hasBallAt(aTo)) {
        PathSearch search;
        for (int x=0; x<LinesGridSize; x++) {
            for (int y=0; y<LinesGridSize; y++) {
                search.iUsed[x][y] = (iGrid[x][y] != LColorNone);
            }
        }

        LinesPoints start;
        start.append(aFrom);
        search.markAsUsed(aFrom);
        search.iPathSet.append(start);
        while (search.step(aTo) && (search.iFound < 0));

        if (search.iFound >= 0) {
            LinesPoints path(search.iPathSet.at(search.iFound));
#if LINES_DEBUG
            QASSERT(path.last().equal(aTo));
            QString pathString(path.first().toString());
            for (int i=1; i<path.size(); i++) {
                pathString.append(" -> ").append(path.at(i).toString());
            }
            QDEBUG("found path" << qPrintable(pathString));
            QDEBUG(search.iPathSet.size() << "paths tried");
#endif
            return path;
        }
    }
    return LinesPoints();
}

LinesPoints LinesState::diff(const LinesState& aState) const
{
    LinesPoints points;
    if (&aState != this) {
        for (int x=0; x<LinesGridSize; x++) {
            for (int y=0; y<LinesGridSize; y++) {
                if (iGrid[x][y] != aState.iGrid[x][y]) {
                    points.append(LinesPoint(x,y));
                }
            }
        }
        LinesPoint p1(selection());
        LinesPoint p2(aState.selection());
        if (!p1.equal(p2)) {
            if (!points.contains(p1)) points.append(p1);
            if (!points.contains(p2)) points.append(p2);
        }
    }
    return points;
}

LinesState* LinesState::move(LinesPoint aFrom, LinesPoint aTo) const
{
    LinesColor color = colorAt(aFrom);
    if (color != LColorNone && aTo.isValid() && !hasBallAt(aTo)) {
        LinesState* state = new LinesState(*this);
        QVERIFY(state->iAvailable.removeOne(aTo));
        QASSERT(!state->iAvailable.contains(aFrom));
        state->iAvailable.append(aFrom);
        state->iGrid[aFrom.x][aFrom.y] = LColorNone;
        state->iGrid[aTo.x][aTo.y] = color;

        QList<LinesSet> lines;
        LinesSet line(state->lineFromPoint(aTo));
        if (line.isEmpty()) {
            // No line was finished, drop the next balls
            state->iSubsequentLines = 0;
            LinesPoints points = state->dropNextBalls();
            for (int i=0; i<points.size(); i++) {
                LinesPoint p = points.at(i);
                bool alreadyDone = false;
                for (int j=0; j<lines.size() && !alreadyDone; j++) {
                    alreadyDone = lines.at(j).contains(p);
                }
                if (!alreadyDone) {
                    line = state->lineFromPoint(p);
                    if (!line.isEmpty()) lines.append(line);
                }
            }
        } else {
            lines.append(line);
        }

        const LinesScoreSystem* ss = scoreSystem();
        for (int i=0; i<lines.size(); i++) {
            line = lines.at(i);
            state->iScore += line.score(ss);
            if (state->iSubsequentLines > 0) {
                state->iScore += ss->extraLine;
            }
            state->iSubsequentLines++;
            state->removeBalls(line.points());
        }

        // In a very unlikely event if all balls have been eliminated
        // drop the next balls
        if (state->isEmpty()) {
            state->dropNextBalls();
        }

        return state;
    }
    return NULL;
}
