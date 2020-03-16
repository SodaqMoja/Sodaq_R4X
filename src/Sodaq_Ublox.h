/*
Copyright (c) 2019, SODAQ
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _SODAQ_UBLOX_H
#define _SODAQ_UBLOX_H

#include <Arduino.h>

#define SODAQ_UBLOX_DEFAULT_RESPONSE_TIMEOUT            5000

enum GSMResponseTypes {
    GSMResponseNotFound = 0,
    GSMResponseOK = 1,
    GSMResponseError = 2,
    GSMResponseSocketPrompt = 3,
    GSMResponseFilePrompt = 4,
    GSMResponseTimeout = 5,
    GSMResponseEmpty = 6,
};

class Sodaq_OnOffBee
{
public:
    virtual ~Sodaq_OnOffBee() {}
    virtual void on()   = 0;
    virtual void off()  = 0;
    virtual bool isOn() = 0;
};

class Sodaq_Ublox
{
public:
    Sodaq_Ublox();
    virtual ~Sodaq_Ublox();

    // Sets the optional "Diagnostics and Debug" print.
    void setDiag(Print &print) { _diagPrint = &print; }
    void setDiag(Print *print) { _diagPrint = print; }

    // Sets the size of the input buffer.
    // Needs to be called before init().
    void setInputBufferSize(size_t value) { _inputBufferSize = value; };

protected:
    /***********************************************************/
    /* UART */

    void initUART(Uart& uart, uint32_t baud) { _modemUART = &uart; _baudRate = baud; }

    /***********************************************************/
    /* ON-OFF */

    // Sets the onoff instance
    void setOnOff(Sodaq_OnOffBee * onoff) { _onoff = onoff; }

    // Returns true if the modem is on.
    bool isOn() const;

    /***********************************************************/
    /* Serial In/Out to the modem via the UART */

    // Initializes the input buffer and makes sure it is only initialized once.
    // Safe to call multiple times.
    void initBuffer();

    const char* getInputBuffer() const { return _inputBuffer; }

    bool waitForPrompt(char prompt, uint32_t timeout);

    GSMResponseTypes readResponse(char* outBuffer = NULL, size_t outMaxSize = 0, const char* prefix = NULL,
                                  uint32_t timeout = SODAQ_UBLOX_DEFAULT_RESPONSE_TIMEOUT);
    virtual bool checkURC(const char* buffer) = 0;

    // Returns a character from the modem UART if read within _timeout ms or -1 otherwise.
    int timedRead(uint32_t timeout = 1000) const;

    // Fills the given "buffer" with characters read from the modem UART up to "length"
    // maximum characters and until the "terminator" character is found or a character read
    // times out (whichever happens first).
    // The buffer does not contain the "terminator" character or a null terminator explicitly.
    // Returns the number of characters written to the buffer, not including null terminator.
    size_t readBytesUntil(char terminator, char* buffer, size_t length, uint32_t timeout = 1000);

    // Fills the given "buffer" with up to "length" characters read from the modem UART.
    // It stops when a character read times out or "length" characters have been read.
    // Returns the number of characters written to the buffer.
    size_t readBytes(uint8_t* buffer, size_t length, uint32_t timeout = 1000);

    // Reads a line from the modem UART into the input buffer.
    // Returns the number of bytes read.
    size_t readLn(uint32_t timeout = 1000) { return readLn(_inputBuffer, _inputBufferSize, timeout); };

    // Reads a line from the modem UART into the "buffer". The line terminator is not
    // written into the buffer. The buffer is terminated with null.
    // Returns the number of bytes read, not including the null terminator.
    size_t readLn(char* buffer, size_t size, uint32_t timeout = 1000);

    // Write a byte
    size_t writeByte(uint8_t value);

    // Write the command prolog (just for debugging
    void writeProlog();

    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
    size_t print(const Printable&);

    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
    size_t println(const Printable&);
    size_t println(void);
    void dbprintln();

    /******************************************************************************
    * Utils
    *****************************************************************************/

    static uint32_t convertDatetimeToEpoch(int y, int m, int d, int h, int min, int sec);
    static bool startsWith(const char* pre, const char* str);

protected:
    // The (optional) stream to show debug information.
    Print* _diagPrint;

    // The on-off pin power controller object.
    Sodaq_OnOffBee* _onoff;

    // The UART that communicates with the device.
    Uart* _modemUART;

    // The requested baudrate
    uint32_t _baudRate;

private:
    // The size of the input buffer. Equals SODAQ_GSM_MODEM_DEFAULT_INPUT_BUFFER_SIZE
    // by default or (optionally) a user-defined value when using USE_DYNAMIC_BUFFER.
    size_t _inputBufferSize;

    // Flag to make sure the buffers are not allocated more than once.
    bool _isBufferInitialized;

    // The buffer used when reading from the modem. The space is allocated during init() via initBuffer().
    char* _inputBuffer;

    // This flag keeps track if the next write is the continuation of the current command
    // A Carriage Return will reset this flag.
    bool _appendCommand;
};

#endif /* _SODAQ_UBLOX_H */
