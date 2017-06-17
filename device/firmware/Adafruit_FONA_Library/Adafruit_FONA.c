/***************************************************
  This is a library for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

 /*
  *  Modified by Marc PLOUHINEC 07/10/2015 for use in nRF51 SDK
  */

#include "Adafruit_FONA.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "app_uart.h"
#include "nrf_delay.h"
#include "nrf_error.h"
#include "nrf_gpio.h"
#include "nordic_common.h"
#include "../util/logging.h"

//#define ENABLE_RAW_FONA_LOGS

#define UART_TX_BUF_SIZE 128                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 128                         /**< UART RX buffer size. */

// Private functions declaration
static void uart_event_handler(app_uart_evt_t* p_event);
static bool is_rx_buffer_full();
static bool is_rx_buffer_empty();
static void increment_rx_buffer_in_index();
static void increment_rx_buffer_out_index();

// HTTP helpers
static bool Adafruit_FONA_HTTP_setup(char *url);

static void flushInput();
static uint16_t readRaw(uint16_t b);
static uint8_t readline(uint16_t timeout, bool multiline);
static uint8_t getReply(const char* send, uint16_t timeout);
static uint8_t getReplyWithPrefixAndStringSuffix(const char* prefix, char *suffix, uint16_t timeout);
static uint8_t getReplyWithPrefixAndIntegerSuffix(const char* prefix, int32_t suffix, uint16_t timeout);
static uint8_t getReplyWithPrefixAndTwoIntegerSuffixes(const char* prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout);
static uint8_t getReplyQuoted(const char* prefix, const char* suffix, uint16_t timeout);

static bool sendCheckReply(const char* send, const char* reply, uint16_t timeout);
static bool sendCheckReplyWithPrefixAndStringSuffix(const char* prefix, char *suffix, const char* reply, uint16_t timeout);
static bool sendCheckReplyWithPrefixAndIntegerSuffix(const char* prefix, int32_t suffix, const char* reply, uint16_t timeout);
static bool sendCheckReplyWithPrefixAndTwoIntegerSuffixes(const char* prefix, int32_t suffix, int32_t suffix2, const char* reply, uint16_t timeout);
static bool sendCheckReplyQuoted(const char* prefix, const char* suffix, const char* reply, uint16_t timeout);

static bool parseReplyWithIntegerValue(const char* toreply, uint16_t *v, char divider, uint8_t index);
static bool parseReply(const char* toreply, char *v, char divider, uint8_t index);
static bool parseReplyQuoted(const char* toreply, char* v, int maxlen, char divider, uint8_t index);

static bool sendParseReply(const char* tosend, const char* toreply, uint16_t *v, char divider, uint8_t index);


// Private variables
static uint8_t m_tx_pin_number;
static uint8_t m_rx_pin_number;
static uint8_t m_rst_pin_number;
static Adafruit_FONA_event_handler_t m_event_handler;

static char replybuffer[255];
static const char* m_default_apn = "FONAnet";
static char* m_apn = NULL;
static char* m_apnusername = NULL;
static char* m_apnpassword = NULL;
static bool httpsredirect = false;
static const char* m_default_useragent = "FONA";
static char* m_useragent = NULL;
static bool _incomingCall = false;

// Circular buffer used to receive serial data from an interruption
static uint8_t m_rx_buffer[RX_BUFFER_SIZE + 1] = {0};
static uint16_t m_rx_buffer_in_index = 0;                  // Index where new data is added to the buffer
static uint16_t m_rx_buffer_out_index = 0;                 // Index where data is removed from the buffer
static char m_current_received_line[RX_BUFFER_SIZE] = {0}; // Array containing the current received line
static uint16_t m_current_received_line_size = 0;


void Adafruit_FONA_init(uint8_t tx_pin_number, uint8_t rx_pin_number, uint8_t rst_pin_number) {
	m_tx_pin_number = tx_pin_number;
	m_rx_pin_number = rx_pin_number;
	m_rst_pin_number = rst_pin_number;

	m_apn = (char*) m_default_apn;
	m_useragent = (char*) m_default_useragent;

	nrf_gpio_cfg_output(m_rst_pin_number);
    nrf_gpio_pin_clear(m_rst_pin_number);
}

bool Adafruit_FONA_begin(uint32_t baudrate) {
	uint32_t err_code;
	const app_uart_comm_params_t comm_params =
	{
			m_tx_pin_number,
			m_rx_pin_number,
			0,
			0,
			APP_UART_FLOW_CONTROL_DISABLED,
			false,
			baudrate
	};
    APP_UART_FIFO_INIT(&comm_params, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, uart_event_handler, APP_IRQ_PRIORITY_LOW, err_code);
    APP_ERROR_CHECK(err_code);

    nrf_gpio_pin_set(m_rst_pin_number);
    nrf_delay_ms(10);
    nrf_gpio_pin_clear(m_rst_pin_number);
    nrf_delay_ms(100);
    nrf_gpio_pin_set(m_rst_pin_number);

    // give 3 seconds to reboot
    nrf_delay_ms(3000);

    while (Adafruit_FONA_readable()) Adafruit_FONA_getc();

    sendCheckReply("AT", "OK", FONA_DEFAULT_TIMEOUT_MS);
    nrf_delay_ms(100);
    sendCheckReply("AT", "OK", FONA_DEFAULT_TIMEOUT_MS);
    nrf_delay_ms(100);
    sendCheckReply("AT", "OK", FONA_DEFAULT_TIMEOUT_MS);
    nrf_delay_ms(100);

    // turn off Echo!
    sendCheckReply("ATE0", "OK", FONA_DEFAULT_TIMEOUT_MS);
    nrf_delay_ms(100);

    if (! sendCheckReply("ATE0", "OK", FONA_DEFAULT_TIMEOUT_MS)) {
        return false;
    }

    return true;
}

void Adafruit_FONA_register_event_handler(Adafruit_FONA_event_handler_t event_handler) {
	m_event_handler = event_handler;
}

/********* Stream ********************************************/

bool Adafruit_FONA_putc(uint8_t value) {
	#ifdef ENABLE_RAW_FONA_LOGS
	LOGF("%c", value);
	#endif
    return app_uart_put(value) == NRF_SUCCESS;
}

