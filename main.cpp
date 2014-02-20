/****************************************************************************
**   nodexmpp is the nodecast xmpp server
**   Copyright (C) 2010-2014  Frédéric Logier <frederic@logier.org>
**
**   https://github.com/nodecast/nodexmpp
**
**   This program is free software: you can redistribute it and/or modify
**   it under the terms of the GNU Affero General Public License as
**   published by the Free Software Foundation, either version 3 of the
**   License, or (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU Affero General Public License for more details.
**
**   You should have received a copy of the GNU Affero General Public License
**   along with this program.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "main.h"
//#include <QtCore/QCoreApplication>
//#include <QCoreApplication>


int Service::sighupFd[2]={};
int Service::sigtermFd[2]={};

QSettings settings("nodecast", "nodexmpp");


// http://doc.qt.nokia.com/4.7/unix-signals.html
static void setup_unix_signal_handlers()
{
    struct sigaction hup, term;

    hup.sa_handler = Service::hupSignalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    /*if (sigaction(SIGHUP, &hup, 0) > 0)
       return 1;*/

    term.sa_handler = Service::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    /*if (sigaction(SIGTERM, &term, 0) > 0)
       return 2;

    return 0;*/

    sigaction (SIGINT, &hup, NULL);
    sigaction (SIGTERM, &term, NULL);
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    params nodexmpp_params;
    bool debug;
    bool verbose;


    QxtCommandOptions options;
    options.add("debug", "show debug informations");
    options.alias("debug", "d");

    options.add("base-directory", "set the ftp base directory", QxtCommandOptions::Required);
    options.alias("base-directory", "bd");

    options.add("xmpp-server-port", "set the xmpp server port", QxtCommandOptions::Optional);
    options.alias("xmpp-server-port", "xsp");

    options.add("xmpp-client-port", "set the xmpp client port", QxtCommandOptions::Optional);
    options.alias("xmpp-client-port", "xcp");


    options.add("xmpp-domain-name", "set the xmpp domain name", QxtCommandOptions::Optional);
    options.alias("xmpp-domain-name", "xdn");


    options.add("help", "show this help text");
    options.alias("help", "h");
    options.parse(QCoreApplication::arguments());
    if(options.count("help") || options.showUnrecognizedWarning()) {
        options.showUsage();
        return -1;
    }


    if(options.count("base-directory")) {
        nodexmpp_params.base_directory = options.value("base-directory").toString();
        settings.setValue("base-directory", nodexmpp_params.base_directory);
    }
    else if(settings.contains("base-directory"))
    {
        nodexmpp_params.base_directory = settings.value("base-directory").toString();
    }
    else {
        std::cout << "nodexmpp: --xmpp-base-directory requires a parameter" << std::endl;
        options.showUsage();
        return -1;
    }


    if(options.count("xmpp-server-port")) {
        nodexmpp_params.xmpp_server_port = options.value("xmpp-server-port").toInt();
        settings.setValue("xmpp-server-port", nodexmpp_params.xmpp_server_port);
    }
    else if(settings.contains("xmpp-server-port"))
    {
        nodexmpp_params.xmpp_server_port = settings.value("xmpp-server-port").toInt();
    }
    else nodexmpp_params.xmpp_server_port = 6269;



    if(options.count("xmpp-client-port")) {
        nodexmpp_params.xmpp_client_port = options.value("xmpp-client-port").toInt();
        settings.setValue("xmpp-client-port", nodexmpp_params.xmpp_client_port);
    }
    else if(settings.contains("xmpp-client-port"))
    {
        nodexmpp_params.xmpp_client_port = settings.value("xmpp-client-port").toInt();
    }
    else nodexmpp_params.xmpp_client_port = 6222;



    if(options.count("xmpp-domain-name")) {
        nodexmpp_params.domain_name = options.value("xmpp-domain-name").toString();
        settings.setValue("xmpp-domain-name", nodexmpp_params.domain_name);
    }
    else if(settings.contains("xmpp-domain-name"))
    {
        nodexmpp_params.domain_name = settings.value("xmpp-domain-name").toString();
    }
    else nodexmpp_params.domain_name = "localhost";



    settings.sync();
    setup_unix_signal_handlers();


    Service *service;
    service = new Service(nodexmpp_params);

    return a.exec();
}
