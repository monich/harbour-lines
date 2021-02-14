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

#include "LinesPrefs.h"

#define DCONF_PATH              "/apps/" LINES_APP "/"
#define DCONF_SHOW_BALL_PATH    "showBallPath"
#define DCONF_SHOW_NEXT_BALLS   "showNextBalls"
#define DCONF_SHOW_BALL_SHADOW  "showBallShadow"
#define DCONF_PLAY_SOUNDS       "playSounds"
#define DCONF_BALL_STYLE        "ballStyle"

LinesPrefs::LinesPrefs(QObject* aParent) :
    QObject(aParent),
    iShowBallPath(new MGConfItem(DCONF_PATH DCONF_SHOW_BALL_PATH, this)),
    iShowNextBalls(new MGConfItem(DCONF_PATH DCONF_SHOW_NEXT_BALLS, this)),
    iShowBallShadow(new MGConfItem(DCONF_PATH DCONF_SHOW_BALL_SHADOW, this)),
    iPlaySounds(new MGConfItem(DCONF_PATH DCONF_PLAY_SOUNDS, this)),
    iBallStyle(new MGConfItem(DCONF_PATH DCONF_BALL_STYLE, this))
{
    connect(iShowBallPath,
        SIGNAL(valueChanged()),
        SIGNAL(showBallPathChanged()));
    connect(iShowNextBalls,
        SIGNAL(valueChanged()),
        SIGNAL(showNextBallsChanged()));
    connect(iShowBallShadow,
        SIGNAL(valueChanged()),
        SIGNAL(showBallShadowChanged()));
    connect(iPlaySounds,
        SIGNAL(valueChanged()),
        SIGNAL(playSoundsChanged()));
    connect(iBallStyle,
        SIGNAL(valueChanged()),
        SIGNAL(ballStyleChanged()));
}

bool LinesPrefs::showBallPath() const
{
    return iShowBallPath->value(DEFAULT_SHOW_BALL_PATH).toBool();
}

bool LinesPrefs::showNextBalls() const
{
    return iShowNextBalls->value(DEFAULT_SHOW_NEXT_BALLS).toBool();
}

bool LinesPrefs::showBallShadow() const
{
    return iShowBallShadow->value(DEFAULT_SHOW_BALL_SHADOW).toBool();
}

bool LinesPrefs::playSounds() const
{
    return iPlaySounds->value(DEFAULT_PLAY_SOUNDS).toBool();
}

QString LinesPrefs::ballStyle() const
{
    return iBallStyle->value(DCONF_DEFAULT_BALL_STYLE).toString();
}

void LinesPrefs::setShowBallPath(bool aValue)
{
    iShowBallPath->set(aValue);
}

void LinesPrefs::setShowNextBalls(bool aValue)
{
    iShowNextBalls->set(aValue);
}

void LinesPrefs::setShowBallShadow(bool aValue)
{
    iShowBallShadow->set(aValue);
}

void LinesPrefs::setPlaySounds(bool aValue)
{
    iPlaySounds->set(aValue);
}

void LinesPrefs::setBallStyle(QString aValue)
{
    iBallStyle->set(aValue);
}

bool LinesPrefs::equals(const LinesPrefs& aPrefs) const
{
    return aPrefs.showBallPath() == showBallPath() &&
           aPrefs.showNextBalls() == showNextBalls() &&
           aPrefs.showBallShadow() == showBallShadow() &&
           aPrefs.playSounds() == playSounds() &&
           aPrefs.ballStyle() == ballStyle();
}
