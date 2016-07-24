project(MetaProcess)
cmake_minimum_required(VERSION 2.8)

set (CMAKE_CXX_STANDARD 11)

find_package(Qt5Widgets)
find_package(Qt5PrintSupport REQUIRED)
find_package(Armadillo REQUIRED)

include_directories(${MetaProcess_SOURCE_DIR}/include
                    ${MetaProcess_BINARY_DIR}
                    ${ARMADILLO_INCLUDE_DIRS}
                    )

set(METAP_CPP

                                                src/brent.cpp
                                                src/mainwindow.cpp
                                                src/analysisdf.cpp
                                                src/completewiz.cpp
                                                src/dataframe.cpp
                                                src/datasetmodel.cpp
                                                src/imputationpage.cpp
                                                src/multivardial.cpp
                                                src/noisefillpage.cpp
                                                src/normpage.cpp
                                                src/peakfilterpage.cpp
                                                src/processdf.cpp
                                                src/qassurancepage.cpp
                                                src/qcustomplot.cpp
                                                src/scalpage.cpp
                                                src/tabselmodel.cpp
                                                src/transfpage.cpp
                                                src/ttestdial.cpp
                                                src/viewmodel.cpp
    )

set(METAP_H
                                                include/brent.hpp
                                                include/multitestresult.h
                                                include/npq.h
                                                include/t_funcs.h
                                                include/wilcox_func_2.h
                                                include/wilcox_funcs.h
    )

set(METAP_H_MOCABLE
                                                include/mainwindow.h
                                                include/analysisdf.h
                                                include/completewiz.h
                                                include/dataframe.h
                                                include/datasetmodel.h
                                                include/imputationpage.h
                                                include/multivardial.h
                                                include/noisefillpage.h
                                                include/normpage.h
                                                include/peakfilterpage.h
                                                include/processdf.h
                                                include/qassurancepage.h
                                                include/qcustomplot.h
                                                include/scalpage.h
                                                include/tabselmodel.h
                                                include/transfpage.h
                                                include/ttestdial.h
                                                include/viewmodel.h
    )

set(METAP_DEMO_APP_SRC src/main.cpp)

set (METAP_UI  ${MetaProcess_SOURCE_DIR}/mainwindow.ui)

qt5_wrap_cpp(METAP_H_moc  ${METAP_H_MOCABLE})
qt5_wrap_ui(METAP_UI_moc  ${METAP_UI})

add_library(MetaP STATIC ${METAP_CPP} ${METAP_H_moc} ${METAP_H_MOCABLE} ${METAP_H} ${METAP_UI_moc})

add_executable(MetaProcess ${METAP_DEMO_APP_SRC})

target_link_libraries (MetaP Qt5::Widgets Qt5::PrintSupport)

target_link_libraries(MetaProcess ${ARMADILLO_LIBRARIES} MetaP)
