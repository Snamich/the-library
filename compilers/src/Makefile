# $Id: Makefile,v 1.5 2013-09-25 13:51:12-07 - - $

GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++0x
MKDEPS     = g++ -MM -std=gnu++0x
VALGRIND   = valgrind --leak-check=full --show-reachable=yes

MKFILE     = Makefile
DEPSFILE   = Makefile.deps
SOURCES    = auxlib.cc stringset.cc astree.cc lyutils.cc symtable.cc typecheck.cc ic.cc oc.cc 
HEADERS    = auxlib.h stringset.h yyparse.h astree.h lyutils.h symtable.h typecheck.h ic.h
OBJECTS    = ${SOURCES:.cc=.o} yyparse.o yylex.o
EXECBIN    = oc
SRCFILES   = ${HEADERS} ${SOURCES} ${MKFILE}
SMALLFILES = ${DEPSFILE}
CHECKINS   = ${SRCFILES} ${SMALLFILES}

LSOURCES = scanner.l 
YSOURCES = parser.y 
CLGEN = yylex.cc
HYGEN = yyparse.h 
CYGEN = yyparse.cc 
LREPORT = yylex.output 
YREPORT = yyparse.output
REPORTS = ${LREPORT} ${YREPORT}

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o ${EXECBIN} ${OBJECTS}

${CLGEN} : ${LSOURCES}
	flex --outfile=${CLGEN} ${LSOURCES} 2>${LREPORT} 

${CYGEN} ${HYGEN} : ${YSOURCES}
	bison --defines=${HYGEN} --output=${CYGEN} ${YSOURCES}

%.o : %.cc
	${GCC} -c $<

clean :
	- rm ${OBJECTS} ${CYGEN} ${HYGEN} ${CLGEN} ${REPORTS}

spotless : clean
	- rm ${EXECBIN}

${DEPSFILE} :
	${MKDEPS} ${SOURCES} >${DEPSFILE}

deps :
	- rm ${DEPSFILE}
	${MAKE} --no-print-directory ${DEPSFILE}

include Makefile.deps
