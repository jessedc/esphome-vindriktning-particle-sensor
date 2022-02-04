import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ID,
    CONF_PM_2_5,
    DEVICE_CLASS_PM25,
    ICON_CHEMICAL_WEAPON,
    STATE_CLASS_MEASUREMENT,
    UNIT_MICROGRAMS_PER_CUBIC_METER,
)

DEPENDENCIES = ["uart"]

vindriktning_ns = cg.esphome_ns.namespace("vindriktning")
VindriktningComponent = vindriktning_ns.class_("VindriktningComponent", uart.UARTDevice, cg.Component)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
        icon="mdi:air-filter",
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_PM25,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(VindriktningComponent),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    await sensor.register_sensor(var, config)
