/*
  Copyright (C) 2015-2019 Jolla Ltd.
  Copyright (C) 2015-2019 Slava Monich <slava.monich@jolla.com>

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

#ifndef LINES_TYPES_H
#define LINES_TYPES_H

#include <QList>
#include <QTime>

#ifdef HARMATTAN
#  define LINES_APP     "meego-lines"
#else
#  define LINES_APP     "harbour-lines"
#endif
#define LINES_PLUGIN    "harbour.lines"

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#include <QtQml>

inline QString linesDataDir()
{
    return QStandardPaths::writableLocation(
         QStandardPaths::GenericDataLocation) +
         QStringLiteral("/" LINES_APP "/");
}
#else
#include <QtDeclarative>
#define QStringLiteral(s) QLatin1String(s)
inline QString linesDataDir()
{
    return QDir::homePath() + QLatin1String("/.config/" LINES_APP "/");
}
#endif

typedef enum _LinesColor {
    LColorNone = -1,
    LColorRed,
    LColorGreen,
    LColorYellow,
    LColorBlue,
    LColorMagenta,
    LColorBrown,
    LColorCyan,
    LColorCount
} LinesColor;

typedef enum _LinesDir {
    LDirNone = -1,
    LDirNorth,
    LDirNorthEast,
    LDirEast,
    LDirSouthEast,
    LDirSouth,
    LDirSouthWest,
    LDirWest,
    LDirNorthWest,
    LDirCount,
    LDirMajorCount = 4
} LinesDir;

enum LinesConstants {
    LinesGridSize = 9,
    LinesMinLineSize = 5,
    LinesFirstDrop = 5,
    LinesNextBalls = 3,
    LinesMaxBallsToDrop = LinesFirstDrop
};

typedef struct LinesScoreSystem {
    int baseScore;    // for the first 5 balls
    int extraBall;    // for each ball after the first 5
    int extraLine;    // for each line destroyed with next move
} LinesScoreSystem;

class LinesPoint {
public:
    int8_t x;
    int8_t y;

    inline LinesPoint() : x(-1), y(-1) {}
    inline LinesPoint(int aX, int aY) : x(aX), y(aY) {}
    inline LinesPoint(const LinesPoint& p) : x(p.x), y(p.y) {}

    inline void validate() {
        if (x < 0) x = 0; else if (x >= LinesGridSize) x = LinesGridSize-1;
        if (y < 0) y = 0; else if (y >= LinesGridSize) y = LinesGridSize-1;
    }
    inline void invalidate() { x = y = -1; }
    inline void normalize() { if (!isValid()) x = y = -1; }
    inline bool isValid() const {
        return (x >= 0 && y >= 0 && x < LinesGridSize && y < LinesGridSize);
    }

    inline bool equal(LinesPoint p) const
        { p.normalize(); return (x == p.x && y == p.y); }
    inline bool lessThan(LinesPoint p) const
        { p.normalize(); return (x < p.x || (x == p.x && y < p.y)); }
    inline bool greaterThan(LinesPoint p) const
        { p.normalize(); return (x > p.x || (x == p.x && y > p.y)); }

    inline bool operator!=(const LinesPoint& p) const { return !equal(p); }
    inline bool operator==(const LinesPoint& p) const { return equal(p); }
    inline bool operator<(const LinesPoint p) const { return lessThan(p); }
    inline bool operator>(const LinesPoint p) const { return greaterThan(p); }
    inline LinesPoint& operator=(const LinesPoint& p)
        { x = p.x; y = p.y; return *this; }

    QString toString() const { return isValid() ? QString("(").
        append(QString::number(x)).append(",").append(QString::number(y)).
        append(")") : QString("(invalid)"); }
};

typedef struct _LinesBall {
    LinesPoint location;
    LinesColor color;
} LinesBall;

typedef struct _LDirection {
    const char *name;
    LinesDir dir;
    bool (*step)(LinesPoint *aPoint);
} LDirection;

typedef const LDirection *LDirectionConstPtr;
extern const LDirectionConstPtr linesDirection[LDirCount];
extern const LDirectionConstPtr linesMajorDirection[LDirMajorCount];

inline static const LDirection* Opposite(const LDirection* d) {
    return (d->dir > LDirCount/2) ?
           (linesDirection[d->dir - LDirCount/2]) :
           (linesDirection[d->dir + LDirCount/2]);
}

typedef QList<LinesPoint> LinesPoints;

/**
 * Pseudo-random number generation algorithm as defined by D. H. Lehmer
 * and described by Donald E. Knuth in "The Art of Computer Programming",
 * Volume 2: "Seminumerical Algorithms", section 3.2.1.
 */
class LinesRandom {
public:
    LinesRandom() { init(QTime::currentTime().msec()); }
    LinesRandom(const LinesRandom& aRandom) { init(aRandom.iSeed); }
    LinesRandom(int64_t aSeed) { init(aSeed); }

    inline int64_t seed() const { return iSeed; }
    inline void setSeed(int64_t aSeed) { iSeed = (aSeed ^ iMultiplier) & iMask; }
    inline int32_t next32() { return generateNext(32); }
    inline int next(int aLimit) { return generateNext(31) % aLimit; }
    inline LinesColor nextColor() { return (LinesColor)next(LColorCount); }

private:
    void init(int64_t aSeed) {
        iMultiplier = 0x5DEECE66DLL;
        iAddend = 0xBLL;
        iMask = (1LL << 48) - 1;
        iSeed = (aSeed ^ iMultiplier) & iMask;
    }
    int32_t generateNext(int aBits) {
        iSeed = (iSeed * iMultiplier + iAddend) & iMask;
        return (int32_t)(iSeed >> (48 - aBits));
    }

private:
    int64_t iMultiplier;
    int64_t iAddend;
    int64_t iMask;
    int64_t iSeed;
};

#endif // LINES_TYPES_H
