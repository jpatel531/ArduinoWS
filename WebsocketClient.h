#include <SPI.h>
#include <String.h>
#include <Ethernet.h>
#include "Arduino.h"

#define FIN         0x80
#define OPCODE_TEXT 0x01
#define IS_MASKED   0x80
#define WS_SIZE16   126
#define WS_SIZE64   127
const byte MASK[] = {32, 25, 208, 9};

class WebsocketClient {
  public:
    bool connect(IPAddress ip, char path[] = "/", int port = 80);
    bool connect(const char *host, char path[] = "/", int port = 80);
    void send(String data);
    String recv();
  private:
    void sendHandshake(char path[]);
    bool readHandshake();
    EthernetClient _client;
    String readLine();
};