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

#include "service.h"


Io::~Io()
{
    notifier->setEnabled(false);
    delete(input);
    io_log->close();
    qDebug() << "END IO";
}

Io::Io() : QObject()
{
    input  = new QTextStream( stdin,  QIODevice::ReadOnly );

    // Demand notification when there is data to be read from stdin
    notifier = new QSocketNotifier( STDIN_FILENO, QSocketNotifier::Read );
    connect(notifier, SIGNAL(activated(int)), this, SLOT(readStdin()), Qt::DirectConnection);


    io_log = new QFile("/tmp/ncw_nodexmpp");
    if (!io_log->open(QIODevice::Append | QIODevice::Text))
            return;


    //*cout << prompt;
}

void Io::readStdin()
{
    notifier->setEnabled(false);


//    qDebug() << "READ STDIN";
    // Read the data
    QString line = input->readLine();

    io_log->write(line.toAscii());
    io_log->write("\n");
    io_log->flush();

    //writeStdout("LINE : " + line);
    //if (!line.isNull() && !line.isEmpty())
    //{
    // Parse received data



    QVariant json = QxtJSON::parse(line);

    if (json.toMap().contains("dest"))
    {
        QString dest = json.toMap()["dest"].toString();

        if (dest == "xmppserver")
                emit parseServerData(line);
        else if (dest == "xmppclient")
                emit parseClientData(line);
    }


    //}

    notifier->setEnabled(true);
}


void Service::hupSignalHandler(int)
{
    char a = 1;
    ::write(sighupFd[0], &a, sizeof(a));
}

void Service::termSignalHandler(int)
{
    char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
}

void Service::handleSigTerm()
{
    snTerm->setEnabled(false);
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));

    // do Qt stuff
    qDebug() << "Received SIGTERM";
    snTerm->setEnabled(true);
}

void Service::handleSigHup()
{
    snHup->setEnabled(false);
    char tmp;
    ::read(sighupFd[1], &tmp, sizeof(tmp));

    qDebug() << "Received SIGHUP";


    qDebug() << "Service::handleSigHup delete ncw";
    ncw->deleteLater();
    ncw_thread->wait();

    qDebug() << "Service::handleSigHup delete xmpp client";
    m_xmpp_client->deleteLater();
    thread_xmpp_client->wait();

    qDebug() << "Service::handleSigHup delete xmpp server";
    m_xmpp_server->deleteLater();
    thread_xmpp_server->wait();


    snHup->setEnabled(true);

    qDebug() << "nodexmpp shutdown init";
    qApp->exit();
}



Service::~Service()
{
    qDebug() << "Service::~Service delete ncw";
    ncw->deleteLater();
    ncw_thread->wait();


    qDebug() << "Service::~Service delete xmpp server";
    m_xmpp_server->deleteLater();
    thread_xmpp_server->wait();

    qDebug() << "Service::~Service delete xmpp client";
    m_xmpp_client->deleteLater();
    thread_xmpp_client->wait();
}



Service::Service(params a_params, QObject *parent) : m_params(a_params), QObject(parent)
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
        qFatal("Couldn't create HUP socketpair");

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
        qFatal("Couldn't create TERM socketpair");
    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, SIGNAL(activated(int)), this, SLOT(handleSigHup()));
    snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    connect(snTerm, SIGNAL(activated(int)), this, SLOT(handleSigTerm()));

    qRegisterMetaType<QXmppLogger::MessageType>("QXmppLogger::MessageType");


    std::cout << "{\"action\": \"get_xmpp_users\", \"from\": \"xmppserver\", \"dest\": \"self\"}" << std::endl;



    ncw_thread = new QThread(this);
    ncw = new Io();
    connect(ncw, SIGNAL(destroyed()), ncw_thread, SLOT(quit()), Qt::DirectConnection);


    thread_xmpp_server = new QThread(this);
    m_xmpp_server = new Xmpp_server(m_params.domain_name, m_params.xmpp_client_port, m_params.xmpp_server_port);
    connect(m_xmpp_server, SIGNAL(destroyed()), thread_xmpp_server, SLOT(quit()), Qt::DirectConnection);
    connect(thread_xmpp_server, SIGNAL(started()), m_xmpp_server, SLOT(init()));
    connect(ncw, SIGNAL(parseServerData(QString)), m_xmpp_server, SLOT(receive_payload(QString)), Qt::QueuedConnection);



    thread_xmpp_client = new QThread(this);
    m_xmpp_client = new Xmpp_client(m_params.base_directory, m_params.domain_name, m_params.xmpp_client_port);
    connect(m_xmpp_client, SIGNAL(destroyed()), thread_xmpp_client, SLOT(quit()), Qt::DirectConnection);
    connect(thread_xmpp_client, SIGNAL(started()), m_xmpp_client, SLOT(init()));
    connect(ncw, SIGNAL(parseClientData(QString)), m_xmpp_client, SLOT(receive_payload(QString)), Qt::QueuedConnection);


    ncw->moveToThread(ncw_thread);
    ncw_thread->start();

    m_xmpp_server->moveToThread(thread_xmpp_server);
    thread_xmpp_server->start();


    m_xmpp_client->moveToThread(thread_xmpp_client);
    thread_xmpp_client->start();

}

