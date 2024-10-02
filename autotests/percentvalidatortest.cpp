// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: 2024 Harald Sitter <sitter@kde.org>

#include <QtTest/QTest>

#include "percentvalidator.h"

class PercentValidatorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test()
    {
        PercentValidator validator;
        QString input;
        int pos = 0;
        input = "1";
        QCOMPARE(validator.validate(input, pos), QValidator::Acceptable);
        input = "1%";
        QCOMPARE(validator.validate(input, pos), QValidator::Acceptable);
        input = "1 %";
        QCOMPARE(validator.validate(input, pos), QValidator::Acceptable);
        input = "1            ";
        QCOMPARE(validator.validate(input, pos), QValidator::Invalid);
        input = "1 % ";
        QCOMPARE(validator.validate(input, pos), QValidator::Invalid);
        input = "1 %   ";
        QCOMPARE(validator.validate(input, pos), QValidator::Invalid);
        input = "1   %";
        QCOMPARE(validator.validate(input, pos), QValidator::Invalid);
        input = "a 1%";
        QCOMPARE(validator.validate(input, pos), QValidator::Invalid);
        input = "1\%a";
        QCOMPARE(validator.validate(input, pos), QValidator::Invalid);
    }
};

QTEST_GUILESS_MAIN(PercentValidatorTest)

#include "percentvalidatortest.moc"
