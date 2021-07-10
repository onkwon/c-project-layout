#if 0
#include "nrf_pwr_mgmt.h"
#include "app_timer.h"
#include "app_error.h"

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"

#include "ble_advdata.h"
#include "ble_advertising.h"

#include "nrf_ble_scan.h"

#include "ble_nus.h"

#include "libmcu/logging.h"

#define DEVICE_NAME			"Nordic_UART"

#define MIN_CONN_INTERVAL		MSEC_TO_UNITS(20, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL		MSEC_TO_UNITS(75, UNIT_1_25_MS)
#define SLAVE_LATENCY			0
#define CONN_SUP_TIMEOUT		MSEC_TO_UNITS(4000, UNIT_10_MS)

#define NUS_SERVICE_UUID_TYPE		BLE_UUID_TYPE_VENDOR_BEGIN
#define APP_ADV_INTERVAL		64  // 40ms. advertising interval (in units of 0.625 ms)
#define APP_ADV_DURATION		18000 // advertising duration (180 seconds) in units of 10 milliseconds

#if 0
#define FIRST_CONN_PARAMS_UPDATE_DELAY	APP_TIMER_TICKS(5000)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY	APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT	3// Number of attempts before giving up the connection parameter negotiation.
#endif

NRF_BLE_QWR_DEF(m_qwr); /**< Context for the Queued Write module.*/
NRF_BLE_GATT_DEF(m_gatt); /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising); /**< Advertising module instance. */
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT); /**< BLE NUS service instance. */

static struct {
	uint16_t connected_handle;
	uint16_t nus_maxlen;
	ble_uuid_t adv_uuid[1];
} m = {
	.connected_handle = BLE_CONN_HANDLE_INVALID,
	.nus_maxlen = BLE_GATT_ATT_MTU_DEFAULT - 3,
	.adv_uuid = { { BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE }, },
};

static void evt_connected(ble_evt_t const *p_ble_evt, void *p_context)
{
	info("Connected");
	m.connected_handle = p_ble_evt->evt.gap_evt.conn_handle;
	uint32_t err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m.connected_handle);
	APP_ERROR_CHECK(err_code);
}

static void evt_disconnected(ble_evt_t const *p_ble_evt, void *p_context)
{
	info("Disconnected");
	m.connected_handle = BLE_CONN_HANDLE_INVALID;
}

static void evt_phy_update_requested(ble_evt_t const *p_ble_evt, void *p_context)
{
	debug("PHY update request.");
	ble_gap_phys_t const phys = {
		.rx_phys = BLE_GAP_PHY_AUTO,
		.tx_phys = BLE_GAP_PHY_AUTO,
	};
	uint32_t err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
	APP_ERROR_CHECK(err_code);
}

static uint8_t gbuf[512];
static volatile uint16_t glen;
static void print_adv(const ble_gap_evt_adv_report_t *p_adv_report_data)
{
#if 0
	info("MAC %02x%02x%02x%02x%02x%02x RSSI %d",
			p_adv_report_data->peer_addr.addr[0],
			p_adv_report_data->peer_addr.addr[1],
			p_adv_report_data->peer_addr.addr[2],
			p_adv_report_data->peer_addr.addr[3],
			p_adv_report_data->peer_addr.addr[4],
			p_adv_report_data->peer_addr.addr[5],
			p_adv_report_data->rssi);
#endif

	ble_data_t *data = &p_adv_report_data->data;
	if (data->len > 0 && glen == 0) {
		glen = data->len;
		memcpy(gbuf, data->p_data, glen);
	}
}

static void evt_advertising(ble_evt_t const *p_ble_evt, void *p_context)
{
	ble_gap_evt_t const *p_gap_evt = &p_ble_evt->evt.gap_evt;
	const ble_gap_evt_adv_report_t *p_adv_report = &p_gap_evt->params.adv_report;
	print_adv(p_adv_report);
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
		evt_phy_update_requested(p_ble_evt, p_context);
		break;
	case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
		// Pairing not supported
		err_code =
			sd_ble_gap_sec_params_reply(m.connected_handle,
					BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
		APP_ERROR_CHECK(err_code);
		break;
	case BLE_GATTS_EVT_SYS_ATTR_MISSING:
		// No system attributes have been stored.
		err_code = sd_ble_gatts_sys_attr_set(m.connected_handle, NULL, 0, 0);
		APP_ERROR_CHECK(err_code);
		break;
	case BLE_GATTC_EVT_TIMEOUT:
		// Disconnect on GATT Client timeout event.
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
				BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		APP_ERROR_CHECK(err_code);
		break;
	case BLE_GATTS_EVT_TIMEOUT:
		// Disconnect on GATT Server timeout event.
		err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
				BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		APP_ERROR_CHECK(err_code);
		break;
	default:
		break;
	}
}

