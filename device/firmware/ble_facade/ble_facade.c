/**
 * Most of the boilerplate code come from the "nrf51-ble-app-lbs" example at https://github.com/NordicSemiconductor/nrf51-ble-app-lbs.
 */
#include "ble_facade.h"
#include <stdbool.h>
#include <string.h>
#include "nordic_common.h"
#include "app_timer.h"
#include "softdevice_handler.h"
#include "ble.h"
#include "ble_conn_params.h"
#include "ble_advdata.h"
#include "pstorage.h"
#include "../util/logging.h"
#include "../main_loop_call.h"
#include "../service/service_time.h"
#include "../service/service_proximity.h"
#include "../service/service_tour.h"
#include "../service/service_gathering.h"
#include "../service/service_fona.h"


#define DEVICE_NAME                     "GGTA"                                      /**< Name of device. Will be included in the advertising data. */

#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. WARNING: MUST BE THE SAME AS IN THE MAIN FILE.*/

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(20000, APP_TIMER_PRESCALER) /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (15 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time between each call to sd_ble_gap_conn_param_update after the first call (5 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

#define BASE_UUID {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define SERVICE_UUID                0xA000
#define IMEI_CHARACTERISTIC_UUID    0xA001
#define COMMAND_CHARACTERISTIC_UUID 0xA002

#define COMMAND_SET_TIME                   'T'
#define COMMAND_SET_PROXIMITY_LEVEL        'P'
#define COMMAND_SET_TOUR_INFO              'I'
#define COMMAND_CLEAR_TOUR_INFO            'C'
#define COMMAND_SCHEDULE_GATHERING         'G'
#define COMMAND_UNSCHEDULE_GATHERING       'U'
#define COMMAND_BEGIN_CHUNKED_TRANSFER     'B'
#define COMMAND_CONTINUE_CHUNKED_TRANSFER  'O'
#define COMMAND_END_CHUNKED_TRANSFER       'E'

#define MAX_CHUNKED_BUFFER_SIZE     400

// Private functions declaration
static void ble_facade_ble_evt_dispatch(ble_evt_t* p_ble_evt);
static void ble_facade_sys_evt_dispatch(uint32_t evt_id);
static void ble_facade_conn_params_error_handler(uint32_t nrf_error);
static void ble_facade_start_advertising();
static void ble_facade_dispatch_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_chuncked_transfer_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_set_time_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_set_proximity_level_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_set_tour_info_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_clear_tour_info_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_schedule_gathering_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_unschedule_gathering_command(uint16_t length, uint8_t* data);
static void ble_facade_handle_set_tour_info_command_in_main_loop();

// Private variables
static ble_gap_sec_params_t m_sec_params;                /**< Security requirements for this application. */
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID; /**< Handle of the current connection. */
static ble_gap_sec_keyset_t m_keys_exchanged;
static ble_gap_evt_auth_status_t m_auth_status;
static ble_gap_master_id_t m_p_master_id;

static ble_gatts_char_handles_t m_command_char_handles;
static ble_gatts_char_handles_t m_imei_char_handles;
static uint8_t m_imei_characteristic_value[IMEI_SIZE] = {'\0'};
static uint8_t m_command_characteristic_value[20];

static uint8_t m_chunked_transfer_buffer[MAX_CHUNKED_BUFFER_SIZE];
static uint16_t m_chunked_transfer_buffer_index = 0;

static char m_guide_phone_number[PHONE_NUMBER_SIZE];
static char m_sim_pin_code[PIN_NUMBER_SIZE];
static char m_apn[APN_SIZE];
static char m_apn_username[APN_USERNAME_SIZE];
static char m_apn_password[APN_PASSWORD_SIZE];


void ble_facade_init(char* imei) {
	uint32_t err_code;

	for (uint8_t i = 0; i < IMEI_SIZE; i++)
	{
		m_imei_characteristic_value[i] = imei[i];
	}

	// Initialize the SoftDevice handler module.
	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, false);

	// Enable BLE stack
	ble_enable_params_t ble_enable_params;
	memset(&ble_enable_params, 0, sizeof(ble_enable_params));
	ble_enable_params.gatts_enable_params.service_changed = 0;
	err_code = sd_ble_enable(&ble_enable_params);
	APP_ERROR_CHECK(err_code);

	ble_gap_addr_t addr;
	err_code = sd_ble_gap_address_get(&addr);
	APP_ERROR_CHECK(err_code);
	sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &addr);
	APP_ERROR_CHECK(err_code);

	// Subscribe for BLE and SoftDevice events
	err_code = softdevice_ble_evt_handler_set(ble_facade_ble_evt_dispatch);
	APP_ERROR_CHECK(err_code);
	err_code = softdevice_sys_evt_handler_set(ble_facade_sys_evt_dispatch);
	APP_ERROR_CHECK(err_code);

	// Initialize GAP (Generic Access Profile) parameters
	ble_gap_conn_sec_mode_t sec_mode;
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
	err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
	APP_ERROR_CHECK(err_code);

	ble_gap_conn_params_t gap_conn_params;
	memset(&gap_conn_params, 0, sizeof(gap_conn_params));
	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;
	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
	APP_ERROR_CHECK(err_code);

	// Initialize GATT (Generic Attribute Profile) service
	ble_uuid128_t base_uuid = {BASE_UUID};
	ble_uuid_t service_ble_uuid;
	err_code = sd_ble_uuid_vs_add(&base_uuid, &service_ble_uuid.type);
	APP_ERROR_CHECK(err_code);
	service_ble_uuid.uuid = SERVICE_UUID;
	uint16_t service_handle;
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_ble_uuid, &service_handle);
	APP_ERROR_CHECK(err_code);

	ble_gatts_attr_md_t imei_cccd_md;
	memset(&imei_cccd_md, 0, sizeof(imei_cccd_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&imei_cccd_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&imei_cccd_md.write_perm);
	imei_cccd_md.vloc = BLE_GATTS_VLOC_STACK;

	ble_gatts_char_md_t imei_char_md;
	memset(&imei_char_md, 0, sizeof(imei_char_md));
	imei_char_md.char_props.read   = 1;
	imei_char_md.char_props.notify = 1;
	imei_char_md.p_char_user_desc  = NULL;
	imei_char_md.p_char_pf         = NULL;
	imei_char_md.p_user_desc_md    = NULL;
	imei_char_md.p_cccd_md         = &imei_cccd_md;
	imei_char_md.p_sccd_md         = NULL;

	ble_uuid_t imei_characteristic_ble_uuid;
	imei_characteristic_ble_uuid.type = service_ble_uuid.type;
	imei_characteristic_ble_uuid.uuid = IMEI_CHARACTERISTIC_UUID;

	ble_gatts_attr_md_t imei_attr_md;
	memset(&imei_attr_md, 0, sizeof(imei_attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&imei_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&imei_attr_md.write_perm);
	imei_attr_md.vloc       = BLE_GATTS_VLOC_STACK;
	imei_attr_md.rd_auth    = 0;
	imei_attr_md.wr_auth    = 0;
	imei_attr_md.vlen       = 0;

	ble_gatts_attr_t imei_attr_char_value;
	memset(&imei_attr_char_value, 0, sizeof(imei_attr_char_value));
	imei_attr_char_value.p_uuid       = &imei_characteristic_ble_uuid;
	imei_attr_char_value.p_attr_md    = &imei_attr_md;
	imei_attr_char_value.init_len     = sizeof(m_imei_characteristic_value);
	imei_attr_char_value.init_offs    = 0;
	imei_attr_char_value.max_len      = sizeof(m_imei_characteristic_value);
	imei_attr_char_value.p_value      = m_imei_characteristic_value;

	err_code = sd_ble_gatts_characteristic_add(service_handle, &imei_char_md, &imei_attr_char_value, &m_imei_char_handles);
	APP_ERROR_CHECK(err_code);

	ble_gatts_char_md_t command_char_md;
	memset(&command_char_md, 0, sizeof(command_char_md));
	command_char_md.char_props.read   = 0;
	command_char_md.char_props.write  = 1;
	command_char_md.p_char_user_desc  = NULL;
	command_char_md.p_char_pf         = NULL;
	command_char_md.p_user_desc_md    = NULL;
	command_char_md.p_cccd_md         = NULL;
	command_char_md.p_sccd_md         = NULL;

	ble_uuid_t command_characteristic_ble_uuid;
	command_characteristic_ble_uuid.type = service_ble_uuid.type;
	command_characteristic_ble_uuid.uuid = COMMAND_CHARACTERISTIC_UUID;

	ble_gatts_attr_md_t command_attr_md;
	memset(&command_attr_md, 0, sizeof(command_attr_md));
	BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&command_attr_md.read_perm);
	BLE_GAP_CONN_SEC_MODE_SET_OPEN(&command_attr_md.write_perm);
	command_attr_md.vloc       = BLE_GATTS_VLOC_STACK;
	command_attr_md.rd_auth    = 0;
	command_attr_md.wr_auth    = 0;
	command_attr_md.vlen       = 0;

	ble_gatts_attr_t command_attr_char_value;
	memset(&command_attr_char_value, 0, sizeof(command_attr_char_value));
	command_attr_char_value.p_uuid       = &command_characteristic_ble_uuid;
	command_attr_char_value.p_attr_md    = &command_attr_md;
	command_attr_char_value.init_len     = sizeof(m_command_characteristic_value);
	command_attr_char_value.init_offs    = 0;
	command_attr_char_value.max_len      = sizeof(m_command_characteristic_value);
	command_attr_char_value.p_value      = m_command_characteristic_value;

	err_code = sd_ble_gatts_characteristic_add(service_handle, &command_char_md, &command_attr_char_value, &m_command_char_handles);
	APP_ERROR_CHECK(err_code);

	// Initialize advertising
	ble_uuid_t adv_uuids[] = {{SERVICE_UUID, service_ble_uuid.type}};
	ble_advdata_t advdata;
	ble_advdata_t scanrsp;
	memset(&advdata, 0, sizeof(advdata));
	advdata.name_type               = BLE_ADVDATA_FULL_NAME;
	advdata.include_appearance      = true;
	advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
	memset(&scanrsp, 0, sizeof(scanrsp));
	scanrsp.uuids_complete.uuid_cnt = sizeof(adv_uuids) / sizeof(adv_uuids[0]);
	scanrsp.uuids_complete.p_uuids = adv_uuids;
	err_code = ble_advdata_set(&advdata, &scanrsp);
	APP_ERROR_CHECK(err_code);

	// Initialize the connection parameters
	ble_conn_params_init_t cp_init;
	memset(&cp_init, 0, sizeof(cp_init));
	cp_init.p_conn_params = NULL;
	cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
	cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
	cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
	cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
	cp_init.disconnect_on_fail             = true;
	cp_init.error_handler                  = ble_facade_conn_params_error_handler;
	err_code = ble_conn_params_init(&cp_init);
	APP_ERROR_CHECK(err_code);

	// Initialize the security parameters
	m_sec_params.bond         = SEC_PARAM_BOND;
	m_sec_params.mitm         = SEC_PARAM_MITM;
	m_sec_params.io_caps      = SEC_PARAM_IO_CAPABILITIES;
	m_sec_params.oob          = SEC_PARAM_OOB;
	m_sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
	m_sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;

	// Start advertising
	ble_facade_start_advertising();
}

