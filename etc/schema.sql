CREATE EXTENSION IF NOT EXISTS uint;

CREATE TABLE IF NOT EXISTS att_conf (
    att_conf_id serial NOT NULL,
    att_name character varying(255) NOT NULL,
    att_conf_data_type_id integer NOT NULL,
    att_ttl integer NOT NULL,
    facility character varying(255) NOT NULL DEFAULT '',
    domain character varying(255) NOT NULL DEFAULT '',
    family character varying(255) NOT NULL DEFAULT '',
    member character varying(255) NOT NULL DEFAULT '',
    name character varying(255) NOT NULL DEFAULT '',
    CONSTRAINT att_conf_pkey PRIMARY KEY (att_conf_id),
    CONSTRAINT att_conf_att_name_key UNIQUE (att_name)
);
COMMENT ON TABLE att_conf is 'Attribute Configuration Table';
CREATE INDEX IF NOT EXISTS att_conf_att_conf_data_type_id_ind ON att_conf (att_conf_data_type_id);

DROP TABLE IF EXISTS att_conf_data_type;
CREATE TABLE IF NOT EXISTS att_conf_data_type (
    att_conf_data_type_id serial NOT NULL,
    data_type character varying(255) NOT NULL,
    tango_data_type smallint NOT NULL,
    CONSTRAINT att_conf_data_type_pkey PRIMARY KEY (att_conf_data_type_id)
);
COMMENT ON TABLE att_conf_data_type is 'Attribute types description';

INSERT INTO att_conf_data_type (data_type, tango_data_type) VALUES
('scalar_devboolean_ro', 1),('scalar_devboolean_rw', 1),('array_devboolean_ro', 1),('array_devboolean_rw', 1),
('scalar_devuchar_ro', 22),('scalar_devuchar_rw', 22),('array_devuchar_ro', 22),('array_devuchar_rw', 22),
('scalar_devshort_ro', 2),('scalar_devshort_rw', 2),('array_devshort_ro', 2),('array_devshort_rw', 2),
('scalar_devushort_ro', 6),('scalar_devushort_rw', 6),('array_devushort_ro', 6),('array_devushort_rw', 6),
('scalar_devlong_ro', 3),('scalar_devlong_rw', 3),('array_devlong_ro', 3),('array_devlong_rw', 3),
('scalar_devulong_ro', 7),('scalar_devulong_rw', 7),('array_devulong_ro', 7),('array_devulong_rw', 7),
('scalar_devlong64_ro', 23),('scalar_devlong64_rw', 23),('array_devlong64_ro', 23),('array_devlong64_rw', 23),
('scalar_devulong64_ro', 24),('scalar_devulong64_rw', 24),('array_devulong64_ro', 24),('array_devulong64_rw', 24),
('scalar_devfloat_ro', 4),('scalar_devfloat_rw', 4),('array_devfloat_ro', 4),('array_devfloat_rw', 4),
('scalar_devdouble_ro', 5),('scalar_devdouble_rw', 5),('array_devdouble_ro', 5),('array_devdouble_rw', 5),
('scalar_devstring_ro', 8),('scalar_devstring_rw', 8),('array_devstring_ro', 8),('array_devstring_rw', 8),
('scalar_devstate_ro', 19),('scalar_devstate_rw', 19),('array_devstate_ro', 19),('array_devstate_rw', 19),
('scalar_devencoded_ro', 28),('scalar_devencoded_rw', 28),('array_devencoded_ro', 28),('array_devencoded_rw', 28),
('scalar_devenum_ro', 29),('scalar_devenum_rw', 29),('array_devenum_ro', 29),('array_devenum_rw', 29);

CREATE TABLE IF NOT EXISTS att_history (
    att_conf_id integer NOT NULL,
    "time" timestamp(6) with time zone,
    att_history_event_id integer NOT NULL
);
COMMENT ON TABLE att_history is 'Attribute Configuration Events History Table';
CREATE INDEX IF NOT EXISTS att_history_att_conf_id_ind ON att_history (att_conf_id);
CREATE INDEX IF NOT EXISTS att_history_att_history_event_id ON att_history (att_history_event_id);

DROP TABLE IF EXISTS att_history_event;
CREATE TABLE IF NOT EXISTS att_history_event (
    att_history_event_id serial NOT NULL,
    event character varying(255) NOT NULL,
    CONSTRAINT att_history_event_pkey PRIMARY KEY (att_history_event_id)
);
COMMENT ON TABLE att_history_event IS 'Attribute history events description';
INSERT INTO att_history_event (event) VALUES
('add'),('remove'),('start'),('stop'),('crash'),('pause');


