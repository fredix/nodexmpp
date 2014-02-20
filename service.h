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

#ifndef SERVICE_H
#define SERVICE_H

#include <QThread>
#include "xmpp_server.h"
#include "xmpp_client.h"
#include <QSocketNotifier>
#include <QCoreApplication>
#include <iostream>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>

struct params {
    QString base_directory;
    int xmpp_server_port;
    int xmpp_client_port;
    QString domain_name;
};



class Io : public QObject
{
    Q_OBJECT

public:
    Io();
    ~Io();

private slots:
    void readStdin();

private:
    QFile *io_log;
    QSocketNotifier* notifier;
    QTextStream* input;

signals:
    void parseServerData(QString s);
    void parseClientData(QString s);

};


class Service : public QObject
{
    Q_OBJECT
public:
    Service(params a_ncs_params, QObject *parent = 0);
    ~Service();

    // Unix signal handlers.
    static void hupSignalHandler(int unused);
    static void termSignalHandler(int unused);


public slots:
    void handleSigHup();
    void handleSigTerm();


private:
    static int sighupFd[2];
    static int sigtermFd[2];

    QSocketNotifier *snHup;
    QSocketNotifier *snTerm;

    Xmpp_server *m_xmpp_server;
    Xmpp_client *m_xmpp_client;

    Io *ncw;

    params m_params;
    QThread *thread_xmpp_server;
    QThread *thread_xmpp_client;
    QThread *ncw_thread;
};

#endif // SERVICE_H
