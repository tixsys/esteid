#include <iostream>
#include <QDebug>

#include "jscardmanager.h"
#include "cardlib/EstEidCard.h"

using namespace std;

JsCardManager::JsCardManager(JsEsteidCard *jsEsteidCard)
:	QObject( jsEsteidCard )
{
    m_jsEsteidCard = jsEsteidCard;
    cardMgr = NULL;

    connect(&pollTimer, SIGNAL(timeout()),
            this, SLOT(pollCard()));
    pollTimer.start(2000);
}

void JsCardManager::pollCard()
{
    try {
        if (!cardMgr)
            cardMgr = new SmartCardManager();
        EstEidCard *card = new EstEidCard(*cardMgr);

        int numReaders = cardMgr->getReaderCount();
        QHash<QString,ReaderState> tmp;

        for (int i = 0; i < numReaders; i++) {
            ReaderState reader;
            reader.name = QString::fromStdString(cardMgr->getReaderName(i));
            if (card->isInReader(i)) {
                reader.connected = true;
				card->connect( i, true );
				reader.cardId = QString::fromStdString( card->readCardID() );
            } else {
                reader.connected = false;
            }
			reader.id = i;
			tmp.insert( reader.name, reader );
			if ( cardReaders.contains( reader.name ) )
			{
				if ( cardReaders.value( reader.name ).connected != reader.connected )
				{
					cardReaders[reader.name] = reader;
					if (reader.connected)
					    emit cardEvent(m_jsCardInsertFunc, i);
					else
					    emit cardEvent(m_jsCardRemoveFunc, i);
				}
			} else if ( cardReaders.value( reader.name ).cardId != reader.cardId && reader.connected ) { //new reader inserted
				cardReaders[reader.name] = reader;
				emit cardEvent(m_jsCardInsertFunc, i);
			}
        }
		if ( cardReaders.size() > numReaders )
		{
			m_jsEsteidCard->setCard( 0 );
			cardReaders = tmp;
			emit cardEvent( m_jsCardRemoveFunc, 0 );
		}
        cardReaders = tmp;
    } catch (std::runtime_error &e) {
		qDebug() << e.what();
        // For now ignore any errors that might have happened during polling.
        // We don't want to spam users too often.
    }
}

void JsCardManager::findCard()
{
    // Valime välja ühe kaardi, mis on sisse pistetud. Hiljem saab kaarti
    // selectReader() funktsiooniga vahetada.

	cardReaders.clear();
	bool cardFound = false;
    try {
        if (!cardMgr)
            cardMgr = new SmartCardManager();
        EstEidCard *card = new EstEidCard(*cardMgr);

        int numReaders = cardMgr->getReaderCount();

        for (int i = 0; i < numReaders; i++) {
            ReaderState reader;
            reader.name = QString::fromStdString(cardMgr->getReaderName(i));
            if (card->isInReader(i)) {
                card->connect(i);
                reader.connected = true;
				reader.cardId = QString::fromStdString( card->readCardID() );
				if ( !cardFound )
				{
					m_jsEsteidCard->setCard(card);
					cardFound = true;
				}
            } else {
                reader.connected = false;
            }
			cardReaders.insert( reader.name, reader );
        }
    } catch (std::runtime_error &err) {
        handleError(err.what());
    }
	if ( !cardFound )
		m_jsEsteidCard->setCard( 0 );
}

bool JsCardManager::isInReader( QString cardId )
{
	if ( cardId == "" )
		return false;
	for ( QHash<QString, ReaderState>::const_iterator i = cardReaders.constBegin(); i != cardReaders.constEnd(); ++i )
		if ( i.value().cardId == cardId )
			return true;
	return false;
}

bool JsCardManager::selectReader(int i)
{
    try {
        if (!cardMgr)
            cardMgr = new SmartCardManager();
        EstEidCard *card = new EstEidCard(*cardMgr);

		int numReaders = cardMgr->getReaderCount();

        if ( i < numReaders ) {
            if (card->isInReader(i)) {
                card->connect(i);
                m_jsEsteidCard->setCard(card);
                return true;
            }
        }
        return false;
    } catch (std::runtime_error &err) {
        handleError(err.what());
    }
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
