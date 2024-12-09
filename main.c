#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h>
#include <BluetoothSerial.h>

#define CS_PIN 5

// Requisitos: Tela TFT Touchscreen, Bluetooth e cartão SD.
BluetoothSerial SerialBT;
TFT_eSPI tft = TFT_eSPI();
File root;
String fileList[10];
int fileCount = 0;
int selectedFile = -1;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT");
  if (!SD.begin(CS_PIN)) while (1);
  listarArquivos();
  iniciarDisplay();
}

void loop() {
  verificarToque();
  if (selectedFile != -1) {
    enviarArquivo(fileList[selectedFile]);
    selectedFile = -1;
  }
}

void listarArquivos() {
  root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    if (!entry.isDirectory() && fileCount < 10) fileList[fileCount++] = entry.name();
    entry.close();
  }
}

void iniciarDisplay() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  for (int i = 0; i < fileCount; i++) {
    tft.setCursor(10, 10 + i * 20);
    tft.print(i + 1);
    tft.print(": ");
    tft.print(fileList[i]);
  }
}

void verificarToque() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    int index = y / 20;
    if (index < fileCount) selectedFile = index;
  }
}

void enviarArquivo(String fileName) {
  File file = SD.open(fileName);
  if (!file) return;
  while (file.available()) SerialBT.write(file.read());
  file.close();
}
