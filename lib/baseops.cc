/*-
 * Copyright (c) 2012 Caoimhe Chaos <caoimhechaos@protonmail.com>,
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
#include <QtCore/QString>

#include "msg.pb.h"
#include "doozer.h"

namespace doozer {

Error*
Conn::Set(QString file, int64_t oldRev, int64_t* newRev, QByteArray body)
{
	return Set(file.toStdString(), oldRev, newRev, body.data(),
			body.length());
}

Error*
Conn::Set(std::string file, int64_t oldRev, int64_t* newRev, const char *body,
		size_t len)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::SET);
	req.set_path(file);
	req.set_value(body, len);
	req.set_rev(oldRev);

	err = send(req);
	if (err)
		return err;

	err = recv(&res);
	if (err)
		return err;

	if (!res.has_err_code())
	{
		if (newRev)
			*newRev = res.rev();
		return 0;
	}

	if (res.has_err_detail())
		return new Error(Response_Err_Name(res.err_code()) + ": " +
				res.err_detail());
	else
		return new Error(Response_Err_Name(res.err_code()));
}

Error*
Conn::Del(QString file, int64_t rev)
{
	return Del(file.toStdString(), rev);
}

Error*
Conn::Del(std::string file, int64_t rev)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::DEL);
	req.set_path(file);
	req.set_rev(rev);

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
Conn::Nop()
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::NOP);

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
Conn::Get(QString file, int64_t* storerev, QByteArray* buf, int64_t* filerev)
{
	Error* err;
	std::string res;

	err = Get(file.toStdString(), storerev, &res, filerev);

	if (err == 0)
	{
		buf->clear();
		buf->append(res.c_str(), res.length());
	}

	return err;
}

Error*
Conn::Get(std::string file, int64_t* storerev, std::string* buf, int64_t* filerev)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::GET);
	req.set_path(file);
	if (storerev)
		req.set_rev(*storerev);

	err = send(req);
	if (err)
		return err;

	err = recv(&res);
	if (err)
		return err;

	if (!res.has_err_code())
	{
		if (buf)
			*buf = res.value();

		if (filerev)
			*filerev = res.rev();

		return 0;
	}

	if (res.has_err_detail())
		return new Error(Response_Err_Name(res.err_code()) + ": " +
				res.err_detail());
	else
		return new Error(Response_Err_Name(res.err_code()));
}

Error*
Conn::Stat(QString path, int64_t* storerev, int* len, int64_t* filerev)
{
	return Stat(path.toStdString(), storerev, len, filerev);
}

Error*
Conn::Stat(std::string path, int64_t* storerev, int* len, int64_t* filerev)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::STAT);
	req.set_path(path);

	if (storerev)
		req.set_rev(*storerev);

	err = send(req);
	if (err)
		return err;

	err = recv(&res);
	if (err)
		return err;

	if (!res.has_err_code())
	{
		if (len)
			*len = res.len();

		if (filerev)
			*filerev = res.rev();

		return 0;
	}

	if (res.has_err_detail())
		return new Error(Response_Err_Name(res.err_code()) + ": " +
				res.err_detail());
	else
		return new Error(Response_Err_Name(res.err_code()));
}

Error*
Conn::Rev(int64_t* rev)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::REV);

	err = send(req);
	if (err)
		return err;

	err = recv(&res);
	if (err)
		return err;

	if (!res.has_err_code())
	{
		if (rev)
			*rev = res.rev();

		return 0;
	}

	if (res.has_err_detail())
		return new Error(Response_Err_Name(res.err_code()) + ": " +
				res.err_detail());
	else
		return new Error(Response_Err_Name(res.err_code()));
}

}  // namespace doozer
