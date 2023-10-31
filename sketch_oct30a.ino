#include <driver/i2s.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>


#define I2S_SD 35
#define I2S_WS 32
#define I2S_SCK 33
#define I2S_PORT I2S_NUM_0

#define bufferLen 2048
int16_t sBuffer[bufferLen];

const char* ssid = "JOAO_REIIS";
const char* password = "96675010";

const char* websocket_server_host = "192.168.0.138";
const uint16_t websocket_server_port = 8888;  // <WEBSOCKET_SERVER_PORT>

using namespace websockets;

WebsocketsClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(500);

  connectWiFi();
  connectWSServer();

  delay(500);

  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);

  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  size_t bytesIn = 0;
  while (1) {
    esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);

    if (result == ESP_OK) {
      client.sendBinary((const char*)sBuffer, bytesIn);

    }
  }

}

void connectWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void connectWSServer() {
  while (!client.connect(websocket_server_host, websocket_server_port, "/")) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Websocket Connected!");
}

void i2s_install() {
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    //.sample_rate = 16000,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}