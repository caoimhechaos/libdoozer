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

#include <QtCore/QString>
#include "doozer.h"
#include <iostream>
#include <stdlib.h>

#include "cli/cli.h"

doozer::Conn* conn;

void usage()
{
	std::cerr << "Each command takes zero or more options and zero or "
		<< "more arguments." << std::endl
		<< "In addition, there are some global options that can be "
		<< "used with any command." << std::endl
		<< "The exit status is 0 on success, 1 for a rev mismatch, "
		<< "and 2 otherwise.";

	exit(1);
}

int main(int argc, char** argv)
{
	conn = new doozer::Conn();

	if (!conn->IsValid())
	{
		doozer::Error* err = conn->GetError();
		std::cerr << err->ToString() << std::endl;
		return 1;
	}

	if (argv[1] == "add" && argc >= 3)
	{
		QString path(argv[2]);
		add(path);
	}
	else if (argv[1] == "del" && argc >= 4)
	{
		QString path(argv[2]);
		QString revstr(argv[3]);
		uint64_t rev = revstr.toLongLong();
		del(path, rev);
	}
	else if (argv[1] == "get" && argc >= 3)
	{
		QString path(argv[2]);
		get(path);
	}
	else if (argv[1] == "nop")
	{
		nop();
	}
	else if (argv[1] == "rev" && argc >= 3)
	{
		QString path(argv[2]);
		rev(path);
	}
	else if (argv[1] == "set" && argc >= 5)
	{
		QString path(argv[2]);
		QString revstr(argv[3]);
		QByteArray data(argv[4]);
		uint64_t rev = revstr.toLongLong();
		set(path, rev, data);
	}
	else if (argv[1] == "stat" && argc >= 3)
	{
		QString path(argv[2]);
		stat(path);
	}
	else if (argv[1] == "touch" && argc >= 3)
	{
		QString path(argv[2]);
		touch(path);
	}
	else if (argv[1] == "wait" && argc >= 3)
	{
		QString glob(argv[2]);
		wait(glob);
	}
	else if (argv[1] == "watch" && argc >= 3)
	{
		QString glob(argv[2]);
		watch(glob);
	}
	else
		usage();

	return 0;
}
