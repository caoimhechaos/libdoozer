TESTS=			
check_PROGRAMS=		${TESTS}
lib_LTLIBRARIES=	libdoozer.la
noinst_PROGRAMS=	ping

libdoozer_la_SOURCES=	msg.pb.h msg.pb.cc error.cc conn.cc baseops.cc	\
			dirops.cc wait.cc
libdoozer_la_LDFLAGS=	-version-info ${LIBRARY_VERSION}

ping_SOURCES=		ping.c
ping_LDADD=		${lib_LTLIBRARIES} @QT_LIBS@

CLEANFILES=	msg.pb.h msg.pb.cc

msg.pb.cc msg.pb.h: msg.proto
	protoc --cpp_out=. $<
