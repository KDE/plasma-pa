install(FILES disable_kmix.upd DESTINATION ${SHARE_INSTALL_PREFIX}/kde4/apps/kconf_update)
install(PROGRAMS plasmaVolumeDisableKMixAutostart.pl DESTINATION ${SHARE_INSTALL_PREFIX}/kde4/apps/kconf_update)

install(FILES disable_kmix.upd DESTINATION ${KDE_INSTALL_DATADIR}/kconf_update)
install(PROGRAMS plasmaVolumeDisableKMixAutostart.pl DESTINATION ${KDE_INSTALL_DATADIR}/kconf_update)