CREATE TABLE IF NOT EXISTS att_parameter (
    att_conf_id integer NOT NULL,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    label character varying(255) NOT NULL DEFAULT '',
    unit character varying(64) NOT NULL DEFAULT '',
    standard_unit character varying(64) NOT NULL DEFAULT '',
    display_unit character varying(64) NOT NULL DEFAULT '',
    format character varying(64) NOT NULL DEFAULT '',
    archive_rel_change character varying(64) NOT NULL DEFAULT '',
    archive_abs_change character varying(64) NOT NULL DEFAULT '',
    archive_period character varying(64) NOT NULL DEFAULT '',
    description character varying(1024) NOT NULL DEFAULT ''
);
COMMENT ON TABLE att_parameter IS 'Attribute configuration parameters';
CREATE INDEX IF NOT EXISTS att_parameter_recv_time ON att_parameter (recv_time);
CREATE INDEX IF NOT EXISTS att_parameter_att_conf_id ON  att_parameter (att_conf_id);

CREATE TABLE IF NOT EXISTS att_error_desc (
    att_error_desc_id serial NOT NULL,
    error_desc character varying(255) NOT NULL,
    CONSTRAINT att_error_desc_pkey PRIMARY KEY (att_error_desc_id),
    CONSTRAINT att_conf_error_desc_key UNIQUE (error_desc)
);
COMMENT ON TABLE att_error_desc IS 'Error Description Table';



