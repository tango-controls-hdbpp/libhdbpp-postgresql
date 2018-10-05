/*
Source code file src/LibHdb++postgres.h
Part of PostgreSQL backend for HDB++ 
Description: Declaration of HdbPPpostgres class

Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
Licence: see LICENCE file
*/

#ifndef _HDBPP_POSTGRES_H
#define _HDBPP_POSTGRES_H

#include <libhdb++/LibHdb++.h>

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <stdint.h>
#include <unordered_map>
#include <typeindex>
//Tango:
#include <tango.h>

// TODO: UNDEFINE IN HEADER!!!!!!!! IT IS NOT OK!!!!!!
// omg, tango defines CONNECTION_OK, just as libpq do. but I don't need one from tango. so I can undefine it before including libpq
#undef CONNECTION_OK
#include <libpq-fe.h>
#include <libpqtypes.h>
//#include <event.h>
#include "pgconverttango.h"

struct PGType {
	std::string pg_type;
	unsigned int size;

};

class HdbPPpostgres: public AbstractDB {
private:
	PGconn *pgconn;
	bool lightschema;	//without recv_time and insert_time
	bool autodetectschema;
	std::map<std::string, uint32_t> attr_ID_map;
	std::map<std::string, bool> table_column_map;
	std::map<std::string, uint32_t> attr_ERR_ID_map;
	std::queue<std::string> attr_ERR_queue;
	std::unordered_map<std::type_index, std::string> type_map;

	void string_vector2map(const std::vector<std::string> &str, const std::string &separator, std::map<std::string, std::string> &results);
	void string_explode(const std::string &exstr, const std::string &separator, std::vector<std::string> &results);
	std::string get_only_tango_host(const std::string &str);
	int find_attr_id(const std::string &facility, const std::string &attr, uint32_t &ID);
	std::string get_only_attr_name(const std::string &str);
	int find_last_event(uint32_t ID, std::string &event);
	std::string get_table_name(int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/);
	std::string get_data_type(int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/);
	int find_attr_id_type(const std::string &facility, const std::string &attr, uint32_t &ID, const std::string &attr_type, unsigned int &conf_ttl);
	void event_Attr(uint32_t id, const std::string &event);
	bool autodetect_column(int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/, const std::string &column_name);
	int cache_err_id(const std::string &error_desc, uint32_t &ERR_ID);
	int insert_error(const std::string &error_desc, uint32_t &ERR_ID);
	int find_err_id(const std::string &err, uint32_t &ERR_ID);
	void store_scalar(const std::string &attr, int quality/*ATTR_VALID, ATTR_INVALID, ..*/, const std::string &error_desc, double ev_time, double rcv_time, const std::string &table_name, PGConvertTango *conv);
	void store_array(const std::string &attr, int quality/*ATTR_VALID, ATTR_INVALID, ..*/, const std::string &error_desc, Tango::AttributeDimension attr_r_dim, Tango::AttributeDimension attr_w_dim, double ev_time, double rcv_time, const std::string &table_name, PGConvertTango *conv);
#ifndef _MULTI_TANGO_HOST
	std::string remove_domain(const std::string &facility);
	std::string add_domain(const std::string &facility);
#endif


public:
	HdbPPpostgres(vector<string> configuration);
	~HdbPPpostgres();

	//seems to be done:
	virtual void updateTTL_Attr(std::string name, unsigned int ttl/*hours, 0=infinity*/);
	virtual void event_Attr(std::string name, unsigned char event);
	virtual void insert_param_Attr(Tango::AttrConfEventData *data, HdbEventDataType ev_data_type);
	virtual void configure_Attr(std::string name, int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/, unsigned int ttl/*hours, 0=infinity*/);
	//seems to be done, but to be expanded:
	virtual void insert_Attr(Tango::EventData *data, HdbEventDataType ev_data_type);
	//unimplemented:
	//HA-HA!! Nothing!
};

class HdbPPpostgresFactory : public DBFactory {
public:
	virtual AbstractDB* create_db(std::vector<std::string> configuration);
};

#endif