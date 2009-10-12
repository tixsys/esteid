/*
 * QEstEidUtil
 *
 * Copyright (C) 2009 Jargo K�ster <jargo@innovaatik.ee>
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
#include "smartcard++/esteid/EstEidCard.h"

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

	//wait javascript/html to initialize
	QTimer::singleShot( 2000, this, SLOT(pollCard()) );
}

JsCardManager::~JsCardManager()
{
	if( cardMgr )
		delete cardMgr;
}

void JsCardManager::pollCard()
{

	if ( !pollTimer.isActive() )
		pollTimer.start( 500 );

	int numReaders = 0;
    try {
		QString insert,remove;
		bool foundConnected = false;

        if (!cardMgr)
            cardMgr = new SmartCardManager();

		// Build current device list with statuses
        QHash<QString,ReaderState> tmp;
        numReaders = cardMgr->getReaderCount( true );
        for (int i = 0; i < numReaders; i++) {
            ReaderState reader;
			reader.id = i;
            reader.name = QString::fromStdString(cardMgr->getReaderName(i));
			reader.state = QString::fromStdString( cardMgr->getReaderState( i ) );
			if ( reader.state.contains( "PRESENT" ) )
				reader.state = "PRESENT";
			reader.connected = false;
			if ( !cardReaders.contains(reader.name) || ( cardReaders.contains(reader.name) && cardReaders.value(reader.name).state != reader.state ) )
			{
				//card in use
				if ( !reader.state.contains( "EMPTY" ) )
				{
					EstEidCard card(*cardMgr);
					if ( card.isInReader(i) )
					{
						card.connect( i );
						reader.cardId = QString::fromStdString( card.readDocumentID() );
						reader.connected = true;
						foundConnected = true;
						insert = reader.name;
					}
				} else if ( !cardReaders.value(reader.name).cardId.isEmpty() && cardReaders.value(reader.name).connected )
					remove = reader.name;
			} else if ( cardReaders.value(reader.name).connected ) {
				foundConnected = true;
				reader.connected = true;
				reader.cardId = cardReaders.value(reader.name).cardId;
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
			if ( remove.isEmpty() )
				remove = "empty";
		} else if ( cardReaders.size() < tmp.size() && insert.isEmpty() )
			insert = "empty";

		if ( !remove.isEmpty() )
		{
			if ( m_jsEsteidCard->m_card && m_jsEsteidCard->getDocumentId() == cardReaders[remove].cardId )
				m_jsEsteidCard->setCard( 0 );
			cardReaders = tmp;
			emit cardEvent( "cardRemoved", remove != "empty" ? cardReaders[remove].id : -1 );
		}
		cardReaders = tmp;
		if ( !insert.isEmpty() )
			emit cardEvent( "cardInserted", insert != "empty" ? cardReaders[insert].id : -1 );
		else if ( !foundConnected ) // Didn't find any connected reader, lets find one
			findCard();
    } catch (std::runtime_error &e) {
		qDebug() << e.what();
		if ( cardReaders.size() > 0 && numReaders == 0 )
		{
			cardReaders = QHash<QString,ReaderState>();
			emit cardEvent( "cardRemoved", cardReaders.value(0).id );
		}
        // For now ignore any errors that might have happened during polling.
        // We don't want to spam users too often.
    }
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
	if ( !cardMgr )
		return;

	try {
		if ( getReaderCount() < 1 )
			return;
	} catch ( const std::runtime_error &e ) {
		qDebug() << e.what();
		return;
	}

	m_jsEsteidCard->setCard( 0 );
	QCoreApplication::processEvents();
	foreach( const ReaderState &reader, cardReaders )
	{
		if( reader.connected && !reader.name.isEmpty() )
		{
			selectReader( reader );
			return;
		}
	}
}

bool JsCardManager::anyCardsInReader()
{
	if ( !cardMgr )
		return false;

	try {
		if ( getReaderCount() < 1 )
			return false;
	} catch ( const std::runtime_error &e ) {
		qDebug() << e.what();
		return false;
	}

	foreach( const ReaderState &reader, cardReaders )
		if( reader.connected && !reader.name.isEmpty() )
			return true;
	return false;
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
		card->connect( reader.id );
		QCoreApplication::processEvents();
		m_jsEsteidCard->setCard(card, reader.id);
        return true;
    } catch (std::runtime_error &err) {
		//ignore Another application is using
		if ( QString::fromStdString( err.what() ).contains( "Another " ) )
			return false;
        handleError(err.what());
    }
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
		readers = cardMgr->getReaderCount( true );
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
    emit cardError( "handleError", msg);
}

void JsCardManager::showDiagnostics()
{ (new DiagnosticsDialog( qApp->activeWindow() ) )->show(); }
