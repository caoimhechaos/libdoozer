/*-
 * Copyright (c) 2012 Tonnerre Lombard <tonnerre@ancient-solutions.com>,
 *                    Ancient Solutions. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions  of source code must retain  the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions  in   binary  form  must   reproduce  the  above
 *    copyright  notice, this  list  of conditions  and the  following
 *    disclaimer in the  documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS  SOFTWARE IS  PROVIDED BY  ANCIENT SOLUTIONS  AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
 * FOR A  PARTICULAR PURPOSE  ARE DISCLAIMED.  IN  NO EVENT  SHALL THE
 * FOUNDATION  OR CONTRIBUTORS  BE  LIABLE FOR  ANY DIRECT,  INDIRECT,
 * INCIDENTAL,   SPECIAL,    EXEMPLARY,   OR   CONSEQUENTIAL   DAMAGES
 * (INCLUDING, BUT NOT LIMITED  TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE,  DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT  LIABILITY,  OR  TORT  (INCLUDING NEGLIGENCE  OR  OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <arpa/inet.h>

#include <string>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>

#include <QtNetwork/QTcpSocket>

#include <google/protobuf/message.h>

#include "msg.pb.h"
#include "doozer.h"

namespace doozer {

Conn::Conn()
{
	QString uri = QProcessEnvironment::systemEnvironment()
		.value("DOOZER_URI");
	QString buri = QProcessEnvironment::systemEnvironment()
		.value("DOOZER_BOOT_URI");

	init(uri, buri);
}

Conn::Conn(std::string addr)
{
	QString qaddr = QString(addr.c_str());

	init(doozer_uri_prefix + "ca=" + qaddr, QString());
}

Conn::Conn(QString addr)
{
	init(doozer_uri_prefix + "ca=" + addr, QString());
}

Conn::Conn(std::string uri, std::string boot_uri)
{
	init(QString(uri.c_str()), QString(boot_uri.c_str()));
}

Conn::Conn(QString uri, QString boot_uri)
{
	init(uri, boot_uri);
}

Conn::~Conn()
{
	conn_->disconnectFromHost();
	conn_->waitForDisconnected();
	conn_->deleteLater();
}

void
Conn::init(QString uri, QString buri)
{
	QString host, port;
	QStringList addrs;
	QUrl p;

	error_ = 0;
	valid_ = false;
	timeout_ = 30000;

	if (!uri.startsWith(doozer_uri_prefix))
	{
		error_ = new Error(QString("Invalid URI (wrong prefix)"));
		return;
	}

	p.setEncodedQuery(uri.mid(sizeof(DOOZER_URI_PREFIX)-1).toUtf8());

	QString name = p.queryItemValue("cn");
	if (name.length() > 0 && buri.length() > 0)
	{
		init(buri, QString());
		if (!IsValid())
			return;

		// TODO(tonnerre): Do the lookup here.
	}
	else
	{
		addrs = p.allQueryItemValues("ca");
		if (addrs.isEmpty())
		{
			error_ = new Error(QString("Invalid URI (no addresses)"));
			return;
		}
	}

	int i = qrand() % addrs.length();
	int pos = addrs[i].lastIndexOf(':');
	host = addrs[i].left(pos);
	port = addrs[i].mid(pos + 1);

	conn_ = new QTcpSocket();

	conn_->connectToHost(host, port.toInt());
	if (!conn_->waitForConnected())
	{
		error_ = new Error(conn_->errorString());
		conn_->deleteLater();
		return;
	}

	QString secret = p.queryItemValue("sk");
	if (secret.length() > 0)
	{
		error_ = Access(secret);

		if (error_)
		{
			conn_->disconnectFromHost();
			conn_->waitForDisconnected();
			conn_->deleteLater();
			return;
		}
	}

	valid_ = true;
}

Error*
Conn::send(const ::google::protobuf::Message& msg)
{
	std::string msgstr = msg.SerializeAsString();
	uint32_t len = htonl(msgstr.length());
	QByteArray buf = QByteArray::fromRawData((char*) &len, 4);

	buf.push_back(msgstr.c_str());

	if (conn_->write(buf) != buf.length())
		return new Error(conn_->errorString());

	if (!conn_->waitForBytesWritten(timeout_))
		return new Error(QString("Unable to wait for the written byte: ") +
				conn_->errorString());

	return 0;
}

Error*
Conn::recv(::google::protobuf::Message* msg)
{
	if (!conn_->bytesAvailable() && !conn_->waitForReadyRead(timeout_))
		return new Error(QString("Timed out waiting for response"));
	QByteArray buf = conn_->read(4);
	uint32_t* lenp;
	uint32_t len;

	if (buf.length() != 4)
		return new Error(conn_->errorString());

	lenp = (uint32_t*) buf.data();
	len = ntohl(*lenp);
	msg->Clear();

	if (len > 0)
	{
		if (!conn_->bytesAvailable() &&
				!conn_->waitForReadyRead(timeout_))
			return new Error(QString("Timed out waiting for "
						"response"));

		buf = conn_->read(len);
		if (buf.length() != len)
			return new Error(conn_->errorString());

		if (!msg->ParseFromArray(buf.data(), buf.length()))
			return new Error(QString("Error parsing message"));
	}

	return 0;
}

void
Conn::SetTimeout(int timeout)
{
	timeout_ = timeout;
}

Error*
Conn::Access(QString token)
{
	return Access(token.toStdString());
}

Error*
Conn::Access(std::string token)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::ACCESS);
	req.set_value(token);

	err = send(req);
	if (err)
		return err;

	err = recv(&res);
	if (err)
		return err;

	if (!res.has_err_code())
		return 0;

	if (res.has_err_detail())
		return new Error(Response_Err_Name(res.err_code()) + ": " +
				res.err_detail());
	else
		return new Error(Response_Err_Name(res.err_code()));
}

Error*
Conn::GetError()
{
	return error_;
}

bool
Conn::IsValid()
{
	return valid_;
}

}  // namespace doozer
