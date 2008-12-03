option(COMPILE_WARNINGS "Whether to compile with compiler warnings" ON)
option(COMPILE_WARN_ERROR "Whether to treat compile warnings like errors" OFF)

if(COMPILE_WARNINGS)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wunused -Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpointer-arith")
endif(COMPILE_WARNINGS)

if(COMPILE_WARN_ERROR)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif(COMPILE_WARN_ERROR)


