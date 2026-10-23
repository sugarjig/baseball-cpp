
add_library(chadwick STATIC
    chadwick-0.11.0/src/cwlib/book.c
    chadwick-0.11.0/src/cwlib/box.c
    chadwick-0.11.0/src/cwlib/file.c
    chadwick-0.11.0/src/cwlib/game.c
    chadwick-0.11.0/src/cwlib/gameiter.c
    chadwick-0.11.0/src/cwlib/league.c
    chadwick-0.11.0/src/cwlib/lint.c
    chadwick-0.11.0/src/cwlib/parse.c
    chadwick-0.11.0/src/cwlib/roster.c
    chadwick-0.11.0/src/cwlib/book.h
    chadwick-0.11.0/src/cwlib/box.h
    chadwick-0.11.0/src/cwlib/chadwick.h
    chadwick-0.11.0/src/cwlib/file.h
    chadwick-0.11.0/src/cwlib/game.h
    chadwick-0.11.0/src/cwlib/gameiter.h
    chadwick-0.11.0/src/cwlib/league.h
    chadwick-0.11.0/src/cwlib/parse.h
    chadwick-0.11.0/src/cwlib/roster.h
    chadwick-0.11.0/src/cwlib/util.h
)

target_include_directories(chadwick PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/chadwick-0.11.0/src/cwlib
)
