/**
 * Service responsible for retrieving device location information.
 *
 * @author Marc Plouhinec
 */
var locationService = {

    /**
     * Find all the {@link DeviceLocation}s.
     *
     * @param {function(deviceLocations: Array.<DeviceLocation>, errorMessage: String?)} callback
     */
    findAllDeviceLocations: function (callback) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState === 4) {
                if (this.status !== 200) {
                    callback([], 'Unable to find the device locations. Status: ' + 200);
                    return;
                }

                var deviceLocations = [];
                var deviceLocationDatas = JSON.parse(this.responseText);
                for (var i = 0; i < deviceLocationDatas.length; i++) {
                    deviceLocations.push(new DeviceLocation(deviceLocationDatas[i]));
                }
                callback(deviceLocations);
            }
        };
        xhttp.open('GET', 'device/locations', true);
        xhttp.send();
    }

};