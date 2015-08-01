#! /usr/bin/env bash
$EXTRACTRC *.ui >> rc.cpp
$XGETTEXT `find . -name \*.qml -name \*.cpp` -o $podir/kcm_plasma_volume.pot
rm -f rc.cpp