/*-
 * Copyright (c) 2013 Caoimhe Chaos <caoimhechaos@protonmail.com>,
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
#include <QtCore/QString>
#include <QtCore/QScopedPointer>

#include <unistd.h>

#include <iostream>
#include <string>
#include <ctime>

#include <vector>
#include "doozer.h"

using doozer::Conn;
using doozer::Error;
using std::clock;
using std::string;

int main(int argc, char** argv)
{
	// Get default parameters from the environment.
	clock_t begin = clock();
	QScopedPointer<Conn> c;
	int idx;
	string doozer_uri, doozer_boot_uri;

	while ((idx = getopt(argc, argv, "a:b:")) != -1)
	{
		switch (idx)
		{
			case 'a':
				doozer_uri = string(optarg);
				break;
			case 'b':
				doozer_boot_uri = string(optarg);
				break;
			default:
				std::cerr << "Usage: check_doozer "
					"[-a <uri> [-b <boot_uri>]]"
					<< std::endl
					<< " -a <uri>: Doozer URI to "
					<< "connect to" << std::endl
					<< " -b <boot_uri>: Boot URI to "
					<< "resolve cluster names"
					<< std::endl;
		}
	}

	if (doozer_uri.length())
		c.reset(new Conn(doozer_uri, doozer_boot_uri));
	else
		c.reset(new Conn());
	Error* err;

	if (!c->IsValid())
	{
		err = c->GetError();
		std::cerr << "CRITICAL - Unable to connect to Doozer: "
			<< err->ToString() << std::endl;
		delete err;
		return 2;
	}

	err = c->Nop();
	if (err)
	{
		std::cerr << "CRITICAL - Unable to connect to Doozer: "
			<< err->ToString() << std::endl;
		delete err;
		return 2;
	}
	else
	{
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		std::cout << "OK - " << elapsed_secs << " seconds"
			<< std::endl;
	}

	return 0;
}
