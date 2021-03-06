cmake_minimum_required(VERSION 2.8)
include_directories(../include)
include_directories(../include/admin)
include_directories(../include/doh)
include_directories(../include/parsers)
include_directories(../include/sm)
include_directories(../include/utils)

add_executable(hello_test 
                hello_test.c 
                ../src/parsers/hello.c 
                ../src/utils/buffer.c)
target_link_libraries(hello_test check subunit rt m)
add_test(NAME hello_test COMMAND hello_test)

add_executable(negotiation_test 
                negotiation_test.c 
                ../src/parsers/negotiation.c 
                ../src/utils/buffer.c)
target_link_libraries(negotiation_test check subunit rt m)
add_test(NAME negotiation_test COMMAND negotiation_test)

add_executable(ettercap_test 
                ettercap_test.c 
                ../src/utils/base64.c
                ../src/parsers/ettercap.c 
                ../src/utils/buffer.c)
target_link_libraries(ettercap_test check subunit rt m)
add_test(NAME ettercap_test COMMAND ettercap_test)

add_executable(admin_test 
                admin_test.c 
                ../src/parsers/admin.c 
                ../src/utils/buffer.c)
target_link_libraries(admin_test check subunit rt m)
add_test(NAME admin_test COMMAND admin_test)

add_executable(buffer_test 
                buffer_test.c 
                ../src/utils/buffer.c)
target_link_libraries(buffer_test check subunit rt m)
add_test(NAME buffer_test COMMAND buffer_test)

add_executable(parser_test 
                parser_test.c 
                ../src/utils/parser.c)
target_link_libraries(parser_test check subunit rt m)
add_test(NAME parser_test COMMAND parser_test)

add_executable(parser_utils_test 
                parser_utils_test.c 
                ../src/utils/parser.c 
                ../src/utils/parser_utils.c)
target_link_libraries(parser_utils_test check subunit rt m)
add_test(NAME parser_utils_test COMMAND parser_utils_test)

add_executable(selector_test 
                selector_test.c)
target_link_libraries(selector_test check subunit rt m)
add_test(NAME selector_test COMMAND selector_test)

add_executable(stm_test 
                stm_test.c 
                ../src/utils/stm.c 
                ../src/utils/selector.c)
target_link_libraries(stm_test check subunit rt m)
add_test(NAME stm_test COMMAND stm_test)

add_executable(request_test 
                request_test.c 
                ../src/parsers/request.c 
                ../src/utils/buffer.c)
target_link_libraries(request_test check subunit rt m)
add_test(NAME request_test COMMAND request_test)

add_executable(netutils_test 
                netutils_test.c
                ../src/utils/netutils.c 
                ../src/utils/buffer.c)
target_link_libraries(netutils_test check subunit rt m)
add_test(NAME netutils_test COMMAND netutils_test)

add_executable(dohParser_test 
                dohParser_test.c 
                ../src/doh/dohParser.c
                ../src/doh/dohclient.c
                ../src/utils/buffer.c )
target_link_libraries(dohParser_test check subunit rt m)
add_test(NAME dohParser_test COMMAND dohParser_test)

add_executable(dohclient_test 
                dohclient_test.c 
                ../src/doh/dohclient.c
                ../src/doh/dohParser.c
                ../src/utils/buffer.c )
target_link_libraries(dohclient_test check subunit rt m)
add_test(NAME dohclient_test COMMAND dohclient_test)