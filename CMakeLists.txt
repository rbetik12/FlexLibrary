cmake_minimum_required(VERSION 3.17)
project(FlexLibrary C)

set(CMAKE_C_STANDARD 11)
include_directories(ui/include)
include_directories(books/include)
include_directories(network/include)
include_directories(utils/include)
add_executable(FlexLibrary main.c ui/include/ui.h ui/ui.c ui/include/ui_defines.h books/include/books.h books/include/books_defines.h books/books.c network/include/server.h network/include/client.h network/client.c network/server.c network/include/network_defines.h utils/include/utils.h utils/utils.c)
target_link_libraries(FlexLibrary ncurses form pthread)