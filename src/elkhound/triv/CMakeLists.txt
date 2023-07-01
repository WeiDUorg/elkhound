#
# triv CMakeLists.txt
#
project(AdB)
project(CAdB)
project(EEb)
project(EFa)
project(ESb)
project(SSSx)
project(SSx)
project(aSEb)
project(angle)
project(ite)
project(testRR)

find_package(Perl)

if(PERL_EXECUTABLE)
    # generate AdB.gr
    add_custom_command(
      OUTPUT AdB.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser AdB < ${CMAKE_CURRENT_SOURCE_DIR}/AdB.gr.in > AdB.gr
    )

    # generate CAdB.gr
    add_custom_command(
      OUTPUT CAdB.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser CAdB < ${CMAKE_CURRENT_SOURCE_DIR}/CAdB.gr.in > CAdB.gr
    )

    # generate EEb.gr
    add_custom_command(
      OUTPUT EEb.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser EEb < ${CMAKE_CURRENT_SOURCE_DIR}/EEb.gr.in > EEb.gr
    )

    # generate EFa.gr
    add_custom_command(
      OUTPUT EFa.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser EFa < ${CMAKE_CURRENT_SOURCE_DIR}/EFa.gr.in > EFa.gr
    )

    # generate ESb.gr
    add_custom_command(
      OUTPUT ESb.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser ESb < ${CMAKE_CURRENT_SOURCE_DIR}/ESb.gr.in > ESb.gr
    )

    # generate SSSx.gr
    add_custom_command(
      OUTPUT SSSx.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser SSSx < ${CMAKE_CURRENT_SOURCE_DIR}/SSSx.gr.in > SSSx.gr
    )

    # generate SSx.gr
    add_custom_command(
      OUTPUT SSx.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser SSx < ${CMAKE_CURRENT_SOURCE_DIR}/SSx.gr.in > SSx.gr
    )

    # generate aSEb.gr
    add_custom_command(
      OUTPUT aSEb.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser aSEb < ${CMAKE_CURRENT_SOURCE_DIR}/aSEb.gr.in > aSEb.gr
    )

    # generate angle.gr
    add_custom_command(
      OUTPUT angle.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser angle < ${CMAKE_CURRENT_SOURCE_DIR}/angle.gr.in > angle.gr
    )

    # generate ite.gr
    add_custom_command(
      OUTPUT ite.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser ite < ${CMAKE_CURRENT_SOURCE_DIR}/ite.gr.in > ite.gr
    )

    # generate testRR.gr
    add_custom_command(
      OUTPUT testRR.gr
      COMMAND ${PERL_EXECUTABLE} ${SCRIPTS_DIR}/make-trivparser testRR < ${CMAKE_CURRENT_SOURCE_DIR}/testRR.gr.in > testRR.gr
    )

    # generate AdB.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT AdB.gr.gen.cc AdB.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o AdB.gr.gen AdB.gr
      DEPENDS elkhound AdB.gr
    )

    # generate CAdB.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT CAdB.gr.gen.cc CAdB.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o CAdB.gr.gen CAdB.gr
      DEPENDS elkhound CAdB.gr
    )

    # generate EEb.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT EEb.gr.gen.cc EEb.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o EEb.gr.gen EEb.gr
      DEPENDS elkhound EEb.gr
    )

    # generate EFa.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT EFa.gr.gen.cc EFa.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o EFa.gr.gen EFa.gr
      DEPENDS elkhound EFa.gr
    )

    # generate ESb.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT ESb.gr.gen.cc ESb.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o ESb.gr.gen ESb.gr
      DEPENDS elkhound ESb.gr
    )

    # generate SSSx.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT SSSx.gr.gen.cc SSSx.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o SSSx.gr.gen SSSx.gr
      DEPENDS elkhound SSSx.gr
    )

    # generate SSx.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT SSx.gr.gen.cc SSx.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o SSx.gr.gen SSx.gr
      DEPENDS elkhound SSx.gr
    )

    # generate aSEb.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT aSEb.gr.gen.cc aSEb.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o aSEb.gr.gen aSEb.gr
      DEPENDS elkhound aSEb.gr
    )

    # generate angle.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT angle.gr.gen.cc angle.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o angle.gr.gen angle.gr
      DEPENDS elkhound angle.gr
    )

    # generate ite.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT ite.gr.gen.cc ite.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o ite.gr.gen ite.gr
      DEPENDS elkhound ite.gr
    )

    # generate testRR.gr.gen.{cc,h}
    add_custom_command(
      OUTPUT testRR.gr.gen.cc testRR.gr.gen.h
      COMMAND ${ELKHOUND_DIR}/elkhound -tr bison,NOconflict,lrtable -o testRR.gr.gen testRR.gr
      DEPENDS elkhound testRR.gr
    )

    # all the files for AdB
    add_executable(AdB
      AdB.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for CAdB
    add_executable(CAdB
      CAdB.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for EEb
    add_executable(EEb
      EEb.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for EFa
    add_executable(EFa
      EFa.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for ESb
    add_executable(ESb
      ESb.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for SSSx
    add_executable(SSSx
      SSSx.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for SSx
    add_executable(SSx
      SSx.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for aSEb
    add_executable(aSEb
      aSEb.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for angle
    add_executable(angle
      angle.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for ite
    add_executable(ite
      ite.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # all the files for testRR
    add_executable(testRR
      testRR.gr.gen.cc
      ../trivmain.cc
      ../trivlex.cc
    )

    # extra compile options
    target_compile_options(AdB PRIVATE -DGRAMMAR_NAME="AdB")
    target_compile_options(CAdB PRIVATE -DGRAMMAR_NAME="CAdB")
    target_compile_options(EEb PRIVATE -DGRAMMAR_NAME="EEb")
    target_compile_options(EFa PRIVATE -DGRAMMAR_NAME="EFa")
    target_compile_options(ESb PRIVATE -DGRAMMAR_NAME="ESb")
    target_compile_options(SSSx PRIVATE -DGRAMMAR_NAME="SSSx")
    target_compile_options(SSx PRIVATE -DGRAMMAR_NAME="SSx")
    target_compile_options(aSEb PRIVATE -DGRAMMAR_NAME="aSEb")
    target_compile_options(angle PRIVATE -DGRAMMAR_NAME="angle")
    target_compile_options(ite PRIVATE -DGRAMMAR_NAME="ite")
    target_compile_options(testRR PRIVATE -DGRAMMAR_NAME="testRR")

    # link against libcparse and libelkhound
    target_link_libraries(AdB libcparse libelkhound)
    target_link_libraries(CAdB libcparse libelkhound)
    target_link_libraries(EEb libcparse libelkhound)
    target_link_libraries(EFa libcparse libelkhound)
    target_link_libraries(ESb libcparse libelkhound)
    target_link_libraries(SSSx libcparse libelkhound)
    target_link_libraries(SSx libcparse libelkhound)
    target_link_libraries(aSEb libcparse libelkhound)
    target_link_libraries(angle libcparse libelkhound)
    target_link_libraries(ite libcparse libelkhound)
    target_link_libraries(testRR libcparse libelkhound)

    if(BUILD_TESTING)
      add_test(
          NAME AdB1
          COMMAND AdB ${CMAKE_CURRENT_SOURCE_DIR}/AdB.in1
      )
      add_test(
          NAME CAdB1
          COMMAND CAdB ${CMAKE_CURRENT_SOURCE_DIR}/CAdB.in1
      )
      add_test(
          NAME EEb1
          COMMAND EEb ${CMAKE_CURRENT_SOURCE_DIR}/EEb.in1
      )
      add_test(
          NAME EEb2
          COMMAND EEb ${CMAKE_CURRENT_SOURCE_DIR}/EEb.in2
      )
      add_test(
          NAME EEb3
          COMMAND EEb ${CMAKE_CURRENT_SOURCE_DIR}/EEb.in3
      )
      add_test(
          NAME ESb1
          COMMAND ESb ${CMAKE_CURRENT_SOURCE_DIR}/ESb.in1
      )
      add_test(
          NAME aSEb1
          COMMAND aSEb ${CMAKE_CURRENT_SOURCE_DIR}/aSEb.in1
      )
      add_test(
          NAME angle1
          COMMAND angle ${CMAKE_CURRENT_SOURCE_DIR}/angle.in1
      )
      add_test(
          NAME angle2
          COMMAND angle ${CMAKE_CURRENT_SOURCE_DIR}/angle.in2
      )
      add_test(
          NAME angle3
          COMMAND angle ${CMAKE_CURRENT_SOURCE_DIR}/angle.in3
      )
      add_test(
          NAME angle4
          COMMAND angle ${CMAKE_CURRENT_SOURCE_DIR}/angle.in4
      )
      add_test(
          NAME angle5
          COMMAND angle ${CMAKE_CURRENT_SOURCE_DIR}/angle.in5
      )
      add_test(
          NAME ite1
          COMMAND ite ${CMAKE_CURRENT_SOURCE_DIR}/ite.in1
      )
      add_test(
          NAME ite2
          COMMAND ite ${CMAKE_CURRENT_SOURCE_DIR}/ite.in2
      )
      add_test(
          NAME ite3
          COMMAND ite ${CMAKE_CURRENT_SOURCE_DIR}/ite.in3
      )
      add_test(
          NAME ite4
          COMMAND ite ${CMAKE_CURRENT_SOURCE_DIR}/ite.in4
      )
      add_test(
          NAME ite5
          COMMAND ite ${CMAKE_CURRENT_SOURCE_DIR}/ite.in5
      )
      add_test(
          NAME testRR1
          COMMAND testRR ${CMAKE_CURRENT_SOURCE_DIR}/testRR.in1
      )
      add_test(
          NAME testRR2
          COMMAND testRR ${CMAKE_CURRENT_SOURCE_DIR}/testRR.in2
      )
    endif(BUILD_TESTING)
else()
    message(WARNING " * Skipping the triv example: Perl not found")
endif()
