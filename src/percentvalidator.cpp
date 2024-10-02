// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include "percentvalidator.h"

using namespace Qt::StringLiterals;

QValidator::State PercentValidator::validate(QString &input_, int &pos) const
{
    QString input = input_;

    if (QIntValidator::validate(input, pos) == QValidator::Acceptable) {
        return QValidator::Acceptable;
    }

    bool terminated = false;
    for (const auto &c : input) {
        auto isPercent = c == '%'_L1;
        if (terminated) {
            // Anything after the termination character is invalid
            return QValidator::Invalid;
        }
        if (!(c.isDigit() || c.isSpace() || isPercent)) {
            // Before termination we only allow a limited set of characters
            return QValidator::Invalid;
        }
        terminated = isPercent;
    }

    // Special handling for excess spacing. Also not valid.
    if (input.contains(u"  "_s)) {
        return QValidator::Invalid;
    }

    // clean up the string for int validator
    input = input.remove('%'_L1);
    input = input.simplified();

    return QIntValidator::validate(input, pos);
}
