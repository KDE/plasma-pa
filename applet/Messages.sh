#! /usr/bin/env bash
$XGETTEXT `find . -name \*.js -o -name \*.qml` `find ../src/kcm/package/contents/ui -name \*.qml` -o $podir/plasma_applet_org.kde.plasma.volume.pot
