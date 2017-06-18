package fr.marcworld.tourox.webserver.entity;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;
import java.io.Serializable;
import java.util.Date;

/**
 * Contain a device geolocation with its IMEI.
 *
 * @author Marc Plouhinec
 */
@Entity
public class DeviceLocation implements Serializable {

    @Id
    @GeneratedValue
    private Long id;

    @Column(nullable = false, unique = true, length = 15)
    private String imei;

    @Column(nullable = false)
    private double latitude;

    @Column(nullable = false)
    private double longitude;

    @Column(nullable = false)
    private Date lastUpdateDate;

    public DeviceLocation() {
    }

    public DeviceLocation(String imei, double latitude, double longitude, Date lastUpdateDate) {
        this.imei = imei;
        this.latitude = latitude;
        this.longitude = longitude;
        this.lastUpdateDate = lastUpdateDate;
    }

    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public String getImei() {
        return imei;
    }

    public void setImei(String imei) {
        this.imei = imei;
    }

    public double getLatitude() {
        return latitude;
    }

    public void setLatitude(double latitude) {
        this.latitude = latitude;
    }

    public double getLongitude() {
        return longitude;
    }

    public void setLongitude(double longitude) {
        this.longitude = longitude;
    }

    public Date getLastUpdateDate() {
        return lastUpdateDate;
    }

    public void setLastUpdateDate(Date lastUpdateDate) {
        this.lastUpdateDate = lastUpdateDate;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        DeviceLocation that = (DeviceLocation) o;

        return id.equals(that.id);
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    @Override
    public String toString() {
        return "DeviceLocation{" +
                "id=" + id +
                ", imei='" + imei + '\'' +
                ", latitude=" + latitude +
                ", longitude=" + longitude +
                ", lastUpdateDate=" + lastUpdateDate +
                '}';
    }
}
