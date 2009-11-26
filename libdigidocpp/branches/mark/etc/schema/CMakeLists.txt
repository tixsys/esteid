set( SCHEMA_FILES
    conf.xsd
    datatypes.dtd
    OpenDocument_manifest.xsd
    XAdES.xsd
    xmldsig-core-schema.xsd
    XMLSchema.dtd
)
install(FILES ${SCHEMA_FILES} DESTINATION ${DIGIDOCPP_CONFIG_DIR}/schema)
