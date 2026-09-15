#!/bin/sh
# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2020-2025 Harald Sitter <sitter@kde.org>

# Ensure passing shellcheck:
# Our l10n scripting isn't working with spaces anywhere and we actively rely on word splitting in our Messages.sh.
# shellcheck disable=SC2046
podir=${podir:?} # ensure it is defined

# Extract JavaScripty files as what they are, otherwise for example template literals won't work correctly (by default we extract as C++).
# https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Template_literals
$XGETTEXT --language=JavaScript $(find . -name \*.qml -o -name \*.js) -o "$podir"/plasma_applet_org.kde.plasma.volume.pot
