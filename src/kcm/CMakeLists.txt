configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set(kcm_SRCS
#    main.cpp
    module.cpp
)

set(lib_SRCS
    ../card.cpp
    ../client.cpp
    ../context.cpp
    ../pulseaudio.cpp
    ../pulseobject.cpp
    ../sink.cpp
    ../sinkinput.cpp
    ../source.cpp
    ../sourceoutput.cpp
)

qt5_wrap_ui(kcm_SRCS module.ui)

add_library(kcm_pulseaudio MODULE ${kcm_SRCS} ${lib_SRCS})

target_link_libraries(kcm_pulseaudio
    Qt5::Quick
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::Declarative
    KF5::I18n
    KF5::QuickAddons
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
)

kcoreaddons_desktop_to_json(kcm_pulseaudio "kcm_pulseaudio.desktop")

install(FILES kcm_pulseaudio.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS kcm_pulseaudio DESTINATION ${PLUGIN_INSTALL_DIR}/kcms)

kpackage_install_package(package kcm_pulseaudio kcms)
