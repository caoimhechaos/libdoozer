TESTS=			
check_PROGRAMS=		${TESTS}
lib_LTLIBRARIES=	libdoozer.la

libdoozer_la_SOURCES=	msg.pb.h msg.pb.cc error.cc conn.cc baseops.cc
libdoozer_la_LDFLAGS=	-version-info ${LIBRARY_VERSION}

CLEANFILES=	msg.pb.h msg.pb.cc

msg.pb.cc msg.pb.h: msg.proto
	protoc --cpp_out=. $<
