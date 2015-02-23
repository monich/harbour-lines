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

#include "SystemState.h"
#include "LinesDebug.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

#define MCE_SERVICE "com.nokia.mce"

SystemState::SystemState(QObject* aParent) :
    QObject(aParent)
{
    QDEBUG("created");
    setupProperty("get_display_status", "display_status_ind",
        SLOT(onDisplayStatusQueryDone(QDBusPendingCallWatcher*)),
        SLOT(onDisplayStatusChanged(QString)));
    setupProperty("get_tklock_mode", "tklock_mode_ind",
        SLOT(onLockModeQueryDone(QDBusPendingCallWatcher*)),
        SLOT(onLockModeChanged(QString)));
}

SystemState::~SystemState()
{
    QDEBUG("destroyed");
}

void SystemState::setupProperty(QString aQueryMethod, QString aSignal,
    const char* aQuerySlot, const char* aSignalSlot)
{
    QDBusConnection systemBus(QDBusConnection::systemBus());
    connect(new QDBusPendingCallWatcher(systemBus.asyncCall(
        QDBusMessage::createMethodCall(MCE_SERVICE, "/com/nokia/mce/request",
        "com.nokia.mce.request", aQueryMethod)), this),
        SIGNAL(finished(QDBusPendingCallWatcher*)), this, aQuerySlot);
    systemBus.connect(MCE_SERVICE, "/com/nokia/mce/signal",
        "com.nokia.mce.signal", aSignal, this, aSignalSlot);
}

void SystemState::setDisplayStatus(QString aStatus)
{
    if (iDisplayStatus != aStatus) {
        iDisplayStatus = aStatus;
        emit displayStatusChanged();
    }
}

void SystemState::onDisplayStatusChanged(QString aStatus)
{
    QDEBUG(aStatus);
    setDisplayStatus(aStatus);
}

void SystemState::onDisplayStatusQueryDone(QDBusPendingCallWatcher* aWatcher)
{
    QDBusPendingReply<QString> reply(*aWatcher);
    QDEBUG(reply);
    if (reply.isValid() && !reply.isError()) {
        setDisplayStatus(reply.value());
    }
    aWatcher->deleteLater();
}

void SystemState::setLockMode(QString aMode)
{
    if (iLockMode != aMode) {
        iLockMode = aMode;
        emit lockModeChanged();
    }
}

void SystemState::onLockModeChanged(QString aMode)
{
    QDEBUG(aMode);
    setLockMode(aMode);
}

void SystemState::onLockModeQueryDone(QDBusPendingCallWatcher* aWatcher)
{
    QDBusPendingReply<QString> reply(*aWatcher);
    QDEBUG(reply);
    if (reply.isValid() && !reply.isError()) {
        setLockMode(reply.value());
    }
    aWatcher->deleteLater();
}
