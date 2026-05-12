
add_library(chadwick STATIC
    chadwick-0.10.0/src/cwlib/book.c
    chadwick-0.10.0/src/cwlib/box.c
    chadwick-0.10.0/src/cwlib/file.c
    chadwick-0.10.0/src/cwlib/game.c
    chadwick-0.10.0/src/cwlib/gameiter.c
    chadwick-0.10.0/src/cwlib/league.c
    chadwick-0.10.0/src/cwlib/lint.c
    chadwick-0.10.0/src/cwlib/parse.c
    chadwick-0.10.0/src/cwlib/roster.c
)

target_include_directories(chadwick PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/chadwick-0.10.0/src/cwlib
)
