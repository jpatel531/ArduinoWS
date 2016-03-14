#include <WebsocketClient.h>
#include <String.h>

bool WebsocketClient::connect(IPAddress ip, char path[], int port) {
  if (_client.connect(ip, port)) {
    sendHandshake(path);
    return readHandshake();
  } else {
    _client.stop();
    delay(1000);
    Serial.println("Trying again");
    return connect(ip, path, port);
  }
}

bool WebsocketClient::connect(const char *host, char path[], int port) {
  if (_client.connect(host, port)) {
    sendHandshake(path);
    return readHandshake();
  } else {
    _client.stop();
    delay(1000);
    Serial.println("Trying again");
    return connect(host, path, port);
  }
}

void WebsocketClient::send(String data) {
  _client.write(OPCODE_TEXT | FIN);
  int data_len = data.length();

  if (data_len >= WS_SIZE16) {
    _client.write(WS_SIZE16 | IS_MASKED);
    _client.write((uint8_t) (data_len >> 8));
    _client.write((uint8_t) (data_len & 0xFF));
  } else {
    _client.write(((uint8_t) data_len) | IS_MASKED);
  }

  _client.write(MASK[0]);
  _client.write(MASK[1]);
  _client.write(MASK[2]);
  _client.write(MASK[3]);

  Serial.println("SEND LENGTH:");
  Serial.println(data_len);

  for (int i = 0; i < data_len; ++i) {
    _client.write(data[i] ^ MASK[i%4]);
  }
}

String WebsocketClient::recv() {
  if (!_client.connected() || !_client.available()){
      return "";
  }

  String data = String("");
  byte msgtype = _client.read();
  byte len = _client.read();

  if (len == WS_SIZE16) {
    len = _client.read() << 8;
    if (!_client.connected()) {
      return "";
    }

    len |= _client.read();
    if (!_client.connected()) {
      return "";
    }

  } else if (len == WS_SIZE64) {
   return "";
  }

  Serial.println("RECV LENGTH:");
  Serial.println(len);

  for (int i=0; i<len; i++) {
    data += (char) _client.read();
  }
  return data;
}

void WebsocketClient::sendHandshake(char path[]){
  _client.println("GET "+String(path)+" HTTP/1.1");
  _client.println("Connection: Upgrade");
  _client.println("Origin: null");
  _client.println("Upgrade: websocket");
  _client.println("Sec-WebSocket-Version: 13");
  _client.println("Sec-WebSocket-Key: eW9sbw==");
  _client.println();
}

String WebsocketClient::readLine() {
  String line = "";
  char character;

  while(_client.available() > 0 && (character = _client.read()) != '\n') {
    if (character != '\r' && character != -1) {
      line += character;
    }
  }

  return line;
}

bool WebsocketClient::readHandshake() {
  while(!_client.available()){
   delay(100);
 }

 String handshake = "", line;

 while((line = readLine()) != "") {
  handshake += line + '\n';
}
return true;
}