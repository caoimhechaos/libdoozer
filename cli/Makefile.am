bin_PROGRAMS=		doozer-cli doozer-ping

doozer_cli_SOURCES=	add.cc del.cc get.cc nop.cc rev.cc set.cc stat.cc \
			touch.cc wait.cc watch.cc main.cc
doozer_cli_LDADD=	${top_builddir}/lib/libdoozer.la
doozer_cli_DEPENDENCIES=${top_builddir}/lib/libdoozer.la

doozer_ping_SOURCES=	ping.cc
doozer_ping_LDADD=	${top_builddir}/lib/libdoozer.la
doozer_ping_DEPENDENCIES=${top_builddir}/lib/libdoozer.la
