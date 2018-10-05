/*
Source code file src/pgconverttango.h
Part of PostgreSQL backend for HDB++ 
Description: Declaration of util-class for converting Tango data to libpqtypes params

Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
Licence: see LICENCE file
*/

#ifndef _PG_CONVERT_TANGO_H
#define _PG_CONVERT_TANGO_H

#include <libpqtypes.h>
#include <vector>
#include <tango.h>

class PGConvertTango {
private:
	virtual void storeData(PGparam *param, bool isRead);
protected:
	bool readNull;
	bool writeNull;
	bool isArray;
	PGarray *pgarr_r;
	PGarray *pgarr_w;
public:
	PGConvertTango(Tango::AttrDataFormat data_format);
	virtual ~PGConvertTango();
	void storeReadData(PGparam *param);
	void storeWriteData(PGparam *param);
	bool isReadNull();
	bool isWriteNull();
};


/*
Tango::DevBoolean
Tango::DevShort
Tango::DevLong
Tango::DevLong64
Tango::DevFloat
Tango::DevDouble
Tango::DevUChar
Tango::DevUShort
Tango::DevULong
Tango::DevULong64
Tango::DevString
Tango::DevState
Tango::DevEncoded
*/

template <typename TangoT, typename PGT>
class PGConvertTangoDevCommon: public PGConvertTango {
private:
	std::vector<TangoT> data_r;
	std::vector<TangoT> data_w;
	void storeData(PGparam *param, bool isRead);
public:
	PGConvertTangoDevCommon(PGconn *pgconn, Tango::DeviceAttribute *attr, int write_type, Tango::AttrDataFormat data_format);
	~PGConvertTangoDevCommon();
};

class PGConvertTangoDevString: public PGConvertTango {
private:
	std::vector<std::string> data_r;
	std::vector<std::string> data_w;
	void storeData(PGparam *param, bool isRead);
public:
	PGConvertTangoDevString(PGconn *pgconn, Tango::DeviceAttribute *attr, int write_type, Tango::AttrDataFormat data_format);
	~PGConvertTangoDevString();
};

class PGConvertTangoDevState: public PGConvertTango {
private:
	std::vector<Tango::DevState> data_r;
	std::vector<Tango::DevState> data_w;
	void storeData(PGparam *param, bool isRead);
public:
	PGConvertTangoDevState(PGconn *pgconn, Tango::DeviceAttribute *attr, int write_type, Tango::AttrDataFormat data_format);
	~PGConvertTangoDevState();
};

typedef PGConvertTangoDevCommon<Tango::DevBoolean, bool> PGConvertTangoDevBoolean;
typedef PGConvertTangoDevCommon<Tango::DevShort, int16_t> PGConvertTangoDevShort;
typedef PGConvertTangoDevCommon<Tango::DevLong, int32_t> PGConvertTangoDevLong;
typedef PGConvertTangoDevCommon<Tango::DevLong64, int64_t> PGConvertTangoDevLong64;
typedef PGConvertTangoDevCommon<Tango::DevFloat, float> PGConvertTangoDevFloat;
typedef PGConvertTangoDevCommon<Tango::DevDouble, double> PGConvertTangoDevDouble;
typedef PGConvertTangoDevCommon<Tango::DevUChar, uint8_t> PGConvertTangoDevUChar;
typedef PGConvertTangoDevCommon<Tango::DevUShort, uint16_t> PGConvertTangoDevUShort;
typedef PGConvertTangoDevCommon<Tango::DevULong, uint32_t> PGConvertTangoDevULong;
typedef PGConvertTangoDevCommon<Tango::DevULong64, uint64_t> PGConvertTangoDevULong64;
typedef PGConvertTangoDevCommon<Tango::DevEnum, int32_t> PGConvertTangoDevEnum;
#endif