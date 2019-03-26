/*
    Copyright (c) 2019 Sodaq.  All rights reserved.

    This file is part of Sodaq_R4X.

    Sodaq_R4X is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or(at your option) any later version.

    Sodaq_R4X is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Sodaq_nbIOT.  If not, see
    <http://www.gnu.org/licenses/>.
*/

#include <Sodaq_R4X.h>

#define CONSOLE_STREAM   SerialUSB
#define MODEM_STREAM     Serial1

#define CURRENT_APN      "data.mono"
#define CURRENT_URAT     "7"
#define HTTP_HOST        "time.sodaq.net"
#define HTTP_PORT        80
#define HTTP_QUERY       "/"

static Sodaq_R4X r4x;
static Sodaq_SARA_R4XX_OnOff saraR4xxOnOff;
static bool isReady;

void setup()
{
    while (!CONSOLE_STREAM);
    CONSOLE_STREAM.println("Booting up...");

    MODEM_STREAM.begin(r4x.getDefaultBaudrate());
    r4x.setDiag(CONSOLE_STREAM);
    r4x.init(&saraR4xxOnOff, MODEM_STREAM);

    isReady = r4x.connect(CURRENT_APN, CURRENT_URAT);
    CONSOLE_STREAM.println(isReady ? "Network connected" : "Network connection failed");

    CONSOLE_STREAM.println("Setup done");

    if (isReady) {
        downloadFile();
    }
}

void loop()
{
    if (CONSOLE_STREAM.available()) {
        int i = CONSOLE_STREAM.read();
        CONSOLE_STREAM.write(i);
        MODEM_STREAM.write(i);
    }

    if (MODEM_STREAM.available()) {
        CONSOLE_STREAM.write(MODEM_STREAM.read());
    }
}

void downloadFile()
{
    char buffer[2048];

    uint32_t i = r4x.httpGet(HTTP_HOST, HTTP_PORT, HTTP_QUERY, buffer, sizeof(buffer));

    CONSOLE_STREAM.print("Read bytes: ");
    CONSOLE_STREAM.println(i);

    if (i > 0) {
        buffer[i] = 0;
        CONSOLE_STREAM.println("Buffer:");
        CONSOLE_STREAM.println(buffer);
    }
}
