/**
 * Controller for the index page.
 *
 * @author Marc Plouhinec
 */
var indexController = {

    /**
     * Controller entry-point.
     */
    main: function () {
        var self = this;

        // Get the device locations and use Google Maps to display them
        locationService.findAllDeviceLocations(function (deviceLocations, error) {
            if (error) {
                alert(error);
                return;
            }

            document.getElementById('map').src = self.buildGoogleMapsUrl(deviceLocations);
        });
    },

    /**
     * Build the URL to the static Google Maps API with the device locations.
     *
     * @private
     * @param {Array.<DeviceLocation>} deviceLocations
     * @return {String} URL
     */
    buildGoogleMapsUrl: function (deviceLocations) {
        var sortedDeviceLocations = deviceLocations.sort(function (dl1, dl2) {
            return dl1.id - dl2.id;
        });

        var width = Math.max(document.documentElement.clientWidth, window.innerWidth || 0);
        var height = Math.max(document.documentElement.clientHeight, window.innerHeight || 0);
        var labels = '0123456789ABCDEFGHIJKLMNOPGRSTUVWXYZ';
        var colors = ['brown', 'green', 'purple', 'yellow', 'blue', 'gray', 'orange', 'red', 'white', 'black'];

        var url = 'https://maps.googleapis.com/maps/api/staticmap?size=' + width + 'x' + height + '&maptype=roadmap';
        if (deviceLocations.length === 0) {
            url += '&center=Luxembourg&zoom=13';
        } else {
            for (var i = 0; i < sortedDeviceLocations.length; i++) {
                var deviceLocation = sortedDeviceLocations[i];
                var label = labels.charAt(i % labels.length);
                var color = colors[i % colors.length];
                url += '&markers=color:' + color + '%7Clabel:' + label + '%7C' + deviceLocation.latitude + ',' + deviceLocation.longitude;
            }
        }
        return url;
    }
};

window.onload = function () {
    indexController.main();
};