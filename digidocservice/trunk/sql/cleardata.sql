update "session" SET document_id = null;
delete from "document";
delete from "event";
delete from "session";
delete from "action_dict";
ALTER SEQUENCE "seq_action"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  RESTART 1
    CACHE 1  NO CYCLE;
ALTER SEQUENCE "seq_document"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  RESTART 1
    CACHE 1  NO CYCLE;
ALTER SEQUENCE "seq_event"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  RESTART 1
    CACHE 1  NO CYCLE;
ALTER SEQUENCE "seq_session"
    INCREMENT 1  MINVALUE 1
    MAXVALUE 9223372036854775807  RESTART 1
    CACHE 1  NO CYCLE;
