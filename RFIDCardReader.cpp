/*
 * \brief Get RFID Card/Tag ID with RFID Card Reader (implementation)
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 2 July 2016
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2016 Quentin Comte-Gaz
 * \version 1.0
 */

#include "HardwareSerial.h"
#include "RFIDCardReader.h"

//--------------------- Singleton (& private Constructor) ---------------------
RFIDCardReader* RFIDCardReader::_instance = NULL;

RFIDCardReader* RFIDCardReader::getInstance()
{
  if (!_instance) {
    _instance = new RFIDCardReader();
  }
  return _instance;
}

RFIDCardReader::RFIDCardReader() :
  _attachedFunction (NULL),
  _serial (NULL),
  _pending_rx_card (""),
  _last_rx_card (""),
  _ms_since_last_card_read (0),
  _lock (false)
{
  // Associate correct serial port to the class
  #if defined(HAVE_HWSERIAL1) && defined(RFIDCardReaderPort1)
    _serial = &Serial1;
  #elif defined(HAVE_HWSERIAL0) && defined(RFIDCardReaderPort1)
    _serial = &Serial;
  #elif defined(HAVE_HWSERIAL2) && defined(RFIDCardReaderPort2)
    _serial = &Serial2;
  #elif defined(HAVE_HWSERIAL3) && defined(RFIDCardReaderPort3)
    _serial = &Serial3;
  #elif defined(TrySerialAnyway)
    _serial = &Serial;
  #else
    ERROR, PLEASE DEFINE AT LEAST ONE CORRECT RFIDCardReaderPort IN RFIDCardReader.h
  #endif
}

//----------------------------- Public functions ------------------------------
void RFIDCardReader::init(int baudrate)
{
  _serial->begin(baudrate);
}

bool RFIDCardReader::getLastCard(String & data, unsigned long &ms_since_reception, bool reset)
{
  data = _last_rx_card;
  unsigned long current_time = millis();
  ms_since_reception = current_time - _ms_since_last_card_read;

  // Note: This assertion may be wrong once every 50day because of millis() implementation
  // https://www.arduino.cc/en/Reference/Millis
  bool last_card_valid = (current_time >= _ms_since_last_card_read) &&
                         (_ms_since_last_card_read != 0);
  if (!last_card_valid) {
    data = "";
    ms_since_reception = 0;
  }

  if (last_card_valid && reset) {
    resetLastCard();
  }

  return last_card_valid;
}

void RFIDCardReader::resetLastCard()
{
  while (_lock) {
    delay(1);
  }
  _lock = true;
  _last_rx_card = "";
  _ms_since_last_card_read = 0;
  _lock = false;
}

void RFIDCardReader::attachFunctionToCardDetectionEvent(void (*pfunction)())
{
  _attachedFunction = pfunction;
}

//---------------------------- Internal functions -----------------------------

void RFIDCardReader::_oneCharReceived(char in_char)
{
  //Serial.print(in_char);
  if ((in_char >= 48 && in_char <= 57) ||
      (in_char >= 65 && in_char <= 90) ||
      (in_char >= 97 && in_char <= 122)) {
    _pending_rx_card.concat(in_char);
  } else {
    if (_pending_rx_card.length() > 0 && !_lock) {
      _lock = true; // To have no issue between reset and reception
      _last_rx_card = _pending_rx_card;
      _ms_since_last_card_read = millis();
      _lock = false;
      if (_attachedFunction != NULL) {
        _attachedFunction();
      }
    }
    _pending_rx_card = "";
  }
}

//////////////////// Serial Events /////////////////////

#if defined(HAVE_HWSERIAL1) && defined(RFIDCardReaderPort1) // Arduino MEGA Only: UART 1
void serialEvent1() {
  while (Serial1.available() > 0) {
    RFIDCardReader::getInstance()->_oneCharReceived((char)Serial1.read());
  }
}
#elif defined(HAVE_HWSERIAL0) && defined(RFIDCardReaderPort1) // All Arduino: UART 0
void serialEvent() {
  while (Serial.available() > 0) {
    RFIDCardReader::getInstance()->_oneCharReceived((char)Serial.read());
  }
}
#elif defined(HAVE_HWSERIAL2) && defined(RFIDCardReaderPort2) // Arduino MEGA Only: UART 2
void serialEvent2() {
  while (Serial2.available() > 0) {
    RFIDCardReader::getInstance()->_oneCharReceived((char)Serial2.read());
  }
}
#elif defined(HAVE_HWSERIAL3) && defined(RFIDCardReaderPort3) // Arduino MEGA Only: UART 3
void serialEvent3() {
  while (Serial3.available() > 0) {
    RFIDCardReader::getInstance()->_oneCharReceived((char)Serial3.read());
  }
}
#elif defined(TrySerialAnyway)
void serialEvent() {
  while (Serial.available() > 0) {
    RFIDCardReader::getInstance()->_oneCharReceived((char)Serial.read());
  }
}
#else
  ERROR, PLEASE DEFINE AT LEAST ONE CORRECT RFIDCardReaderPort IN RFIDCardReader.h
#endif