CREATE TABLE IF NOT EXISTS att_scalar_devboolean_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r boolean,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devboolean_ro IS 'Scalar Boolean ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devboolean_ro_att_conf_id_data_time ON att_scalar_devboolean_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devboolean_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r boolean,
    value_w boolean,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devboolean_rw IS 'Scalar Boolean ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devboolean_rw_att_conf_id_data_time ON att_scalar_devboolean_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devboolean_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r boolean[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devboolean_ro IS 'Array Boolean ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devboolean_ro_att_conf_id_data_time ON att_array_devboolean_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devboolean_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r boolean[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w boolean[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devboolean_rw IS 'Array Boolean ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devboolean_rw_att_conf_id_data_time ON att_array_devboolean_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devuchar_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint1,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devuchar_ro IS 'Scalar UChar ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devuchar_ro_att_conf_id_data_time ON att_scalar_devuchar_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devuchar_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint1,
    value_w uint1,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devuchar_rw IS 'Scalar UChar ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devuchar_rw_att_conf_id_data_time ON att_scalar_devuchar_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devuchar_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint1[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devuchar_ro IS 'Array UChar ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devuchar_ro_att_conf_id_data_time ON att_array_devuchar_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devuchar_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint1[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w uint1[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devuchar_rw IS 'Array UChar ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devuchar_rw_att_conf_id_data_time ON att_array_devuchar_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devshort_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r smallint,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devshort_ro IS 'Scalar Short ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devshort_ro_att_conf_id_data_time ON att_scalar_devshort_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devshort_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r smallint,
    value_w smallint,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devshort_rw IS 'Scalar Short ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devshort_rw_att_conf_id_data_time ON att_scalar_devshort_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devshort_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r smallint[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devshort_ro IS 'Array Short ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devshort_ro_att_conf_id_data_time ON att_array_devshort_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devshort_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r smallint[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w smallint[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devshort_rw IS 'Array Short ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devshort_rw_att_conf_id_data_time ON att_array_devshort_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devushort_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint2,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devushort_ro IS 'Scalar UShort ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devushort_ro_att_conf_id_data_time ON att_scalar_devushort_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devushort_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint2,
    value_w uint2,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devushort_rw IS 'Scalar UShort ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devushort_rw_att_conf_id_data_time ON att_scalar_devushort_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devushort_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint2[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devushort_ro IS 'Array UShort ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devushort_ro_att_conf_id_data_time ON att_array_devushort_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devushort_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint2[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w uint2[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devushort_rw IS 'Array UShort ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devushort_rw_att_conf_id_data_time ON att_array_devushort_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devlong_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r integer,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devlong_ro IS 'Scalar Long ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devlong_ro_att_conf_id_data_time ON att_scalar_devlong_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devlong_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r integer,
    value_w integer,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devlong_rw IS 'Scalar Long ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devlong_rw_att_conf_id_data_time ON att_scalar_devlong_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devlong_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r integer[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devlong_ro IS 'Array Long ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devlong_ro_att_conf_id_data_time ON att_array_devlong_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devlong_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r integer[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w integer[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devlong_rw IS 'Array Long ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devlong_rw_att_conf_id_data_time ON att_array_devlong_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devulong_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint4,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devulong_ro IS 'Scalar ULong ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devulong_ro_att_conf_id_data_time ON att_scalar_devulong_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devulong_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint4,
    value_w uint4,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devulong_rw IS 'Scalar ULong ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devulong_rw_att_conf_id_data_time ON att_scalar_devulong_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devulong_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint4[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devulong_ro IS 'Array ULong ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devulong_ro_att_conf_id_data_time ON att_array_devulong_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devulong_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint4[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w uint4[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devulong_rw IS 'Array ULong ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devulong_rw_att_conf_id_data_time ON att_array_devulong_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devlong64_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r bigint,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devlong64_ro IS 'Scalar Long64 ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devlong64_ro_att_conf_id_data_time ON att_scalar_devlong64_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devlong64_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r bigint,
    value_w bigint,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devlong64_rw IS 'Scalar Long64 ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devlong64_rw_att_conf_id_data_time ON att_scalar_devlong64_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devlong64_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r bigint[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devlong64_ro IS 'Array Long64 ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devlong64_ro_att_conf_id_data_time ON att_array_devlong64_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devlong64_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r bigint[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w bigint[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devlong64_rw IS 'Array Long64 ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devlong64_rw_att_conf_id_data_time ON att_array_devlong64_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devulong64_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint8,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devulong64_ro IS 'Scalar ULong64 ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devulong64_ro_att_conf_id_data_time ON att_scalar_devulong64_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devulong64_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r uint8,
    value_w uint8,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devulong64_rw IS 'Scalar ULong64 ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devulong64_rw_att_conf_id_data_time ON att_scalar_devulong64_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devulong64_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint8[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devulong64_ro IS 'Array ULong64 ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devulong64_ro_att_conf_id_data_time ON att_array_devulong64_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devulong64_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r uint8[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w uint8[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devulong64_rw IS 'Array ULong64 ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devulong64_rw_att_conf_id_data_time ON att_array_devulong64_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devfloat_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r real,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devfloat_ro IS 'Scalar Float ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devfloat_ro_att_conf_id_data_time ON att_scalar_devfloat_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devfloat_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r real,
    value_w real,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devfloat_rw IS 'Scalar Float ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devfloat_rw_att_conf_id_data_time ON att_scalar_devfloat_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devfloat_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r real[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devfloat_ro IS 'Array Float ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devfloat_ro_att_conf_id_data_time ON att_array_devfloat_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devfloat_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r real[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w real[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devfloat_rw IS 'Array Float ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devfloat_rw_att_conf_id_data_time ON att_array_devfloat_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devdouble_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r double precision,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devdouble_ro IS 'Scalar Double ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devdouble_ro_att_conf_id_data_time ON att_scalar_devdouble_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devdouble_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r double precision,
    value_w double precision,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devdouble_rw IS 'Scalar Double ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devdouble_rw_att_conf_id_data_time ON att_scalar_devdouble_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devdouble_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r double precision[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devdouble_ro IS 'Array Double ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devdouble_ro_att_conf_id_data_time ON att_array_devdouble_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devdouble_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r double precision[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w double precision[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devdouble_rw IS 'Array Double ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devdouble_rw_att_conf_id_data_time ON att_array_devdouble_rw (att_conf_id,data_time);


CREATE TABLE IF NOT EXISTS att_scalar_devstring_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r character varying(16384),
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devstring_ro IS 'Scalar String ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devstring_ro_att_conf_id_data_time ON att_scalar_devstring_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devstring_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r character varying(16384),
    value_w character varying(16384),
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devstring_rw IS 'Scalar String ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devstring_rw_att_conf_id_data_time ON att_scalar_devstring_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devstring_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r character varying(16384)[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devstring_ro IS 'Array String ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devstring_ro_att_conf_id_data_time ON att_array_devstring_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devstring_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r character varying(16384)[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w character varying(16384)[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devstring_rw IS 'Array String ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devstring_rw_att_conf_id_data_time ON att_array_devstring_rw (att_conf_id,data_time);



CREATE TABLE IF NOT EXISTS att_scalar_devstate_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r integer,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devstate_ro IS 'Scalar State ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devstate_ro_att_conf_id_data_time ON att_scalar_devstate_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devstate_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r integer,
    value_w integer,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devstate_rw IS 'Scalar State ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devstate_rw_att_conf_id_data_time ON att_scalar_devstate_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devstate_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r integer[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devstate_ro IS 'Array State ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devstate_ro_att_conf_id_data_time ON att_array_devstate_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devstate_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r integer[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w integer[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devstate_rw IS 'Array State ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devstate_rw_att_conf_id_data_time ON att_array_devstate_rw (att_conf_id,data_time);




CREATE TABLE IF NOT EXISTS att_scalar_devenum_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r integer,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devenum_ro IS 'Scalar Enum ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devenum_ro_att_conf_id_data_time ON att_scalar_devenum_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_scalar_devenum_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    value_r integer,
    value_w integer,
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_scalar_devenum_rw IS 'Scalar Enum ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_scalar_devenum_rw_att_conf_id_data_time ON att_scalar_devenum_rw (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devenum_ro (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r integer[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devenum_ro IS 'Array Enum ReadOnly Values Table';
CREATE INDEX IF NOT EXISTS att_array_devenum_ro_att_conf_id_data_time ON att_array_devenum_ro (att_conf_id,data_time);

CREATE TABLE IF NOT EXISTS att_array_devenum_rw (
    att_conf_id integer NOT NULL,
    data_time timestamp with time zone,
    recv_time timestamp with time zone,
    insert_time timestamp with time zone,
    dim_x_r integer NOT NULL,
    dim_y_r integer NOT NULL,
    value_r integer[],
    dim_x_w integer NOT NULL,
    dim_y_w integer NOT NULL,
    value_w integer[],
    quality smallint,
    att_error_desc_id integer
);
COMMENT ON TABLE att_array_devenum_rw IS 'Array Enum ReadWrite Values Table';
CREATE INDEX IF NOT EXISTS att_array_devenum_rw_att_conf_id_data_time ON att_array_devenum_rw (att_conf_id,data_time);
