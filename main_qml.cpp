//
// C++ Implementation: main
//
// Description:
//
//
// Author: Thibaut GRIDEL <tgridel@free.fr>
//
// Copyright (c) 2014 Thibaut GRIDEL
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

#include <QDir>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QQuickView>

int debugLevel = 0;

int main(int argc, char* argv[]) {

    int i;
    QStringList arguments = QCoreApplication::arguments();
    arguments.removeAt(0);
    if (-1 != (i=arguments.indexOf("-debug"))) {
        debugLevel = arguments[i+1].toInt();
        std::cout << "debug level set to " << debugLevel << std::endl;
        arguments.removeAt(i+1);
        arguments.removeAt(i);
    }

    QGuiApplication app(argc,argv);
    app.setApplicationName("boats-qml");
    QQuickView view;
    view.connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));
    view.setSource(QUrl("boats.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();
    return app.exec();
}
