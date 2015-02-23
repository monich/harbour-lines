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

#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include <QtQml>

class QDBusPendingCallWatcher;

class SystemState: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayStatus READ displayStatus NOTIFY displayStatusChanged)
    Q_PROPERTY(QString lockMode READ lockMode NOTIFY lockModeChanged)

public:
    explicit SystemState(QObject* aParent = NULL);
    ~SystemState();

    QString displayStatus() const { return iDisplayStatus; }
    QString lockMode() const { return iLockMode; }

private:
    void setupProperty(QString aQueryMethod, QString aSignal,
        const char* aQuerySlot, const char* aSignalSlot);
    void setDisplayStatus(QString aStatus);
    void setLockMode(QString aStatus);

signals:
    void displayStatusChanged();
    void lockModeChanged();

private slots:
    void onDisplayStatusChanged(QString);
    void onDisplayStatusQueryDone(QDBusPendingCallWatcher*);
    void onLockModeChanged(QString);
    void onLockModeQueryDone(QDBusPendingCallWatcher*);

private:
    QString iDisplayStatus;
    QString iLockMode;
};

QML_DECLARE_TYPE(SystemState)

#endif // SYSTEM_STATE_H
