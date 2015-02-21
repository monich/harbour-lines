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

#include "QuickLines.h"
#include "QuickLinesModel.h"
#include "QuickLinesGame.h"
#include "LinesPrefs.h"

#include <QtGui>
#include <QtQuick>
#include <sailfishapp.h>

Q_IMPORT_PLUGIN(QSvgPlugin)
QML_DECLARE_TYPE(LinesPrefs)

#define REGISTER(type,name) qmlRegisterType<type>(HARBOUR_PLUGIN,1,0,name);

int main(int argc, char *argv[])
{
    QGuiApplication* app = SailfishApp::application(argc, argv);

    // Load translations
    QLocale locale;
    QTranslator* translator = new QTranslator(app);
    QString transDir = SailfishApp::pathTo("translations").toLocalFile();
    QString transFile(HARBOUR_APP);
    if (translator->load(locale, transFile, "-", transDir) ||
        translator->load(transFile, transDir)) {
        app->installTranslator(translator);
    } else {
        qWarning() << "Failed to load translator for" << locale;
        delete translator;
    }

    REGISTER(QuickLines, "Lines");
    REGISTER(QuickLinesModel, "LinesModel");
    REGISTER(QuickLinesGame, "LinesGame");
    REGISTER(LinesPrefs, "LinesPrefs");

    QQuickView *view = SailfishApp::createView();
    view->setSource(SailfishApp::pathTo(QString("qml/main.qml")));
    view->show();

    int result = app->exec();
    delete view;
    delete app;
    return result;
}
