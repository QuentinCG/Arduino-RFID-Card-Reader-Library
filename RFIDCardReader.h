/*
 * \brief Get RFID Card/Tag ID with RFID Card Reader
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 2 July 2016
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2016 Quentin Comte-Gaz
 * \version 1.0
 */

#ifndef RFIDCardReader_h
#define RFIDCardReader_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Only one port must be uncommented
#define RFIDCardReaderPort1 // Base UART port available on all Arduino devices
//#define RFIDCardReaderPort2 // Usable only with Arduino MEGA
//#define RFIDCardReaderPort3 // Usable only with Arduino MEGA

#define TrySerialAnyway // Force serial port to "Serial" if none found

class HardwareSerial;

class RFIDCardReader
{
  public:
    /*!
     * \brief getInstance Get a pointer to RFIDCardReader class
     *
     * \return (RFIDCardReader*) Pointer to RFIDCardReader class
     */
    static RFIDCardReader* getInstance();

    /*!
     * \brief init Initialize class
     *
     * \param baudrate (int) Baudrate of RFID Card reader (most of the time: 9600)
     */
    void init(int baudrate = 9600);

    /*!
     * \brief getLastCard Get the last Card ID found by the RFID Card Reader
     *
     * \param data (String)[out] Baudrate of RFID Card reader (most of the time: 9600)
     * \param ms_since_reception (unsigned long)[out] Time passed since the card was found
     * \param reset (bool)[in] Clean the card ID from buffer (else, the card will be stored until a card is detected).
     *
     * \return One card was received (--> values \p data and \p ms_since_reception are correct)
     */
    bool getLastCard(String &data, unsigned long &ms_since_reception, bool reset = true);

    /*!
     * \brief resetLastCard Clean the card ID from buffer
     *
     * This will force \p getLastCard to return false until a new card is detected.
     */
    void resetLastCard();

    /*!
     * \brief attachFunctionToCardDetectionEvent Attach a specific user function to "Card detected" event
     *
     * \param (void function()) Function that will be launched every time a card is detected
     */
    void attachFunctionToCardDetectionEvent(void (*pfunction)());




  //---------------------------------------------------------------------------
  //-------------------------- Implementation details -------------------------
  //---------------------------------------------------------------------------
  public:
    /*! !!Internal implementation!! (get one char by one char from RFID reader).
        Must be public because of HardwareSerial Arduino library implementation. */
    void _oneCharReceived(char in_char);

  private:
    RFIDCardReader(); /*!< Private constructor, please use RFIDCardReader::getInstance() to initialize the class */

  private:
    static RFIDCardReader* _instance; /*!< Singleton instance */
    void (*_attachedFunction)(); /*!< Attached function to launch every time a new card is fully received */
    HardwareSerial *_serial; /*!< Pointer to serial port used to communicate with the RFID card reader */
    String _pending_rx_card; /*!< Card ID which is in process to be retrieved */
    String _last_rx_card;  /*!< Last full received card ID */
    unsigned long _ms_since_last_card_read; /*!< Time since _last_rx_card has been received */
    bool _lock; /*!< Lock _last_rx_card and _ms_since_last_card_read (write vs reset) */
};

#endif //RDIFCardReader_h
