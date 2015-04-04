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

#include "LinesScores.h"
#include "LinesDebug.h"

#include <QVariantList>
#include <QDateTime>

#define LINES_SCORES_MAX_COUNT (10)

/* JSON keys */
#define LINES_SCORES_JSON_VERSION (1)
static QString kJsonScoresVersion("version");
static QString kJsonScoresArray("scores");
static QString kJsonScoreValue("value");
static QString kJsonScoreTimestamp("timestamp");

//===========================================================================
// LinesScore
//===========================================================================

class LinesScore {
public:
    LinesScore(int aValue) :
        iValue(aValue),
        iTimestamp(QDateTime::currentDateTime()) {}
    LinesScore(int aValue, const QDateTime &aTimestamp) :
        iValue(aValue),
        iTimestamp(aTimestamp) {}

    QVariantMap toVariantMap() const {
        QVariantMap map;
        map.insert(kJsonScoreValue, iValue);
        map.insert(kJsonScoreTimestamp,
            iTimestamp.toUTC().toString(Qt::ISODate));
        return map;
    }

public:
    int iValue;
    QDateTime iTimestamp;
};

static bool scoreLessThan(LinesScore* aScore1, LinesScore* aScore2)
{
    // We are sorting in descending order
    return aScore1->iValue > aScore2->iValue;
}

//===========================================================================
// LinesScores
//===========================================================================

LinesScores::LinesScores(QVariantMap* aMap)
{
    if (aMap) {
        QVariant version = aMap->value(kJsonScoresVersion);
        if (version.toInt() != LINES_SCORES_JSON_VERSION) {
            qWarning() << "Unexpected scores JSON version:" << version;
            aMap = NULL;
        }
    }

    if (aMap) {
        QVariantList array = aMap->value(kJsonScoresArray).toList();
        int i, n = array.count();
        for (i=0; i<n; i++) {
            QVariantMap entry = array.at(i).toMap();
            int value = entry.value(kJsonScoreValue).toInt();
            QDateTime timestamp = QDateTime::fromString(
                entry.value(kJsonScoreTimestamp).toString(), Qt::ISODate);
            if (value > 0 && timestamp.isValid()) {
                iScores.append(new LinesScore(value, timestamp.toLocalTime()));
            }
        }
        QDEBUG("Loaded" << iScores.count() << "score entries");
        normalize();
    }
}

LinesScores::~LinesScores()
{
    qDeleteAll(iScores);
}

QVariantMap LinesScores::toVariantMap() const
{
    QVariantList array;
    int i, n = iScores.count();
    for (i=0; i<n; i++) {
        array.append(iScores.at(i)->toVariantMap());
    }

    QVariantMap map;
    map.insert(kJsonScoresVersion, LINES_SCORES_JSON_VERSION);
    map.insert(kJsonScoresArray, array);
    return map;
}

int LinesScores::highScore() const
{
    return (iScores.count() > 0) ? iScores.at(0)->iValue : 0;
}

bool LinesScores::addHighScore(int aScore)
{
    if (aScore > highScore()) {
        QDEBUG("new high score" << aScore);
        iScores.append(new LinesScore(aScore));
        normalize();
        QASSERT(highScore() == aScore);
        return true;
    }
    return false;
}

void LinesScores::normalize()
{
    qSort(iScores.begin(), iScores.end(), scoreLessThan);
    while (iScores.count() > LINES_SCORES_MAX_COUNT) {
        delete iScores.takeLast();
    }
}
