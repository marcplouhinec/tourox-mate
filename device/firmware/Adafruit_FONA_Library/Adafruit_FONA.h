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

#ifndef ADAFRUIT_FONA_H
#define ADAFRUIT_FONA_H

#include <stdbool.h>
#include <stdint.h>

#define FONA_HEADSETAUDIO 0
#define FONA_EXTAUDIO 1

#define FONA_STTONE_DIALTONE 1
#define FONA_STTONE_BUSY 2
#define FONA_STTONE_CONGESTION 3
#define FONA_STTONE_PATHACK 4
#define FONA_STTONE_DROPPED 5
#define FONA_STTONE_ERROR 6
#define FONA_STTONE_CALLWAIT 7
#define FONA_STTONE_RINGING 8
#define FONA_STTONE_BEEP 16
#define FONA_STTONE_POSTONE 17
#define FONA_STTONE_ERRTONE 18
#define FONA_STTONE_INDIANDIALTONE 19
#define FONA_STTONE_USADIALTONE 20

#define FONA_DEFAULT_TIMEOUT_MS 500

#define FONA_HTTP_GET   0
#define FONA_HTTP_POST  1
#define FONA_HTTP_HEAD  2

#define RX_BUFFER_SIZE  255

/**
 * @brief Type of event the FONA can send.
 */
typedef enum {
	RING = 0,       // Somebody call the FONA
	NO_CARRIER = 1, // The calling person stop his call
	BUSY = 2        // The calling person doesn't want to answer
} Adafruit_FONA_event_type;

/**
 * @brief Function called when a FONA event occurs.
 */
typedef void (*Adafruit_FONA_event_handler_t) (Adafruit_FONA_event_type event_type);

/**
 * @brief Initialize the Adafruit_FONA library.
 *
 * Note that this function was originally a class constructor.
 */
void Adafruit_FONA_init(uint8_t tx_pin_number, uint8_t rx_pin_number, uint8_t rst_pin_number);

bool Adafruit_FONA_begin(uint32_t baudrate);

/**
 * @brief register a function that will be called when an event occurs.
 */
void Adafruit_FONA_register_event_handler(Adafruit_FONA_event_handler_t event_handler);

// Stream
bool Adafruit_FONA_putc(uint8_t value);
bool Adafruit_FONA_putc16(uint16_t value);
bool Adafruit_FONA_putc32(uint32_t value);
uint8_t Adafruit_FONA_getc();
bool Adafruit_FONA_readable();
bool Adafruit_FONA_print(char* value);

// RTC
bool Adafruit_FONA_enableRTC(uint8_t i); // i = 0 <=> disable, i = 1 <=> enable

// Battery and ADC
bool Adafruit_FONA_getADCVoltage(uint16_t *v);
bool Adafruit_FONA_getBattPercent(uint16_t *p);
bool Adafruit_FONA_getBattVoltage(uint16_t *v);

// SIM query
uint8_t Adafruit_FONA_unlockSIM(char *pin);
uint8_t Adafruit_FONA_getSIMCCID(char *ccid);
uint8_t Adafruit_FONA_getNetworkStatus(void);
uint8_t Adafruit_FONA_getRSSI(void);

// IMEI
uint8_t Adafruit_FONA_getIMEI(char *imei);

// set Audio output
bool Adafruit_FONA_setAudio(uint8_t a);
bool Adafruit_FONA_setVolume(uint8_t i);
uint8_t Adafruit_FONA_getVolume(void);
bool Adafruit_FONA_playToolkitTone(uint8_t t, uint16_t len);
bool Adafruit_FONA_setMicVolume(uint8_t a, uint8_t level);
bool Adafruit_FONA_playDTMF(char tone);

// FM radio functions
bool Adafruit_FONA_tuneFMradio(uint16_t station);
bool Adafruit_FONA_FMradio(bool onoff, uint8_t a); // default a = FONA_HEADSETAUDIO
bool Adafruit_FONA_setFMVolume(uint8_t i);
int8_t Adafruit_FONA_getFMVolume();
int8_t Adafruit_FONA_getFMSignalLevel(uint16_t station);

