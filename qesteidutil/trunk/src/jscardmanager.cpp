/*
 * QEstEidUtil
 *
 * Copyright (C) 2009 Jargo Kõster <jargo@innovaatik.ee>
 * Copyright (C) 2009 Raul Metsma <raul@innovaatik.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QTextEdit>

#include "jscardmanager.h"
#include "DiagnosticsDialog.h"
#include "cardlib/EstEidCard.h"

using namespace std;

JsCardManager::JsCardManager(JsEsteidCard *jsEsteidCard)
:	QObject( jsEsteidCard )
,	cardMgr( 0 )
,	m_jsEsteidCard( jsEsteidCard )
{
	try {
		cardMgr = new SmartCardManager();
	} catch ( std::runtime_error &e ) {
		qDebug() << e.what();
	}

	connect(&pollTimer, SIGNAL(timeout()),
            this, SLOT(pollCard()));
	pollTimer.start( 2000 );
}

JsCardManager::~JsCardManager()
{
	if( cardMgr )
		delete cardMgr;
}

void JsCardManager::pollCard()
{
    EstEidCard *card = 0;
    try {
		QString insert,remove;
		bool foundConnected = false;

        if (!cardMgr)
            cardMgr = new SmartCardManager();

		// Build current device list with statuses
        QHash<QString,ReaderState> tmp;
        int numReaders = cardMgr->getReaderCount();
        for (int i = 0; i < numReaders; i++) {
            ReaderState reader;
			reader.id = i;
            reader.name = QString::fromStdString(cardMgr->getReaderName(i));
			reader.state = QString::fromStdString( cardMgr->getReaderState( i ) );
			reader.connected = false;
			if ( cardReaders[reader.name].state != reader.state )
			{
				//card in use
				if ( !reader.state.contains( "EMPTY" ) )
				{
					if ( !card )
						card = new EstEidCard(*cardMgr);
					if ( card->isInReader(i) )
					{
						card->connect( i, true );
						reader.cardId = QString::fromStdString( card->readDocumentID() );
						reader.connected = true;
						if ( !foundConnected )
						{
							foundConnected = true;
							insert = reader.name;
						}
					}
				} else if ( !cardReaders[reader.name].cardId.isEmpty() && cardReaders[reader.name].connected )
					remove = reader.name;
			} else if ( cardReaders[reader.name].connected ) {
				foundConnected = true;
				reader.connected = true;
				reader.cardId = cardReaders[reader.name].cardId;
			}
			tmp[reader.name] = reader;
        }
		// check if connected card or reader has removed
		if ( cardReaders.size() > tmp.size() )
		{
			foreach( const ReaderState &r, cardReaders )
			{
				if ( r.connected && ( !tmp.contains( r.name ) || tmp[r.name].cardId.isEmpty() ) )
				{
					remove = r.name;
					break;
				}
			}
		}

		cardReaders = tmp;
		if ( !remove.isEmpty() )
		{
			m_jsEsteidCard->setCard( 0 );
			emit cardEvent( m_jsCardRemoveFunc, cardReaders[remove].id );
		}
		if ( !insert.isEmpty() )
			emit cardEvent( m_jsCardInsertFunc, cardReaders[insert].id );
		else if ( !foundConnected ) // Didn't find any connected reader, lets find one
		{
			foreach( const ReaderState &r, cardReaders )
			{
				if( !r.cardId.isEmpty() )
				{
					emit cardEvent( m_jsCardInsertFunc, r.id );
					foundConnected = true;
					break;
				}
			}
			if ( !foundConnected )
				emit cardEvent( m_jsCardRemoveFunc, 0 );
		}
    } catch (std::runtime_error &e) {
		qDebug() << e.what();
        // For now ignore any errors that might have happened during polling.
        // We don't want to spam users too often.
    }
    if( card )
        delete card;
}

bool JsCardManager::isInReader( const QString &cardId )
{
	if ( cardId == "" )
		return false;
	foreach( const ReaderState &r, cardReaders )
		if ( r.cardId == cardId )
			return true;
	return false;
}

bool JsCardManager::isInReader( int readerNum )
{
	foreach( const ReaderState &r, cardReaders )
		if ( r.id == readerNum && !r.state.contains( "EMPTY") )
			return true;
	return false;
}

QString JsCardManager::cardId( int readerNum )
{
	foreach( const ReaderState &r, cardReaders )
		if ( r.id == readerNum && !r.state.contains( "EMPTY") )
			return r.cardId;
	return "";
}

void JsCardManager::findCard()
{
	QCoreApplication::processEvents();
	foreach( const ReaderState &reader, cardReaders )
	{
		if( reader.connected )
		{
			selectReader( reader );
			return;
		}
	}
}

bool JsCardManager::selectReader(int i)
{
	QCoreApplication::processEvents();
	foreach( const ReaderState &reader, cardReaders )
	{
		if( reader.id == i && reader.connected )
			return selectReader( reader );
	}
	return false;
}

bool JsCardManager::selectReader( const ReaderState &reader )
{
	QCoreApplication::processEvents();
    EstEidCard *card = 0;
    try {
        if (!cardMgr)
            cardMgr = new SmartCardManager();
		card = new EstEidCard(*cardMgr);
		card->connect( reader.id, true );
		QCoreApplication::processEvents();
		m_jsEsteidCard->setCard(card, reader.id);
        return true;
    } catch (std::runtime_error &err) {
		//ignore Another application is using
		if ( QString::fromStdString( err.what() ).contains( "Another " ) )
			return false;
        handleError(err.what());
    }
	if ( card )
		delete card;
	m_jsEsteidCard->setCard( 0 );
	return false;
}

int JsCardManager::activeReaderNum()
{
	if ( !m_jsEsteidCard || !m_jsEsteidCard->m_card )
		return -1;
	return m_jsEsteidCard->m_reader;
}

int JsCardManager::getReaderCount()
{
    if (!cardMgr)
        return 0;

	int readers = 0;
	try {
		readers = cardMgr->getReaderCount();
	} catch( std::runtime_error & ) {}

    return readers;
}

QString JsCardManager::getReaderName(int i)
{
    if (!cardMgr)
        return "";

    return QString::fromStdString(cardMgr->getReaderName(i));
}

void JsCardManager::handleError(QString msg)
{
    qDebug() << "Error: " << msg << endl;
    emit cardError(m_jsHandleErrorFunc, msg);
}

void JsCardManager::registerCallBack(QString event, QString function)
{
    if (event == "cardInsert") {
        m_jsCardInsertFunc = function;
    } else if (event == "cardRemove") {
        m_jsCardRemoveFunc = function;
    } else if (event == "handleError") {
        m_jsHandleErrorFunc = function;
    }
}

void JsCardManager::showDiagnostics()
{ (new DiagnosticsDialog( 0 ) )->show(); }
