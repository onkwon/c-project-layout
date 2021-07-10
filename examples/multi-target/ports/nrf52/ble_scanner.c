#include "ble/ble.h"

#include <assert.h>

#include "nrf_pwr_mgmt.h"

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_scan.h"

#define BLE_LINK_ID			1

#define MIN_CONN_INTERVAL		MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL		MSEC_TO_UNITS(75, UNIT_1_25_MS)
#define SLAVE_LATENCY			0
#define CONN_SUP_TIMEOUT		MSEC_TO_UNITS(4000, UNIT_10_MS)

#if !defined(debug)
#define debug(...)
#endif
#if !defined(info)
#define info(...)
#endif
#if !defined(error)
#define error(...)
#endif

NRF_BLE_GATT_DEF(m_gatt);
NRF_BLE_SCAN_DEF(m_scan);

static struct {
	uint16_t connected_handle;

	struct {
		struct ble_adv *payloads;
		int index;
		int capacity;
	} ads;
} m = {
	.connected_handle = BLE_CONN_HANDLE_INVALID,
	.ads = { .index = 0, },
};

static struct ble_adv *find_ad(const ble_adv_addr_t *addr)
{
	int n = m.ads.index;

	for (int i = 0; i < n; i++) {
		struct ble_adv *ad = &m.ads.payloads[i];
		if (memcmp(addr->addr, ad->addr.addr, sizeof(addr->addr)) == 0) {
			return ad;
		}
	}

	return NULL;
}

static bool insert_ad(const struct ble_adv *ad)
{
	if (m.ads.index >= m.ads.capacity) {
		return false;
	}

	int index = m.ads.index;
	m.ads.index += 1;

	memcpy(&m.ads.payloads[index], ad, sizeof(*ad));
	memcpy(&m.ads.payloads[index].header, &ad->header, sizeof(ad->header));
	memcpy(&m.ads.payloads[index].adv_data_raw, &ad->adv_data_raw,
			BLE_ADV_DATA_MAXSIZE);

	return true;
}

static bool delete_ad(const ble_adv_addr_t *addr)
{
	return false;
}

static void start_scan(void)
{
	ret_code_t ret;

	ret = nrf_ble_scan_start(&m_scan);
	assert(ret == NRF_SUCCESS);
}

static void evt_connected(ble_evt_t const *p_ble_evt, void *p_context)
{
	debug("Connected");
	m.connected_handle = p_ble_evt->evt.gap_evt.conn_handle;

	int err = sd_ble_gap_authenticate(p_ble_evt->evt.gap_evt.conn_handle,
			&(const ble_gap_sec_params_t){
				.bond = 1,
				.mitm = 1,
				.lesc = 1,
				.keypress = 1,
				.io_caps = BLE_GAP_IO_CAPS_KEYBOARD_DISPLAY,
				.oob = 0,
				.min_key_size = 7,
				.max_key_size = 16,
				.kdist_own = { .enc = 1, .id = 1, },
				.kdist_peer = {. enc = 1, .id = 1, },
			});
}

static void evt_disconnected(ble_evt_t const *p_ble_evt, void *p_context)
{
	debug("Disconnected");
	m.connected_handle = BLE_CONN_HANDLE_INVALID;
	start_scan();
}

static void evt_phy_update_requested(ble_evt_t const *p_ble_evt, void *p_context)
{
	debug("PHY update request.");
	ble_gap_phys_t const phys = {
		.rx_phys = BLE_GAP_PHY_AUTO,
		.tx_phys = BLE_GAP_PHY_AUTO,
	};
	uint32_t err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
	assert(err_code == NRF_SUCCESS);
}

