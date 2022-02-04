#include "vindriktning.h"
#include "esphome/core/log.h"

namespace esphome {
namespace vindriktning {

static const char *const TAG = "vindriktning";

void VindriktningComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "VINDRIKTNING:");
  LOG_SENSOR("  ", "PM2.5", this);
}

void VindriktningComponent::loop() {
  if (!available()) {
    return;
  }

  while (available()) {
    serialRxBuf[rxBufIdx++] = read();

    delay(15); //TODO: This is not acceptable as per ESPHome's rules (https://esphome.io/guides/contributing.html#runtime)

    if (rxBufIdx >= 64) {
      clearRxBuf();
    }
  }

  if (isValidHeader() && isValidChecksum()) {
    uint16_t state = parseState();
    ESP_LOGD(TAG, "Received PM 2.5 reading: %d", state);
    publish_state((uint32_t) state);
  }

  clearRxBuf();
}

void VindriktningComponent::clearRxBuf() {
  memset(serialRxBuf, 0, sizeof(serialRxBuf));
  rxBufIdx = 0;
}

uint16_t VindriktningComponent::parseState() {
/**
 *         MSB  DF 3     DF 4  LSB
 * uint16_t = xxxxxxxx xxxxxxxx
 */
  const uint16_t pm25 = (serialRxBuf[5] << 8) | serialRxBuf[6];
  return pm25;
}

bool VindriktningComponent::isValidHeader() {
  bool headerValid = serialRxBuf[0] == 0x16 && serialRxBuf[1] == 0x11 && serialRxBuf[2] == 0x0B;
  if (!headerValid) {
    ESP_LOGD(TAG, "Received message with invalid header");
  }
  return headerValid;
}

bool VindriktningComponent::isValidChecksum() {
  uint8_t checksum = 0;

  for (uint8_t i = 0; i < 20; i++) {
    checksum += serialRxBuf[i];
  }

  if (checksum != 0) {
    ESP_LOGD(TAG, "Received message with invalid checksum. Expected: 0. Actual: %d", checksum);
  }

  return checksum == 0;
}

}  // namespace vindriktning
}  // namespace esphome