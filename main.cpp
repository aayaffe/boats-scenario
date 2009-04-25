//
// C++ Implementation: main
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2008-2009 Thibaut GRIDEL
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
#include <iostream>

#include <QtCore>

#include "boatapplication.h"
#include "mainwindow.h"

int debugLevel = 0;


int main(int argc, char *argv[]) {
    BoatApplication app(argc, argv);

    int i;
    QStringList arguments = QCoreApplication::arguments();
    arguments.removeAt(0);
    if (-1 != (i=arguments.indexOf("-debug"))) {
        debugLevel = arguments[i+1].toInt();
        std::cout << "debug level set to " << debugLevel << std::endl;
        arguments.removeAt(i+1);
        arguments.removeAt(i);
    }

    // Locale and translation setup
    QString locale = QLocale::system().name();

    QTranslator translator;
    translator.load(QString("boats_") + locale, TRANSLATEDIR);
    app.installTranslator(&translator);

    // MainWindow
    MainWindow window;
    app.setWindow(&window);
    window.show();
    window.openFiles(arguments);

    return app.exec();
}
