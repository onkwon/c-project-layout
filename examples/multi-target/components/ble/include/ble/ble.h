#ifndef BLE_BLE_H
#define BLE_BLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define BLE_SCAN_TIMEOUT_UNLIMITED		0
#define BLE_ADDR_LEN				6
#define BLE_ADV_DATA_MAXSIZE			(31 - 1/*length*/)
#define BLE_ADV_MAXLEN				(31 / 3/*sizeof(ble_adv_data)*/)

typedef struct {
	uint8_t addr[BLE_ADDR_LEN];
	uint8_t type;
} ble_adv_addr_t;

typedef struct ble_adv_header {
	uint16_t connectable   : 1;
	uint16_t scannable     : 1;
	uint16_t directed      : 1;
	uint16_t scan_response : 1;
	uint16_t extended_pdu  : 1;
	uint16_t status        : 2;
	uint16_t reserved      : 9;
} ble_adv_header_t;

struct ble_scan_param {
	uint16_t interval_ms;
	uint16_t window_ms;
	uint16_t timeout_ms;
	bool active;
};

struct ble_adv_data {
	uint8_t length;
	uint8_t type;
	union {
		uint8_t data[BLE_ADV_DATA_MAXSIZE];
		uint8_t *pdata;
	};
};

struct ble_adv {
	ble_adv_header_t header;
	ble_adv_addr_t addr;
	int8_t rssi;
	uint16_t payload_length;
	union {
		struct ble_adv_data adv_data[BLE_ADV_MAXLEN];
		uint8_t adv_data_raw[BLE_ADV_DATA_MAXSIZE];
	};
};

void ble_init(void);

void ble_scan_init(uint8_t *buf, size_t bufsize);
void ble_scan_start(const struct ble_scan_param *param);
void ble_scan_stop(void);

#endif
