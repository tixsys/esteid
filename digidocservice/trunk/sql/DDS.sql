CREATE SEQUENCE "seq_action"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  START 1
    CACHE 1;

COMMENT ON SEQUENCE "seq_action"
IS 'Actions dict seq';

CREATE SEQUENCE "seq_document"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  START 1
    CACHE 1;
    
CREATE SEQUENCE "seq_event"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  START 1
    CACHE 1;
    
CREATE SEQUENCE "seq_session"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  START 1
    CACHE 1;

COMMENT ON SEQUENCE "seq_session"
IS 'SESSION PK';

CREATE TABLE "session" (
  "id" BIGINT NOT NULL, 
  "code" VARCHAR(32), 
  "host" VARCHAR(75), 
  "create_date" BIGINT, 
  "access_date" BIGINT, 
  "close_date" BIGINT, 
  "document_id" BIGINT
) WITH OIDS;

CREATE TABLE "event" (
  "id" BIGINT NOT NULL, 
  "session_id" BIGINT, 
  "event_time" BIGINT, 
  "action_id" INTEGER, 
  "comment" VARCHAR(255), 
  "parent_ref_event_id" BIGINT, 
) WITH OIDS;

CREATE TABLE "document" (
  "id" BIGINT NOT NULL, 
  "data" BYTEA NOT NULL, 
  "comment" VARCHAR
) WITH OIDS;

CREATE TABLE "action_dict" (
  "id" BIGINT NOT NULL, 
  "name" VARCHAR(50), 
  "valid" BOOLEAN DEFAULT true, 
  CONSTRAINT "action_dict_index01" PRIMARY KEY("id")
) WITHOUT OIDS;

COMMENT ON TABLE "action_dict"
IS 'Actions classificator dict';

COMMENT ON COLUMN "session"."id"
IS 'Parent key';

COMMENT ON COLUMN "session"."code"
IS 'Session code';

COMMENT ON COLUMN "session"."host"
IS 'session created IP';

COMMENT ON COLUMN "session"."create_date"
IS 'Session open date time';

COMMENT ON COLUMN "session"."access_date"
IS 'Session change date';

COMMENT ON COLUMN "session"."close_date"
IS 'Session close date';

COMMENT ON COLUMN "session"."document_id"
IS 'ref to document table';


