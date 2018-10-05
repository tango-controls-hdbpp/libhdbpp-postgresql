/*
Source code file src/pgconverttango.cpp
Part of PostgreSQL backend for HDB++ 
Description: Definition of util-class for converting Tango data to libpqtypes params

Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
Licence: see LICENCE file
*/

#include "pgconverttango.h"

template class PGConvertTangoDevCommon<Tango::DevBoolean, bool>;
template class PGConvertTangoDevCommon<Tango::DevShort, int16_t>;
template class PGConvertTangoDevCommon<Tango::DevLong, int32_t>;
template class PGConvertTangoDevCommon<Tango::DevLong64, int64_t>;
template class PGConvertTangoDevCommon<Tango::DevFloat, float>;
template class PGConvertTangoDevCommon<Tango::DevDouble, double>;
template class PGConvertTangoDevCommon<Tango::DevUChar, uint8_t>;
template class PGConvertTangoDevCommon<Tango::DevUShort, uint16_t>;
template class PGConvertTangoDevCommon<Tango::DevULong, uint32_t>;
template class PGConvertTangoDevCommon<Tango::DevULong64, uint64_t>;
template class PGConvertTangoDevCommon<Tango::DevEnum, int32_t>;


template<typename T> const char *getPGTypeName(){
	return NULL;
}
template<typename T> const char *getPGArrTypeName(){
	return NULL;
}
template<> const char    *getPGTypeName<bool>(){return "%bool";}
template<> const char *getPGArrTypeName<bool>(){return "%bool[]";}
template<> const char    *getPGTypeName< int8_t>(){return "%int1";}
template<> const char *getPGArrTypeName< int8_t>(){return "%int1[]";}
template<> const char    *getPGTypeName<int16_t>(){return "%int2";}
template<> const char *getPGArrTypeName<int16_t>(){return "%int2[]";}
template<> const char    *getPGTypeName<int32_t>(){return "%int4";}
template<> const char *getPGArrTypeName<int32_t>(){return "%int4[]";}
template<> const char    *getPGTypeName<int64_t>(){return "%int8";}
template<> const char *getPGArrTypeName<int64_t>(){return "%int8[]";}
template<> const char    *getPGTypeName< uint8_t>(){return "%uint1";}
template<> const char *getPGArrTypeName< uint8_t>(){return "%uint1[]";}
template<> const char    *getPGTypeName<uint16_t>(){return "%uint2";}
template<> const char *getPGArrTypeName<uint16_t>(){return "%uint2[]";}
template<> const char    *getPGTypeName<uint32_t>(){return "%uint4";}
template<> const char *getPGArrTypeName<uint32_t>(){return "%uint4[]";}
template<> const char    *getPGTypeName<uint64_t>(){return "%uint8";}
template<> const char *getPGArrTypeName<uint64_t>(){return "%uint8[]";}
template<> const char    *getPGTypeName< float>(){return "%float4";}
template<> const char *getPGArrTypeName< float>(){return "%float4[]";}
template<> const char    *getPGTypeName<double>(){return "%float8";}
template<> const char *getPGArrTypeName<double>(){return "%float8[]";}


void PGConvertTango::storeReadData(PGparam *param){
	storeData(param, true);
}

void PGConvertTango::storeWriteData(PGparam *param){
	storeData(param, false);
}

bool PGConvertTango::isReadNull(){
	return readNull;
}

bool PGConvertTango::isWriteNull(){
	return writeNull;
}

PGConvertTango::PGConvertTango(Tango::AttrDataFormat data_format){
	isArray = data_format != Tango::SCALAR;
	readNull = true;
	writeNull = true;
}

PGConvertTango::~PGConvertTango(){
	if (isArray && !readNull){
		PQparamClear(pgarr_r->param);
		delete pgarr_r;
	}
	if (isArray && !writeNull){
		PQparamClear(pgarr_w->param);
		delete pgarr_w;
	}
}

void PGConvertTango::storeData(PGparam *param, bool isRead){
}



template <typename TangoT, typename PGT>
PGConvertTangoDevCommon<TangoT, PGT>::PGConvertTangoDevCommon(PGconn *pgconn, Tango::DeviceAttribute *attr, int write_type, Tango::AttrDataFormat data_format): PGConvertTango(data_format){
	readNull = !attr->extract_read(data_r);
	if (!isArray && data_r.size() < 1){
		readNull = true;
	}
	if (write_type != Tango::READ){
		writeNull = !attr->extract_set(data_w);
		if (!isArray && data_w.size() < 1){
			writeNull = true;
		}
	} else {
		writeNull = true;
	}

	if (isArray && !readNull){
		pgarr_r = new PGarray();
		pgarr_r->param = PQparamCreate(pgconn);
	}
	if (isArray && !writeNull){
		pgarr_w = new PGarray();
		pgarr_w->param = PQparamCreate(pgconn);
	}
}