static void fetch_and_save(const ble_gap_evt_adv_report_t *p_adv_report_data)
{
	ble_adv_addr_t *addr = (ble_adv_addr_t *)p_adv_report_data->peer_addr.addr;
	struct ble_adv *ad = find_ad(addr);

	if (ad == NULL) {
		struct ble_adv t = {
			.rssi = p_adv_report_data->rssi,
			.addr.type = p_adv_report_data->peer_addr.addr_type,
			.payload_length = p_adv_report_data->data.len,
		};
		memcpy(&t.header, &p_adv_report_data->type, sizeof(t.header));
		memcpy(t.addr.addr, p_adv_report_data->peer_addr.addr, sizeof(t.addr));
		memset(t.adv_data_raw, 0, BLE_ADV_MAXLEN);
		memcpy(t.adv_data_raw, p_adv_report_data->data.p_data, p_adv_report_data->data.len);
		insert_ad((const struct ble_adv *)&t);
	} else {
		ad->rssi = p_adv_report_data->rssi;

		if (p_adv_report_data->type.scan_response &&
				(ad->payload_length + p_adv_report_data->data.len) < BLE_ADV_DATA_MAXSIZE) {
			//update_ad();
			memcpy(&ad->adv_data_raw[ad->payload_length],
					p_adv_report_data->data.p_data,
					p_adv_report_data->data.len);
			ad->payload_length += p_adv_report_data->data.len;
		}
		return;
	}

	debug("Connecting to target %02x%02x%02x%02x%02x%02x",
			p_adv_report_data->peer_addr.addr[0],
			p_adv_report_data->peer_addr.addr[1],
			p_adv_report_data->peer_addr.addr[2],
			p_adv_report_data->peer_addr.addr[3],
			p_adv_report_data->peer_addr.addr[4],
			p_adv_report_data->peer_addr.addr[5]);
	if (!p_adv_report_data->type.connectable) {
		debug("not connectable!");
		return;
	}

	const ble_gap_scan_params_t scan_param = {
		.active        = 0x01,
		.interval      = MSEC_TO_UNITS(40, UNIT_0_625_MS),
		.window        = MSEC_TO_UNITS(30, UNIT_0_625_MS),
		.filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
		.timeout       = BLE_GAP_SCAN_TIMEOUT_UNLIMITED,
		.scan_phys     = BLE_GAP_PHY_AUTO,
		.extended      = true,
		.report_incomplete_evts = true,
	};
	const ble_gap_conn_params_t conn_param = {
		.min_conn_interval = MIN_CONN_INTERVAL,
		.max_conn_interval = MAX_CONN_INTERVAL,
		.slave_latency = SLAVE_LATENCY,
		.conn_sup_timeout = CONN_SUP_TIMEOUT,
	};
	uint32_t err = sd_ble_gap_connect(&p_adv_report_data->peer_addr,
			&scan_param, &conn_param, BLE_LINK_ID);
}

static void evt_advertising(ble_evt_t const *p_ble_evt, void *p_context)
{
	ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;
	const ble_gap_evt_adv_report_t *p_adv_report = &p_gap_evt->params.adv_report;
	fetch_and_save(p_adv_report);
}

