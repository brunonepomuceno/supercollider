/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "main.hpp"
#include "settings/manager.hpp"
#include "session_manager.hpp"
#include "../widgets/main_window.hpp"
#include "../widgets/code_editor/highlighter.hpp"
#include "SC_DirUtils.h"

#include <QApplication>
#include <QAction>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDir>

using namespace ScIDE;

int main( int argc, char *argv[] )
{
    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator scideTranslator;
    scideTranslator.load("scide_" + QLocale::system().name());
    app.installTranslator(&scideTranslator);

    Main * main = Main::instance();

    MainWindow *win = new MainWindow(main);
    win->show();

    // NOTE: load session after GUI is created, so that GUI can respond
    QString lastSession = main->sessionManager()->lastSession();
    if (lastSession.isEmpty())
        lastSession = "default";
    main->sessionManager()->openSession(lastSession);

    bool startInterpreter = main->settings()->value("IDE/interpreter/autoStart").toBool();
    if(startInterpreter)
        main->scProcess()->start();

    return app.exec();
}

static QString getSettingsFile()
{
    char config_dir[PATH_MAX];
    bool configured = false;
    sc_GetUserConfigDirectory(config_dir, PATH_MAX);
    return QString(config_dir) + SC_PATH_DELIMITER + "sc_ide_conf.yaml";
}

// NOTE: mSettings must be the first to initialize,
// because other members use it!

Main::Main(void) :
    mSettings( new Settings::Manager( getSettingsFile(), this ) ),
    mDocManager( new DocumentManager(this) ),
    mSessionManager( new SessionManager(mDocManager, this) ),
    mSCProcess( new SCProcess(this) )
{
    new SyntaxHighlighterGlobals(this);
}

void Main::quit() {
    mSessionManager->closeSession();
    storeSettings();
    QApplication::quit();
}