uint8_t Adafruit_FONA_getc() {
    __disable_irq(); // Start Critical Section - don't interrupt while changing global buffer variables

    // Wait for data if the buffer is empty
    if (is_rx_buffer_empty()) {
        __enable_irq(); // End Critical Section - need to allow rx interrupt to get new characters for buffer

        while(is_rx_buffer_empty());

        __disable_irq(); // Start Critical Section - don't interrupt while changing global buffer variables
    }

    uint8_t data = m_rx_buffer[m_rx_buffer_out_index];
    increment_rx_buffer_out_index();

    __enable_irq(); // End Critical Section

	#ifdef ENABLE_RAW_FONA_LOGS
	LOGF("%c", data);
	#endif
    return data;
}

bool Adafruit_FONA_readable() {
    return !is_rx_buffer_empty();
}

bool Adafruit_FONA_print(char* value) {
	uint16_t length = strnlen(value, UART_TX_BUF_SIZE);
	for (uint16_t i = 0; i < length; i++)
	{
		if (!Adafruit_FONA_putc((uint8_t) value[i]))
		{
			return false;
		}
	}
	return true;
}

bool Adafruit_FONA_putc16(uint16_t value) {
	char value_as_string[6];
	sprintf(value_as_string, "%d", value);
    return Adafruit_FONA_print(value_as_string);
}

bool Adafruit_FONA_putc32(uint32_t value) {
	char value_as_string[11];
	sprintf(value_as_string, "%d", (int) value);
    return Adafruit_FONA_print(value_as_string);
}

static void uart_event_handler(app_uart_evt_t* p_event) {
	if (p_event->evt_type == APP_UART_DATA_READY)
	{
	    while (!is_rx_buffer_full())
	    {
	        uint8_t data;
	        if (app_uart_get(&data) != NRF_SUCCESS)
	        {
	        	break;
	        }
	        m_rx_buffer[m_rx_buffer_in_index] = data;

	        //
	        // Analyze the received data in order to detect events like RING or NO CARRIER
	        //

	        // Copy the data in the current line
	        if (m_current_received_line_size < RX_BUFFER_SIZE && data != '\r' && data != '\n')
	        {
	            m_current_received_line[m_current_received_line_size] = (char) data;
	            m_current_received_line_size++;
	        }

	        // Check if the line is complete
	        if (data == '\n')
	        {
	        	m_current_received_line[m_current_received_line_size] = 0;

	            if (m_event_handler != NULL) {
	                // Check if we have a special event
	                if (strcmp(m_current_received_line, "RING") == 0)
	                {
	                	m_event_handler(RING);
	                	_incomingCall = true;
	                }
	                else if (strcmp(m_current_received_line, "NO CARRIER") == 0)
	                {
	                	m_event_handler(NO_CARRIER);
	                	_incomingCall = false;
	                }
	                else if (strcmp(m_current_received_line, "BUSY") == 0)
	                {
	                	m_event_handler(BUSY);
	                	_incomingCall = false;
	                }
	            }

	            m_current_received_line_size = 0;
	        }

	        increment_rx_buffer_in_index();
	    }
	}
	else if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_communication);
	}
	else if (p_event->evt_type == APP_UART_FIFO_ERROR)
	{
		APP_ERROR_HANDLER(p_event->data.error_code);
	}
}

static bool is_rx_buffer_full() {
	return ((m_rx_buffer_in_index + 1) % RX_BUFFER_SIZE) == m_rx_buffer_out_index;
}

static bool is_rx_buffer_empty() {
	return m_rx_buffer_in_index == m_rx_buffer_out_index;
}

static void increment_rx_buffer_in_index() {
	m_rx_buffer_in_index = (m_rx_buffer_in_index + 1) % RX_BUFFER_SIZE;
}

static void increment_rx_buffer_out_index() {
	m_rx_buffer_out_index = (m_rx_buffer_out_index + 1) % RX_BUFFER_SIZE;
}

/********* Real Time Clock ********************************************/

bool Adafruit_FONA_enableRTC(uint8_t i) {
    if (! sendCheckReplyWithPrefixAndIntegerSuffix("AT+CLTS=", i, "OK", FONA_DEFAULT_TIMEOUT_MS))
        return false;
    return sendCheckReply("AT&W", "OK", FONA_DEFAULT_TIMEOUT_MS);
}


/********* BATTERY & ADC ********************************************/

/* returns value in mV (uint16_t) */
bool Adafruit_FONA_getBattVoltage(uint16_t *v) {
    return sendParseReply("AT+CBC", "+CBC: ", v, ',', 2);
}

/* returns the percentage charge of battery as reported by sim800 */
bool Adafruit_FONA_getBattPercent(uint16_t *p) {
    return sendParseReply("AT+CBC", "+CBC: ", p, ',', 1);
}

bool Adafruit_FONA_getADCVoltage(uint16_t *v) {
    return sendParseReply("AT+CADC?", "+CADC: 1,", v, ',', 0);
}

/********* SIM ***********************************************************/

