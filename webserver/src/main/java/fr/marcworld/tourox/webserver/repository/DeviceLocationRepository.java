package fr.marcworld.tourox.webserver.repository;

import fr.marcworld.tourox.webserver.entity.DeviceLocation;
import org.springframework.data.repository.CrudRepository;

import java.util.Optional;

/**
 * Repository for {@link DeviceLocation}.
 *
 * @author Marc Plouhinec
 */
public interface DeviceLocationRepository extends CrudRepository<DeviceLocation, Long> {

    /**
     * Find a device location by the device IMEI.
     *
     * @param imei Device identifier.
     * @return Found {@link DeviceLocation}.
     */
    Optional<DeviceLocation> findByImei(String imei);
}
