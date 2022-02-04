#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace vindriktning {

class VindriktningComponent: public uart::UARTDevice, public Component, public sensor::Sensor {
	public:
		VindriktningComponent() = default;

		void loop() override;
		void dump_config() override;

	protected:
		uint8_t rxBufIdx = 0;
  	uint8_t serialRxBuf[255];

  	void clearRxBuf();
  	uint16_t parseState();
  	bool isValidHeader();
  	bool isValidChecksum();
};

}  // namespace vindriktrning
}  // namespace esphome