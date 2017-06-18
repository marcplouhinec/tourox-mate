package fr.marcworld.tourox.webserver.controller;

import fr.marcworld.tourox.webserver.entity.DeviceLocation;
import fr.marcworld.tourox.webserver.repository.DeviceLocationRepository;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RestController;

import javax.transaction.Transactional;
import java.util.Date;
import java.util.Optional;

/**
 * REST controller that provides CRU operations on {@link DeviceLocation}s.
 *
 * @author Marc Plouhinec
 */
@RestController
@Transactional
public class LocationController {

    private static final Logger LOG = LoggerFactory.getLogger(LocationController.class);

    private final DeviceLocationRepository deviceLocationRepository;

    public LocationController(DeviceLocationRepository deviceLocationRepository) {
        this.deviceLocationRepository = deviceLocationRepository;
    }

    @RequestMapping(value = "/sl/{imei}/{longitude}/{latitude:.+}", method = RequestMethod.GET)
    public ResponseEntity<Void> setLocation(@PathVariable("imei") String imei, @PathVariable("latitude") double latitude, @PathVariable("longitude") double longitude) {
        LOG.info("Set the location of the device {}: latitude = {}, longitude = {}.", imei, latitude, longitude);
        if (imei == null || imei.length() != 15) {
            return new ResponseEntity<>(HttpStatus.BAD_REQUEST);
        }
        Optional<DeviceLocation> deviceLocationOptional = deviceLocationRepository.findByImei(imei);
        DeviceLocation deviceLocation = deviceLocationOptional.orElseGet(() -> new DeviceLocation(imei, latitude, longitude, new Date()));
        if (deviceLocationOptional.isPresent()) {
            deviceLocation.setLatitude(latitude);
            deviceLocation.setLongitude(longitude);
        }
        deviceLocationRepository.save(deviceLocation);
        return new ResponseEntity<>(HttpStatus.OK);
    }

    @RequestMapping(value = "/device/{imei}/location", method = RequestMethod.GET)
    public ResponseEntity<DeviceLocation> getLocation(@PathVariable("imei") String imei) {
        if (imei == null || imei.length() != 15) {
            return new ResponseEntity<>(HttpStatus.BAD_REQUEST);
        }

        Optional<DeviceLocation> deviceLocationOptional = deviceLocationRepository.findByImei(imei);
        return deviceLocationOptional
                .map(deviceLocation -> new ResponseEntity<>(deviceLocation, HttpStatus.OK))
                .orElseGet(() -> new ResponseEntity<>(HttpStatus.NOT_FOUND));
    }

    @RequestMapping(value = "/device/locations", method = RequestMethod.GET)
    public ResponseEntity<Iterable<DeviceLocation>> getAllLocations() {
        Iterable<DeviceLocation> deviceLocations = deviceLocationRepository.findAll();
        return new ResponseEntity<Iterable<DeviceLocation>>(deviceLocations, HttpStatus.OK);
    }
}
