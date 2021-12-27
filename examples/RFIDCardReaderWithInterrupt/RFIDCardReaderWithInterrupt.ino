/*
 * \brief Get latest RFID card ID when arrived (asynchronous)
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 27 December 2021
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2021 Quentin Comte-Gaz
 * \version 1.0
 */

#include <RFIDCardReader.h>

RFIDCardReader* card_reader = RFIDCardReader::getInstance();

static void checkRFIDCardID()
{
  unsigned long ms_since_card_found;
  String data;
  if (card_reader->getLastCard(data, ms_since_card_found, true))
  {
    //card_reader->resetLastCard();
    Serial.print("Card detected: ");
    Serial.print(data);
    Serial.print(" (time since card was found: ");
    Serial.print(ms_since_card_found/1000);
    Serial.print(" sec)\n");
  }
  else
  {
    Serial.print("No card detected (should not happen with interrupt)\n");
  }
}

void setup(void)
{
  Serial.begin(9600);
  card_reader->init();

  Serial.print("Begining RFID card reading...\n");

  card_reader->attachFunctionToCardDetectionEvent(checkRFIDCardID);
}

void loop()
{
  // Everything is asynchronous, so nothing to do in loop
}
