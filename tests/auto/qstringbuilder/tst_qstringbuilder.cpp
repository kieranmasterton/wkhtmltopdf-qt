/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

// This is included in various .cpp files as a compile test for various scenarios
// depending on NO_CAST_*  and  QT_USE_FAST_OPERATOR_PLUS and QT_USE_FAST_CONCATENATION

#if SCENARIO == 1
// this is the "no harm done" version. Only operator% is active,
// with NO_CAST * defined
#define P %
#undef QT_USE_FAST_OPERATOR_PLUS
#undef QT_USE_FAST_CONCATENATION
#define QT_NO_CAST_FROM_ASCII
#define QT_NO_CAST_TO_ASCII
#endif


#if SCENARIO == 2
// this is the "full" version. Operator+ is replaced by a QStringBuilder
// based version
// with NO_CAST * defined
#define P +
#define QT_USE_FAST_OPERATOR_PLUS
#define QT_USE_FAST_CONCATENATION
#define QT_NO_CAST_FROM_ASCII
#define QT_NO_CAST_TO_ASCII
#endif

#if SCENARIO == 3
// this is the "no harm done" version. Only operator% is active,
// with NO_CAST * _not_ defined
#define P %
#undef QT_USE_FAST_OPERATOR_PLUS
#undef QT_USE_FAST_CONCATENATION
#undef QT_NO_CAST_FROM_ASCII
#undef QT_NO_CAST_TO_ASCII
#endif

#if SCENARIO == 4
// this is the "full" version. Operator+ is replaced by a QStringBuilder
// based version
// with NO_CAST * _not_ defined
#define P +
#define QT_USE_FAST_OPERATOR_PLUS
#define QT_USE_FAST_CONCATENATION
#undef QT_NO_CAST_FROM_ASCII
#undef QT_NO_CAST_TO_ASCII
#endif


#include <QtTest/QtTest>

//TESTED_CLASS=QStringBuilder
//TESTED_FILES=qstringbuilder.cpp

#include <qtest.h>

#define LITERAL "some literal"

class tst_QStringBuilder : public QObject
{
    Q_OBJECT

public:
    tst_QStringBuilder() {}
    ~tst_QStringBuilder() {}

public slots:
    void init() {}
    void cleanup() {}

    void scenario();
};

void tst_QStringBuilder::scenario()
{
    QLatin1Literal l1literal(LITERAL);
    QLatin1String l1string(LITERAL);
    QString string(l1string);
    QStringRef stringref(&string, 2, 10);
    QLatin1Char achar('c');
    QString r2(QLatin1String(LITERAL LITERAL));
    QString r;

    r = l1literal P l1literal;
    QCOMPARE(r, r2);
    r = string P string;
    QCOMPARE(r, r2);
    r = stringref P stringref;
    QCOMPARE(r, QString(stringref.toString() + stringref.toString()));
    r = string P l1literal;
    QCOMPARE(r, r2);
    r = string P l1string;
    QCOMPARE(r, r2);
    r = string + achar;
    QCOMPARE(r, QString(string P achar));
    r = achar + string;
    QCOMPARE(r, QString(achar P string));
#ifndef QT_NO_CAST_FROM_ASCII
    r = string P LITERAL;
    QCOMPARE(r, r2);
    r = LITERAL P string;
    QCOMPARE(r, r2);
#endif
}

QTEST_APPLESS_MAIN(tst_QStringBuilder)

#include "tst_qstringbuilder.moc"