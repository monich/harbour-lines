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

#include "LinesSet.h"

class LinesSet::Private: public QSharedData
{
public:
    Private(LinesColor aColor) : iRef(1), iColor(aColor), iMaxSize(-1) {}
    bool append(LinesPoint aPoint);
    int countFrom(LinesPoint aPoint, const LDirection* aDir) const;
    int maxSize() const;
    void removeShortSegments();

public:
    QAtomicInt iRef;
    LinesPoints iPoints;
    LinesColor iColor;
    mutable int iMaxSize;
};

bool LinesSet::Private::append(LinesPoint aPoint)
{
    if (aPoint.isValid() && !iPoints.contains(aPoint)) {
        iPoints.append(aPoint);
        iMaxSize = -1;
        return true;
    }
    return false;
}

int LinesSet::Private::countFrom(LinesPoint aPoint, const LDirection* aDir) const
{
    int n = 0;
    LinesPoint nextPoint = aPoint;
    if (aDir->step(&nextPoint)) {
        if (iPoints.contains(nextPoint)) {
            n += 1 + countFrom(nextPoint, aDir);
        }
    }
    return n;
}

int LinesSet::Private::maxSize() const
{
    if (iMaxSize < 0) {
        iMaxSize = 0;
        const int n = iPoints.size();
        for (int i=0; i<n; i++) {
            const LinesPoint p = iPoints.at(i);
            for (int k = 0; k<LDirCount/2; k++) {
                const LDirection* d1 = linesDirection[k];
                const LDirection* d2 = Opposite(d1);
                const int n = countFrom(p, d1) + countFrom(p, d2);
                if (n >= iMaxSize) iMaxSize = n+1;
            }
        }
    }
    return iMaxSize;
}

void LinesSet::Private::removeShortSegments()
{
    LinesPoints retain;
    LinesPoint startPoint = iPoints.at(0);

    int i;
    for (i=0; i<LDirCount/2; i++) {
        const LDirection* d1 = linesDirection[i];
        const LDirection* d2 = Opposite(d1);
        const int n = countFrom(startPoint, d1) + countFrom(startPoint, d2);
        if ((n+1) >= LinesMinLineSize) {

            if (retain.isEmpty()) {
                retain.append(startPoint);
            }

            LinesPoint p = startPoint;
            if (d1->step(&p)) {
                while (iPoints.contains(p)) {
                    retain.append(p);
                    if (!d1->step(&p)) {
                        break;
                    }
                }
            }

            p = startPoint;
            if (d2->step(&p)) {
                while (iPoints.contains(p)) {
                    retain.append(p);
                    if (!d2->step(&p)) {
                        break;
                    }
                }
            }
        }
    }

    // Now remove the points that don't belong to long straight segments
    // containing the start point
    for (i=iPoints.size()-1; i>=0; i--) {
        LinesPoint p = iPoints.at(i);
        if (!retain.contains(p)) {
            iPoints.removeAt(i);
            if (iPoints.size() == retain.size()) {
                break;
            }
        }
    }

    iMaxSize = -1;
}

// ===========================================================================
// LinesSet
// ===========================================================================

LinesSet::LinesSet() : iData(NULL)
{

}

LinesSet::LinesSet(LinesColor aColor) : iData(new LinesSet::Private(aColor))
{
}

LinesSet::LinesSet(const LinesSet& aLine) : iData(aLine.iData)
{
    if (iData) iData->iRef.ref();
}

LinesSet::~LinesSet()
{
    if (iData && !iData->iRef.deref()) delete iData;
}

LinesSet& LinesSet::operator=(const LinesSet& aLine)
{
    if (iData != aLine.iData) {
        if (aLine.iData) aLine.iData->iRef.ref();
        if (iData && !iData->iRef.deref()) delete iData;
        iData = aLine.iData;
    }
    return *this;
}

bool LinesSet::isEmpty() const
{
    return !iData || iData->iPoints.isEmpty();
}

LinesColor LinesSet::color() const
{
    return iData ? iData->iColor : LColorNone;
}

LinesPoints LinesSet::points() const
{
    return iData ? iData->iPoints : LinesPoints();
}

bool LinesSet::append(LinesPoint aPoint)
{
    return iData && iData->append(aPoint);
}

bool LinesSet::contains(LinesPoint aPoint) const
{
    return iData && iData->iPoints.contains(aPoint);
}

int LinesSet::maxSize() const
{
    return iData ? iData->maxSize() : 0;
}

void LinesSet::removeShortSegments()
{
    if (iData) iData->removeShortSegments();
}

int LinesSet::score(const LinesScoreSystem* aScoreSystem) const
{
    if (maxSize() >= LinesMinLineSize) {
        return aScoreSystem->baseScore + aScoreSystem->extraBall *
            (iData->iPoints.count() - LinesMinLineSize);
    }
    return 0;
}
