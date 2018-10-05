# liphdbpp-postgres

Library for HDB++ implementing PostgreSQL schema

## Version

The current release version is 1.0.0

## Building

### Dependencies

Ensure the development version of the dependencies are installed. These are as follows:

* HDB++ library libhdbpp
* Tango Controls 9 or higher - either via debian package or source install.
* omniORB release 4 - libomniorb4 and libomnithread.
* libzmq - libzmq3-dev or libzmq5-dev.
* libpq - libpq-dev
* libpqtypes - libpqtypes-dev
* PostgreSQL Server 8 or higher for database server
* pguint extension fork with binary mode (https://github.com/proger10/pguint) for PostgreSQL Server 

### Build Flags

There are a set of library and include variables that can be set to inform the build of various dependencies. The flags are only required if you have installed a dependency in a non-standard location. 

| Flag | Notes |
|------|-------|
| TANGO_INC | Tango include files directory |
| TANGO_LIB | Tango lib files directory |
| OMNIORB_INC | Omniorb include files directory |
| OMNIORB_LIB | Omniorb lib files directory |
| ZMQ_INC | ZMQ include files directory |
| LIBHDBPP_INC | Libhdb++ include files directory |
| LIBHDBPP_LIB | Libhdb++ lib files directory |

### Build

Get the source.
Set appropriate flags in the environment (or pass them to make) if required, then:

```bash
make
```

## Installation

Once built simply run `make install`. The install can be passed a PREFIX variable, this is set to /usr/local by default. It is also possible to use DESTDIR. Install path is constructed as DESTDIR/PREFIX.

#### Building Against Tango Controls 9.2.5a

**The debian package and source install place the headers under /usr/include/tango, so its likely you will need to set TANGO_INC=/usr/include/tango or TANGO_INC=/usr/local/include/tango, depending on your install method.**

## DB Schema

To create the DB schema use `etc/schema.sql` file

## Usage

To use this library with HDB++ you must specify it as libname in LibConfiguration property of HDB++ Configuration Manager and HDB++ Event Subscriper. Also you must specify database connetion parameters: host, port, username. password and database name.
LibConfiguration should looks like:

```
libname=<path to libhdb++postgres.so>
host=<hostname or ip-address of PostgreSQL Server>
port=<port of PostgreSQL Server>
user=<username>
password=<password>
dbname=<database name>
```


## License

The code is released under the LGPL3 license and a copy of this license is provided with the code. 
