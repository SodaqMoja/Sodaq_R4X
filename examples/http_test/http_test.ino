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
