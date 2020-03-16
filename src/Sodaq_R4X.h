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

#ifndef _Sodaq_R4X_h
#define _Sodaq_R4X_h

#define R4X_DEFAULT_RESPONSE_TIMEOUT            5000
#define SODAQ_MAX_SEND_MESSAGE_SIZE     512
#define SODAQ_R4X_DEFAULT_CID           1
#define SODAQ_R4X_DEFAULT_READ_TIMOUT   15000
#define SODAQ_R4X_MAX_SOCKET_BUFFER     1024

#define SODAQ_R4X_LTEM_URAT             "7"
#define SODAQ_R4X_NBIOT_URAT            "8"
#define SODAQ_R4X_2G_URAT               "9"
#define SODAQ_R4X_LTEM_NBIOT_URAT       "7,8"
#define SODAQ_R4X_LTEM_2G_URAT          "7,9"
#define SODAQ_R4X_NBIOT_2G_URAT         "8,9"
#define SODAQ_R4X_LTEM_NBIOT_2G_URAT    "7,8,9"

#define DEFAULT_URAT                    SODAQ_R4X_NBIOT_URAT
#define AUTOMATIC_OPERATOR              "0"
#define BAND_MASK_UNCHANGED             0

#include <Arduino.h>

#include "Sodaq_Ublox.h"

enum HttpRequestTypes {
    POST = 0,
    GET,
    HEAD,
    DELETE,
    PUT,
    HttpRequestTypesMAX
};

enum MNOProfiles {
    SWD_DEFAULT     = 0,
    SIM_ICCID       = 1,
    ATT             = 2,
    VERIZON         = 3,
    TELSTRA         = 4,
    T_MOBILE_US     = 5,
    CHINA_TELECOM   = 6,
    VODAFONE        = 19,
    STANDARD_EUROPE = 100
};

enum Protocols {
    TCP = 0,
    UDP
};

enum SimStatuses {
    SimStatusUnknown = 0,
    SimMissing,
    SimNeedsPin,
    SimReady
};

enum TriBoolStates
{
    TriBoolFalse,
    TriBoolTrue,
    TriBoolUndefined
};

typedef TriBoolStates tribool_t;

typedef void(*PublishHandlerPtr)(const char* topic, const char* msg);

#define BAND_TO_MASK(x) (1 << (x - 1))

#define SOCKET_COUNT 7

class Sodaq_SARA_R4XX_OnOff : public Sodaq_OnOffBee
{
public:
    Sodaq_SARA_R4XX_OnOff();
    void on();
    void off();
    bool isOn();
private:
    bool _onoff_status;
};

class Sodaq_R4X : public Sodaq_Ublox
{
public:
    /******************************************************************************
    * Main
    *****************************************************************************/

    Sodaq_R4X();

    // Initializes the modem instance. Sets the modem UART and the on-off power pins.
    void init(Sodaq_OnOffBee* onoff, Uart& uart, uint32_t baud, uint8_t cid = SODAQ_R4X_DEFAULT_CID);

    // Turns the modem on/off and returns true if successful.
    bool on();
    bool off();

    void switchEchoOff();

    bool enableHexMode();

    // Turns on and initializes the modem, then connects to the network and activates the data connection.
    bool connect(const char* apn, const char* urat = DEFAULT_URAT,
        const char* bandMask = BAND_MASK_UNCHANGED);
    bool connect(const char* apn, const char* urat, uint8_t mnoProfile,
        const char* operatorSelect = AUTOMATIC_OPERATOR, const char* bandMaskLTE = BAND_MASK_UNCHANGED, 
        const char* bandMaskNB = BAND_MASK_UNCHANGED);
    void setConnectTimeout(uint32_t t) { _connect_timeout = t; }

    // Disconnects the modem from the network.
    bool disconnect();

#define R4X_DEFAULT_ATTACH_TIMEOUT      (10L * 60L * 1000)
    bool attachGprs(uint32_t timeout = R4X_DEFAULT_ATTACH_TIMEOUT);
    bool bandMasktoStr(const uint64_t bandMask, char* str, size_t size);
    bool getCCID(char* buffer, size_t size);
    bool getIMSI(char* buffer, size_t size);
    bool getOperatorInfo(uint16_t* mcc, uint16_t* mnc);
    bool getOperatorInfoString(char* buffer, size_t size);
    bool getCellInfo(uint16_t* tac, uint32_t* cid, uint16_t* urat);
    bool getEpoch(uint32_t* epoch);
    bool getManufacturer(char* buffer, size_t size);
    bool getModel(char* buffer, size_t size);
    bool getFirmwareVersion(char* buffer, size_t size);
    bool getFirmwareRevision(char* buffer, size_t size);
    bool getIMEI(char* buffer, size_t size);
    SimStatuses getSimStatus();
    bool execCommand(const char* command, uint32_t timeout = R4X_DEFAULT_RESPONSE_TIMEOUT);
    bool execCommand(const String& command, uint32_t timeout = R4X_DEFAULT_RESPONSE_TIMEOUT);
    bool execCommand(const char* command, char* buffer, size_t size, uint32_t timeout = R4X_DEFAULT_RESPONSE_TIMEOUT);
    bool execCommand(const String& command, char* buffer, size_t size, uint32_t timeout = R4X_DEFAULT_RESPONSE_TIMEOUT);

