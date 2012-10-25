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

#include <string>
#include <QtCore/QString>

#include "msg.pb.h"
#include "doozer.h"

namespace doozer {

Event::Event()
: rev_(0), flags_(0)
{
}

Event::Event(int64_t rev, QString path, QByteArray body, uint32_t flags)
: rev_(rev), path_(path), body_(body), flags_(flags)
{
}

int64_t
Event::Rev()
{
	return rev_;
}

void
Event::Rev(int64_t newrev)
{
	rev_ = newrev;
}

QString
Event::QPath()
{
	return path_;
}

std::string
Event::Path()
{
	return path_.toStdString();
}

void
Event::QPath(QString newpath)
{
	path_ = newpath;
}

QByteArray
Event::QBody()
{
	return body_;
}

std::string
Event::Body()
{
	return std::string(body_.data());
}

void
Event::QBody(QByteArray newbody)
{
	body_ = newbody;
}

uint32_t
Event::Flags()
{
	return flags_;
}

void
Event::Flags(uint32_t newflags)
{
	flags_ = newflags;
}

Error*
Conn::Wait(QString glob, int64_t rev, doozer::Event* ev)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::WAIT);
	req.set_path(glob.toStdString());
	req.set_rev(rev);

	err = send(req);
	if (err)
		return err;

	err = recv(&res);
	if (err)
		return err;

	if (!res.has_err_code())
	{
		ev->Rev(res.rev());
		ev->QPath(QString(res.path().c_str()));
		ev->QBody(QByteArray(res.value().c_str(),
					res.value().length()));
		ev->Flags(res.flags());
		return 0;
	}

	if (res.has_err_detail())
		return new Error(Response_Err_Name(res.err_code()) + ": " +
				res.err_detail());
	else
		return new Error(Response_Err_Name(res.err_code()));
}

Error*
Conn::Wait(std::string glob, int64_t rev, doozer::Event* ev)
{
	return Wait(QString(glob.c_str()), rev, ev);
}

}  // namespace doozer
