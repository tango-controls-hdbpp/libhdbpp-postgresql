/*
Source code file src/LibHdb++postgres_database.h
Part of PostgreSQL backend for HDB++ 
Description: Definitions of database table and column names

Authors: Vladimir Sitnov, Alexander Senchenko, George Fatkin
Licence: see LICENCE file
*/

#ifndef _HDBPP_POSTGRES_DATABASE_H
#define _HDBPP_POSTGRES_DATABASE_H

//Names of types
#define TYPE_SCALAR					"scalar"
#define TYPE_ARRAY					"array"

#define TYPE_DEV_BOOLEAN			"devboolean"
#define TYPE_DEV_UCHAR				"devuchar"
#define TYPE_DEV_SHORT				"devshort"
#define TYPE_DEV_USHORT				"devushort"
#define TYPE_DEV_LONG				"devlong"
#define TYPE_DEV_ULONG				"devulong"
#define TYPE_DEV_LONG64				"devlong64"
#define TYPE_DEV_ULONG64			"devulong64"
#define TYPE_DEV_FLOAT				"devfloat"
#define TYPE_DEV_DOUBLE				"devdouble"
#define TYPE_DEV_STRING				"devstring"
#define TYPE_DEV_STATE				"devstate"
#define TYPE_DEV_ENCODED			"devencoded"
#define TYPE_DEV_ENUM				"devenum"

#define TYPE_RO						"ro"
#define TYPE_RW						"rw"


//Names of events
#define EVENT_ADD					"add"
#define EVENT_REMOVE				"remove"
#define EVENT_START					"start"
#define EVENT_STOP					"stop"
#define EVENT_CRASH					"crash"
#define EVENT_PAUSE					"pause"

//######## att_conf ########
#define CONF_TABLE_NAME				"att_conf"
#define CONF_COL_ID					"att_conf_id"
#define CONF_COL_NAME				"att_name"
#define CONF_COL_TYPE_ID			"att_conf_data_type_id"
#define CONF_COL_TTL				"att_ttl"
#define CONF_COL_FACILITY			"facility"
#define CONF_COL_DOMAIN				"domain"
#define CONF_COL_FAMILY				"family"
#define CONF_COL_MEMBER				"member"
#define CONF_COL_LAST_NAME			"name"

//######## att_conf_data_type ########
#define CONF_TYPE_TABLE_NAME		"att_conf_data_type"
#define CONF_TYPE_COL_TYPE_ID		"att_conf_data_type_id"
#define CONF_TYPE_COL_TYPE			"data_type"

//######## att_history ########
#define HISTORY_TABLE_NAME			"att_history"
#define HISTORY_COL_ID				"att_conf_id"
#define HISTORY_COL_EVENT_ID		"att_history_event_id"
#define HISTORY_COL_TIME			"time"

//######## att_history_event ########
#define HISTORY_EVENT_TABLE_NAME	"att_history_event"
#define HISTORY_EVENT_COL_EVENT_ID	"att_history_event_id"
#define HISTORY_EVENT_COL_EVENT		"event"

//######## att_parameter ########
#define PARAM_TABLE_NAME				"att_parameter"
#define PARAM_COL_ID					"att_conf_id"
#define PARAM_COL_INS_TIME				"insert_time"
#define PARAM_COL_EV_TIME				"recv_time"
#define PARAM_COL_LABEL					"label"
#define PARAM_COL_UNIT					"unit"
#define PARAM_COL_STANDARDUNIT			"standard_unit"
#define PARAM_COL_DISPLAYUNIT			"display_unit"
#define PARAM_COL_FORMAT				"format"
#define PARAM_COL_ARCHIVERELCHANGE		"archive_rel_change"
#define PARAM_COL_ARCHIVEABSCHANGE		"archive_abs_change"
#define PARAM_COL_ARCHIVEPERIOD			"archive_period"
#define PARAM_COL_DESCRIPTION			"description"

//######## INFORMATION SCHEMA ########
#define INFORMATION_SCHEMA				"INFORMATION_SCHEMA"
#define INF_SCHEMA_COLUMN_NAME			"COLUMN_NAME"
#define INF_SCHEMA_COLUMNS				"COLUMNS"
// #define INF_SCHEMA_TABLE_SCHEMA			"TABLE_SCHEMA"`
#define INF_SCHEMA_TABLE_NAME			"TABLE_NAME"



//######## att_scalar_... ########
#define SC_COL_ID					"att_conf_id"
#define SC_COL_INS_TIME				"insert_time"
#define SC_COL_RCV_TIME				"recv_time"
#define SC_COL_EV_TIME				"data_time"
#define SC_COL_VALUE_R				"value_r"
#define SC_COL_VALUE_W				"value_w"
#define SC_COL_QUALITY				"quality"
#define SC_COL_ERROR_DESC_ID		"att_error_desc_id"


//######## att_error_desc ########
#define ERR_TABLE_NAME				"att_error_desc"
#define ERR_COL_ID					"att_error_desc_id"
#define ERR_COL_ERROR_DESC			"error_desc"


//######## att_array_... ########
#define ARR_COL_ID					SC_COL_ID
#define ARR_COL_INS_TIME			SC_COL_INS_TIME
#define ARR_COL_RCV_TIME			SC_COL_RCV_TIME
#define ARR_COL_EV_TIME				SC_COL_EV_TIME
#define ARR_COL_VALUE_R				SC_COL_VALUE_R
#define ARR_COL_VALUE_W				SC_COL_VALUE_W
#define ARR_COL_DIMX_R				"dim_x_r"
#define ARR_COL_DIMY_R				"dim_y_r"
#define ARR_COL_DIMX_W				"dim_x_w"
#define ARR_COL_DIMY_W				"dim_y_w"
#define ARR_COL_QUALITY				SC_COL_QUALITY
#define ARR_COL_ERROR_DESC_ID		SC_COL_ERROR_DESC_ID

#endif