// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#pragma once

#include <QIntValidator>
#include <qqmlregistration.h>

class PercentValidator : public QIntValidator
{
    Q_OBJECT
    QML_ELEMENT
public:
    using QIntValidator::QIntValidator;
    QValidator::State validate(QString &input, int &pos) const override;
};
