/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QStringList>
#include <qtest.h>

class tst_QString: public QObject
{
    Q_OBJECT
private slots:
    void equals() const;
    void equals_data() const;
};

void tst_QString::equals() const
{
    QFETCH(QString, a);
    QFETCH(QString, b);

    QBENCHMARK {
        a == b;
    }
}

void tst_QString::equals_data() const
{
    static const struct {
        ushort data[80];
        int dummy;              // just to ensure 4-byte alignment
    } data = {
        {
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 16
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 32
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 48
            64, 64, 64, 64,  64, 64, 64, 64,
            64, 64, 64, 64,  64, 64, 64, 64, // 64
            64, 64, 64, 64,  96, 96, 96, 96,
            64, 64, 96, 96,  96, 96, 96, 96  // 80
        }, 0
    };
    const QChar *ptr = reinterpret_cast<const QChar *>(data.data);

    QTest::addColumn<QString>("a");
    QTest::addColumn<QString>("b");
    QString base = QString::fromRawData(ptr, 64);

    QTest::newRow("different-length") << base << QString::fromRawData(ptr, 4);
    QTest::newRow("same-string") << base << base;
    QTest::newRow("same-data") << base << QString::fromRawData(ptr, 64);

    // try to avoid crossing a cache line (that is, at ptr[64])
    QTest::newRow("aligned-aligned-4n")
            << QString::fromRawData(ptr, 60) << QString::fromRawData(ptr + 2, 60);
    QTest::newRow("aligned-unaligned-4n")
            << QString::fromRawData(ptr, 60) << QString::fromRawData(ptr + 1, 60);
    QTest::newRow("unaligned-unaligned-4n")
            << QString::fromRawData(ptr + 1, 60) << QString::fromRawData(ptr + 3, 60);

    QTest::newRow("aligned-aligned-4n+1")
            << QString::fromRawData(ptr, 61) << QString::fromRawData(ptr + 2, 61);
    QTest::newRow("aligned-unaligned-4n+1")
            << QString::fromRawData(ptr, 61) << QString::fromRawData(ptr + 1, 61);
    QTest::newRow("unaligned-unaligned-4n+1")
            << QString::fromRawData(ptr + 1, 61) << QString::fromRawData(ptr + 3, 61);

    QTest::newRow("aligned-aligned-4n-1")
            << QString::fromRawData(ptr, 59) << QString::fromRawData(ptr + 2, 59);
    QTest::newRow("aligned-unaligned-4n-1")
            << QString::fromRawData(ptr, 59) << QString::fromRawData(ptr + 1, 59);
    QTest::newRow("unaligned-unaligned-4n-1")
            << QString::fromRawData(ptr + 1, 59) << QString::fromRawData(ptr + 3, 59);

    QTest::newRow("aligned-aligned-2n")
            << QString::fromRawData(ptr, 58) << QString::fromRawData(ptr + 2, 58);
    QTest::newRow("aligned-unaligned-2n")
            << QString::fromRawData(ptr, 58) << QString::fromRawData(ptr + 1, 58);
    QTest::newRow("unaligned-unaligned-2n")
            << QString::fromRawData(ptr + 1, 58) << QString::fromRawData(ptr + 3, 58);
}

QTEST_MAIN(tst_QString)

#include "main.moc"