static void ble_stack_init(void)
{
	ret_code_t err_code;

	err_code = nrf_sdh_enable_request();
	APP_ERROR_CHECK(err_code);

	// Configure the BLE stack using the default settings.
	// Fetch the start address of the application RAM.
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(1, &ram_start);
	APP_ERROR_CHECK(err_code);

	// Enable BLE stack.
	err_code = nrf_sdh_ble_enable(&ram_start);
	APP_ERROR_CHECK(err_code);

	// Register a handler for BLE events.
	NRF_SDH_BLE_OBSERVER(m_ble_observer, 3, ble_evt_handler, NULL);
}

static void gap_params_init(void)
{
	uint32_t                err_code;
	ble_gap_conn_params_t   gap_conn_params;
	ble_gap_conn_sec_mode_t sec_mode;

	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

	err_code = sd_ble_gap_device_name_set(&sec_mode,
			(const uint8_t *)DEVICE_NAME,
			strlen(DEVICE_NAME));
	APP_ERROR_CHECK(err_code);

	memset(&gap_conn_params, 0, sizeof(gap_conn_params));

	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency     = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	APP_ERROR_CHECK(err_code);
}

static void gatt_evt_handler(nrf_ble_gatt_t *p_gatt, nrf_ble_gatt_evt_t const *p_evt)
{
	if ((m.connected_handle == p_evt->conn_handle) &&
			(p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)) {
		m.nus_maxlen = p_evt->params.att_mtu_effective
			- 1/*OPCODE_LENGTH*/ - 2/*HANDLE_LENGTH*/;
		info("Data len is set to 0x%X(%d)", m.nus_maxlen, m.nus_maxlen);
	}
	debug("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
			p_gatt->att_mtu_desired_central,
			p_gatt->att_mtu_desired_periph);
}

static void gatt_init(void)
{
	ret_code_t err_code;

	err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	APP_ERROR_CHECK(err_code);
}

static void nus_data_handler(ble_nus_evt_t * p_evt)
{
#if 0
	if (p_evt->type == BLE_NUS_EVT_RX_DATA) {
		uint32_t err_code;

		NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
		NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

		for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++) {
			do {
				err_code = app_uart_put(p_evt->params.rx_data.p_data[i]);
				if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY)) {
					NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
					APP_ERROR_CHECK(err_code);
				}
			} while (err_code == NRF_ERROR_BUSY);
		}
		if (p_evt->params.rx_data.p_data[p_evt->params.rx_data.length - 1] == '\r') {
			while (app_uart_put('\n') == NRF_ERROR_BUSY);
		}
	}
#endif
}

static void nrf_qwr_error_handler(uint32_t nrf_error)
{
	APP_ERROR_HANDLER(nrf_error);
}

static void services_init(void)
{
	uint32_t           err_code;
	ble_nus_init_t     nus_init;
	nrf_ble_qwr_init_t qwr_init = {0};

	// Initialize Queued Write Module.
	qwr_init.error_handler = nrf_qwr_error_handler;

	err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
	APP_ERROR_CHECK(err_code);

	// Initialize NUS.
	memset(&nus_init, 0, sizeof(nus_init));

	nus_init.data_handler = nus_data_handler;

	err_code = ble_nus_init(&m_nus, &nus_init);
	APP_ERROR_CHECK(err_code);
}

static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
	switch (ble_adv_evt) {
	case BLE_ADV_EVT_FAST:
		debug("adv_evt_fast");
		//err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
		//APP_ERROR_CHECK(err_code);
		break;
	case BLE_ADV_EVT_IDLE:
		debug("adv_evt_idle");
		//sleep_mode_enter();
		break;
	default:
		break;
	}
}

static void advertising_init(void)
{
	ble_advertising_init_t init;

	memset(&init, 0, sizeof(init));

	init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
	init.advdata.include_appearance = false;
	init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

	init.srdata.uuids_complete.uuid_cnt = sizeof(m.adv_uuid) / sizeof(m.adv_uuid[0]);
	init.srdata.uuids_complete.p_uuids  = m.adv_uuid;

	init.config.ble_adv_fast_enabled  = true;
	init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
	init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
	init.evt_handler = on_adv_evt;

	uint32_t err_code = ble_advertising_init(&m_advertising, &init);
	APP_ERROR_CHECK(err_code);

	ble_advertising_conn_cfg_tag_set(&m_advertising, 1);
}

