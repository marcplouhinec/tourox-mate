/**
 * Contain a device geolocation with its IMEI.
 *
 * @constructor
 * @param {{id: Number, imei: String, latitude: Number, longitude: Number, lastUpdateDate: Number}} properties
 * @author Marc Plouhinec
 */
function DeviceLocation(properties) {

    /**
     * @type {Number}
     */
    this.id = properties.id;

    /**
     * @type {String}
     */
    this.imei = properties.imei;

    /**
     * @type {Number}
     */
    this.latitude = properties.latitude;

    /**
     * @type {Number}
     */
    this.longitude = properties.longitude;

    /**
     * @type {Number}
     */
    this.lastUpdateDate = properties.lastUpdateDate;
}