// SMS handling
bool Adafruit_FONA_setSMSInterrupt(uint8_t i);
uint8_t Adafruit_FONA_getSMSInterrupt(void);
int8_t Adafruit_FONA_getNumSMS(void);
bool Adafruit_FONA_readSMS(uint8_t i, char *smsbuff, uint16_t max, uint16_t *readsize);
bool Adafruit_FONA_sendSMS(char *smsaddr, char *smsmsg);
bool Adafruit_FONA_deleteSMS(uint8_t i);
bool Adafruit_FONA_getSMSSender(uint8_t i, char *sender, int senderlen);

// Time
bool Adafruit_FONA_enableNetworkTimeSync(bool onoff);
bool Adafruit_FONA_enableNTPTimeSync(bool onoff, const char* ntpserver); // default ntpserver = 0
bool Adafruit_FONA_getTime(char* buff, uint16_t maxlen);

// GPRS handling
bool Adafruit_FONA_enableGPRS(bool onoff);
uint8_t Adafruit_FONA_GPRSstate(void);
bool Adafruit_FONA_getGSMLoc_raw(uint16_t *replycode, char *buff, uint16_t maxlen);
bool Adafruit_FONA_getGSMLoc(float *lat, float *lon);
void Adafruit_FONA_setGPRSNetworkSettings(const char* apn, const char* username, const char* password); // default username = 0, password = 0

// GPS handling
bool Adafruit_FONA_enableGPS(bool onoff);
int8_t Adafruit_FONA_GPSstatus(void);
uint8_t Adafruit_FONA_getGPS_raw(uint8_t arg, char *buffer, uint8_t maxbuff);
bool Adafruit_FONA_getGPS(float *lat, float *lon, float *speed_kph, float *heading, float *altitude); // default speed_kph = 0, heading = 0, altitude = 0
bool Adafruit_FONA_enableGPSNMEA(uint8_t nmea);

// TCP raw connections
bool Adafruit_FONA_TCPconnect(char *server, uint16_t port);
bool Adafruit_FONA_TCPclose(void);
bool Adafruit_FONA_TCPconnected(void);
bool Adafruit_FONA_TCPsend(char *packet, uint8_t len);
uint16_t Adafruit_FONA_TCPavailable(void);
uint16_t Adafruit_FONA_TCPread(uint8_t *buff, uint8_t len);

// HTTP low level interface (maps directly to SIM800 commands).
bool Adafruit_FONA_HTTP_init();
bool Adafruit_FONA_HTTP_term();
void Adafruit_FONA_HTTP_para_start(const char* parameter, bool quoted); // default quoted = true
bool Adafruit_FONA_HTTP_para_end(bool quoted); // default quoted = true
bool Adafruit_FONA_HTTP_para(const char* parameter, const char *value);
bool Adafruit_FONA_HTTP_para_with_integer_value(const char* parameter, int32_t value);
bool Adafruit_FONA_HTTP_data(uint32_t size, uint32_t maxTime); // default maxTime = 10000
bool Adafruit_FONA_HTTP_action(uint8_t method, uint16_t *status, uint16_t *datalen, int32_t timeout); // default timeout = 10000
bool Adafruit_FONA_HTTP_readall(uint16_t *datalen);
bool Adafruit_FONA_HTTP_ssl(bool onoff);

// HTTP high level interface (easier to use, less flexible).
bool Adafruit_FONA_HTTP_GET_start(char *url, uint16_t *status, uint16_t *datalen);
void Adafruit_FONA_HTTP_GET_end(void);
bool Adafruit_FONA_HTTP_POST_start(char *url, const char* contenttype, const uint8_t *postdata, uint16_t postdatalen, uint16_t *status, uint16_t *datalen);
void Adafruit_FONA_HTTP_POST_end(void);
void Adafruit_FONA_setUserAgent(const char* useragent);

// HTTPS
void Adafruit_FONA_setHTTPSRedirect(bool onoff);

// PWM (buzzer)
bool Adafruit_FONA_setPWM(uint16_t period, uint8_t duty); // default duty = 50

// Phone calls
bool Adafruit_FONA_callPhone(char *phonenum);
bool Adafruit_FONA_hangUp(void);
bool Adafruit_FONA_pickUp(void);
bool Adafruit_FONA_callerIdNotification(bool enable);
bool Adafruit_FONA_incomingCallNumber(char* phonenum);

// Helper functions to verify responses.
bool Adafruit_FONA_expectReply(const char* reply, uint16_t timeout); // default timeout = 10000

#endif