#if 0
NRF_BLE_SCAN_BUFFER
static void timers_init(void)
{
	ret_code_t err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);
}
#endif

static void adv_test(void)
{
	//timers_init();
	ret_code_t err_code = nrf_pwr_mgmt_init();
	APP_ERROR_CHECK(err_code);

	ble_stack_init();
	gap_params_init();
	gatt_init();
	services_init();
	advertising_init();
	ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
}

NRF_BLE_SCAN_DEF(m_scan);

static void scan_start(void)
{
    ret_code_t ret;

    ret = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(ret);

    //ret = bsp_indication_set(BSP_INDICATE_SCANNING);
    //APP_ERROR_CHECK(ret);
}

static void scan_evt_handler(scan_evt_t const *p_scan_evt)
{
	ret_code_t err_code;
	ble_gap_evt_connected_t *p_connected;

	switch(p_scan_evt->scan_evt_id) {
	case NRF_BLE_SCAN_EVT_CONNECTING_ERROR:
		err_code = p_scan_evt->params.connecting_err.err_code;
		APP_ERROR_CHECK(err_code);
		break;
	case NRF_BLE_SCAN_EVT_CONNECTED:
		p_connected = p_scan_evt->params.connected.p_connected;
		// Scan is automatically stopped by the connection.
		info("Connecting to target %02x%02x%02x%02x%02x%02x",
				p_connected->peer_addr.addr[0],
				p_connected->peer_addr.addr[1],
				p_connected->peer_addr.addr[2],
				p_connected->peer_addr.addr[3],
				p_connected->peer_addr.addr[4],
				p_connected->peer_addr.addr[5]);
		break;
	case NRF_BLE_SCAN_EVT_SCAN_TIMEOUT:
		info("Scan timed out.");
		scan_start();
		break;
	default:
		break;
	}
}

static void scan_init(void)
{
	ret_code_t          err_code;
	nrf_ble_scan_init_t init_scan;

	memset(&init_scan, 0, sizeof(init_scan));

	init_scan.connect_if_match = true;
	init_scan.conn_cfg_tag     = 1;

	err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
	APP_ERROR_CHECK(err_code);

#if 0
	err_code = nrf_ble_scan_filter_set(&m_scan, SCAN_UUID_FILTER, &m_nus_uuid);
	APP_ERROR_CHECK(err_code);

	err_code = nrf_ble_scan_filters_enable(&m_scan, NRF_BLE_SCAN_UUID_FILTER, false);
	APP_ERROR_CHECK(err_code);
#endif
}

#if 0
NRF_BLE_GQ_DEF(m_ble_gatt_queue,
		NRF_SDH_BLE_CENTRAL_LINK_COUNT,
		NRF_BLE_GQ_QUEUE_SIZE);
static void db_disc_handler(ble_db_discovery_evt_t *p_evt)
{
	ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
}
static void db_discovery_init(void)
{
	ble_db_discovery_init_t db_init;

	memset(&db_init, 0, sizeof(ble_db_discovery_init_t));

	db_init.evt_handler  = db_disc_handler;
	db_init.p_gatt_queue = &m_ble_gatt_queue;

	ret_code_t err_code = ble_db_discovery_init(&db_init);
	APP_ERROR_CHECK(err_code);
}
#endif

static void scan_test(void)
{
	ret_code_t err_code = nrf_pwr_mgmt_init();
	APP_ERROR_CHECK(err_code);

	ble_stack_init();
	//gap_params_init();
	gatt_init();
	//services_init();

	scan_init();
	scan_start();
}

#include "sleep.h"
void ble_test(void)
{
	//adv_test();
	scan_test();

	while (1) {
		if (glen > 0) {
			char str[256];
			int idx = 0;
			for (uint16_t i = 0; i < glen; i++) {
				idx += sprintf(&str[idx], "%02X ", gbuf[i]);
			}
			debug("LEN %u -> %d", glen, idx);
			for (int i = 0; i < idx; i += 32) {
				debug("%02u: %.*s", i, 32, &str[i]);
			}
			glen = 0;
		}
	}
}
#endif
