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

#ifndef XMPP_SERVER_H
#define XMPP_SERVER_H

#include "QXmppLogger.h"
//#include "QXmppPasswordChecker.h"
#include "QXmppServer.h"
#include "QXmppPasswordChecker.h"
#include "QXmppIncomingClient.h"
//#include "mongodb.h"
#include <QHash>
#include <QFile>
#include <QxtJSON>
#include <QTextStream>



class passwordChecker : public QXmppPasswordChecker
{

    /// Returns true as we implemented getPassword().
    bool hasGetPassword() const
    {
        return false;
    }

private:
    QXmppPasswordReply* checkPassword(const QXmppPasswordRequest &request);
    QHash<QString, QString> *users;

public:
    passwordChecker(QHash<QString, QString> &xmpp_users);
    QString m_username;
    QString m_password;
    //Mongodb *mongodb_;
};

class Xmpp_server : public QObject
{
    Q_OBJECT
public:
    Xmpp_server(QString a_domain, int a_xmpp_client_port, int a_xmpp_server_port);
    ~Xmpp_server();


private:
    void writeStdout(QString s);

    QHash<QString, QString> xmpp_users;
    QTextStream* output;
    QFile *log;
    QString m_domain;
    int m_xmpp_client_port;
    int m_xmpp_server_port;
    QString m_jabberid;
    QString m_jabberpassword;
    passwordChecker *m_checker;
    QXmppLogger m_logger;
    QXmppServer m_server;


public slots:
    void init();
    void receive_payload(QString s);
};

#endif // XMPP_SERVER_H
