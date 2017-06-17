/**
 * @brief BLE Facade definitions.
 *
 * This application layer is responsible for the communication with external devices via BLE.
 * It interacts with the rest of the application via the service layer.
 *
 * @author Marc Plouhinec
 */

#ifndef BLE_FACADE_H__
#define BLE_FACADE_H__

/**
 * @brief Initialize the BLE Facade.
 *
 * Initialize the SOFT DEVICE, set the GAP and GATT configuration and start advertising.
 */
void ble_facade_init(char* imei);

/**
 * @brief Put the microcontroller in sleep mode until an event occurs.
 */
void ble_facade_wait_for_event();


#endif /* BLE_FACADE_H__ */
