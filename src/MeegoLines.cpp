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

#include "MeegoLines.h"
#include "HarbourDebug.h"

#include <QFileInfo>

#ifdef HARMATTAN_BOOSTER
#include <MDeclarativeCache>
#endif

// ==========================================================================
// MeegoView
// ==========================================================================

MeegoView::MeegoView(QWidget* aParent) : QDeclarativeView(aParent)
{
    connect(engine(), SIGNAL(quit()), SLOT(close()));
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
}

void MeegoView::show()
{
    showFullScreen();
}

// ==========================================================================
// MeegoApp
// ==========================================================================

QApplication* MeegoApp::application(int &argc, char **argv)
{
#ifdef HARMATTAN_BOOSTER
    return MDeclarativeCache::qApplication(argc, argv);
#else
    return new QApplication(argc, argv);
#endif
}

QUrl MeegoApp::pathTo(const QString &path)
{
    QString fullPath = QString::fromLatin1("%1/../%2").arg(
        QCoreApplication::applicationDirPath(), path);
    if (!QFileInfo(fullPath).exists()) fullPath = path;
    return QUrl::fromLocalFile(fullPath);
}

MeegoView* MeegoApp::createView()
{
    return new MeegoView();
}

// ==========================================================================
// Translation
// ==========================================================================

bool loadTranslations(QTranslator* aTranslator, QLocale& aLocale,
    QString aFilename, QString aPrefix, QString aDirectory,
    QString aSuffix)
{
    QString lang(aLocale.name().left(2));
    if (!lang.isEmpty()) aFilename.append(aPrefix).append(lang);
    HDEBUG("Loading" << aFilename);
    if (!aTranslator->load(aFilename, aDirectory)) {
        qWarning() << "Failed to load translations for" << lang;
        return false;
    }
    return true;
}
