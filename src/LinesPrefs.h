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

#ifndef LINES_PREFS_H
#define LINES_PREFS_H

#include "LinesTypes.h"
#include <MGConfItem>

#define DEFAULT_SHOW_BALL_PATH      true
#define DEFAULT_SHOW_NEXT_BALLS     false
#define DEFAULT_SHOW_BALL_SHADOW    true
#define DEFAULT_PLAY_SOUNDS         true
#define DCONF_DEFAULT_BALL_STYLE    QStringLiteral("ball")

class LinesPrefs: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showBallPath READ showBallPath WRITE setShowBallPath NOTIFY showBallPathChanged)
    Q_PROPERTY(bool showNextBalls READ showNextBalls WRITE setShowNextBalls NOTIFY showNextBallsChanged)
    Q_PROPERTY(bool showBallShadow READ showBallShadow WRITE setShowBallShadow NOTIFY showBallShadowChanged)
    Q_PROPERTY(bool playSounds READ playSounds WRITE setPlaySounds NOTIFY playSoundsChanged)
    Q_PROPERTY(QString ballStyle READ ballStyle WRITE setBallStyle NOTIFY ballStyleChanged)

public:
    explicit LinesPrefs(QObject* aParent = NULL);

    bool showBallPath() const;
    bool showNextBalls() const;
    bool showBallShadow() const;
    bool playSounds() const;
    QString ballStyle() const;

    void setShowBallPath(bool aValue);
    void setShowNextBalls(bool aValue);
    void setShowBallShadow(bool aValue);
    void setPlaySounds(bool aValue);
    void setBallStyle(QString aValue);

    bool operator==(const LinesPrefs& aPrefs) const { return equals(aPrefs); }
    bool operator!=(const LinesPrefs& aPrefs) const { return !equals(aPrefs); }
    bool equals(const LinesPrefs& aPrefs) const;

Q_SIGNALS:
    void showBallPathChanged();
    void showNextBallsChanged();
    void showBallShadowChanged();
    void playSoundsChanged();
    void ballStyleChanged();

private:
    MGConfItem* iShowBallPath;
    MGConfItem* iShowNextBalls;
    MGConfItem* iShowBallShadow;
    MGConfItem* iPlaySounds;
    MGConfItem* iBallStyle;
};

QML_DECLARE_TYPE(LinesPrefs)

#endif // LINES_PREFS_H
