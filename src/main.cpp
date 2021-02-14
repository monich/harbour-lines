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

#include "QuickLines.h"
#include "QuickLinesModel.h"
#include "QuickLinesGame.h"
#include "QuickNextBallsModel.h"
#include "QuickScoresModel.h"
#include "LinesPrefs.h"

#include "HarbourDebug.h"

#include <QtGui>

#if QT_VERSION >= 0x050000
#include <QtQuick>
#else
#include <QtDeclarative>
#endif

#ifdef HARMATTAN
#include "MeegoLines.h"
#define SailfishApp MeegoApp
#define QQuickView MeegoView
#define MAIN_QML "qml/meego/main.qml"
#else
#include <sailfishapp.h>
#include "HarbourImageProvider.h"
#include "HarbourTheme.h"
#define loadTranslations(translator,locale,filename,prefix,directory) \
    ((translator)->load(locale,filename,prefix,directory))
#define MAIN_QML "qml/sailfish/main.qml"
#endif

#define REGISTER(type,name) qmlRegisterType<type>(LINES_PLUGIN,1,0,name)
#define REGISTER_SINGLETON(type,name) \
    qmlRegisterSingletonType<type>(LINES_PLUGIN,1,0,name,type::createSingleton)

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QCoreApplication* app = SailfishApp::application(argc, argv);

    // Load translations
    QLocale locale;
    QTranslator* translator = new QTranslator(app);
    QString transDir = SailfishApp::pathTo("translations").toLocalFile();
    QString transFile(LINES_APP);
    if (loadTranslations(translator, locale, transFile, "-", transDir) ||
        translator->load(transFile, transDir)) {
        app->installTranslator(translator);
    } else {
        HWARN("Failed to load translator for" << locale);
        delete translator;
    }

    REGISTER(QuickLines, "Lines");
    REGISTER(QuickLinesModel, "LinesModel");
    REGISTER(QuickLinesGame, "LinesGame");
    REGISTER(QuickNextBallsModel, "NextBallsModel");
    REGISTER(QuickScoresModel, "ScoresModel");
    REGISTER(LinesPrefs, "LinesPrefs");

    QQuickView *view = SailfishApp::createView();

#ifndef HARMATTAN
    QQmlContext* context = view->rootContext();
    QQmlEngine* engine = context->engine();

    QString imageProvider("harbour");
    context->setContextProperty("HarbourImageProvider", imageProvider);
    engine->addImageProvider(imageProvider, new HarbourImageProvider);
    REGISTER_SINGLETON(HarbourTheme, "HarbourTheme");
#endif

    view->setSource(SailfishApp::pathTo(MAIN_QML));
    view->show();

    int result = app->exec();
    delete view;
    delete app;
    return result;
}
