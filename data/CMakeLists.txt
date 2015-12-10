# installed twice for kmix builds on kdelibs4 and kf5
execute_process(COMMAND kde4-config --install data
                OUTPUT_VARIABLE KDE4_DATA_DIR
	        OUTPUT_STRIP_TRAILING_WHITESPACE
	        RESULT_VARIABLE kde4config_failed)

if (kde4config_failed)
    message(FAILED "Couldn't find kde4-config from kde4libs for kconf_update files")
else()
	execute_process(COMMAND kde4-config --prefix OUTPUT_VARIABLE KDE4_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REPLACE "${KDE4_PREFIX}/" ""  KDE4_PROPER_DATA_DIR ${KDE4_DATA_DIR})
	install(FILES disable_kmix.upd DESTINATION ${KDE4_PROPER_DATA_DIR}/kconf_update)
	install(PROGRAMS plasmaVolumeDisableKMixAutostart.pl DESTINATION ${KDE4_PROPER_DATA_DIR}/kconf_update)
endif()
install(FILES disable_kmix.upd DESTINATION ${KDE_INSTALL_DATADIR}/kconf_update)
install(PROGRAMS plasmaVolumeDisableKMixAutostart.pl DESTINATION ${KDE_INSTALL_DATADIR}/kconf_update)
