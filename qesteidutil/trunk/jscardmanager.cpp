#include <iostream>
#include <QDebug>

#include "jscardmanager.h"
#include "cardlib/EstEidCard.h"

using namespace std;

JsCardManager::JsCardManager(JsEsteidCard *jsEsteidCard)
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
        QVector<ReaderState> tmp;

        for (int i = 0; i < numReaders; i++) {
            ReaderState reader;
            reader.name = QString::fromStdString(cardMgr->getReaderName(i));
            if (card->isInReader(i)) {
                reader.connected = true;
            } else {
                reader.connected = false;
            }
            tmp.append(reader);
            if (cardReaders.value(i).connected != reader.connected) {
                if (reader.connected == true)
                    emit cardEvent(m_jsCardInsertFunc, i);
                else
                    emit cardEvent(m_jsCardRemoveFunc, i);
            }
        }
        cardReaders = tmp;
    } catch (std::runtime_error &) {
        // For now ignore any errors that might have happened during polling.
        // We don't want to spam users too often.
    }
}

void JsCardManager::findCard()
{
    // Valime välja ühe kaardi, mis on sisse pistetud. Hiljem saab kaarti
    // selectReader() funktsiooniga vahetada.

    try {
        if (!cardMgr)
            cardMgr = new SmartCardManager();
        EstEidCard *card = new EstEidCard(*cardMgr);

        int numReaders = cardMgr->getReaderCount();
        cardReaders.clear();

        for (int i = 0; i < numReaders; i++) {
            ReaderState reader;
            reader.name = QString::fromStdString(cardMgr->getReaderName(i));
            if (card->isInReader(i)) {
                card->connect(i);
                reader.connected = true;
                m_jsEsteidCard->setCard(card);
            } else {
                reader.connected = false;
            }
            cardReaders.append(reader);
        }
    } catch (std::runtime_error &err) {
        handleError(err.what());
    }
}

bool JsCardManager::selectReader(int i)
{
    try {
        if (!cardMgr)
            cardMgr = new SmartCardManager();
        EstEidCard *card = new EstEidCard(*cardMgr);

        int numReaders = cardMgr->getReaderCount();

        if (i < numReaders) {
            ReaderState reader;
            reader.name = QString::fromStdString(cardMgr->getReaderName(i));
            if (card->isInReader(i)) {
                card->connect(i);
                reader.connected = true;
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

    return cardMgr->getReaderCount();
}

QString JsCardManager::getReaderName(int i)
{
    if (!cardMgr)
        return 0;

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