void ble_facade_wait_for_event() {
	uint32_t err_code = sd_app_evt_wait();
	APP_ERROR_CHECK(err_code);
}

/**
 * @brief Make the BLE stack to start sending advertisements.
 */
static void ble_facade_start_advertising() {
	ble_gap_adv_params_t adv_params;
	memset(&adv_params, 0, sizeof(adv_params));
	adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
	adv_params.p_peer_addr = NULL;
	adv_params.fp          = BLE_GAP_ADV_FP_ANY;
	adv_params.interval    = APP_ADV_INTERVAL;
	adv_params.timeout     = APP_ADV_TIMEOUT_IN_SECONDS;

	uint32_t err_code = sd_ble_gap_adv_start(&adv_params);
	APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function called for each received BLE event.
 */
static void ble_facade_ble_evt_dispatch(ble_evt_t* p_ble_evt) {
	uint32_t err_code;

	// Boilerplate code (do not change)
	switch (p_ble_evt->header.evt_id)
	{
	case BLE_GAP_EVT_CONNECTED:
		LOG("BLE Event: BLE_GAP_EVT_CONNECTED\r\n");
		m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
		break;
	case BLE_GAP_EVT_DISCONNECTED:
		LOG("BLE Event: BLE_GAP_EVT_DISCONNECTED\r\n");
		m_conn_handle = BLE_CONN_HANDLE_INVALID;
		ble_facade_start_advertising();
		break;
	case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
		err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_SUCCESS, &m_sec_params, &m_keys_exchanged);
		APP_ERROR_CHECK(err_code);
		break;
	case BLE_GATTS_EVT_SYS_ATTR_MISSING:
		err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0,BLE_GATTS_SYS_ATTR_FLAG_USR_SRVCS);
		APP_ERROR_CHECK(err_code);
		break;
	case BLE_GAP_EVT_AUTH_STATUS:
		m_auth_status = p_ble_evt->evt.gap_evt.params.auth_status;
		break;
	case BLE_GAP_EVT_SEC_INFO_REQUEST:
		if (m_p_master_id.ediv == p_ble_evt->evt.gap_evt.params.sec_info_request.master_id.ediv)
		{
			err_code = sd_ble_gap_sec_info_reply(
					m_conn_handle,
					&m_keys_exchanged.keys_central.p_enc_key->enc_info,
					&m_keys_exchanged.keys_central.p_id_key->id_info,
					NULL);
			APP_ERROR_CHECK(err_code);
			m_p_master_id.ediv = p_ble_evt->evt.gap_evt.params.sec_info_request.master_id.ediv;
		}
		else
		{
			// No keys found for this device
			err_code = sd_ble_gap_sec_info_reply(m_conn_handle, NULL, NULL, NULL);
			APP_ERROR_CHECK(err_code);
		}
		break;
	case BLE_GAP_EVT_TIMEOUT:
		if (p_ble_evt->evt.gap_evt.params.timeout.src == BLE_GAP_TIMEOUT_SRC_ADVERTISING)
		{
			// Because of a bug, it is impossible to avoid timeout advertising, so we restart it manually
			// For more info, see https://devzone.nordicsemi.com/question/39418/infinite-advertising-with-sd-800/
			ble_facade_start_advertising();

			// Original timeout code:
			// Go to system-off mode (this function will not return; wakeup will cause a reset)
			//err_code = sd_power_system_off();
			//APP_ERROR_CHECK(err_code);
		}
		break;
	}

	ble_conn_params_on_ble_evt(p_ble_evt);

	// Not boilerplate code anymore
	ble_gatts_evt_write_t* p_evt_write;
	switch (p_ble_evt->header.evt_id)
	{
	case BLE_GATTS_EVT_WRITE:
		p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
		if (p_evt_write->handle == m_command_char_handles.value_handle)
		{
			ble_facade_dispatch_command(p_evt_write->len, p_evt_write->data);
		}
		break;
	}
}

