#
# cparse test CMakeLists.txt
#
add_test(
  NAME cparse1
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in1
)
add_test(
  NAME cparse2
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in2
)
add_test(
  NAME cparse3
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in3
)
add_test(
  NAME cparse4
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in4
)
add_test(
  NAME cparse5
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in5
)
add_test(
  NAME cparse6
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in6
)
add_test(
  NAME cparse7
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in7
)
add_test(
  NAME cparse8
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in8
)
add_test(
  NAME cparse9
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in9
)
add_test(
  NAME cparse10
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in10
)
add_test(
  NAME cparse11
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in11
)
add_test(
  NAME cparse12
  COMMAND ${CPARSE_DIR}/cparse -tr stopAfterTCheck,suppressAddrOfError ${CMAKE_CURRENT_SOURCE_DIR}/c.in12
)
add_test(
  NAME cc2_1
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in1
)
add_test(
  NAME cc2_2
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in2
)
add_test(
  NAME cc2_3
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in3
)
add_test(
  NAME cc2_5
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in5
)
add_test(
  NAME cc2_6
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in6
)
add_test(
  NAME cc2_7
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in7
)
add_test(
  NAME cc2_8
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in8
)
add_test(
  NAME cc2_9
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in9
)
add_test(
  NAME cc2_10
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in10
)
add_test(
  NAME cc2_11
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in11
)
add_test(
  NAME cc2_12
  COMMAND ${CC2_DIR}/cc2 ${CMAKE_CURRENT_SOURCE_DIR}/c.in12
)
