# Makefile
# Part of PostgreSQL backend for HDB++
# Description: Makefile for PostgreSQL backend for HDB++
# 
# Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
# Licence: see LICENCE file

PREFIX=/usr/local

DBIMPL_INC = -I`pg_config --includedir`
DBIMPL_LIB = -L`pg_config --libdir` -lpq -lpqtypes

ifdef TANGO_INC
	INC_DIR += -I${TANGO_INC}
endif

ifdef TANGO_LIB
	LIB_DIR	+= -L${TANGO_LIB}
endif

ifdef OMNIORB_INC
	INC_DIR	+= -I${OMNIORB_INC}
endif

ifdef OMNIORB_LIB
	LIB_DIR	+= -L${OMNIORB_LIB}
endif

ifdef ZMQ_INC
	INC_DIR += -I${ZMQ_INC}
endif

ifdef LIBHDBPP_INC
	INC_DIR += -I${LIBHDBPP_INC}
endif

CXXFLAGS += -std=gnu++0x -Wall -DRELEASE='"$HeadURL$ "' $(DBIMPL_INC) $(INC_DIR)
LDFLAGS += $(LIB_DIR)

##############################################
# support for shared libray versioning
#
LFLAGS_SONAME = $(DBIMPL_LIB) $(LDFLAGS) -Wl,-soname,
SHLDFLAGS = -shared
BASELIBNAME       =  libhdb++postgres
SHLIB_SUFFIX = so

#  release numbers for libraries
#
 LIBVERSION    = 1
 LIBRELEASE    = 0
 LIBSUBRELEASE = 0
#

LIBRARY       = $(BASELIBNAME).a
DT_SONAME     = $(BASELIBNAME).$(SHLIB_SUFFIX).$(LIBVERSION)
DT_SHLIB      = $(BASELIBNAME).$(SHLIB_SUFFIX).$(LIBVERSION).$(LIBRELEASE).$(LIBSUBRELEASE)
SHLIB         = $(BASELIBNAME).$(SHLIB_SUFFIX)

.PHONY : install clean

lib/LibHdb++postgres: lib obj obj/LibHdb++postgres.o obj/pgtypesuint.o obj/pgconverttango.o
	$(CXX) obj/LibHdb++postgres.o obj/pgtypesuint.o obj/pgconverttango.o $(SHLDFLAGS) $(LFLAGS_SONAME)$(DT_SONAME) -o lib/$(DT_SHLIB)
	ln -sf $(DT_SHLIB) lib/$(SHLIB)
	ln -sf $(SHLIB) lib/$(DT_SONAME)
	ar rcs lib/$(LIBRARY) obj/LibHdb++postgres.o

obj/LibHdb++postgres.o: src/LibHdb++postgres.cpp src/LibHdb++postgres.h
	$(CXX) $(CXXFLAGS) -fPIC -c src/LibHdb++postgres.cpp -o $@

obj/pgtypesuint.o: src/pgtypesuint.c src/pgtypesuint.h
	$(CXX) $(CXXFLAGS) -fPIC -c src/pgtypesuint.c -o $@

obj/pgconverttango.o: src/pgconverttango.cpp src/pgconverttango.h
	$(CXX) $(CXXFLAGS) -fPIC -c src/pgconverttango.cpp -o $@

clean:
	rm -f obj/*.o lib/*.so* lib/*.a

lib obj:
	@mkdir $@
	
install:
	install -d ${DESTDIR}${PREFIX}/lib
	install -d ${DESTDIR}${PREFIX}/share/libhdb++postgres
	install -m 755 lib/libhdb++postgres.so.${LIBVERSION}.${LIBRELEASE}.${LIBSUBRELEASE} ${DESTDIR}${PREFIX}/lib	
	rm -f ${DESTDIR}${PREFIX}/lib/libhdb++postgres.so.${LIBVERSION}
	rm -f ${DESTDIR}${PREFIX}/lib/libhdb++postgres.so
	ln -s ${DESTDIR}${PREFIX}/lib/libhdb++postgres.so.${LIBVERSION}.${LIBRELEASE}.${LIBSUBRELEASE} ${DESTDIR}${PREFIX}/lib/libhdb++postgres.so.${LIBVERSION}
	ln -s ${DESTDIR}${PREFIX}/lib/libhdb++postgres.so.${LIBVERSION}.${LIBRELEASE}.${LIBSUBRELEASE} ${DESTDIR}${PREFIX}/lib/libhdb++postgres.so
#	install -m 644 etc/add_devenum__hdb++_postgres.sql ${DESTDIR}${PREFIX}/share/libhdb++postgres
#	install -m 644 etc/create_hdb++_postgres_delete_attr_procedure.sql ${DESTDIR}${PREFIX}/share/libhdb++postgres
#	install -m 644 etc/create_hdb++_postgres.sql ${DESTDIR}${PREFIX}/share/libhdb++postgres