    // Returns true if the modem replies to "AT" commands without timing out.
    bool isAlive();

    // Returns true if the modem is attached to the network and has an activated data connection.
    bool isAttached();

    // Returns true if the modem is connected to the network and IP address is not 0.0.0.0.
    bool isConnected();

    // Returns true if defined IP4 address is not 0.0.0.0.
    bool isDefinedIP4();

    void purgeAllResponsesRead();
    bool setApn(const char* apn);
    bool setIndicationsActive(bool on);
    void setNetworkStatusLED(bool on) { _networkStatusLED = on; };
    void setPin(const char* pin);
    bool setRadioActive(bool on);
    void setPowerSavingMode(bool on) { _psm = on; }
    bool setVerboseErrors(bool on);


    /******************************************************************************
    * Sockets
    *****************************************************************************/

    int    socketCreate(uint16_t localPort = 0, Protocols protocol = UDP);

    bool   socketSetR4KeepAlive(uint8_t socketID);
    bool   socketSetR4Option(uint8_t socketID, uint16_t level, uint16_t optName, uint32_t optValue, uint32_t optValue2 = 0);

    // Required for TCP, optional for UDP (for UDP socketConnect() + socketWrite() == socketSend())
    bool   socketConnect(uint8_t socketID, const char* remoteHost, const uint16_t remotePort);
    size_t socketWrite(uint8_t socketID, const uint8_t* buffer, size_t size);

    // TCP only
    bool   socketWaitForRead(uint8_t socketID, uint32_t timeout = SODAQ_R4X_DEFAULT_READ_TIMOUT);
    size_t socketRead(uint8_t socketID, uint8_t* buffer, size_t length);

    // UDP only
    size_t socketSend(uint8_t socketID, const char* remoteHost, const uint16_t remotePort, const uint8_t* buffer, size_t size);
    bool   socketWaitForReceive(uint8_t socketID, uint32_t timeout = SODAQ_R4X_DEFAULT_READ_TIMOUT);
    size_t socketReceive(uint8_t socketID, uint8_t* buffer, size_t length);

    bool   socketClose(uint8_t socketID, bool async = false);
    int    socketCloseAll();
    bool   socketFlush(uint8_t socketID, uint32_t timeout = 20000);
    bool   socketIsClosed(uint8_t socketID);
    bool   socketWaitForClose(uint8_t socketID, uint32_t timeout);

    size_t socketGetPendingBytes(uint8_t socketID);
    bool   socketHasPendingBytes(uint8_t socketID);

    void   setSocketWriteTimeout(uint32_t t) { _socket_write_timeout = t; }

    /******************************************************************************
    * MQTT
    *****************************************************************************/

    int8_t  mqttGetLoginResult();
    int16_t mqttGetPendingMessages();

    bool mqttLogin(uint32_t timeout = 3 * 60 * 1000);
    bool mqttLogout();
    void mqttLoop();
    bool mqttPing(const char* server);
    bool mqttPublish(const char* topic, const uint8_t* msg, size_t size, uint8_t qos = 0, uint8_t retain = 0, bool useHEX = false);
    uint16_t mqttReadMessages(char* buffer, size_t size, uint32_t timeout = 60 * 1000);

    bool mqttSetAuth(const char* name, const char* pw);
    bool mqttSetCleanSession(bool enabled);
    bool mqttSetClientId(const char* id);
    bool mqttSetInactivityTimeout(uint16_t timeout);
    bool mqttSetLocalPort(uint16_t port);
    bool mqttSetSecureOption(bool enabled, int8_t profile = -1);
    bool mqttSetServer(const char* server, uint16_t port);
    bool mqttSetServerIP(const char* ip, uint16_t port);

    bool mqttSubscribe(const char* filter, uint8_t qos = 0, uint32_t timeout = 30 * 1000);
    bool mqttUnsubscribe(const char* filter);
    void mqttSetPublishHandler(PublishHandlerPtr handler);

    /******************************************************************************
    * HTTP
    *****************************************************************************/

    // Creates an HTTP GET request and optionally returns the received data.
    // Note. Endpoint should include the initial "/".
    // The UBlox device stores the received data in http_last_response_<profile_id>
    uint32_t httpGet(const char* server, uint16_t port, const char* endpoint,
                     char* buffer, size_t bufferSize, uint32_t timeout = 60000, bool useURC = true);

    // Determine HTTP header size
    uint32_t httpGetHeaderSize(const char* filename);

