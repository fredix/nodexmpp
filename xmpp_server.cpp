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

#include "xmpp_server.h"




passwordChecker::passwordChecker(QHash<QString, QString> &xmpp_users)
{
    users = &xmpp_users;
}



/// Checks that the given credentials are valid.
QXmppPasswordReply* passwordChecker::checkPassword(const QXmppPasswordRequest &request)
{
         QXmppPasswordReply *reply = new QXmppPasswordReply;

        // construct a bson object with username and token
      /*  bo auth = BSON("node_uuid" << request.username().toStdString() << "node_password" << request.password().toStdString());
        std::cout << "checkPassword AUTH !!!!!!!!   :     " << auth << std::endl;


        // search user through users's collection with the bson object payload
        bo node = mongodb_->Find("nodes", auth);
        if (node.nFields() != 0)
        {
            be node_id = node.getField("_id");
            std::cout << "node_id : " << node_id << std::endl;
            reply->setError(QXmppPasswordReply::NoError);
            reply->finishLater();
            return  reply;
        }*/

         if (users->contains(request.username()) && (*users)[request.username()].compare(request.password())==0 )
         {
             qDebug() << "XMPP SERVER";
             reply->setError(QXmppPasswordReply::NoError);
             reply->finishLater();
             return reply;
         }


        // used only for the qxmpp client embedded into the ncs server.
        if (request.username() == m_username && request.password() == m_password)
        {
            qDebug() << "XMPP SERVER";
            reply->setError(QXmppPasswordReply::NoError);
            reply->finishLater();
            return reply;
        }
        else
        {
            qDebug() << "XMPP auth failed !";
            reply->setError(QXmppPasswordReply::AuthorizationError);
            reply->finishLater();
            return reply;
        }

        /*

        if (username == m_username && password == m_password)
            return QXmppPasswordChecker::NoError;
        else
            return QXmppPasswordChecker::AuthorizationError;
            */
}





Xmpp_server::Xmpp_server(QString a_domain, int a_xmpp_client_port, int a_xmpp_server_port) : m_domain(a_domain), m_xmpp_client_port(a_xmpp_client_port), m_xmpp_server_port(a_xmpp_server_port)
{
    qDebug() << "Xmpp_server construct param";


    m_checker = new passwordChecker(xmpp_users);



  //  m_checker.mongodb_ = Mongodb::getInstance();
    m_checker->m_username = "ncs";
    m_checker->m_password = "scn";

    //m_logger.setLoggingType(QXmppLogger::StdoutLogging);
    //m_logger.setLoggingType(QXmppLogger::FileLogging);


    //const QString domain = QString::fromLocal8Bit("localhost");

    m_server.setDomain(a_domain);
    m_server.setLogger(&m_logger);
    m_server.setPasswordChecker(m_checker);

    QHostAddress bind_ip(QHostAddress::Any);

    m_server.listenForClients(bind_ip, m_xmpp_client_port);
    m_server.listenForServers(bind_ip, m_xmpp_server_port);
}



Xmpp_server::~Xmpp_server()
{
    qDebug() << "Xmpp_server shutdown";
    m_server.close ();
    log->close();
}



void Xmpp_server::init()
{
    output = new QTextStream( stdout, QIODevice::WriteOnly );

    log = new QFile("/tmp/xmppserver");
     if (!log->open(QIODevice::Append | QIODevice::Text))
             return;

}




void Xmpp_server::writeStdout(QString s)
{
    (*output) << s;
    //while (!(*output).atEnd()) {};
    (*output).flush();
}


void Xmpp_server::receive_payload(QString s)
{
   // writeStdout("PARSE : " + s);
    //return;
    //std::cout << s.toStdString() << std::endl;
    // Do something with your received data here
    //writeStdout(s);

    QString t_log = "Xmpp_server::receive_payload : " + s;
    log->write(t_log.toAscii());
    log->write("\n");
    log->flush();


    QVariant json = QxtJSON::parse(s);



    if (json.toMap().contains("login") &&
            json.toMap().contains("password"))
    {
        QString user_login = json.toMap()["login"].toString();
        QString user_password = json.toMap()["password"].toString();


      /*  writeStdout("event : " + event);
        qDebug() << "usageid : " << usageid;
        qDebug() << "value : " << value;
        qDebug() << "contentid : " << contentid;
        */

        //$datetime=date('Y-m-d H:i:s',$dt);
        //QDateTime l_timestamp = QDateTime::currentDateTimeUtc();
        //QString timestamp = l_timestamp.toString("yyyy-MM-dd HH:mm:ss");

        bool res = true;
  ///      if (command == "add")
            //res = add_user(user_email, user_password, user_path);
            xmpp_users[user_login] = user_password;

       /* if (command == "reload")
        {
            res = delete_user(user_email);
            res = add_user(user_email, user_password, user_path);
        }
*/
        //writeStdout("REQ SQL : " + req);


        /*query->exec("SELECT VERSION()");

        while (query->next())
        {
            QString name = query->value(0).toString();
            //qDebug() << name;
            writeStdout(name);
        }*/


        QString t_bool = res ? "ok" : "ko";
        writeStdout("{\"action\": \"terminate\", \"status\": \"" + t_bool + "\"}");
    }
    else
        writeStdout("{\"error\": \"parsing json\", \"status\": \"false\"}");



}
