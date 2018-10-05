/*
Source code file src/LibHdb++postgres.cpp
Part of PostgreSQL backend for HDB++ 
Description: Definitions of HdbPPpostgres class methods

Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
Licence: see LICENCE file
*/

// #define _LIB_DEBUG

#include "LibHdb++postgres.h"
#include "LibHdb++postgres_database.h"
#include "pgtypesuint.h"
#include "pgconverttango.h"
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <netdb.h> //for getaddrinfo

#include <libpq-fe.h>
#include <libpqtypes.h>

#ifndef LIB_BUILDTIME
#define LIB_BUILDTIME   RELEASE " " __DATE__ " "  __TIME__
#endif

#define DB_ERROR		"Database Error"
#define CONFIG_ERROR	"Configuration Error"
#define QUERY_ERROR		"Query Error"
#define DATA_ERROR		"Data Error"

#define ERR_MAP_MAX_SIZE				50000

const char version_string[] = "$Build: " LIB_BUILDTIME " $";
static const char __FILE__rev[] = __FILE__ " $Id: $";


HdbPPpostgres::HdbPPpostgres(vector<string> configuration) {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
	cout << "configuration begin"<< endl;
	for (string s: configuration) {
		cout << s << endl;
	}
	cout << "configuration end"<< endl;
#endif

	std::map<std::string, std::string> db_conf;
	string_vector2map(configuration, "=", db_conf);
	string host, user, password, dbname;
	int port;
	try {
		host = db_conf.at("host");
		user = db_conf.at("user");
		password = db_conf.at("password");
		dbname = db_conf.at("dbname");
		port = atoi(db_conf.at("port").c_str());
	} catch(const std::out_of_range& e) {
		stringstream tmp;
		tmp << "Configuration parsing error: " << e.what();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(CONFIG_ERROR,tmp.str(),__func__);
	}
	string connect_str = "postgresql://"+user+":"+password+"@"+host+":"+std::to_string(port)+"/"+dbname;
	pgconn = PQconnectdb(connect_str.c_str());
	if(PQstatus(pgconn) != CONNECTION_OK) {
		char *err_msg = PQerrorMessage(pgconn);
		#ifdef _LIB_DEBUG
		cout << "Connection was unsuccessful..." << endl
			 << "Error message returned: " << PQerrorMessage(pgconn) << endl;
		#endif

		PQfinish(pgconn);
		// TODO: exception
		Tango::Except::throw_exception(DB_ERROR, err_msg, __func__);
	}
	#ifdef _LIB_DEBUG
	cout << "Connection ok!" << endl;
	#endif
	PQinitTypes(pgconn);

	lightschema = false;
	autodetectschema = false;
	try {
		int ilightschema;
		ilightschema = atoi(db_conf.at("lightschema").c_str());
		lightschema = (ilightschema == 1);
	} catch(const std::out_of_range& e) {
#ifdef _LIB_DEBUG
		cout << __func__<< ": lightschema key not found" << endl;
#endif
		autodetectschema = true;
	}


	
	std::vector<Tango::CmdArgType> v_type;/*DEV_DOUBLE, DEV_STRING, ..*/
	std::vector<Tango::AttrDataFormat> v_format;/*SCALAR, SPECTRUM, ..*/
	std::vector<Tango::AttrWriteType> v_write_type;/*READ, READ_WRITE, ..*/
	v_type.push_back(Tango::DEV_BOOLEAN);
	v_type.push_back(Tango::DEV_UCHAR);
	v_type.push_back(Tango::DEV_SHORT);
	v_type.push_back(Tango::DEV_USHORT);
	v_type.push_back(Tango::DEV_LONG);
	v_type.push_back(Tango::DEV_ULONG);
	v_type.push_back(Tango::DEV_LONG64);
	v_type.push_back(Tango::DEV_ULONG64);
	v_type.push_back(Tango::DEV_FLOAT);
	v_type.push_back(Tango::DEV_DOUBLE);
	v_type.push_back(Tango::DEV_STRING);
	v_type.push_back(Tango::DEV_STATE);
	v_type.push_back(Tango::DEV_ENUM);
	v_type.push_back(Tango::DEV_ENCODED);

	v_format.push_back(Tango::SCALAR);
	v_format.push_back(Tango::SPECTRUM);
	// v_format.push_back(Tango::IMAGE);

	v_write_type.push_back(Tango::READ);
	v_write_type.push_back(Tango::READ_WRITE);

	if(autodetectschema) {
		for(vector<Tango::CmdArgType>::iterator it_type=v_type.begin(); it_type!=v_type.end(); it_type++) {
			for(vector<Tango::AttrDataFormat>::iterator it_format=v_format.begin(); it_format!=v_format.end(); it_format++) {
				for(vector<Tango::AttrWriteType>::iterator it_write_type=v_write_type.begin(); it_write_type!=v_write_type.end(); it_write_type++) {
					if(*it_format == Tango::SCALAR) {
						bool detected=autodetect_column(*it_type, *it_format, *it_write_type, SC_COL_INS_TIME);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+SC_COL_INS_TIME, detected));

						detected=autodetect_column(*it_type, *it_format, *it_write_type, SC_COL_RCV_TIME);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+SC_COL_RCV_TIME, detected));

						detected=autodetect_column(*it_type, *it_format, *it_write_type, SC_COL_QUALITY);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+SC_COL_QUALITY, detected));

						detected=autodetect_column(*it_type, *it_format, *it_write_type, SC_COL_ERROR_DESC_ID);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+SC_COL_ERROR_DESC_ID, detected));
					} else {
						bool detected=autodetect_column(*it_type, *it_format, *it_write_type, ARR_COL_INS_TIME);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+ARR_COL_INS_TIME, detected));

						detected=autodetect_column(*it_type, *it_format, *it_write_type, ARR_COL_RCV_TIME);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+ARR_COL_RCV_TIME, detected));

						detected=autodetect_column(*it_type, *it_format, *it_write_type, ARR_COL_QUALITY);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+ARR_COL_QUALITY, detected));

						detected=autodetect_column(*it_type, *it_format, *it_write_type, ARR_COL_ERROR_DESC_ID);
						table_column_map.insert(make_pair(get_table_name(*it_type, *it_format, *it_write_type)+"_"+ARR_COL_ERROR_DESC_ID, detected));
					}
				}
			}
		}
	}

	PGregisterType types[] = {{"uint1", uint1_put, uint1_get}, {"uint2", uint2_put, uint2_get}, {"uint4", uint4_put, uint4_get}, {"uint8", uint8_put, uint8_get}, {"int1", int1_put, int1_get}};
	PQregisterTypes(pgconn, PQT_USERDEFINED, types, 5, 0);


}

