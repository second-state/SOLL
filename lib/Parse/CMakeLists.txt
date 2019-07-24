add_library(sollParse
  ParseAST.cpp
  Parser.cpp
  )

target_link_libraries(sollParse
  sollAST
  sollBasic
  )