uint8_t Adafruit_FONA_unlockSIM(char *pin)
{
    char sendbuff[14] = "AT+CPIN=";
    sendbuff[8] = pin[0];
    sendbuff[9] = pin[1];
    sendbuff[10] = pin[2];
    sendbuff[11] = pin[3];
    sendbuff[12] = 0;

    return sendCheckReply(sendbuff, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

uint8_t Adafruit_FONA_getSIMCCID(char *ccid) {
    getReply("AT+CCID", FONA_DEFAULT_TIMEOUT_MS);
    // up to 20 chars
    strncpy(ccid, replybuffer, 20);
    ccid[20] = 0;

    readline(FONA_DEFAULT_TIMEOUT_MS, false); // eat 'OK'

    return strlen(ccid);
}

/********* IMEI **********************************************************/

uint8_t Adafruit_FONA_getIMEI(char *imei) {
    getReply("AT+GSN", FONA_DEFAULT_TIMEOUT_MS);

    // up to 15 chars
    strncpy(imei, replybuffer, 15);
    imei[15] = 0;

    readline(FONA_DEFAULT_TIMEOUT_MS, false); // eat 'OK'

    return strlen(imei);
}

/********* NETWORK *******************************************************/

uint8_t Adafruit_FONA_getNetworkStatus(void) {
    uint16_t status;

    if (! sendParseReply("AT+CREG?", "+CREG: ", &status, ',', 1)) return 0;

    return status;
}


uint8_t Adafruit_FONA_getRSSI(void) {
    uint16_t reply;

    if (! sendParseReply("AT+CSQ", "+CSQ: ", &reply, ',', 0) ) return 0;

    return reply;
}

/********* AUDIO *******************************************************/

bool Adafruit_FONA_setAudio(uint8_t a) {
    // 0 is headset, 1 is external audio
    if (a > 1) return false;

    return sendCheckReplyWithPrefixAndIntegerSuffix("AT+CHFA=", a, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

uint8_t Adafruit_FONA_getVolume(void) {
    uint16_t reply;

    if (! sendParseReply("AT+CLVL?", "+CLVL: ", &reply, ',', 0) ) return 0;

    return reply;
}

bool Adafruit_FONA_setVolume(uint8_t i) {
    return sendCheckReplyWithPrefixAndIntegerSuffix("AT+CLVL=", i, "OK", FONA_DEFAULT_TIMEOUT_MS);
}


bool Adafruit_FONA_playDTMF(char dtmf) {
    char str[4];
    str[0] = '\"';
    str[1] = dtmf;
    str[2] = '\"';
    str[3] = 0;
    return sendCheckReplyWithPrefixAndStringSuffix("AT+CLDTMF=3,", str, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_playToolkitTone(uint8_t t, uint16_t len) {
    return sendCheckReplyWithPrefixAndTwoIntegerSuffixes("AT+STTONE=1,", t, len, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_setMicVolume(uint8_t a, uint8_t level) {
    // 0 is headset, 1 is external audio
    if (a > 1) return false;

    return sendCheckReplyWithPrefixAndTwoIntegerSuffixes("AT+CMIC=", a, level, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

/********* FM RADIO *******************************************************/


bool Adafruit_FONA_FMradio(bool onoff, uint8_t a) {
    if (! onoff) {
        return sendCheckReply("AT+FMCLOSE", "OK", FONA_DEFAULT_TIMEOUT_MS);
    }

    // 0 is headset, 1 is external audio
    if (a > 1) return false;

    return sendCheckReplyWithPrefixAndIntegerSuffix("AT+FMOPEN=", a, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_tuneFMradio(uint16_t station) {
    // Fail if FM station is outside allowed range.
    if ((station < 870) || (station > 1090))
        return false;

    return sendCheckReplyWithPrefixAndIntegerSuffix("AT+FMFREQ=", station, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_setFMVolume(uint8_t i) {
    // Fail if volume is outside allowed range (0-6).
    if (i > 6) {
    return false;
    }
    // Send FM volume command and verify response.
    return sendCheckReplyWithPrefixAndIntegerSuffix("AT+FMVOLUME=", i, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

int8_t Adafruit_FONA_getFMVolume() {
    uint16_t level;

    if (! sendParseReply("AT+FMVOLUME?", "+FMVOLUME: ", &level, ',', 0) ) return 0;

    return level;
}

int8_t Adafruit_FONA_getFMSignalLevel(uint16_t station) {
    // Fail if FM station is outside allowed range.
    if ((station < 875) || (station > 1080)) {
        return -1;
    }

    // Send FM signal level query command.
    // Note, need to explicitly send timeout so right overload is chosen.
    getReplyWithPrefixAndIntegerSuffix("AT+FMSIGNAL=", station, FONA_DEFAULT_TIMEOUT_MS);
    // Check response starts with expected value.
    char *p = strstr(replybuffer, "+FMSIGNAL: ");
    if (p == 0) return -1;
    p+=11;
    // Find second colon to get start of signal quality.
    p = strchr(p, ':');
    if (p == 0) return -1;
    p+=1;
    // Parse signal quality.
    int8_t level = atoi(p);
    readline(FONA_DEFAULT_TIMEOUT_MS, false);  // eat the "OK"
    return level;
}

/********* PWM/BUZZER **************************************************/

bool Adafruit_FONA_setPWM(uint16_t period, uint8_t duty) {
    if (period > 2000) return false;
    if (duty > 100) return false;

    return sendCheckReplyWithPrefixAndTwoIntegerSuffixes("AT+SPWM=0,", period, duty, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

/********* CALL PHONES **************************************************/
bool Adafruit_FONA_callPhone(char *number) {
    char sendbuff[35] = "ATD";
    strncpy(sendbuff+3, number, MIN((int)30, (int)strlen(number)));
    uint8_t x = strlen(sendbuff);
    sendbuff[x] = ';';
    sendbuff[x+1] = 0;

    return sendCheckReply(sendbuff, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_hangUp(void) {
    return sendCheckReply("ATH0", "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_pickUp(void) {
    return sendCheckReply("ATA", "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_callerIdNotification(bool enable) {
    if(enable){
        return sendCheckReply("AT+CLIP=1", "OK", FONA_DEFAULT_TIMEOUT_MS);
    }

    return sendCheckReply("AT+CLIP=0", "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_incomingCallNumber(char* phonenum) {
    //+CLIP: "<incoming phone number>",145,"",0,"",0
    if(!_incomingCall)
        return false;

    readline(FONA_DEFAULT_TIMEOUT_MS, false);
    while(!strcmp(replybuffer, "RING") == 0) {
        flushInput();
        readline(FONA_DEFAULT_TIMEOUT_MS, false);
    }

    readline(FONA_DEFAULT_TIMEOUT_MS, false); //reads incoming phone number line

    parseReply("+CLIP: \"", phonenum, '"', 0);

    _incomingCall = false;
    return true;
}

/********* SMS **********************************************************/

uint8_t Adafruit_FONA_getSMSInterrupt(void) {
    uint16_t reply;

    if (! sendParseReply("AT+CFGRI?", "+CFGRI: ", &reply, ',', 0) ) return 0;

    return reply;
}

bool Adafruit_FONA_setSMSInterrupt(uint8_t i) {
    return sendCheckReplyWithPrefixAndIntegerSuffix("AT+CFGRI=", i, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

int8_t Adafruit_FONA_getNumSMS(void) {
    uint16_t numsms;

    if (! sendCheckReply("AT+CMGF=1", "OK", FONA_DEFAULT_TIMEOUT_MS)) return -1;
    // ask how many sms are stored

    if (! sendParseReply("AT+CPMS?", "+CPMS: \"SM_P\",", &numsms, ',', 0) ) return -1;

    return numsms;
}

// Reading SMS's is a bit involved so we don't use helpers that may cause delays or debug
// printouts!
bool Adafruit_FONA_readSMS(uint8_t i, char *smsbuff, uint16_t maxlen, uint16_t *readlen) {
    // text mode
    if (! sendCheckReply("AT+CMGF=1", "OK", FONA_DEFAULT_TIMEOUT_MS)) return false;

    // show all text mode parameters
    if (! sendCheckReply("AT+CSDH=1", "OK", FONA_DEFAULT_TIMEOUT_MS)) return false;

    // parse out the SMS len
    uint16_t thesmslen = 0;

    //getReply(F("AT+CMGR="), i, 1000);  //  do not print debug!
    Adafruit_FONA_print("AT+CMGR=");
    Adafruit_FONA_putc(i);
    Adafruit_FONA_print("\r\n");
    readline(1000, false); // timeout

    // parse it out...
    if (! parseReplyWithIntegerValue("+CMGR:", &thesmslen, ',', 11)) {
        *readlen = 0;
        return false;
    }

    readRaw(thesmslen);

    flushInput();

    uint16_t thelen = MIN(maxlen, (uint16_t)strlen(replybuffer));
    strncpy(smsbuff, replybuffer, thelen);
    smsbuff[thelen] = 0; // end the string

    *readlen = thelen;
    return true;
}

// Retrieve the sender of the specified SMS message and copy it as a string to
// the sender buffer.  Up to senderlen characters of the sender will be copied
// and a null terminator will be added if less than senderlen charactesr are
// copied to the result.  Returns true if a result was successfully retrieved,
// otherwise false.
bool Adafruit_FONA_getSMSSender(uint8_t i, char *sender, int senderlen) {
    // Ensure text mode and all text mode parameters are sent.
    if (! sendCheckReply("AT+CMGF=1", "OK", FONA_DEFAULT_TIMEOUT_MS)) return false;
    if (! sendCheckReply("AT+CSDH=1", "OK", FONA_DEFAULT_TIMEOUT_MS)) return false;
    // Send command to retrieve SMS message and parse a line of response.
    Adafruit_FONA_print("AT+CMGR=");
    Adafruit_FONA_putc(i);
    Adafruit_FONA_print("\r\n");
    readline(1000, false);
    // Parse the second field in the response.
    bool result = parseReplyQuoted("+CMGR:", sender, senderlen, ',', 1);
    // Drop any remaining data from the response.
    flushInput();
    return result;
}

bool Adafruit_FONA_sendSMS(char *smsaddr, char *smsmsg) {
    if (! sendCheckReply("AT+CMGF=1", "OK", FONA_DEFAULT_TIMEOUT_MS)) return -1;

    char sendcmd[30] = "AT+CMGS=\"";
    strncpy(sendcmd+9, smsaddr, 30-9-2);  // 9 bytes beginning, 2 bytes for close quote + null
    sendcmd[strlen(sendcmd)] = '\"';

    if (! sendCheckReply(sendcmd, "> ", FONA_DEFAULT_TIMEOUT_MS)) return false;
    Adafruit_FONA_print(smsmsg);
    Adafruit_FONA_print("\r\n\r\n");
    Adafruit_FONA_putc(0x1A);
    readline(10000, false); // read the +CMGS reply, wait up to 10 seconds!!!
    //Serial.print("* "); Serial.println(replybuffer);
    if (strstr(replybuffer, "+CMGS") == 0) {
        return false;
    }
    readline(1000, false); // read OK
    //Serial.print("* "); Serial.println(replybuffer);

    if (strcmp(replybuffer, "OK") != 0) {
        return false;
    }

    return true;
}


bool Adafruit_FONA_deleteSMS(uint8_t i) {
    if (! sendCheckReply("AT+CMGF=1", "OK", FONA_DEFAULT_TIMEOUT_MS)) return -1;
    // read an sms
    char sendbuff[12] = "AT+CMGD=000";
    sendbuff[8] = (i / 100) + '0';
    i %= 100;
    sendbuff[9] = (i / 10) + '0';
    i %= 10;
    sendbuff[10] = i + '0';

    return sendCheckReply(sendbuff, "OK", 2000);
}

/********* TIME **********************************************************/

bool Adafruit_FONA_enableNetworkTimeSync(bool onoff) {
    if (onoff) {
        if (! sendCheckReply("AT+CLTS=1", "OK", FONA_DEFAULT_TIMEOUT_MS))
            return false;
    } else {
        if (! sendCheckReply("AT+CLTS=0", "OK", FONA_DEFAULT_TIMEOUT_MS))
            return false;
    }

    flushInput(); // eat any 'Unsolicted Result Code'

    return true;
}

bool Adafruit_FONA_enableNTPTimeSync(bool onoff, const char* ntpserver) {
    if (onoff) {
        if (! sendCheckReply("AT+CNTPCID=1", "OK", FONA_DEFAULT_TIMEOUT_MS))
            return false;

        Adafruit_FONA_print("AT+CNTP=\"");
        if (ntpserver != 0) {
        	Adafruit_FONA_print((char*) ntpserver);
        } else {
        	Adafruit_FONA_print("pool.ntp.org");
        }
        Adafruit_FONA_print("\",0\r\n");
        readline(FONA_DEFAULT_TIMEOUT_MS, false);
        if (strcmp(replybuffer, "OK") != 0)
            return false;

        if (! sendCheckReply("AT+CNTP", "OK", 10000))
            return false;

        uint16_t status;
        readline(10000, false);
        if (! parseReplyWithIntegerValue("+CNTP:", &status, ',', 0))
            return false;
    } else {
        if (! sendCheckReply("AT+CNTPCID=0", "OK", FONA_DEFAULT_TIMEOUT_MS))
            return false;
    }

    return true;
}

bool Adafruit_FONA_getTime(char* buff, uint16_t maxlen) {
	getReplyWithPrefixAndIntegerSuffix("AT+CCLK?", (uint16_t) 10000, FONA_DEFAULT_TIMEOUT_MS);
    if (strncmp(replybuffer, "+CCLK: ", 7) != 0)
        return false;

    char *p = replybuffer+7;
    uint16_t lentocopy = MIN((uint16_t)(maxlen-1), (uint16_t)strlen(p));
    strncpy(buff, p, lentocopy+1);
    buff[lentocopy] = 0;

    readline(FONA_DEFAULT_TIMEOUT_MS, false); // eat OK

    return true;
}

/********* GPS **********************************************************/


bool Adafruit_FONA_enableGPS(bool onoff) {
    uint16_t state;

    // first check if its already on or off
    if (! sendParseReply("AT+CGPSPWR?", "+CGPSPWR: ", &state, ',', 0) )
        return false;

    if (onoff && !state) {
        if (! sendCheckReply("AT+CGPSPWR=1", "OK", FONA_DEFAULT_TIMEOUT_MS))
            return false;
    } else if (!onoff && state) {
        if (! sendCheckReply("AT+CGPSPWR=0", "OK", FONA_DEFAULT_TIMEOUT_MS))
            return false;
    }
    return true;
}

int8_t Adafruit_FONA_GPSstatus(void) {
    getReply("AT+CGPSSTATUS?", FONA_DEFAULT_TIMEOUT_MS);

    char *p = strstr(replybuffer, "+CGPSSTATUS: Location ");
    if (p == 0) return -1;

    p+=22;

    readline(FONA_DEFAULT_TIMEOUT_MS, false); // eat 'OK'


    if (p[0] == 'U') return 0;
    if (p[0] == 'N') return 1;
    if (p[0] == '2') return 2;
    if (p[0] == '3') return 3;

    // else
    return 0;
}

uint8_t Adafruit_FONA_getGPS_raw(uint8_t arg, char *buffer, uint8_t maxbuff) {
    int32_t x = arg;

    getReplyWithPrefixAndIntegerSuffix("AT+CGPSINF=", x, FONA_DEFAULT_TIMEOUT_MS);

    char *p = strstr(replybuffer, "CGPSINF: ");
    if (p == 0){
        buffer[0] = 0;
        return 0;
    }
    p+=9;
    uint8_t len = MAX((uint8_t)(maxbuff-1), (uint8_t)strlen(p));
    strncpy(buffer, p, len);
    buffer[len] = 0;

    readline(FONA_DEFAULT_TIMEOUT_MS, false); // eat 'OK'
    return len;
}

bool Adafruit_FONA_getGPS(float *lat, float *lon, float *speed_kph, float *heading, float *altitude) {
    char gpsbuffer[120];

    // we need at least a 2D fix
    if (Adafruit_FONA_GPSstatus() < 2)
        return false;

    // grab the mode 2^5 gps csv from the sim808
    uint8_t res_len = Adafruit_FONA_getGPS_raw(32, gpsbuffer, 120);

    // make sure we have a response
    if (res_len == 0)
        return false;

    // skip mode
    char *tok = strtok(gpsbuffer, ",");
    if (! tok) return false;

    // skip date
    tok = strtok(NULL, ",");
    if (! tok) return false;

    // skip fix
    tok = strtok(NULL, ",");
    if (! tok) return false;

    // grab the latitude
    char *latp = strtok(NULL, ",");
    if (! latp) return false;

    // grab latitude direction
    char *latdir = strtok(NULL, ",");
    if (! latdir) return false;

    // grab longitude
    char *longp = strtok(NULL, ",");
    if (! longp) return false;

    // grab longitude direction
    char *longdir = strtok(NULL, ",");
    if (! longdir) return false;

    double latitude = atof(latp);
    double longitude = atof(longp);

    // convert latitude from minutes to decimal
    float degrees = (int)(latitude / 100);
    double minutes = latitude - (100 * degrees);
    minutes /= 60;
    degrees += minutes;

    // turn direction into + or -
    if (latdir[0] == 'S') degrees *= -1;

    *lat = degrees;

    // convert longitude from minutes to decimal
    degrees = (int)(longitude / 100);
    minutes = longitude - (100 * degrees);
    minutes /= 60;
    degrees += minutes;

    // turn direction into + or -
    if (longdir[0] == 'W') degrees *= -1;

    *lon = degrees;

    // only grab speed if needed
    if (speed_kph != NULL) {

        // grab the speed in knots
        char *speedp = strtok(NULL, ",");
        if (! speedp) return false;

        // convert to kph
        *speed_kph = atof(speedp) * 1.852;

    }

    // only grab heading if needed
    if (heading != NULL) {

        // grab the speed in knots
        char *coursep = strtok(NULL, ",");
        if (! coursep) return false;

        *heading = atof(coursep);

    }

    // no need to continue
    if (altitude == NULL)
        return true;

    // we need at least a 3D fix for altitude
    if (Adafruit_FONA_GPSstatus() < 3)
        return false;

    // grab the mode 0 gps csv from the sim808
    res_len = Adafruit_FONA_getGPS_raw(0, gpsbuffer, 120);

    // make sure we have a response
    if (res_len == 0)
        return false;

    // skip mode
    tok = strtok(gpsbuffer, ",");
    if (! tok) return false;

    // skip lat
    tok = strtok(NULL, ",");
    if (! tok) return false;

    // skip long
    tok = strtok(NULL, ",");
    if (! tok) return false;

    // grab altitude
    char *altp = strtok(NULL, ",");
    if (! altp) return false;

    *altitude = atof(altp);

    return true;
}

bool Adafruit_FONA_enableGPSNMEA(uint8_t i) {
    char sendbuff[15] = "AT+CGPSOUT=000";
    sendbuff[11] = (i / 100) + '0';
    i %= 100;
    sendbuff[12] = (i / 10) + '0';
    i %= 10;
    sendbuff[13] = i + '0';

    return sendCheckReply(sendbuff, "OK", 2000);
}


/********* GPRS **********************************************************/


bool Adafruit_FONA_enableGPRS(bool onoff) {
    if (onoff) {
        // disconnect all sockets
        sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000);

        if (! sendCheckReply("AT+CGATT=1", "OK", 10000))
            return false;

        // set bearer profile! connection type GPRS
        if (! sendCheckReply("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", "OK", 10000))
            return false;

        // set bearer profile access point name
        if (m_apn) {
            // Send command AT+SAPBR=3,1,"APN","<apn value>" where <apn value> is the configured APN value.
            if (! sendCheckReplyQuoted("AT+SAPBR=3,1,\"APN\",", m_apn, "OK", 10000))
                return false;

            // set username/password
            if (m_apnusername) {
                // Send command AT+SAPBR=3,1,"USER","<user>" where <user> is the configured APN username.
                if (! sendCheckReplyQuoted("AT+SAPBR=3,1,\"USER\",", m_apnusername, "OK", 10000))
                    return false;
            }
            if (m_apnpassword) {
                // Send command AT+SAPBR=3,1,"PWD","<password>" where <password> is the configured APN password.
                if (! sendCheckReplyQuoted("AT+SAPBR=3,1,\"PWD\",", m_apnpassword, "OK", 10000))
                    return false;
            }
        }

        // open GPRS context
        if (! sendCheckReply("AT+SAPBR=1,1", "OK", 10000))
            return false;
    } else {
        // disconnect all sockets
        if (! sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000))
            return false;

        // close GPRS context
        if (! sendCheckReply("AT+SAPBR=0,1", "OK", 10000))
            return false;

        if (! sendCheckReply("AT+CGATT=0", "OK", 10000))
            return false;
    }
    return true;
}

uint8_t Adafruit_FONA_GPRSstate(void) {
    uint16_t state;

    if (! sendParseReply("AT+CGATT?", "+CGATT: ", &state, ',', 0) )
        return -1;

    return state;
}

void Adafruit_FONA_setGPRSNetworkSettings(const char* apn, const char* ausername, const char* apassword) {
	m_apn = (char*) apn;
	m_apnusername = (char*) ausername;
	m_apnpassword = (char*) apassword;
}

bool Adafruit_FONA_getGSMLoc_raw(uint16_t *errorcode, char *buff, uint16_t maxlen) {
	getReply("AT+CIPGSMLOC=1,1", (uint16_t)10000);

    if (! parseReplyWithIntegerValue("+CIPGSMLOC: ", errorcode, ',', 0))
        return false;

    char *p = replybuffer+14;
    uint16_t lentocopy = MIN((uint16_t)(maxlen-1), (uint16_t)strlen(p));
    strncpy(buff, p, lentocopy+1);

    readline(FONA_DEFAULT_TIMEOUT_MS, false); // eat OK

    return true;
}

bool Adafruit_FONA_getGSMLoc(float *lat, float *lon) {
    uint16_t returncode;
    char gpsbuffer[120];

    // make sure we could get a response
    if (! Adafruit_FONA_getGSMLoc_raw(&returncode, gpsbuffer, 120))
        return false;

    // make sure we have a valid return code
    if (returncode != 0)
        return false;

    // tokenize the gps buffer to locate the lat & long
    char *latp = strtok(gpsbuffer, ",");
    if (! latp) return false;

    char *longp = strtok(NULL, ",");
    if (! longp) return false;

    *lat = atof(latp);
    *lon = atof(longp);

    return true;
}

/********* TCP FUNCTIONS  ************************************/


bool Adafruit_FONA_TCPconnect(char *server, uint16_t port) {
    flushInput();

    // close all old connections
    if (! sendCheckReply("AT+CIPSHUT", "SHUT OK", 5000) ) return false;

    // single connection at a time
    if (! sendCheckReply("AT+CIPMUX=0", "OK", FONA_DEFAULT_TIMEOUT_MS) ) return false;

    // manually read data
    if (! sendCheckReply("AT+CIPRXGET=1", "OK", FONA_DEFAULT_TIMEOUT_MS) ) return false;

    Adafruit_FONA_print("AT+CIPSTART=\"TCP\",\"");
    Adafruit_FONA_print(server);
    Adafruit_FONA_print("\",\"");
    Adafruit_FONA_putc16(port);
    Adafruit_FONA_print("\"\r\n");

    if (! Adafruit_FONA_expectReply("OK", FONA_DEFAULT_TIMEOUT_MS)) return false;
    if (! Adafruit_FONA_expectReply("CONNECT OK", FONA_DEFAULT_TIMEOUT_MS)) return false;
    return true;
}

bool Adafruit_FONA_TCPclose(void) {
    return sendCheckReply("AT+CIPCLOSE", "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_TCPconnected(void) {
    if (! sendCheckReply("AT+CIPSTATUS", "OK", 100) ) return false;
    readline(100, false);
    return (strcmp(replybuffer, "STATE: CONNECT OK") == 0);
}

bool Adafruit_FONA_TCPsend(char *packet, uint8_t len) {

	Adafruit_FONA_print("AT+CIPSEND=");
	Adafruit_FONA_putc(len);
	Adafruit_FONA_print("\r\n");
    readline(FONA_DEFAULT_TIMEOUT_MS, false);
    if (replybuffer[0] != '>') return false;

    for (uint16_t i=0; i<len; i++) {
    	Adafruit_FONA_putc((uint8_t) packet[i]);
    }
    readline(3000, false); // wait up to 3 seconds to send the data

    return (strcmp(replybuffer, "SEND OK") == 0);
}

uint16_t Adafruit_FONA_TCPavailable(void) {
    uint16_t avail;

    if (! sendParseReply("AT+CIPRXGET=4", "+CIPRXGET: 4,", &avail, ',', 0) ) return false;

    return avail;
}


uint16_t Adafruit_FONA_TCPread(uint8_t *buff, uint8_t len) {
    uint16_t avail;

    Adafruit_FONA_print("AT+CIPRXGET=2,");
    Adafruit_FONA_putc(len);
    Adafruit_FONA_print("\r\n");
    readline(FONA_DEFAULT_TIMEOUT_MS, false);
    if (! parseReplyWithIntegerValue("+CIPRXGET: 2,", &avail, ',', 0)) return false;

    readRaw(avail);

    memcpy(buff, replybuffer, avail);

    return avail;
}

/********* HTTP LOW LEVEL FUNCTIONS  ************************************/

bool Adafruit_FONA_HTTP_init() {
    return sendCheckReply("AT+HTTPINIT", "OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_HTTP_term() {
    return sendCheckReply("AT+HTTPTERM", "OK", FONA_DEFAULT_TIMEOUT_MS);
}

void Adafruit_FONA_HTTP_para_start(const char* parameter, bool quoted) {
    flushInput();

    Adafruit_FONA_print("AT+HTTPPARA=\"");
    Adafruit_FONA_print((char*) parameter);
    if (quoted)
    	Adafruit_FONA_print("\",\"");
    else
    	Adafruit_FONA_print("\",");
}

bool Adafruit_FONA_HTTP_para_end(bool quoted) {
    if (quoted)
    	Adafruit_FONA_print("\"\r\n");
    else
    	Adafruit_FONA_print("\r\n");

    return Adafruit_FONA_expectReply("OK", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_HTTP_para(const char* parameter, const char* value) {
	Adafruit_FONA_HTTP_para_start(parameter, true);
    Adafruit_FONA_print((char*) value);
    return Adafruit_FONA_HTTP_para_end(true);
}

bool Adafruit_FONA_HTTP_para_with_integer_value(const char* parameter, int32_t value) {
	Adafruit_FONA_HTTP_para_start(parameter, false);
    Adafruit_FONA_putc32(value);
    return Adafruit_FONA_HTTP_para_end(false);
}

bool Adafruit_FONA_HTTP_data(uint32_t size, uint32_t maxTime) {
    flushInput();

    Adafruit_FONA_print("AT+HTTPDATA=");
    Adafruit_FONA_putc32(size);
    Adafruit_FONA_print(",");
    Adafruit_FONA_putc32(maxTime);
    Adafruit_FONA_print("\r\n");

    return Adafruit_FONA_expectReply("DOWNLOAD", FONA_DEFAULT_TIMEOUT_MS);
}

bool Adafruit_FONA_HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout) {
    // Send request.
    if (! sendCheckReplyWithPrefixAndIntegerSuffix("AT+HTTPACTION=", method, "OK", FONA_DEFAULT_TIMEOUT_MS))
        return false;

    // Parse response status and size.
    readline(timeout, false);
    if (! parseReplyWithIntegerValue("+HTTPACTION:", status, ',', 1))
        return false;
    if (! parseReplyWithIntegerValue("+HTTPACTION:", datalen, ',', 2))
        return false;

    return true;
}

bool Adafruit_FONA_HTTP_readall(uint16_t *datalen) {
    getReply("AT+HTTPREAD", FONA_DEFAULT_TIMEOUT_MS);
    if (! parseReplyWithIntegerValue("+HTTPREAD:", datalen, ',', 0))
        return false;

    return true;
}

bool Adafruit_FONA_HTTP_ssl(bool onoff) {
    return sendCheckReplyWithPrefixAndIntegerSuffix("AT+HTTPSSL=", onoff ? 1 : 0, "OK", FONA_DEFAULT_TIMEOUT_MS);
}

/********* HTTP HIGH LEVEL FUNCTIONS ***************************/

bool Adafruit_FONA_HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen){
    if (! Adafruit_FONA_HTTP_setup(url))
        return false;

    // HTTP GET
    if (! Adafruit_FONA_HTTP_action(FONA_HTTP_GET, status, datalen, FONA_DEFAULT_TIMEOUT_MS))
        return false;

    // HTTP response data
    if (! Adafruit_FONA_HTTP_readall(datalen))
        return false;

    return true;
}

void Adafruit_FONA_HTTP_GET_end(void) {
	Adafruit_FONA_HTTP_term();
}

bool Adafruit_FONA_HTTP_POST_start(char *url, const char* contenttype, const uint8_t *postdata, uint16_t postdatalen, uint16_t *status, uint16_t *datalen) {
    if (! Adafruit_FONA_HTTP_setup(url))
        return false;

    if (! Adafruit_FONA_HTTP_para("CONTENT", contenttype)) {
        return false;
    }

    // HTTP POST data
    if (! Adafruit_FONA_HTTP_data(postdatalen, 10000))
        return false;
    for (uint16_t i = 0; i < postdatalen; i++) {
    	Adafruit_FONA_putc(postdata[i]);
    }
    if (! Adafruit_FONA_expectReply("OK", FONA_DEFAULT_TIMEOUT_MS))
        return false;

    // HTTP POST
    if (! Adafruit_FONA_HTTP_action(FONA_HTTP_POST, status, datalen, FONA_DEFAULT_TIMEOUT_MS))
        return false;

    // HTTP response data
    if (! Adafruit_FONA_HTTP_readall(datalen))
        return false;

    return true;
}

void Adafruit_FONA_HTTP_POST_end(void) {
	Adafruit_FONA_HTTP_term();
}

void Adafruit_FONA_setUserAgent(const char* useragent) {
	m_useragent = (char*) useragent;
}

void Adafruit_FONA_setHTTPSRedirect(bool onoff) {
    httpsredirect = onoff;
}

/********* HTTP HELPERS ****************************************/

static bool Adafruit_FONA_HTTP_setup(char *url) {
    // Handle any pending
	Adafruit_FONA_HTTP_term();

    // Initialize and set parameters
    if (! Adafruit_FONA_HTTP_init())
        return false;
    if (! Adafruit_FONA_HTTP_para_with_integer_value("CID", 1))
        return false;
    if (! Adafruit_FONA_HTTP_para("UA", m_useragent))
        return false;
    if (! Adafruit_FONA_HTTP_para("URL", url))
        return false;

    // HTTPS redirect
    if (httpsredirect) {
        if (! Adafruit_FONA_HTTP_para_with_integer_value("REDIR",1))
            return false;

        if (! Adafruit_FONA_HTTP_ssl(true))
            return false;
    }

    return true;
}


/********* HELPERS *********************************************/

bool Adafruit_FONA_expectReply(const char* reply, uint16_t timeout) {
    readline(timeout, false);
    return (strcmp(replybuffer, reply) == 0);
}

/********* LOW LEVEL *******************************************/

static void flushInput() {
    // Read all available serial input to flush pending data.
    uint16_t timeoutloop = 0;
    while (timeoutloop++ < 40) {
        while(Adafruit_FONA_readable()) {
        	Adafruit_FONA_getc();
            timeoutloop = 0;  // If char was received reset the timer
        }
        nrf_delay_ms(1);
    }
}

static uint16_t readRaw(uint16_t b) {
    uint16_t idx = 0;

    while (b && (idx < sizeof(replybuffer)-1)) {
        if (Adafruit_FONA_readable()) {
            replybuffer[idx] = Adafruit_FONA_getc();
            idx++;
            b--;
        }
    }
    replybuffer[idx] = 0;

    return idx;
}

static uint8_t readline(uint16_t timeout, bool multiline) {
    uint16_t replyidx = 0;

    while (timeout--) {
        if (replyidx >= 254) {
            break;
        }

        while(Adafruit_FONA_readable()) {
            char c =  Adafruit_FONA_getc();
            if (c == '\r') continue;
            if (c == 0xA) {
                if (replyidx == 0)   // the first 0x0A is ignored
                    continue;

                if (!multiline) {
                    timeout = 0;         // the second 0x0A is the end of the line
                    break;
                }
            }
            replybuffer[replyidx] = c;
            replyidx++;
        }

        if (timeout == 0) {
            break;
        }
        nrf_delay_ms(1);
    }
    replybuffer[replyidx] = 0;  // null term
    return replyidx;
}

static uint8_t getReply(const char* send, uint16_t timeout) {
    flushInput();

    Adafruit_FONA_print((char*) send);
    Adafruit_FONA_print("\r\n");

    uint8_t l = readline(timeout, false);
    return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
static uint8_t getReplyWithPrefixAndStringSuffix(const char* prefix, char* suffix, uint16_t timeout) {
    flushInput();

    Adafruit_FONA_print((char*) prefix);
    Adafruit_FONA_print(suffix);
    Adafruit_FONA_print("\r\n");

    uint8_t l = readline(timeout, false);
    return l;
}

// Send prefix, suffix, and newline. Return response (and also set replybuffer with response).
static uint8_t getReplyWithPrefixAndIntegerSuffix(const char* prefix, int32_t suffix, uint16_t timeout) {
    flushInput();

    Adafruit_FONA_print((char*) prefix);
    Adafruit_FONA_putc32(suffix);
    Adafruit_FONA_print("\r\n");

    uint8_t l = readline(timeout, false);
    return l;
}

// Send prefix, suffix, suffix2, and newline. Return response (and also set replybuffer with response).
static uint8_t getReplyWithPrefixAndTwoIntegerSuffixes(const char* prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout) {
    flushInput();

    Adafruit_FONA_print((char*) prefix);
    Adafruit_FONA_putc32(suffix1);
    Adafruit_FONA_print(",");
    Adafruit_FONA_putc32(suffix2);
    Adafruit_FONA_print("\r\n");

    uint8_t l = readline(timeout, false);
    return l;
}

// Send prefix, ", suffix, ", and newline. Return response (and also set replybuffer with response).
static uint8_t getReplyQuoted(const char* prefix, const char* suffix, uint16_t timeout) {
    flushInput();

    Adafruit_FONA_print((char*) prefix);
    Adafruit_FONA_print("\"");
    Adafruit_FONA_print((char*) suffix);
    Adafruit_FONA_print("\"\r\n");

    uint8_t l = readline(timeout, false);
    return l;
}


static bool sendCheckReply(const char *send, const char *reply, uint16_t timeout) {
    getReply(send, timeout);

    return (strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
static bool sendCheckReplyWithPrefixAndStringSuffix(const char* prefix, char *suffix, const char* reply, uint16_t timeout) {
	getReplyWithPrefixAndStringSuffix(prefix, suffix, timeout);
    return (strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, and newline.  Verify FONA response matches reply parameter.
static bool sendCheckReplyWithPrefixAndIntegerSuffix(const char* prefix, int32_t suffix, const char* reply, uint16_t timeout) {
	getReplyWithPrefixAndIntegerSuffix(prefix, suffix, timeout);
    return (strcmp(replybuffer, reply) == 0);
}

// Send prefix, suffix, suffix2, and newline.  Verify FONA response matches reply parameter.
static bool sendCheckReplyWithPrefixAndTwoIntegerSuffixes(const char* prefix, int32_t suffix1, int32_t suffix2, const char* reply, uint16_t timeout) {
	getReplyWithPrefixAndTwoIntegerSuffixes(prefix, suffix1, suffix2, timeout);
    return (strcmp(replybuffer, reply) == 0);
}

// Send prefix, ", suffix, ", and newline.  Verify FONA response matches reply parameter.
static bool sendCheckReplyQuoted(const char* prefix, const char* suffix, const char* reply, uint16_t timeout) {
  getReplyQuoted(prefix, suffix, timeout);
  return (strcmp(replybuffer, reply) == 0);
}

static bool parseReplyWithIntegerValue(const char* toreply, uint16_t *v, char divider, uint8_t index) {
    char *p = strstr(replybuffer, toreply);  // get the pointer to the voltage
    if (p == 0) return false;
    p += strlen(toreply);

    for (uint8_t i=0; i<index;i++) {
        // increment dividers
        p = strchr(p, divider);
        if (!p) return false;
        p++;
    }

    *v = atoi(p);

    return true;
}

static bool parseReply(const char* toreply, char *v, char divider, uint8_t index) {
    uint8_t i=0;
    char *p = strstr(replybuffer, toreply);
    if (p == 0) return false;
    p+=strlen(toreply);

    for (i=0; i<index;i++) {
        // increment dividers
        p = strchr(p, divider);
        if (!p) return false;
        p++;
    }

    for(i=0; i<strlen(p);i++) {
        if(p[i] == divider)
            break;
        v[i] = p[i];
    }

    v[i] = '\0';

    return true;
}

// Parse a quoted string in the response fields and copy its value (without quotes)
// to the specified character array (v).  Only up to maxlen characters are copied
// into the result buffer, so make sure to pass a large enough buffer to handle the
// response.
static bool parseReplyQuoted(const char* toreply, char* v, int maxlen, char divider, uint8_t index) {
    uint8_t i=0, j;
    // Verify response starts with toreply.
    char *p = strstr(replybuffer, toreply);
    if (p == 0) return false;
    p+=strlen(toreply);

    // Find location of desired response field.
    for (i=0; i<index;i++) {
        // increment dividers
        p = strchr(p, divider);
        if (!p) return false;
            p++;
    }

    // Copy characters from response field into result string.
    for(i=0, j=0; j<maxlen && i<strlen(p); ++i) {
        // Stop if a divier is found.
        if(p[i] == divider)
            break;
        // Skip any quotation marks.
        else if(p[i] == '"')
            continue;
        v[j++] = p[i];
    }

    // Add a null terminator if result string buffer was not filled.
    if (j < maxlen)
        v[j] = '\0';

    return true;
}

static bool sendParseReply(const char* tosend, const char* toreply, uint16_t *v, char divider, uint8_t index) {
    getReply(tosend, FONA_DEFAULT_TIMEOUT_MS);

    if (! parseReplyWithIntegerValue(toreply, v, divider, index)) return false;

    readline(FONA_DEFAULT_TIMEOUT_MS, false); // eat 'OK'

    return true;
}