    // Return a partial result of the previous HTTP Request (GET or POST)
    // Offset 0 is the byte directly after the HTTP Response header
    size_t httpGetPartial(uint8_t* buffer, size_t size, uint32_t offset);

    // Creates an HTTP POST request and optionally returns the received data.
    // Note. Endpoint should include the initial "/".
    // The UBlox device stores the received data in http_last_response_<profile_id>
    uint32_t httpPost(const char* server, uint16_t port, const char* endpoint,
                      char* responseBuffer, size_t responseSize,
                      const char* sendBuffer, size_t sendSize, uint32_t timeout = 60000, bool useURC = true);

    // Creates an HTTP POST request and optionally returns the received data.
    // Note. Endpoint should include the initial "/".
    // The UBlox device stores the received data in http_last_response_<profile_id>
    // Request body must first be prepared in a file on the modem
    uint32_t httpPostFromFile(const char* server, uint16_t port, const char* endpoint,
                      char* responseBuffer, size_t responseSize,
                      const char* fileName, uint32_t timeout = 60000, bool useURC = true);

    // Creates an HTTP request using the (optional) given buffer and
    // (optionally) returns the received data.
    // endpoint should include the initial "/".
    size_t httpRequest(const char* server, uint16_t port, const char* endpoint,
                       HttpRequestTypes requestType = HttpRequestTypes::GET,
                       char* responseBuffer = NULL, size_t responseSize = 0,
                       const char* sendBuffer = NULL, size_t sendSize = 0, uint32_t timeout = 60000, bool useURC = true);

     // Creates an HTTP request using the (optional) given buffer and
     // (optionally) returns the received data.
     // endpoint should include the initial "/".
     // Request body must first be prepared in a file on the modem
     // Can only be used for POST and PUT requests
     size_t httpRequestFromFile(const char* server, uint16_t port, const char* endpoint,
                       HttpRequestTypes requestType = HttpRequestTypes::GET,
                       char* responseBuffer = NULL, size_t responseSize = 0,
                       const char* fileName = NULL, uint32_t timeout = 60000, bool useURC = true);

    //  Paremeter index has a range [0-4]
    //  Parameters 'name' and 'value' can have a maximum length of 64 characters
    //  Parameters 'name' and 'value' must not include the ':' character
    bool httpSetCustomHeader(uint8_t index, const char* name, const char* value);
    bool httpClearCustomHeader(uint8_t index);


    /******************************************************************************
    * Files
    *****************************************************************************/

    bool   deleteFile(const char* filename);
    bool   getFileSize(const char* filename, uint32_t& size);
    size_t readFile(const char* filename, uint8_t* buffer, size_t size);
    size_t readFilePartial(const char* filename, uint8_t* buffer, size_t size, uint32_t offset);

    // If the file already exists, the data will be appended to the file already stored in the file system.
    bool   writeFile(const char* filename, const uint8_t* buffer, size_t size);


private:
    bool   checkApn(const char* requiredAPN);
    bool   checkBandMasks(const char* bandMaskLTE, const char* bandMaskNB);
    bool   checkCFUN();
    bool   checkCOPS(const char* requiredOperator, const char* requiredURAT);
    bool   checkProfile(const uint8_t requiredProfile);
    bool   checkUrat(const char* requiredURAT);
    bool   checkURC(const char* buffer);
    bool   doSIMcheck();
    bool   setNetworkLEDState();

    void   reboot();
    bool   setSimPin(const char* simPin);


    /******************************************************************************
     * Network Stuff
     *****************************************************************************/

    uint8_t     _cid;
    uint32_t    _httpGetHeaderSize;
    tribool_t   _httpRequestSuccessBit[HttpRequestTypesMAX];
    int8_t      _mqttLoginResult;
    int16_t     _mqttPendingMessages;
    int8_t      _mqttSubscribeReason;
    bool        _networkStatusLED;
    char*       _pin;
    bool        _socketClosed[SOCKET_COUNT];
    size_t      _socketPendingBytes[SOCKET_COUNT];

    PublishHandlerPtr _mqttPublishHandler = NULL;

    uint32_t    _socket_close_timeout;
    uint32_t    _socket_connect_timeout;
    uint32_t    _socket_write_timeout;

    uint32_t    _umqtt_timeout;

    /******************************************************************************
     * Generic
     *****************************************************************************/

    // Keep track if ATE0 was sent
    bool _echoOff;

    // Keep track if HEX mode was sent
    bool _hexMode;

    // Power Saving Mode (PSM)
    bool _psm;

    // Keep track when connect started. Use this to record various status changes.
    uint32_t _startOn;

    uint32_t    _connect_timeout;
    uint32_t    _disconnect_timeout;
    uint32_t    _cgact_timeout;
    uint32_t    _cops_timeout;

    // Determine the current baudrate
    uint32_t determineBaudRate();

    bool waitForSocketPrompt(uint32_t timeout);
    bool waitForFilePrompt(uint32_t timeout);
};

#endif