HdbPPpostgres::~HdbPPpostgres() {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
#endif
	PQclearTypes(pgconn);
	PQfinish(pgconn);
}

void HdbPPpostgres::insert_Attr(Tango::EventData *data, HdbEventDataType ev_data_type) {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
#endif
	std::string attr_name = data->attr_name;
	double ev_time;
	double rcv_time = data->get_date().tv_sec + (double)data->get_date().tv_usec/1.0e6;
	int quality = (int)data->attr_value->get_quality();
	string error_desc("");
#ifdef _LIB_DEBUG
	cout << __func__<< ": entering quality="<<quality << endl;
#endif


	Tango::AttributeDimension attr_w_dim;
	Tango::AttributeDimension attr_r_dim;
	int data_type = ev_data_type.data_type; //data->attr_value->get_type() // char, long, ulong
	Tango::AttrDataFormat data_format = ev_data_type.data_format; // SCALAR, SPECTRUM, IMAGE
	int write_type = ev_data_type.write_type; // READ, READ_WRITE

	ev_time = data->attr_value->get_date().tv_sec + (double)data->attr_value->get_date().tv_usec/1.0e6;
	bool isNull = false;
	data->attr_value->reset_exceptions(Tango::DeviceAttribute::isempty_flag); //disable is_empty exception
	if(data->err || data->attr_value->is_empty()/* || data->attr_value->get_quality() == Tango::ATTR_INVALID */) {
#ifdef _LIB_DEBUG
		cout << __func__<< ": going to archive as NULL..." << endl;
#endif
		isNull = true;
		if(data->err) {
			error_desc = data->errors[0].desc;
			ev_time = rcv_time;
		}
	}
#ifdef _LIB_DEBUG
	cout << __func__<< ": data_type="<<data_type<<" data_format="<<data_format<<" write_type="<<write_type << endl;
#endif
	if(!isNull) {
		attr_w_dim = data->attr_value->get_w_dimension();
		attr_r_dim = data->attr_value->get_r_dimension();
	}else{
		attr_r_dim.dim_x = 0;//max_dim_x;//TODO: OK?
		attr_w_dim.dim_x = 0;//max_dim_x;//TODO: OK?
		attr_r_dim.dim_y = 0;//max_dim_y;//TODO: OK?
		attr_w_dim.dim_y = 0;//max_dim_y;//TODO: OK?
	}
	//TODO: what?? 
	if(ev_time < 1)
		ev_time=1;
	if(rcv_time < 1)
		rcv_time=1;

	std::string table_name = get_table_name(data_type, data_format, write_type);

	PGConvertTango *conv = NULL;
	if (isNull){
		conv = new PGConvertTango(data_format);
	}else{
		switch(data_type) {
			case Tango::DEV_BOOLEAN: {
				conv = new PGConvertTangoDevBoolean(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_SHORT: {
				conv = new PGConvertTangoDevShort(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_LONG: {
				conv = new PGConvertTangoDevLong(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_LONG64: {
				conv = new PGConvertTangoDevLong64(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_FLOAT: {
				conv = new PGConvertTangoDevFloat(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_DOUBLE: {
				conv = new PGConvertTangoDevDouble(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_UCHAR: {
				conv = new PGConvertTangoDevUChar(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_USHORT: {
				conv = new PGConvertTangoDevUShort(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_ULONG: {
				conv = new PGConvertTangoDevULong(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_ULONG64: {
				conv = new PGConvertTangoDevULong64(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_STRING: {
				conv = new PGConvertTangoDevString(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_STATE: {
				conv = new PGConvertTangoDevState(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			case Tango::DEV_ENUM: {
				conv = new PGConvertTangoDevEnum(pgconn, data->attr_value, write_type, data_format);
				break;
			}
			// case Tango::DEV_ENCODED: {
			// 	conv = new PGConvertTangoDevEncoded(pgconn, data->attr_value, write_type, data_format);
			// 	break;
			// }
			default: {
				TangoSys_MemStream	os;
				os << "Attribute " << data->attr_name<< " type (" << data_type << "<->" << (int)(data->attr_value->get_type()) << ") not supported";
				cout << __func__<<": " << os.str() << endl;
				Tango::Except::throw_exception(DATA_ERROR,os.str(),__func__);
			}
		}
	}
	if (data_format == Tango::SCALAR){
		store_scalar(attr_name, quality, error_desc, ev_time, rcv_time, table_name, conv);
	}else{
		store_array(attr_name, quality, error_desc, attr_r_dim, attr_w_dim, ev_time, rcv_time, table_name, conv);
	}
	delete conv;
#ifdef _LIB_DEBUG
//	cout << __func__<< ": exiting... ret="<<ret << endl;
#endif

}

void HdbPPpostgres::insert_param_Attr(Tango::AttrConfEventData *data, HdbEventDataType ev_data_type) {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
#endif

	std::string attr = data->attr_name;
	double	ev_time = data->get_date().tv_sec + (double)data->get_date().tv_usec/1.0e6;
	std::string error_desc("");

	std::map<std::string, uint32_t>::iterator it = attr_ID_map.find(attr);
	//if not already present in cache, look for ID in the DB
	if(it == attr_ID_map.end()) {
		uint32_t ID;
		std::string facility = get_only_tango_host(attr);
		std::string attr_name = get_only_attr_name(attr);
		if(find_attr_id(facility, attr_name, ID) == 0){
			attr_ID_map.insert(make_pair(attr,ID));
			it = attr_ID_map.find(attr);
		} else {
			cout << __func__<< ": ID not found!" << endl;
			Tango::Except::throw_exception(DATA_ERROR,"ID not found",__func__);
		}
	}
	if(it == attr_ID_map.end()) { // TODO: seems to be redundant. but not wrong, ok
		cout << __func__<< ": ID not found for attr="<<attr << endl;
		Tango::Except::throw_exception(DATA_ERROR,"ID not found",__func__);
	}
	uint32_t ID=it->second;
	std::ostringstream query_str;

	query_str << "INSERT INTO " << PARAM_TABLE_NAME << " (" << 
			PARAM_COL_ID << "," << PARAM_COL_INS_TIME << "," << PARAM_COL_EV_TIME << "," <<
			PARAM_COL_LABEL << "," << PARAM_COL_UNIT << "," << PARAM_COL_STANDARDUNIT << "," <<
			PARAM_COL_DISPLAYUNIT << "," << PARAM_COL_FORMAT << "," << PARAM_COL_ARCHIVERELCHANGE << "," <<
			PARAM_COL_ARCHIVEABSCHANGE << "," << PARAM_COL_ARCHIVEPERIOD << "," << PARAM_COL_DESCRIPTION << ")";

	query_str << " VALUES ($1, CURRENT_TIMESTAMP(6), TO_TIMESTAMP($2), $3, $4, $5, $6, $7, $8, $9, $10, $11)" ;
	PGparam *param = PQparamCreate(pgconn);
	std::string desc = data->attr_conf->description.substr(0,1023);
	PQputf(param, "%int4 %float8 %varchar* %varchar* %varchar* %varchar* %varchar* %varchar* %varchar* %varchar* %varchar*", ID, ev_time, data->attr_conf->label.c_str(), data->attr_conf->unit.c_str(), data->attr_conf->standard_unit.c_str(), data->attr_conf->display_unit.c_str(), data->attr_conf->format.c_str(), data->attr_conf->events.arch_event.archive_rel_change.c_str(), data->attr_conf->events.arch_event.archive_abs_change.c_str(), data->attr_conf->events.arch_event.archive_period.c_str(), desc.c_str());
	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
	}
#ifdef _LIB_DEBUG
	else {
		cout << __func__<< ": SUCCESS in query: " << query_str.str() << endl;
	}
#endif
	PQclear(res);
}

int HdbPPpostgres::find_attr_id_type(const std::string &facility, const std::string &attr, uint32_t &ID, const std::string &attr_type, unsigned int &conf_ttl) {
	std::ostringstream query_str;

	PGparam *param = PQparamCreate(pgconn);
	bool hasTTL;
#ifndef _MULTI_TANGO_HOST
	hasTTL = true;
	query_str << "SELECT " << CONF_TABLE_NAME << "." << CONF_COL_ID << "," << CONF_TYPE_TABLE_NAME << "." << CONF_TYPE_COL_TYPE << "," << CONF_TABLE_NAME << "." << CONF_COL_TTL <<
			" FROM " << CONF_TABLE_NAME << " JOIN " << CONF_TYPE_TABLE_NAME <<
			" ON " << CONF_TABLE_NAME << "." << CONF_COL_TYPE_ID << "=" << CONF_TYPE_TABLE_NAME << "." << CONF_TYPE_COL_TYPE_ID <<
			" WHERE " << CONF_COL_NAME << " = $1";


	std::string paramstr = std::string("tango://") + facility + std::string("/") + std::string(attr);
	PQputf(param, "%varchar*", paramstr.c_str());
#else
	hasTTL = false;
	std::vector<std::string> facilities;
	string_explode(facility, ",", facilities);

	query_str << "SELECT " << CONF_COL_ID << " FROM " <<  CONF_TABLE_NAME <<" WHERE (";
	query_str << "SELECT " << CONF_COL_ID << "," << CONF_COL_TYPE << " FROM " << CONF_TABLE_NAME << " WHERE (";
	int param_num = 1;
	for(std::vector<std::string>::const_iterator it = facilities.begin(); it != facilities.end(); it++) {
		query_str << CONF_COL_NAME<< " LIKE $" << param_num;
		param_num++;
		std::string paramstr = std::string("tango://%") + (*it) + std::string("%/") + std::string(attr);
		PQputf(param, "%varchar", paramstr.c_str());

		if(it != facilities.end() - 1)
			query_str << " OR ";
	}
	query_str << ")";
#endif


	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
	}

	int num_rows = PQntuples(res);
#ifdef _LIB_DEBUG
	if(num_rows > 0) {
		cout << __func__<< ": SUCCESS in query: " << query_str.str() << endl;
	} else {
		cout << __func__<< ": NO RESULT in query: " << query_str.str() << endl;
	}
#endif

	bool found = false;
	// int id_fnum = PQfnumber(res, CONF_COL_ID);
	// int type_fnum = PQfnumber(res, CONF_TYPE_COL_TYPE);
	// int ttl_fnum = PQfnumber(res, CONF_COL_TTL);

	std::string db_type;
	for (int i = 0; i < num_rows; i++) {
		found = true;
		PGvarchar type_str;
		if (hasTTL){
			PQgetf(res, i, "#int4 #varchar #int4", CONF_COL_ID, &ID, CONF_TYPE_COL_TYPE, &type_str, CONF_COL_TTL, &conf_ttl);
		} else {
			PQgetf(res, i, "#int4 #varchar", CONF_COL_ID, &ID, CONF_TYPE_COL_TYPE, &type_str);
			conf_ttl = 0;
		}
		db_type = std::string(type_str);
	}
	PQclear(res);
	if(!found){
		return -1;
	}

	if(db_type != attr_type) {
		cout << __func__<< ": FOUND ID="<<ID<<" but different type: attr_type="<<attr_type<<"-db_type="<<db_type << endl;
		return -2;
	} else {
		cout << __func__<< ": FOUND ID="<<ID<<" with SAME type: attr_type="<<attr_type<<"-db_type="<<db_type << endl;
		return 0;
	}
}

void HdbPPpostgres::configure_Attr(string name, int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/, unsigned int ttl/*hours, 0=infinity*/) {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
#endif

	std::ostringstream insert_str;
	std::ostringstream update_ttl_str;
	std::string facility = get_only_tango_host(name);
#ifndef _MULTI_TANGO_HOST
	facility = add_domain(facility);
#endif
	std::string attr_name = get_only_attr_name(name);
	std::cout<< __func__ << ": name="<<name<<" -> facility="<<facility<<" attr_name="<<attr_name<< std::endl;
	uint32_t id;
	std::string data_type = get_data_type(type, format, write_type);
	unsigned int conf_ttl = 0;
	int ret = find_attr_id_type(facility, attr_name, id, data_type, conf_ttl);
	//ID already present but different configuration (attribute type)
	if(ret == -2) {
		stringstream tmp;
		tmp << "ERROR "<<facility<<"/"<<attr_name<<" already configured with ID="<<id;
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(DATA_ERROR,tmp.str(),__func__);
	}

	//ID found and same configuration (attribute type): do nothing
	if(ret == 0)
	{
#ifdef _LIB_DEBUG
		cout<< __func__ << ": ALREADY CONFIGURED with same configuration: "<<facility<<"/"<<attr_name<<" with ID="<<id << endl;
#endif
		if(conf_ttl != ttl)
		{
#ifdef _LIB_DEBUG
			cout<< __func__ << ": .... BUT different ttl: updating " << conf_ttl << " to " << ttl << endl;
#endif
			//TODO: rewrite to use ID
			updateTTL_Attr(name, ttl);
		}
		event_Attr(id, EVENT_ADD);
		return;
	}

	//add domain name to fqdn
	name = string("tango://")+facility+string("/")+attr_name;
	string complete_facility=string("tango://")+facility;

	vector<string> exploded_name;
	string_explode(attr_name, "/", exploded_name);

	string domain="";
	string family="";
	string member="";
	string last_name="";
	if(exploded_name.size() == 4) {
		domain=exploded_name[0];
		family=exploded_name[1];
		member=exploded_name[2];
		last_name=exploded_name[3];
	} else {
		cout<< __func__ << ": FAILED to explode " << attr_name << " into 4 fields, result is " << exploded_name.size() << endl;
		//TODO: maybe need exception?
	}

	insert_str <<
		"INSERT INTO " << CONF_TABLE_NAME << " ("<<CONF_COL_NAME<<","<<CONF_COL_TYPE_ID<<","<<CONF_COL_TTL<<","<<
			CONF_COL_FACILITY<<","<<CONF_COL_DOMAIN<<","<<CONF_COL_FAMILY<<","<<CONF_COL_MEMBER<<","<<CONF_COL_LAST_NAME<<")"<<
			" (SELECT $1, " << CONF_TYPE_COL_TYPE_ID << ", $2, $3, $4, $5, $6, $7 FROM " CONF_TYPE_TABLE_NAME << " WHERE " << CONF_TYPE_COL_TYPE << " = $8) RETURNING " << CONF_COL_ID;
	// $1 = name
	// $2 = ttl
	// $3 = complete_facility
	// $4 = domain
	// $5 = family
	// $6 = member
	// $7 = last_name
	// $8 = data_type

	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%varchar* %int4 %varchar* %varchar* %varchar* %varchar* %varchar* %varchar*", name.c_str(), ttl, complete_facility.c_str(), domain.c_str(), family.c_str(), member.c_str(), last_name.c_str(), data_type.c_str());
	PGresult *res = PQparamExec(pgconn, param, insert_str.str().c_str(), 1);
	PQparamClear(param);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << insert_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
	}
#ifdef _LIB_DEBUG
	else {
		cout << __func__<< ": SUCCESS in query: " << insert_str.str() << endl;
	}
#endif
	if (PQntuples(res) == 0){
		PQclear(res);
		stringstream tmp;
		tmp << "ERROR: UNKNOWN data type: " << data_type;
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(DATA_ERROR,tmp.str(),__func__);
	}
	PQgetf(res, 0, "#int4", CONF_COL_ID, &id);
	#ifdef _LIB_DEBUG
	cout << __func__ << ": returned id " << id << endl;
	#endif
	PQclear(res);
	
	event_Attr(id, EVENT_ADD);
}

void HdbPPpostgres::updateTTL_Attr(std::string name, unsigned int ttl/*hours, 0=infinity*/) {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
#endif
	std::ostringstream update_ttl_str;
	std::string facility = get_only_tango_host(name);
#ifndef _MULTI_TANGO_HOST
	facility = add_domain(facility);
#endif
	string attr_name = get_only_attr_name(name);

	uint32_t id=0;
	int ret = find_attr_id(facility, attr_name, id);
	if(ret < 0) {
		stringstream tmp;
		tmp << "ERROR "<<facility<<"/"<<attr_name<<" NOT FOUND";
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(DATA_ERROR,tmp.str(),__func__);
	}

	update_ttl_str << "UPDATE " << CONF_TABLE_NAME << " SET " << CONF_COL_TTL << "=" << ttl << " WHERE " << CONF_COL_ID << "= $1";
	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%int4", id);
	PGresult *res = PQparamExec(pgconn, param, update_ttl_str.str().c_str(), 1);
	PQparamClear(param);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << update_ttl_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
	}
	PQclear(res);
}

void HdbPPpostgres::event_Attr(string name, unsigned char event) {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
	switch(event){
		case DB_INSERT: cout << "DB_INSERT" << endl; break;
		case DB_START: cout << "DB_START" << endl; break;
		case DB_STOP: cout << "DB_STOP" << endl; break;
		case DB_REMOVE: cout << "DB_REMOVE" << endl; break;
		case DB_INSERT_PARAM: cout << "DB_INSERT_PARAM" << endl; break;
		case DB_PAUSE: cout << "DB_PAUSE" << endl; break;
		case DB_UPDATETTL: cout << "DB_UPDATETTL" << endl; break;
		default: break;
	}
#endif

	std::string facility = get_only_tango_host(name);

#ifndef _MULTI_TANGO_HOST
	facility = add_domain(facility);
#endif

	std::string attr_name = get_only_attr_name(name);

	uint32_t id=0;
	int ret = find_attr_id(facility, attr_name, id);
	if(ret < 0) {
		std::stringstream tmp;
		tmp << "ERROR "<<facility<<"/"<<attr_name<<" NOT FOUND";
		std::cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(DATA_ERROR,tmp.str(),__func__);
	}

	std::string event_str;
	switch(event) {
		case DB_START: {
			std::string last_event;
			ret = find_last_event(id, last_event);
			if(ret == 0 && last_event == EVENT_START) {
				event_Attr(id, EVENT_CRASH);
			}

			event_str = EVENT_START;
			break;
		}
		case DB_STOP: {
			event_str = EVENT_STOP;
			break;
		}
		case DB_REMOVE: {
			event_str = EVENT_REMOVE;
			break;
		}
		case DB_PAUSE: {
			event_str = EVENT_PAUSE;
			break;
		}
		default: {
			stringstream tmp;
			tmp << "ERROR for "<<facility<<"/"<<attr_name<<" event=" << (int)event << " NOT SUPPORTED";
			cout << __func__<< ": " << tmp.str() << endl;
			Tango::Except::throw_exception(DATA_ERROR,tmp.str(),__func__);
		}
	}

	event_Attr(id, event_str);
}

void HdbPPpostgres::event_Attr(uint32_t id, const std::string &event) {
#ifdef _LIB_DEBUG
	cout << __func__<< endl;
	cout << "Event is "<< event << endl;
#endif

	std::ostringstream insert_event_str;

	insert_event_str << "INSERT INTO " << HISTORY_TABLE_NAME << " ("<<HISTORY_COL_ID<<","<<HISTORY_COL_EVENT_ID<<","<<HISTORY_COL_TIME<<")" <<
			" SELECT $1," << HISTORY_EVENT_COL_EVENT_ID << ",CURRENT_TIMESTAMP(6)" <<
			" FROM " << HISTORY_EVENT_TABLE_NAME << " WHERE " << HISTORY_EVENT_COL_EVENT << " = $2";
	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%int4 %varchar*", id, event.c_str());
	PGresult *res = PQparamExec(pgconn, param, insert_event_str.str().c_str(), 1);
	PQparamClear(param);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << insert_event_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(DATA_ERROR,tmp.str(),__func__);
	}
	PQclear(res);
}

void HdbPPpostgres::string_vector2map(const std::vector<std::string> &str, const std::string &separator, std::map<std::string, std::string> &results) {
	for(std::vector<std::string>::const_iterator it=str.begin(); it != str.end(); it++) {
		string::size_type found_eq;
		found_eq = it->find_first_of(separator);
		if(found_eq != string::npos && found_eq > 0)
			results.insert(make_pair(it->substr(0,found_eq),it->substr(found_eq+1)));
	}
}


void HdbPPpostgres::string_explode(const std::string &exstr, const std::string &separator, std::vector<std::string> &results) {
	std::string str = exstr;
	std::string::size_type found;

	found = str.find_first_of(separator);
	while(found != string::npos) {
		if(found > 0) {
			results.push_back(str.substr(0,found));
		}
		str = str.substr(found+1);
		found = str.find_first_of(separator);
	}
	if(str.length() > 0) {
		results.push_back(str);
	}
}

std::string HdbPPpostgres::get_only_tango_host(const std::string &str) {
	string::size_type start = str.find("tango://");
	if (start == string::npos) {
		return "unknown";
	} else {
		start += 8; //	"tango://" length
		string::size_type	end = str.find('/', start);
		string th = str.substr(start, end-start);
		return th;
	}
}

int HdbPPpostgres::find_attr_id(const std::string &facility, const std::string &attr, uint32_t &ID) {
	std::ostringstream query_str;

	PGparam *param = PQparamCreate(pgconn);
#ifndef _MULTI_TANGO_HOST
	query_str << "SELECT " << CONF_COL_ID << " FROM " <<  CONF_TABLE_NAME << " WHERE " << CONF_COL_NAME << " = $1";
	std::string paramstr = std::string("tango://") + facility + std::string("/") + std::string(attr);
	PQputf(param, "%varchar*", paramstr.c_str());
#else
	std::vector<std::string> facilities;
	string_explode(facility, ",", facilities);

	query_str << "SELECT " << CONF_COL_ID << " FROM " <<  CONF_TABLE_NAME <<" WHERE (";

	int param_num = 1;
	for(std::vector<std::string>::const_iterator it = facilities.begin(); it != facilities.end(); it++) {
		query_str << CONF_COL_NAME<< " LIKE $" << param_num;
		param_num++;
		std::string paramstr = std::string("tango://%") + (*it) + std::string("%/") + std::string(attr);
		PQputf(param, "%varchar", paramstr.c_str());

		if(it != facilities.end() - 1)
			query_str << " OR ";
	}
	query_str << ")";
#endif


	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		// seems to be not ok
		std::stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(), __func__);
	}

	int num_rows = PQntuples(res);
#ifdef _LIB_DEBUG
	if(num_rows > 0) {
		cout << __func__<< ": SUCCESS in query: " << query_str.str() << endl;
	} else {
		cout << __func__<< ": NO RESULT in query: " << query_str.str() << endl;
	}
#endif

	bool found = false;
	for (int i = 0; i < num_rows; i++) {
		found = true;
		PQgetf(res, i, "#int4", CONF_COL_ID, &ID);
	}
	PQclear(res);
	if(!found){
		return -1;
	}

	return 0;
}

#ifndef _MULTI_TANGO_HOST
std::string HdbPPpostgres::add_domain(const std::string &str) {
	std::string::size_type end1 = str.find(".");
	if (end1 == std::string::npos) {
		//get host name without tango://
		std::string::size_type start = str.find("tango://");
		if (start == std::string::npos) {
			start = 0;
		} else {
			start = 8;	//tango:// len
		}
		std::string::size_type end2 = str.find(":", start);

		std::string th = str.substr(start, end2);
		std::string with_domain = str;
		struct addrinfo hints;
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC; //either IPV4 or IPV6
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_CANONNAME;
		struct addrinfo *result, *rp;
		int ret = getaddrinfo(th.c_str(), NULL, &hints, &result);
		if (ret != 0) {
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
			return str;
		}

		for (rp = result; rp != NULL; rp = rp->ai_next) {
			with_domain = string(rp->ai_canonname) + str.substr(end2);
			cout << __func__ <<": found domain -> " << with_domain<<endl;
		}
		freeaddrinfo(result); // all done with this structure
		return with_domain;
	} else {
		return str;
	}
}

std::string HdbPPpostgres::remove_domain(const std::string &str)
{
	string::size_type	end1 = str.find(".");
	if (end1 == string::npos) {
		return str;
	} else {
		std::string::size_type start = str.find("tango://");
		if (start == std::string::npos) {
			start = 0;
		} else {
			start = 8;	//tango:// len
		}
		std::string::size_type end2 = str.find(":", start);
		if(end1 > end2)	//'.' not in the tango host part
			return str;
		string th = str.substr(0, end1);
		th += str.substr(end2, str.size()-end2);
		return th;
	}
}
#endif

std::string HdbPPpostgres::get_only_attr_name(const std::string &str) {
	std::string::size_type start = str.find("tango://");
	if (start == string::npos){
		return str;
	} else {
		start += 8; //	"tango://" length
		start = str.find('/', start);
		start++;
		string	signame = str.substr(start);
		return signame;
	}
}

int HdbPPpostgres::find_last_event(uint32_t ID, std::string &event) {
	std::ostringstream query_str;
	query_str <<
		"SELECT " << HISTORY_EVENT_COL_EVENT <<
			" FROM " << HISTORY_TABLE_NAME <<
			" JOIN " << HISTORY_EVENT_TABLE_NAME <<
			" ON " << HISTORY_EVENT_TABLE_NAME << "." << HISTORY_EVENT_COL_EVENT_ID << "=" << HISTORY_TABLE_NAME << "." << HISTORY_COL_EVENT_ID <<
			" WHERE " << HISTORY_COL_ID << " = $1" <<
			" ORDER BY " << HISTORY_COL_TIME << " DESC LIMIT 1";
	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%int4", ID);
	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
	}

	int num_rows = PQntuples(res);
#ifdef _LIB_DEBUG
	if(num_rows > 0) {
		cout << __func__<< ": SUCCESS in query: " << query_str.str() << endl;
	} else {
		cout << __func__<< ": NO RESULT in query: " << query_str.str() << endl;
	}
#endif

	bool found = false;
	for (int i = 0; i < num_rows; i++) {
		found = true;
		PGvarchar event_str;
		PQgetf(res, i, "#varchar", HISTORY_EVENT_COL_EVENT, &event_str);
		event = std::string(event_str);
	}
	PQclear(res);
	if(!found){
		return -1;
	}
	return 0;
}

std::string HdbPPpostgres::get_table_name(int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/) {
	ostringstream table_name;
	table_name << "att_" << get_data_type(type,format,write_type);
	return table_name.str();
}

std::string HdbPPpostgres::get_data_type(int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/) {
	std::ostringstream data_type;
	if(format==Tango::SCALAR) {
		data_type << TYPE_SCALAR << "_";
	} else {
		data_type << TYPE_ARRAY << "_";
	}

	if(type==Tango::DEV_DOUBLE) {
		data_type << TYPE_DEV_DOUBLE << "_";
	} else if(type==Tango::DEV_FLOAT) {
		data_type << TYPE_DEV_FLOAT << "_";
	} else if(type==Tango::DEV_STRING) {
		data_type << TYPE_DEV_STRING << "_";
	} else if(type==Tango::DEV_LONG) {
		data_type << TYPE_DEV_LONG << "_";
	} else if(type==Tango::DEV_ULONG) {
		data_type << TYPE_DEV_ULONG << "_";
	} else if(type==Tango::DEV_LONG64) {
		data_type << TYPE_DEV_LONG64 << "_";
	} else if(type==Tango::DEV_ULONG64) {
		data_type << TYPE_DEV_ULONG64 << "_";
	} else if(type==Tango::DEV_SHORT) {
		data_type << TYPE_DEV_SHORT << "_";
	} else if(type==Tango::DEV_USHORT) {
		data_type << TYPE_DEV_USHORT << "_";
	} else if(type==Tango::DEV_BOOLEAN) {
		data_type << TYPE_DEV_BOOLEAN << "_";
	} else if(type==Tango::DEV_UCHAR) {
		data_type << TYPE_DEV_UCHAR << "_";
	} else if(type==Tango::DEV_STATE) {
		data_type << TYPE_DEV_STATE << "_";
	} else if(type==Tango::DEV_ENCODED) {
		data_type << TYPE_DEV_ENCODED << "_";
	} else if(type==Tango::DEV_ENUM) {
		data_type << TYPE_DEV_ENUM << "_";
	}

	if(write_type==Tango::READ) {
		data_type << TYPE_RO;
	} else {
		data_type << TYPE_RW;
	}

	return data_type.str();
}

void HdbPPpostgres::store_scalar(const std::string &attr, int quality/*ATTR_VALID, ATTR_INVALID, ..*/, const std::string &error_desc, double ev_time, double rcv_time, const std::string &table_name, PGConvertTango *conv) {
#ifdef _LIB_DEBUG
	cout << __func__<< ": entering..." << endl;
#endif
	std::map<std::string, uint32_t>::iterator it = attr_ID_map.find(attr);
	//if not already present in cache, look for ID in the DB
	if(it == attr_ID_map.end()) {
		uint32_t ID;
		string facility = get_only_tango_host(attr);
		string attr_name = get_only_attr_name(attr);
		if(find_attr_id(facility, attr_name, ID) == 0) {
			attr_ID_map.insert(make_pair(attr,ID));
			it = attr_ID_map.find(attr);
		} else {
			cout << __func__<< ": ID not found!" << endl;
			Tango::Except::throw_exception(DATA_ERROR,"ID not found",__func__);
		}
	}
	if(it == attr_ID_map.end()) {
		cout << __func__<< ": ID not found for attr="<<attr << endl;
		Tango::Except::throw_exception(DATA_ERROR,"ID not found",__func__);
	}
	uint32_t ID=it->second;

	bool detected_insert_time = true;
	bool detected_recv_time = true;
	bool detected_quality = true;
	bool detected_error = true;
	if(autodetectschema) {
		try {
			detected_insert_time = table_column_map.at(table_name+"_"+SC_COL_INS_TIME);
		} catch(std::out_of_range &e) {
			detected_insert_time = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_INS_TIME) NOT FOUND" << endl;
#endif
		}
		try {
			detected_recv_time = table_column_map.at(table_name+"_"+SC_COL_RCV_TIME);
		} catch(std::out_of_range &e) {
			detected_recv_time = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_RCV_TIME) NOT FOUND" << endl;
#endif
		}
		try {
			detected_quality = table_column_map.at(table_name+"_"+SC_COL_QUALITY);
		}catch(std::out_of_range &e) {
			detected_quality = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_QUALITY) NOT FOUND" << endl;
#endif
		}
		try {
			detected_error = table_column_map.at(table_name+"_"+SC_COL_ERROR_DESC_ID);
		} catch(std::out_of_range &e) {
			detected_error = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_ERROR_DESC) NOT FOUND" << endl;
#endif
		}
	} else if(lightschema) {
		detected_insert_time = false;
		detected_recv_time = false;
	}
#ifdef _LIB_DEBUG
	cout << __func__<< ": INSERT_TIME: " << (detected_insert_time?"YES":"NO") << ", RECV_TIME: " << (detected_recv_time?"YES":"NO") << ", QUALITY: " << (detected_quality?"YES":"NO") << ", ERROR: "<<(detected_error?"YES":"NO") <<endl;
#endif
	uint32_t ERR_ID;
	bool has_err;
	if(detected_error)
		has_err = cache_err_id(error_desc, ERR_ID) == 0;

	std::ostringstream query_str;

	query_str << "INSERT INTO " << table_name << " (" << SC_COL_ID << "," << SC_COL_EV_TIME;
	if(detected_insert_time)
		query_str << "," << SC_COL_INS_TIME;
	if(detected_recv_time)
		query_str << "," << SC_COL_RCV_TIME;
	if(detected_quality)
		query_str << "," << SC_COL_QUALITY;
	if(detected_error)
		query_str << "," << SC_COL_ERROR_DESC_ID;
	if (!conv->isReadNull()){
		query_str << "," << SC_COL_VALUE_R;
	}
	if(!conv->isWriteNull()){
		query_str << "," << SC_COL_VALUE_W;
	}
	query_str << ")";

	int param_num = 2;
	query_str << " VALUES ($1, TO_TIMESTAMP($2)";
	if(detected_insert_time){
		query_str << ", CURRENT_TIMESTAMP(6)";//insert_time
	}
	if(detected_recv_time){
		++param_num;
		query_str << ", TO_TIMESTAMP($" << param_num << ")";//recv_time
	}
	if(detected_quality){
		++param_num;
		query_str << ", $" << param_num;//quality
	}
	if(detected_error){
		++param_num;
		query_str << ", $" << param_num;//error
	}
	if (!conv->isReadNull()){
		++param_num;
		query_str << ", $" << param_num ; //value_r
	}
	if(!conv->isWriteNull()){	//RW
		++param_num;
		query_str << ", $" << param_num;//value_w
	}
	query_str << ")";

#ifdef _LIB_DEBUG
	cout << __func__<< ": QUERY: " << query_str.str() <<std::endl;
	cout << __func__<< ": PN1: " << param_num <<std::endl;
#endif


	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%int4 %float8", ID, ev_time);
	#ifdef _LIB_DEBUG
	cout << __func__<< ": ev_time: "<< std::setprecision (15) << ev_time <<std::endl;
	#endif
	if(detected_recv_time){
		PQputf(param, "%float8", rcv_time);
		#ifdef _LIB_DEBUG
		cout << __func__<< ": rcv_time: "<< std::setprecision (15) << rcv_time <<std::endl;
		#endif
	}
	if(detected_quality) {
		PQputf(param, "%int2", quality);
	}
	if(detected_error){
		if (has_err){
			PQputf(param, "%int4", ERR_ID);
		}
		else{
			PQputf(param, "%null");
		}
	}
	if (!conv->isReadNull()){
		conv->storeReadData(param);
	}
	if(!conv->isWriteNull()){	//RW
		conv->storeWriteData(param);
	}

	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);

	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
	}
#ifdef _LIB_DEBUG
	else {
		cout << __func__<< ": SUCCESS in query: " << query_str.str() << endl;
	}
#endif
	PQclear(res);
}

bool HdbPPpostgres::autodetect_column(int type/*DEV_DOUBLE, DEV_STRING, ..*/, int format/*SCALAR, SPECTRUM, ..*/, int write_type/*READ, READ_WRITE, ..*/, const std::string &column_name) {
	std::ostringstream query_str;
	query_str << "SELECT " << INF_SCHEMA_COLUMN_NAME << " FROM " << INFORMATION_SCHEMA << "." << INF_SCHEMA_COLUMNS << " WHERE " << /*INF_SCHEMA_TABLE_SCHEMA <<
			"='public' AND " <<*/ INF_SCHEMA_TABLE_NAME << "= $1 AND " << INF_SCHEMA_COLUMN_NAME << "= $2";
	PGparam *param = PQparamCreate(pgconn);
	std::string table = get_table_name(type, format, write_type);
	PQputf(param, "%varchar* %varchar*", table.c_str(), column_name.c_str());
	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
	}
	bool found = PQntuples(res)>0;
	PQclear(res);
	return found;
}

int HdbPPpostgres::cache_err_id(const std::string &error_desc, uint32_t &ERR_ID) {
	if(error_desc.length() == 0)
		return -1;
#ifdef _LIB_DEBUG
	cout << __func__<< ": entering for '"<< error_desc << "' map size=" << attr_ERR_queue.size() << endl;
#endif
	std::map<std::string, uint32_t>::iterator it = attr_ERR_ID_map.find(error_desc);
	//if not already present in cache, look for ID in the DB
	if(it == attr_ERR_ID_map.end()) {
		if(find_err_id(error_desc, ERR_ID) == 0) {
#ifdef _LIB_DEBUG
			cout << __func__<< ": '"<<error_desc << "' found in table, ERR_ID="<<ERR_ID << endl;
#endif
			attr_ERR_ID_map.insert(make_pair(error_desc, ERR_ID));
			attr_ERR_queue.push(error_desc);
			while(attr_ERR_queue.size() > ERR_MAP_MAX_SIZE) {
				attr_ERR_ID_map.erase(attr_ERR_queue.front());
				attr_ERR_queue.pop();
			}
			it = attr_ERR_ID_map.find(error_desc);
		}
	}
	if(it == attr_ERR_ID_map.end()) {
#ifdef _LIB_DEBUG
		cout << __func__<< ": ERR_ID not found for err='"<<error_desc << "' inserting new one" << endl;
#endif
		if(insert_error(error_desc, ERR_ID) == 0) {
#ifdef _LIB_DEBUG
			cout << __func__<< ": '"<<error_desc << "' INSERTED in table, ERR_ID="<<ERR_ID << endl;
#endif
			attr_ERR_ID_map.insert(make_pair(error_desc,ERR_ID));
			attr_ERR_queue.push(error_desc);
			while(attr_ERR_queue.size() > ERR_MAP_MAX_SIZE) {
				attr_ERR_ID_map.erase(attr_ERR_queue.front());
				attr_ERR_queue.pop();
			}
		} else{
			return -1;
		}
	} else {
		ERR_ID=it->second;
	}

	return 0;
}

int HdbPPpostgres::find_err_id(const std::string &err, uint32_t &ERR_ID) {
	std::ostringstream query_str;
	//string facility_no_domain = remove_domain(facility);
	//string facility_with_domain = add_domain(facility);
	query_str << "SELECT " << ERR_COL_ID << " FROM " << ERR_TABLE_NAME << " WHERE " << ERR_COL_ERROR_DESC << " = $1";// '" << err_escaped << "'";
	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%varchar*", err.c_str());
	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		std::stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR, tmp.str(),__func__);
	}

	int num_rows = PQntuples(res);
#ifdef _LIB_DEBUG
	if(num_rows > 0) {
		cout << __func__<< ": SUCCESS in query: " << query_str.str() << endl;
	} else {
		cout << __func__<< ": NO RESULT in query: " << query_str.str() << endl;
	}
#endif

	bool found = false;
	for (int i = 0; i < num_rows; i++) {
		found = true;
		PQgetf(res, i, "#int4", ERR_COL_ID, &ERR_ID);
	}

	PQclear(res);
	if(!found){
		return -1;
	}

	return 0;
}

int HdbPPpostgres::insert_error(const std::string &error_desc, uint32_t &ERR_ID) {
	std::ostringstream query_str;
	query_str << "INSERT INTO " << ERR_TABLE_NAME << " (" << ERR_COL_ERROR_DESC << ")" << " VALUES ($1) RETURNING " << ERR_COL_ID;
	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%varchar*", error_desc.c_str());
	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(DATA_ERROR,tmp.str(),__func__);
	}
	PQgetf(res, 0, "#int4", ERR_COL_ID, &ERR_ID);
	#ifdef _LIB_DEBUG
	cout << __func__ << " returned id " << ERR_ID << endl;
	#endif

	PQclear(res);

	return 0;
}

void HdbPPpostgres::store_array(const std::string &attr, int quality/*ATTR_VALID, ATTR_INVALID, ..*/, const std::string &error_desc, Tango::AttributeDimension attr_r_dim, Tango::AttributeDimension attr_w_dim, double ev_time, double rcv_time, const std::string &table_name, PGConvertTango *conv) {
#ifdef _LIB_DEBUG
//	cout << __func__<< ": entering..." << endl;
#endif
	std::map<std::string, uint32_t>::iterator it = attr_ID_map.find(attr);
	//if not already present in cache, look for ID in the DB
	if(it == attr_ID_map.end()) {
		uint32_t ID;
		string facility = get_only_tango_host(attr);
		string attr_name = get_only_attr_name(attr);
		if(find_attr_id(facility, attr_name, ID) == 0) {
			attr_ID_map.insert(make_pair(attr,ID));
			it = attr_ID_map.find(attr);
		} else {
			cout << __func__<< ": ID not found!" << endl;
			Tango::Except::throw_exception(DATA_ERROR,"ID not found",__func__);
		}
	}
	if(it == attr_ID_map.end()) {
		cout << __func__<< ": ID not found for attr="<<attr << endl;
		Tango::Except::throw_exception(DATA_ERROR,"ID not found",__func__);
	}
	uint32_t ID=it->second;

	bool detected_insert_time = true;
	bool detected_recv_time = true;
	bool detected_quality = true;
	bool detected_error = true;
	if(autodetectschema) {
		try {
			detected_insert_time = table_column_map.at(table_name+"_"+SC_COL_INS_TIME);
		} catch(std::out_of_range &e) {
			detected_insert_time = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_INS_TIME) NOT FOUND" << endl;
#endif
		}
		try {
			detected_recv_time = table_column_map.at(table_name+"_"+SC_COL_RCV_TIME);
		} catch(std::out_of_range &e) {
			detected_recv_time = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_RCV_TIME) NOT FOUND" << endl;
#endif
		}
		try {
			detected_quality = table_column_map.at(table_name+"_"+SC_COL_QUALITY);
		}catch(std::out_of_range &e) {
			detected_quality = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_QUALITY) NOT FOUND" << endl;
#endif
		}
		try {
			detected_error = table_column_map.at(table_name+"_"+SC_COL_ERROR_DESC_ID);
		} catch(std::out_of_range &e) {
			detected_error = false;
#ifdef _LIB_DEBUG
			cout << __func__<< ": after table_column_map.at(...SC_COL_ERROR_DESC) NOT FOUND" << endl;
#endif
		}
	} else if(lightschema) {
		detected_insert_time = false;
		detected_recv_time = false;
	}
#ifdef _LIB_DEBUG
	cout << __func__<< ": INSERT_TIME: " << (detected_insert_time?"YES":"NO") << ", RECV_TIME: " << (detected_recv_time?"YES":"NO") << ", QUALITY: " << (detected_quality?"YES":"NO") << ", ERROR: "<<(detected_error?"YES":"NO") <<endl;
#endif
	uint32_t ERR_ID;
	bool has_err;
	if(detected_error)
		has_err = cache_err_id(error_desc, ERR_ID) == 0;

	std::ostringstream query_str;

	query_str << "INSERT INTO " << table_name << " (" << ARR_COL_ID << "," << ARR_COL_EV_TIME;
	if(detected_insert_time)
		query_str << "," << ARR_COL_INS_TIME;
	if(detected_recv_time)
		query_str << "," << ARR_COL_RCV_TIME;
	if(detected_quality)
		query_str << "," << ARR_COL_QUALITY;
	if(detected_error)
		query_str << "," << ARR_COL_ERROR_DESC_ID;
	query_str << "," << ARR_COL_DIMX_R << "," << ARR_COL_DIMY_R;
	if (!conv->isReadNull()){
		query_str << "," << ARR_COL_VALUE_R;
	}
	if(!conv->isWriteNull()) {
		query_str << "," << ARR_COL_DIMX_W << "," << ARR_COL_DIMY_W << "," << ARR_COL_VALUE_W;
	}
	query_str << ")";

	int param_num = 2;
	query_str << " VALUES ($1, TO_TIMESTAMP($2)";
	if(detected_insert_time){
		query_str << ", CURRENT_TIMESTAMP(6)";//insert_time
	}
	if(detected_recv_time){
		++param_num;
		query_str << ", TO_TIMESTAMP($" << param_num << ")";//recv_time
	}
	if(detected_quality){
		++param_num;
		query_str << ", $" << param_num;//quality
	}
	if(detected_error){
		++param_num;
		query_str << ", $" << param_num;//error
	}
	++param_num;
	query_str << ", $" << param_num;	//value_r
	++param_num;
	query_str << ", $" << param_num;	//value_r
	if (!conv->isReadNull()){
		++param_num;
		query_str << ", $" << param_num;	//value_r
	}
	if(!conv->isWriteNull()){	//RW
		++param_num;
		query_str << ", $" << param_num;
		++param_num;
		query_str << ", $" << param_num;
		++param_num;
		query_str << ", $" << param_num;//value_w
	}
	query_str << ")";


	PGparam *param = PQparamCreate(pgconn);
	PQputf(param, "%int4 %float8", ID, ev_time);
	if(detected_recv_time){
		PQputf(param, "%float8", rcv_time);
	}
	if(detected_quality) {
		PQputf(param, "%int2", quality);
	}

	if(detected_error){
		if (has_err){
			PQputf(param, "%int4", ERR_ID);
		}else{
			PQputf(param, "%null");
		}
	}
	PQputf(param, "%int4 %int4", attr_r_dim.dim_x, attr_r_dim.dim_y);
	if (!conv->isReadNull()){
		conv->storeReadData(param);
	}
	if(!conv->isWriteNull()){	//RW
		PQputf(param, "%int4 %int4", attr_r_dim.dim_x, attr_r_dim.dim_y);
		conv->storeWriteData(param);
	}


	PGresult *res = PQparamExec(pgconn, param, query_str.str().c_str(), 1);
	PQparamClear(param);
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		stringstream tmp;
		tmp << "ERROR in query=" << query_str.str() << ", err=" << PQgeterror();
		cout << __func__<< ": " << tmp.str() << endl;
		Tango::Except::throw_exception(QUERY_ERROR,tmp.str(),__func__);
		return; //is it necessary?
	}
#ifdef _LIB_DEBUG
	else {
		cout << __func__<< ": SUCCESS in query: " << query_str.str() << endl;
	}
#endif
	PQclear(res);
}

//=============================================================================
//=============================================================================
AbstractDB* HdbPPpostgresFactory::create_db(vector<string> configuration)
{
	return new HdbPPpostgres(configuration);
}

//=============================================================================
//=============================================================================
DBFactory *getDBFactory()
{
	HdbPPpostgresFactory *db_postgres_factory = new HdbPPpostgresFactory();
	return static_cast<DBFactory*>(db_postgres_factory);
}