template <typename TangoT, typename PGT>
PGConvertTangoDevCommon<TangoT, PGT>::~PGConvertTangoDevCommon(){
}

template <typename TangoT, typename PGT>
void PGConvertTangoDevCommon<TangoT, PGT>::storeData(PGparam *param, bool isRead){
	std::vector<TangoT> &data = isRead?data_r:data_w;
	PGarray *pgarr = isRead?pgarr_r:pgarr_w;
	
	if (isArray){
		const char *type = getPGTypeName<PGT>();
		const char *arrtype = getPGArrTypeName<PGT>();
		if (type == NULL || arrtype == NULL) {

		}
		for (const TangoT &val: data){
			PQputf(pgarr->param, type, (PGT)val);
		}
		PQputf(param, arrtype, pgarr);
	} else {
		const char *type = getPGTypeName<PGT>();
		if (type == NULL) {
			//TODO: make exception
		}
		PQputf(param, type, (PGT)(data[0]));
	}
}



PGConvertTangoDevString::PGConvertTangoDevString(PGconn *pgconn, Tango::DeviceAttribute *attr, int write_type, Tango::AttrDataFormat data_format): PGConvertTango(data_format){
	readNull = !attr->extract_read(data_r);
	if (!isArray && data_r.size() < 1){
		readNull = true;
	}
	if (write_type != Tango::READ){
		writeNull = !attr->extract_set(data_w);
		if (!isArray && data_w.size() < 1){
			writeNull = true;
		}
	} else {
		writeNull = true;
	}

	if (isArray && !readNull){
		pgarr_r = new PGarray();
		pgarr_r->param = PQparamCreate(pgconn);
	}
	if (isArray && !writeNull){
		pgarr_w = new PGarray();
		pgarr_w->param = PQparamCreate(pgconn);
	}
}

PGConvertTangoDevString::~PGConvertTangoDevString(){
}

void PGConvertTangoDevString::storeData(PGparam *param, bool isRead){
	std::vector<std::string> &data = isRead?data_r:data_w;
	PGarray *pgarr = isRead?pgarr_r:pgarr_w;
	
	if (isArray){
		for (const std::string &val: data){
			PQputf(pgarr->param, "%varchar", val.c_str());
		}
		PQputf(param, "%varchar[]", pgarr);
	} else {
		PQputf(param, "%varchar*", data[0].c_str());
	}
}



PGConvertTangoDevState::PGConvertTangoDevState(PGconn *pgconn, Tango::DeviceAttribute *attr, int write_type, Tango::AttrDataFormat data_format): PGConvertTango(data_format){
	if(write_type == Tango::READ && data_format == Tango::SCALAR) {
		Tango::DevState	st;
		readNull = !(*attr >> st);
		if (!readNull)
			data_r.push_back(st);
	}else{
		readNull = !attr->extract_read(data_r);
	}
	if (!isArray && data_r.size() < 1){
		readNull = true;
	}
	if (write_type != Tango::READ){
		writeNull = !attr->extract_set(data_w);
		if (!isArray && data_w.size() < 1){
			writeNull = true;
		}
	} else {
		writeNull = true;
	}

	if (isArray && !readNull){
		pgarr_r = new PGarray();
		pgarr_r->param = PQparamCreate(pgconn);
	}
	if (isArray && !writeNull){
		pgarr_w = new PGarray();
		pgarr_w->param = PQparamCreate(pgconn);
	}
}

PGConvertTangoDevState::~PGConvertTangoDevState(){
}

void PGConvertTangoDevState::storeData(PGparam *param, bool isRead){
	std::vector<Tango::DevState> &data = isRead?data_r:data_w;
	PGarray *pgarr = isRead?pgarr_r:pgarr_w;
	
	if (isArray){
		for (const Tango::DevState &val: data){
			PQputf(pgarr->param, "%int4", (int32_t)val);
		}
		PQputf(param, "%int4[]", pgarr);
	} else {
		PQputf(param, "%int4", (int32_t)(data[0]));
	}
}

