deps_config := \
	/home/esp/esp-adf/esp-idf/components/app_trace/Kconfig \
	/home/esp/esp-adf/esp-idf/components/aws_iot/Kconfig \
	/home/esp/esp-adf/esp-idf/components/bt/Kconfig \
	/home/esp/esp-adf/esp-idf/components/driver/Kconfig \
	/home/esp/esp-adf/esp-idf/components/esp32/Kconfig \
	/home/esp/esp-adf/esp-idf/components/esp_adc_cal/Kconfig \
	/home/esp/esp-adf/esp-idf/components/esp_event/Kconfig \
	/home/esp/esp-adf/esp-idf/components/esp_http_client/Kconfig \
	/home/esp/esp-adf/esp-idf/components/esp_http_server/Kconfig \
	/home/esp/esp-adf/esp-idf/components/ethernet/Kconfig \
	/home/esp/esp-adf/esp-idf/components/fatfs/Kconfig \
	/home/esp/esp-adf/esp-idf/components/freemodbus/Kconfig \
	/home/esp/esp-adf/esp-idf/components/freertos/Kconfig \
	/home/esp/esp-adf/esp-idf/components/heap/Kconfig \
	/home/esp/esp-adf/esp-idf/components/libsodium/Kconfig \
	/home/esp/esp-adf/esp-idf/components/log/Kconfig \
	/home/esp/esp-adf/esp-idf/components/lwip/Kconfig \
	/home/esp/esp-adf/esp-idf/components/mbedtls/Kconfig \
	/home/esp/esp-adf/esp-idf/components/mdns/Kconfig \
	/home/esp/esp-adf/esp-idf/components/mqtt/Kconfig \
	/home/esp/esp-adf/esp-idf/components/nvs_flash/Kconfig \
	/home/esp/esp-adf/esp-idf/components/openssl/Kconfig \
	/home/esp/esp-adf/esp-idf/components/pthread/Kconfig \
	/home/esp/esp-adf/esp-idf/components/spi_flash/Kconfig \
	/home/esp/esp-adf/esp-idf/components/spiffs/Kconfig \
	/home/esp/esp-adf/esp-idf/components/tcpip_adapter/Kconfig \
	/home/esp/esp-adf/esp-idf/components/vfs/Kconfig \
	/home/esp/esp-adf/esp-idf/components/wear_levelling/Kconfig \
	C:/msys32/home/esp/esp-adf/components/audio_hal/Kconfig.projbuild \
	/home/esp/esp-adf/esp-idf/components/bootloader/Kconfig.projbuild \
	C:/msys32/home/esp/esp-adf/components/esp-adf-libs/Kconfig.projbuild \
	/home/esp/esp-adf/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/esp/esp-adf/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/esp/esp-adf/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