/**
 * @brief Function called for each received System (SOC) event.
 */
static void ble_facade_sys_evt_dispatch(uint32_t evt_id) {
	pstorage_sys_event_handler(evt_id);
}

/**
 * @brief Function for handling a Connection Parameters error.
 */
static void ble_facade_conn_params_error_handler(uint32_t nrf_error) {
	APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief Function called when the GATT client has sent a command.
 */
static void ble_facade_dispatch_command(uint16_t length, uint8_t* data) {
	if (length <= 0)
		return;

	switch(data[0])
	{
	case COMMAND_SET_TIME:
		LOG("BLE command: COMMAND_SET_TIME\r\n");
		ble_facade_handle_set_time_command(length, data);
		break;
    case COMMAND_SET_PROXIMITY_LEVEL:
		LOG("BLE command: COMMAND_SET_PROXIMITY_LEVEL\r\n");
    	ble_facade_handle_set_proximity_level_command(length, data);
        break;
    case COMMAND_SET_TOUR_INFO:
		LOG("BLE command: COMMAND_SET_TOUR_INFO\r\n");
    	ble_facade_handle_set_tour_info_command(length, data);
        break;
    case COMMAND_CLEAR_TOUR_INFO:
		LOG("BLE command: COMMAND_CLEAR_TOUR_INFO\r\n");
    	ble_facade_handle_clear_tour_info_command(length, data);
        break;
    case COMMAND_SCHEDULE_GATHERING:
		LOG("BLE command: COMMAND_SCHEDULE_GATHERING\r\n");
    	ble_facade_handle_schedule_gathering_command(length, data);
        break;
    case COMMAND_UNSCHEDULE_GATHERING:
		LOG("BLE command: COMMAND_UNSCHEDULE_GATHERING\r\n");
    	ble_facade_handle_unschedule_gathering_command(length, data);
        break;

    case COMMAND_BEGIN_CHUNKED_TRANSFER:
		LOG("BLE command: COMMAND_BEGIN_CHUNKED_TRANSFER\r\n");
    	ble_facade_handle_chuncked_transfer_command(length, data);
		break;
    case COMMAND_CONTINUE_CHUNKED_TRANSFER:
		LOG("BLE command: COMMAND_CONTINUE_CHUNKED_TRANSFER\r\n");
    	ble_facade_handle_chuncked_transfer_command(length, data);
		break;
    case COMMAND_END_CHUNKED_TRANSFER:
		LOG("BLE command: COMMAND_END_CHUNKED_TRANSFER\r\n");
    	ble_facade_handle_chuncked_transfer_command(length, data);
		break;
    default:
		LOGF("BLE command: UNKNOWN %c\r\n", data[0]);
		break;
	}
}

static void ble_facade_handle_chuncked_transfer_command(uint16_t length, uint8_t* data) {
	if (length >= 1)
	{
		if (COMMAND_BEGIN_CHUNKED_TRANSFER == data[0])
		{
			m_chunked_transfer_buffer_index = 0;
		}

		for (uint8_t i = 1; i < length && m_chunked_transfer_buffer_index < MAX_CHUNKED_BUFFER_SIZE; i++)
		{
			m_chunked_transfer_buffer[m_chunked_transfer_buffer_index] = data[i];
			m_chunked_transfer_buffer_index++;
		}
		if (m_chunked_transfer_buffer_index >= MAX_CHUNKED_BUFFER_SIZE)
		{
			LOG("BLE ERROR: chunked buffer full\r\n");
		}
		else
		{
			LOGF("BLE chunked buffer size: %u\r\n", m_chunked_transfer_buffer_index);
		}

		if (COMMAND_END_CHUNKED_TRANSFER == data[0])
		{
			ble_facade_dispatch_command(m_chunked_transfer_buffer_index, m_chunked_transfer_buffer);
			m_chunked_transfer_buffer_index = 0;
		}
	}
}

static void ble_facade_handle_set_time_command(uint16_t length, uint8_t* data) {
    if (length >= 4 && COMMAND_SET_TIME == data[0])
    {
        uint8_t hour = data[1];
        uint8_t minute = data[2];
        uint8_t second = data[3];
        service_time_set_current_time(hour, minute, second);
    }
}

static void ble_facade_handle_set_proximity_level_command(uint16_t length, uint8_t* data) {
	if (length >= 2 && COMMAND_SET_PROXIMITY_LEVEL == data[0])
	{
		uint8_t proximity_level = data[1];
		switch (proximity_level)
		{
		case 1:
			service_proximity_set_level(VERY_FAR);
			break;
		case 2:
			service_proximity_set_level(FAR);
			break;
		case 3:
			service_proximity_set_level(AVERAGE);
			break;
		case 4:
			service_proximity_set_level(CLOSE);
			break;
		case 5:
			service_proximity_set_level(VERY_CLOSE);
			break;
		}
	}
}

static void ble_facade_handle_set_tour_info_command(uint16_t length, uint8_t* data) {
	if (length >= 6 && COMMAND_SET_TOUR_INFO == data[0])
	{
		// Extract the guide phone number
	    uint8_t guide_phone_number_length = data[5];
        for (uint8_t i = 0; i < guide_phone_number_length && i < PHONE_NUMBER_SIZE; i++) {
        	m_guide_phone_number[i] = data[6 + i];
        }
        m_guide_phone_number[MIN(guide_phone_number_length, PHONE_NUMBER_SIZE - 1)] = '\0';

		// Extract the SIM card info
        m_sim_pin_code[0] = data[1];
        m_sim_pin_code[1] = data[2];
        m_sim_pin_code[2] = data[3];
        m_sim_pin_code[3] = data[4];
        m_sim_pin_code[4] = '\0';
        uint8_t apn_length = data[6 + guide_phone_number_length];
        for (uint8_t i = 0; i < apn_length && i < APN_SIZE; i++) {
        	m_apn[i] = data[7 + guide_phone_number_length + i];
        }
        m_apn[MIN(apn_length, APN_SIZE - 1)] = '\0';
        uint8_t apn_username_length = data[7 + guide_phone_number_length + apn_length];
        for (uint8_t i = 0; i < apn_username_length && i < APN_USERNAME_SIZE; i++) {
        	m_apn_username[i] = data[8 + guide_phone_number_length + apn_length + i];
        }
        m_apn_username[MIN(apn_username_length, APN_USERNAME_SIZE - 1)] = '\0';
        uint8_t apn_password_length = data[8 + guide_phone_number_length + apn_length + apn_username_length];
        for (uint8_t i = 0; i < apn_password_length && i < APN_PASSWORD_SIZE; i++) {
        	m_apn_password[i] = data[9 + guide_phone_number_length + apn_length + apn_username_length + i];
        }
        m_apn_password[MIN(apn_password_length, APN_PASSWORD_SIZE - 1)] = '\0';

        // Give the configuration to the service
        call_in_main_loop(ble_facade_handle_set_tour_info_command_in_main_loop);
	}
}
static void ble_facade_handle_set_tour_info_command_in_main_loop() {
	service_fona_set_guide_phone_number(m_guide_phone_number);
    service_fona_unlock_sim(m_sim_pin_code, m_apn, m_apn_username, m_apn_password);
	service_tour_set_tour_configured(true);
}

static void ble_facade_handle_clear_tour_info_command(uint16_t length, uint8_t* data) {
	if (length >= 1 && COMMAND_CLEAR_TOUR_INFO == data[0])
	{
		service_tour_set_tour_configured(false);
	}
}

static void ble_facade_handle_schedule_gathering_command(uint16_t length, uint8_t* data) {
	if (length >= 4 && COMMAND_SCHEDULE_GATHERING == data[0])
	{
		uint8_t hour = data[1];
		uint8_t minute = data[2];
		uint16_t description_length = data[3] + 1;
		if (description_length > SERVICE_GATHERING_MAX_DESCRIPTION_LENGTH - 1)
		{
			description_length = SERVICE_GATHERING_MAX_DESCRIPTION_LENGTH - 1;
		}
		char description[description_length];
		for (uint16_t i = 0; i < description_length; i++)
		{
			description[i] = data[4 + i];
		}
		description[description_length - 1] = '\0';

		service_gathering_schedule(hour, minute, description);
	}
}

static void ble_facade_handle_unschedule_gathering_command(uint16_t length, uint8_t* data) {
	if (length >= 1 && COMMAND_UNSCHEDULE_GATHERING == data[0])
	{
		service_gathering_unschedule();
	}
}
