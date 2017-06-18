# Tourox Webserver

## Introduction
The goal of the Tourox webserver is to collect tourist device geolocations, and allow the tour guide to view
them on a map.

Warning: the current version of this server is a prototype so all information is public (no access enforcement).

## Technologies
* [Java 8](http://www.oracle.com/technetwork/java/javase/overview/java8-2100321.html)
* [Spring Boot](https://projects.spring.io/spring-boot/)
* [Spring Data JPA](http://projects.spring.io/spring-data-jpa/)
* [HSQLDB](http://hsqldb.org/)
* [Apache HTTP Server](https://httpd.apache.org/) as a reverse proxy

## Build
The scripts in this section target the [Debian](https://www.debian.org/) operating system.

After cloning the repository from GitHub, type the following commands:

    cd webserver
    mvn clean install
    chmod +x build-deb-package.sh
    ./build-deb-package.sh $(git rev-parse --short HEAD)

The package is named: "./target/touroxwebserver_1.0.0.GIT_HASH_all.deb"

## Installation
Copy the .deb package on your server and stop the existing version if applicable:

    sudo service touroxwebserver stop
    
Install the package:

    sudo dpkg -i ./touroxwebserver_1.0.0.GIT_HASH_all.deb
    sudo apt-get -f install

Adapt the Apache configuration file:

    cd /etc/touroxwebserver
    vi logback.xml
    cd /etc/apache2/sites-available
    vi touroxwebserver
    ln -s /etc/apache2/sites-available/touroxwebserver /etc/apache2/sites-enabled/touroxwebserver

> Warning: the server IP must be accessible from internet, if not the device will not be able to connect to the server
> via its GPRS connection.

Start the service:

    sudo service touroxwebserver start
    sudo service apache2 restart

The logs are located in the folder /var/log/touroxwebserver.

## Usage
The webserver is not meant to be directly used by a human, but testing can be done with [curl](https://curl.haxx.se/).

To add some device location into the database, type the following command in a terminal:

    curl http://<hostname>:<port>/sl/<device IMEI>/<longitude>/<latitude>
    
Example:

    curl http://localhost:8080/sl/012345678901234/6.101516/49.607767
    curl http://localhost:8080/sl/012345678955555/6.112162/49.606249
    
To read the location of all devices, type the following command:

    curl http://<hostname>:<port>/device/locations
    
Example:

    curl http://localhost:8080/device/locations

Example result:
```json
[
  {
    "id": 1,
    "imei": "012345678901234",
    "latitude": 49.607767,
    "longitude": 6.101516,
    "lastUpdateDate": 1497810099518
  },
  {
    "id": 2,
    "imei": "012345678955555",
    "latitude": 49.606249,
    "longitude": 6.112162,
    "lastUpdateDate": 1497810141561
  }
]
```

You can also browse the URL `http://<hostname>:<port>` with a web browser like [Mozilla Firefox](https://www.mozilla.org/)
to see a map with device locations.