static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
	uint32_t err_code;

	switch (p_ble_evt->header.evt_id) {
	case BLE_GAP_EVT_ADV_REPORT:
		evt_advertising(p_ble_evt, p_context);
		break;
	case BLE_GAP_EVT_CONNECTED:
		evt_connected(p_ble_evt, p_context);
		break;
	case BLE_GAP_EVT_DISCONNECTED:
		evt_disconnected(p_ble_evt, p_context);
		break;
	case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
		debug("phy_update_req");
		evt_phy_update_requested(p_ble_evt, p_context);
		break;
	case BLE_GATTS_EVT_SYS_ATTR_MISSING:
		debug("sys_attr_missing");
		err_code = sd_ble_gatts_sys_attr_set(m.connected_handle, NULL, 0, 0);
		assert(err_code == NRF_SUCCESS);
		break;
	case BLE_GATTC_EVT_TIMEOUT:
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
				BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		assert(err_code == NRF_SUCCESS);
		break;
	case BLE_GATTS_EVT_TIMEOUT:
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
				BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		assert(err_code == NRF_SUCCESS);
		break;
	case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
		sd_ble_gap_conn_param_update(p_ble_evt->evt.gap_evt.conn_handle,
				&p_ble_evt->evt.gap_evt.params.conn_param_update_request.conn_params);
		break;
	case BLE_GAP_EVT_SEC_REQUEST:
		debug("SEC %x", p_ble_evt->evt.gap_evt.params.sec_request);
		break;
	case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
		debug("SEC bond %x, mitm %x, lesc %x, keypress %x, io_caps %x, oob %x",
				p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.bond,
				p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.mitm,
				p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.lesc,
				p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.keypress,
				p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.io_caps,
				p_ble_evt->evt.gap_evt.params.sec_params_request.peer_params.oob);
#if 0
		// Pairing not supported
		err_code = sd_ble_gap_sec_params_reply(m.connected_handle,
					BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
#else
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
				BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
#endif
		assert(err_code == NRF_SUCCESS);
		break;
	case BLE_GAP_EVT_AUTH_STATUS:
		info("AUTH : status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
				p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
				p_ble_evt->evt.gap_evt.params.auth_status.bonded,
				p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
				*((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
				*((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
#if 1
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
				BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		assert(err_code == NRF_SUCCESS);
#endif
		break;
	case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
	case BLE_GAP_EVT_CONN_SEC_UPDATE:
	case BLE_GAP_EVT_AUTH_KEY_REQUEST:
	case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
	case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
	case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
	default:
		debug("BLE event: %d", p_ble_evt->header.evt_id);
		break;
	}
}

static void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt)
{
	debug("GATT event: %#x", p_evt->evt_id);

	switch (p_evt->evt_id) {
	case NRF_BLE_GATT_EVT_ATT_MTU_UPDATED:
		debug("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
				p_gatt->att_mtu_desired_central,
				p_gatt->att_mtu_desired_periph);
		break;
	case NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED:
	default:
		break;
	}
}

static void scan_evt_handler(scan_evt_t const *p_scan_evt)
{
	ret_code_t err_code;
	ble_gap_evt_connected_t *p_connected;
	(void)err_code;
	(void)p_connected;

	switch(p_scan_evt->scan_evt_id) {
	case NRF_BLE_SCAN_EVT_NOT_FOUND:
		break;
	case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
		err_code = p_scan_evt->params.connecting_err.err_code;
		error("connecting error: %x", err_code);
		break;
	case NRF_BLE_SCAN_EVT_CONNECTED: // Scan is automatically stopped by the connection.
		p_connected = p_scan_evt->params.connected.p_connected;
		debug("Connected to target %02x%02x%02x%02x%02x%02x",
				p_connected->peer_addr.addr[0],
				p_connected->peer_addr.addr[1],
				p_connected->peer_addr.addr[2],
				p_connected->peer_addr.addr[3],
				p_connected->peer_addr.addr[4],
				p_connected->peer_addr.addr[5]);
		break;
	case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
		info("Scan timed out.");
		start_scan();
		break;
	case NRF_BLE_SCAN_EVT_FILTER_MATCH:
	case NRF_BLE_SCAN_EVT_WHITELIST_ADV_REPORT:
	case NRF_BLE_SCAN_EVT_WHITELIST_REQUEST:
	default:
		debug("Scan event: %#x", p_scan_evt->scan_evt_id);
		break;
	}
}

static void initialize_ble_stack(void)
{
	ret_code_t err_code;

	err_code = nrf_sdh_enable_request();
	assert(err_code == NRF_SUCCESS);

	// Configure the BLE stack using the default settings.
	// Fetch the start address of the application RAM.
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(BLE_LINK_ID, &ram_start);
	assert(err_code == NRF_SUCCESS);

	// Enable BLE stack.
	err_code = nrf_sdh_ble_enable(&ram_start);
	assert(err_code == NRF_SUCCESS);

	// Register a handler for BLE events.
	NRF_SDH_BLE_OBSERVER(m_ble_observer, 3, ble_evt_handler, NULL);
}

static void initialize_gatt(void)
{
	ret_code_t err_code;

	err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
	assert(err_code == NRF_SUCCESS);

	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	assert(err_code == NRF_SUCCESS);
}

static void initialize_scan(const struct ble_scan_param *cfg)
{
	ret_code_t          err_code;
	nrf_ble_scan_init_t init_scan;

	ble_gap_scan_params_t scan_param = {
		.active        = (uint8_t)cfg->active,
		.interval      = MSEC_TO_UNITS(cfg->interval_ms, UNIT_0_625_MS),
		.window        = MSEC_TO_UNITS(cfg->window_ms, UNIT_0_625_MS),
		.filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
		.timeout       = cfg->timeout_ms,
		.scan_phys     = BLE_GAP_PHY_AUTO,
		//.extended      = true,
		//.report_incomplete_evts = true,
	};

	memset(&init_scan, 0, sizeof(init_scan));

	init_scan.connect_if_match = false;
	init_scan.conn_cfg_tag = BLE_LINK_ID;
	init_scan.p_scan_param = &scan_param;

	err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
	assert(err_code == NRF_SUCCESS);
}

static void initialize_pwr_mgmt(void)
{
	ret_code_t err_code = nrf_pwr_mgmt_init();
	assert(err_code == NRF_SUCCESS);
}

void ble_init(void)
{
	initialize_pwr_mgmt();

	initialize_ble_stack();
	initialize_gatt();
}

void ble_scan_init(uint8_t *buf, size_t bufsize)
{
	m.ads.payloads = (struct ble_adv *)buf;
	m.ads.capacity = bufsize / (BLE_ADV_DATA_MAXSIZE + 8/*index&capacity*/);
}

#include "sleep.h"
void ble_scan_start(const struct ble_scan_param *param)
{
	initialize_scan(param);

	start_scan();

	while (1) {
		int n = m.ads.index;
		info("Scanned = %d", n);
		for (int i = 0; i < n; i++) {
			// peer address(6bytes), header(2bytes), rssi(1byte), sec(?), ad data(?)
			info("MAC %08x%04x(%x) RSSI %d Header %x",
					(m.ads.payloads[i].addr.addr[0] << 24) +
					(m.ads.payloads[i].addr.addr[1] << 16) +
					(m.ads.payloads[i].addr.addr[2] << 8) +
					(m.ads.payloads[i].addr.addr[3] << 0),
					(m.ads.payloads[i].addr.addr[4] << 8) +
					(m.ads.payloads[i].addr.addr[5]),
					m.ads.payloads[i].addr.type,
					m.ads.payloads[i].rssi,
					m.ads.payloads[i].header);
#if 0
			uint16_t j = 0;
			while (j < BLE_ADV_DATA_MAXSIZE) {
				struct ble_adv_data payload = {
					.length = m.ads.payloads[i].adv_data_raw[j],
					.type = m.ads.payloads[i].adv_data_raw[j+1],
					.pdata = &m.ads.payloads[i].adv_data_raw[j+2] };
				if (payload.length == 0) {
					break;
				}

				j += payload.length + 1;
				info("Len %u, Type %x", payload.length, payload.type);
				if (payload.type == 0x09 || payload.type == 0x08) {
					info("\t %.*s", payload.length, payload.pdata);
#if 0
				} else if (payload.type == 0x02 || payload.type == 0x03) {
					info("\t %02X%02X", payload.data[0], payload.data[1]);
				} else if (payload.type == 0x04 || payload.type == 0x05) {
					info("\t %02X%02X%02X%02X", payload.data[0], payload.data[1], payload.data[2], payload.data[3]);
				} else if (payload.type == 0x06 || payload.type == 0x07) {
					info("\t %02X%02X%02X%02X_%02X%02X%02X%02X "
							"%02X%02X%02X%02X_%02X%02X%02X%02X",
							payload.data[0], payload.data[1], payload.data[2], payload.data[3],
							payload.data[4], payload.data[5], payload.data[6], payload.data[7],
							payload.data[8], payload.data[9], payload.data[10], payload.data[11],
							payload.data[12], payload.data[13], payload.data[14], payload.data[15]);
#endif
				}
			}
#endif
		}
		sleep_ms(10000);
	}
}
