#include "esphome.h"

/**
*  Core functionality parseState(), isValidHeader(), isValidChecksum() and loop() are 
*  derrived from https://github.com/Hypfer/esp8266-vindriktning-particle-sensor/blob/master/src/SerialCom.h
*/
class VindriktningComponent : public Component, public UARTDevice, public Sensor {
    public:

    uint8_t rxBufIdx = 0;
    uint8_t serialRxBuf[255];

    VindriktningComponent(UARTComponent *parent) : UARTDevice(parent) {}

    void setup() override {
    }

    void clearRxBuf() {
        memset(serialRxBuf, 0, sizeof(serialRxBuf));
        rxBufIdx = 0;
    }

    uint16_t parseState() {
        /**
         *         MSB  DF 3     DF 4  LSB
         * uint16_t = xxxxxxxx xxxxxxxx
         */
        const uint16_t pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];
        return pm25;
    }

    bool isValidHeader() {
        bool headerValid = serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B;
        if (!headerValid) {
            ESP_LOGD("custom", "Received message with invalid header");
        }
        return headerValid;
    }

    bool isValidChecksum() {
        uint8_t checksum = 0;

        for (uint8_t i = 0; i < 20; i++) {
            checksum += serialRxBuf[i];
        }

        if (checksum != 0) {
            ESP_LOGD("custom", "Received message with invalid checksum. Expected: 0. Actual: %d", checksum);
        }

        return checksum == 0;
    }

    void loop() override {
        if (!available()) {
            return;
        }

        while (available()) {
            serialRxBuf[rxBufIdx++] = read();

            // Necessary delay, see SerialComm.h handleUart()
            delay(15);

            if (rxBufIdx >= 64) {
                clearRxBuf();
            }
        }

        if (isValidHeader() && isValidChecksum()) {
            uint16_t state = parseState();
            ESP_LOGD("custom", "Received PM 2.5 reading: %d", state);
            publish_state((uint32_t) state);
        }

        clearRxBuf();
    }
};