/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "birthdayparty.h"

BirthdayParty::BirthdayParty(QObject *parent)
: QObject(parent), m_celebrant(0)
{
}

Person *BirthdayParty::celebrant() const
{
    return m_celebrant;
}

void BirthdayParty::setCelebrant(Person *c)
{
    m_celebrant = c;
}

QmlListProperty<Person> BirthdayParty::guests() 
{
    return QmlListProperty<Person>(this, m_guests);
}

int BirthdayParty::guestCount() const
{
    return m_guests.count();
}

Person *BirthdayParty::guest(int index) const
{
    return m_guests.at(index);
}

QML_DEFINE_TYPE(People, 1,0, BirthdayParty, BirthdayParty);