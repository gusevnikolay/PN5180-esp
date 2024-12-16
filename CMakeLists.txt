idf_component_register(SRCS 
                        "pn5180/pn5180.c"
                        "pn5180/pn5180_FeliCa.c"
                        "pn5180/pn5180_iclass.c"
                        "pn5180/pn5180_iso14443.c"
                        "pn5180/pn5180_iso15693.c"
                       INCLUDE_DIRS "pn5180")

target_compile_definitions(${COMPONENT_LIB} PRIVATE MG_ENABLE_MBEDTLS=1)