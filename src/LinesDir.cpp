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

#include "LinesTypes.h"

static bool StepNorth(LinesPoint* aPoint)
{
    if (aPoint->y < (LinesGridSize-1)) {
        aPoint->y++;
        return true;
    }
    return false;
}

static bool StepNorthEast(LinesPoint* aPoint)
{
    if (aPoint->x < (LinesGridSize-1) && aPoint->y < (LinesGridSize-1)) {
        aPoint->x++;
        aPoint->y++;
        return true;
    }
    return false;
}

static bool StepEast(LinesPoint* aPoint)
{
    if (aPoint->x < (LinesGridSize-1)) {
        aPoint->x++;
        return true;
    }
    return false;
}

static bool StepSouthEast(LinesPoint* aPoint)
{
    if (aPoint->x < (LinesGridSize-1) && aPoint->y > 0) {
        aPoint->x++;
        aPoint->y--;
        return true;
    }
    return false;
}

static bool StepSouth(LinesPoint* aPoint)
{
    if (aPoint->y > 0) {
        aPoint->y--;
        return true;
    }
    return false;
}

static bool StepSouthWest(LinesPoint* aPoint)
{
    if (aPoint->x > 0 && aPoint->y > 0) {
        aPoint->x--;
        aPoint->y--;
        return true;
    }
    return false;
}

static bool StepWest(LinesPoint* aPoint)
{
    if (aPoint->x > 0) {
        aPoint->x--;
        return true;
    }
    return false;
}

static bool StepNorthWest(LinesPoint* aPoint)
{
    if (aPoint->x > 0 && aPoint->y < (LinesGridSize-1)) {
        aPoint->x--;
        aPoint->y++;
        return true;
    }
    return false;
}

#define DEFINE_DIR(d) \
static const LDirection d = { #d, LDir##d, Step##d }

DEFINE_DIR(North);
DEFINE_DIR(NorthEast);
DEFINE_DIR(East);
DEFINE_DIR(SouthEast);
DEFINE_DIR(South);
DEFINE_DIR(SouthWest);
DEFINE_DIR(West);
DEFINE_DIR(NorthWest);

// NOTE: it's important that direction[k+4] is an opposite to direction[k]
const LDirectionConstPtr linesDirection[LDirCount] = {
    &North, &NorthEast, &East, &SouthEast,
    &South, &SouthWest, &West, &NorthWest,
};

const LDirectionConstPtr linesMajorDirection[LDirMajorCount] = {
    &North, &East, &South, &West
};
