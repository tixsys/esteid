#include <iostream>
#include <QApplication>
#include <QDebug>
#include <QTextEdit>

#include "jscardmanager.h"
#include "cardlib/EstEidCard.h"

using namespace std;

JsCardManager::JsCardManager(JsEsteidCard *jsEsteidCard)
:	QObject( jsEsteidCard )
{
    m_jsEsteidCard = jsEsteidCard;
    
	cardMgr = NULL;
	try {
		cardMgr = new SmartCardManager();
	} catch ( std::runtime_error &e ) {
		qDebug() << e.what();
	}
    
	connect(&pollTimer, SIGNAL(timeout()),
            this, SLOT(pollCard()));
    pollTimer.start(2000);
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
						reader.cardId = QString::fromStdString( card->readCardID() );
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

		if ( !remove.isEmpty() )
		{
			m_jsEsteidCard->setCard( 0 );
			emit cardEvent( m_jsCardRemoveFunc, cardReaders[remove].id );
		}
		cardReaders = tmp;
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

bool JsCardManager::selectReader(int i)
{
	foreach( const ReaderState &reader, cardReaders )
	{
		if( reader.id == i && reader.connected )
			return selectReader( reader );
	}
	return false;
}

bool JsCardManager::selectReader( const ReaderState &reader )
{
    EstEidCard *card = 0;
    try {
        if (!cardMgr)
            cardMgr = new SmartCardManager();
		card = new EstEidCard(*cardMgr);
		card->connect( reader.id, true );
		m_jsEsteidCard->setCard(card, reader.id);
        return true;
    } catch (std::runtime_error &err) {
        handleError(err.what());
    }
	if( card )
        delete card;
	m_jsEsteidCard->setCard( 0 );
	return false;
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
{
	QTextEdit *edit = new QTextEdit( qApp->activeWindow() );
	edit->setAttribute( Qt::WA_DeleteOnClose );
	edit->setWindowFlags( Qt::Dialog );
	edit->setStyleSheet( "background: #F5F5F5;" );
	edit->setWindowTitle( tr("Diagnostics") );
	edit->setFixedSize( 300, 200 );
	edit->show();
}
