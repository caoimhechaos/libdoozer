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
#include <QtCore/QString>
#include <QtCore/QVector>

#include <libgen.h>
#include <string.h>

#include "msg.pb.h"
#include "doozer.h"

namespace doozer {

#define MISSING		(-1)
#define CLOBBER		(-2)
#define DIRECTORY	(-3)
#define NOP		(-4)


FileInfo::FileInfo()
: len_(0), rev_(0), isset_(false), isdir_(false)
{
}

FileInfo::FileInfo(QString name, int len, int64_t rev, bool is_set,
		bool is_dir)
: name_(name), len_(len), rev_(rev), isset_(is_set), isdir_(is_dir)
{
}

FileInfo::~FileInfo()
{
}

std::string
FileInfo::Name()
{
	return name_.toStdString();
}

QString
FileInfo::QName()
{
	return name_;
}

void
FileInfo::QName(QString newname)
{
	name_ = newname;
}

int
FileInfo::Len()
{
	return len_;
}

void
FileInfo::Len(int newlen)
{
	len_ = newlen;
}

int64_t
FileInfo::Rev()
{
	return rev_;
}

void
FileInfo::Rev(int64_t newrev)
{
	rev_ = newrev;
}

bool
FileInfo::IsSet()
{
	return isset_;
}

void
FileInfo::IsSet(bool newset)
{
	isset_ = newset;
}

bool
FileInfo::IsDir()
{
	return isdir_;
}

void
FileInfo::IsDir(bool newdir)
{
	isdir_ = newdir;
}

Error*
Conn::Getdir(QString dir, int64_t rev, int32_t off, int lim,
		QVector<QString>* names)
{
	Error* err;
	Request req;
	Response res;

	req.set_verb(Request::GETDIR);
	req.set_path(dir.toStdString());
	req.set_rev(rev);

	names->clear();

	for (; lim; lim--, off++)
	{
		req.set_offset(off);

		err = send(req);
		if (err)
			return err;

		err = recv(&res);
		if (err)
			return err;

		if (res.has_err_code())
		{
			if (res.has_err_detail())
				return new Error(
					Response_Err_Name(res.err_code())
					+ ": " + res.err_detail());
			else
				return new Error(
					Response_Err_Name(res.err_code()));
		}

		names->push_back(QString(res.path().c_str()));
	}

	return 0;
}

Error*
Conn::Getdir(std::string dir, int64_t rev, int32_t off, int lim,
		std::vector<std::string>* names)
{
	QVector<QString> qres;
	Error* err = Getdir(QString(dir.c_str()), rev, off, lim, &qres);

	if (err)
		return err;

	names->clear();

	for (QString it : qres)
		names->push_back(it.toStdString());

	return 0;
}

Error*
Conn::Statinfo(int64_t rev, std::string path, FileInfo** info)
{
	return Statinfo(rev, QString(path.c_str()), info);
}

Error*
Conn::Statinfo(int64_t rev, QString path, FileInfo** info)
{
	Error* err;
	int len, slashpos;
	int64_t filerev;
	QString shortname;

	if (path.endsWith('/'))
		path = path.left(path.length() - 1);

	slashpos = path.lastIndexOf('/');
	if (slashpos > 0)
		shortname = path.right(path.length() - slashpos);
	else
		shortname = path;

	err = Stat(path, &rev, &len, &filerev);
	if (err)
		return err;

	*info = new FileInfo(shortname, len, filerev, true,
			(filerev == DIRECTORY));
	return 0;
}

Error*
Conn::Getdirinfo(QString dir, int64_t rev, int32_t off, int lim,
		QVector<FileInfo>* info)
{
	QVector<QString> names;
	Error* err = Getdir(dir, rev, off, lim, &names);

	if (err)
		return err;

	if (!dir.endsWith('/'))
		dir += "/";

	info->clear();

	for (QString name : names)
	{
		FileInfo* fp = 0;

		err = Statinfo(rev, dir + name, &fp);
		if (err && !fp)
			fp = new FileInfo(name, 0, 0, false, false);
		info->push_back(*fp);
		delete fp;
	}

	return 0;
}

Error*
Conn::Getdirinfo(std::string dir, int64_t rev, int32_t off, int lim,
		std::vector<FileInfo>* info)
{
	QVector<FileInfo> qinfo;
	QString qdir(dir.c_str());
	Error* err = Getdirinfo(qdir, rev, off, lim, &qinfo);

	if (err)
		return err;

	info->clear();

	for (FileInfo it : qinfo)
		info->push_back(it);

	return 0;
}

}  // namespace doozer
