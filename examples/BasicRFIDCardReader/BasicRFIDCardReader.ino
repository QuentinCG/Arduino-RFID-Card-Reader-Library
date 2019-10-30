/*
 * \brief Check latest RFID card ID every second (synchronous)
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 2 July 2016
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2016 Quentin Comte-Gaz
 * \version 1.0
 */

#include <RFIDCardReader.h>

RFIDCardReader* card_reader = RFIDCardReader::getInstance();

static void checkRFIDCardID()
{
  unsigned long ms_since_card_found;
  String data;
  if (card_reader->getLastCard(data, ms_since_card_found, true /*change this to false to keep card ID in memory when you got it*/)) {
    //card_reader->resetLastCard();
    Serial.print("Card detected: ");
    Serial.print(data);
    Serial.print(" (time since card was found: ");
    Serial.print(ms_since_card_found/1000);
    Serial.print(" sec)\n");
  } else {
    Serial.print("No card detected\n");
  }
}

void setup(void)
{
  Serial.begin(9600);
  card_reader->init();

  Serial.print("Begining RFID card reading...\n");

  //card_reader->attachFunctionToCardDetectionEvent(checkRFIDCardID);
}

void loop()
{
  checkRFIDCardID();
  delay(1000);
}
