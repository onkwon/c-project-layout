#include "ble/ble.h"

#include <assert.h>
#include <string.h>

#include "esp_bt.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "libmcu/logging.h"

#define EXAMPLE_PROFILE_ID			0x55

static uint8_t device_name[] = "myexample";

static esp_ble_adv_params_t adv_params = {
	.adv_int_min = 0x100,
	.adv_int_max = 0x100,
	.adv_type = ADV_TYPE_IND,
	.own_addr_type = BLE_ADDR_TYPE_RANDOM,
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static uint8_t sec_service_uuid[16] = { /* LSB ... MSB */
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
	0x00, 0x10, 0x00, 0x00, 0x18, 0x0D, 0x00, 0x00,
};

static esp_ble_adv_data_t adv_config = {
	.set_scan_rsp = true,
	.include_name = true,
	.manufacturer_len = 0,//sizeof(device_name),
	.p_manufacturer_data = NULL,//device_name,

	.include_txpower = true,
	.min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
	.max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
	.appearance = 0x00,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = sizeof(sec_service_uuid),
	.p_service_uuid = sec_service_uuid,
	.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static void show_bonded_devices(void)
{
	int dev_num = esp_ble_get_bond_device_num();

	esp_ble_bond_dev_t *dev_list = malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
	esp_ble_get_bond_device_list(&dev_num, dev_list);
	info("Bonded devices: %d\n", dev_num);
	for (int i = 0; i < dev_num; i++) {
		info("\t%02x%02x%02x%02x%02x%02x",
				dev_list[i].bd_addr[0],
				dev_list[i].bd_addr[1],
				dev_list[i].bd_addr[2],
				dev_list[i].bd_addr[3],
				dev_list[i].bd_addr[4],
				dev_list[i].bd_addr[5]);
	}

	free(dev_list);
}

static void gap_event_handler(esp_gap_ble_cb_event_t event,
		esp_ble_gap_cb_param_t *param)
{
	esp_bd_addr_t bd_addr;

	debug("GAP event %d", event);

	switch (event) {
	case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
		if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
			error("advertising start failed, error status = %x",
					param->adv_start_cmpl.status);
			break;
		}
		debug("Start advertising");
		break;
	case ESP_GAP_BLE_NC_REQ_EVT: /* The app will receive this evt when the
					IO has DisplayYesNO capability and the
					peer device IO also has DisplayYesNo
					capability. show the passkey number to
					the user to confirm it with the number
					displayed by peer device. */
		esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
		info("ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%d",
				param->ble_security.key_notif.passkey);
		break;
	case ESP_GAP_BLE_SEC_REQ_EVT: /* send the positive(true) security
					 response to the peer device to accept
					 the security request. If not accept
					 the security request, should send the
					 security response with negative(false)
					 accept value*/
		esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
		break;
	case ESP_GAP_BLE_PASSKEY_NOTIF_EVT: /* the app will receive this evt
					       when the IO  has Output
					       capability and the peer device
					       IO has Input capability. show
					       the passkey number to the user
					       to input it in the peer device.
					       */
		info("The passkey Notify number:%06d", param->ble_security.key_notif.passkey);
		break;
	case ESP_GAP_BLE_KEY_EVT: //shows the ble key info share with peer device to the user.
		info("key type = %x", param->ble_security.ble_key.key_type);
		break;
	case ESP_GAP_BLE_AUTH_CMPL_EVT:
		memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
		info("remote BD_ADDR: %08x%04x",
				(bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
				(bd_addr[4] << 8) + bd_addr[5]);
		info("address type = %d", param->ble_security.auth_cmpl.addr_type);
		info("pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
		if(!param->ble_security.auth_cmpl.success) {
			info("fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
		} else {
			info("auth mode = %x",param->ble_security.auth_cmpl.auth_mode);
		}
		show_bonded_devices();

		//esp_ble_gap_disconnect(param->ble_security.auth_cmpl.bd_addr);
		break;
	default:
		break;
	}
}

static void gatts_event_handler(esp_gatts_cb_event_t event,
		esp_gatt_if_t gatts_if,
		esp_ble_gatts_cb_param_t *param)
{
	debug("GATT event %x", event);

	switch (event) {
	case ESP_GATTS_CONNECT_EVT:
		esp_ble_set_encryption(param->connect.remote_bda,
				ESP_BLE_SEC_ENCRYPT_MITM);
		break;
	case ESP_GATTS_DISCONNECT_EVT:
		esp_ble_gap_start_advertising(&adv_params);
		break;
	default:
		break;
	}
}

void ble_init(void)
{
	int err;

	err = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
	assert(err == ESP_OK);

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	err = esp_bt_controller_init(&bt_cfg);
	assert(err == ESP_OK);

	err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	assert(err == ESP_OK);

	err = esp_bluedroid_init();
	assert(err == ESP_OK);

	err = esp_bluedroid_enable();
	assert(err == ESP_OK);

	////////////////////////////////////////////////////////////////////
	err = esp_ble_gap_register_callback(gap_event_handler);
	assert(err == ESP_OK);
	err = esp_ble_gatts_register_callback(gatts_event_handler);
	assert(err == ESP_OK);
	err = esp_ble_gap_set_device_name((const char *)device_name);
	assert(err == ESP_OK);
	err = esp_ble_gap_config_adv_data(&adv_config);
	assert(err == ESP_OK);
#if 1
	esp_bd_addr_t rand_addr = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
	err = esp_ble_gap_set_rand_addr(rand_addr);
	assert(err == ESP_OK);
#else
	err = esp_ble_gap_config_local_privacy(true); // resolvable random addr
	assert(err == ESP_OK);
#endif
	err = esp_ble_gap_start_advertising(&adv_params);
	assert(err == ESP_OK);
	err = esp_ble_gatts_app_register(EXAMPLE_PROFILE_ID);
	assert(err == ESP_OK);

#if 1
	esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND; // bonding with peer device after authentication
	esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE; // set the IO capability to No output No input
	uint8_t key_size = 16; // the key size should be 7~16 bytes
	uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint32_t passkey = 123456; // set static passkey
	uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
	uint8_t oob_support = ESP_BLE_OOB_DISABLE;
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY,
			&passkey, sizeof(uint32_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE,
			&auth_req, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE,
			&iocap, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE,
			&key_size, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH,
			&auth_option, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT,
			&oob_support, sizeof(uint8_t));
	/* If your BLE device acts as a Slave, the init_key means you hope
	 * which types of key of the master should distribute to you, and the
	 * response key means which key you can distribute to the master;
	 * If your BLE device acts as a master, the response key means you hope
	 * which types of key of the slave should distribute to you, and the
	 * init key means which key you can distribute to the slave. */
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY,
			&init_key, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY,
			&rsp_key, sizeof(uint8_t));

	//remove_all_bonded_devices();
#endif
}
