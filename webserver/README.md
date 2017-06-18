# Tourox Webserver

## Introduction
The goal of the Tourox webserver is to collect the tourist device geolocations, and allow the tour guide to view
them on a map.

Warning: the current version of this server is a prototype so all information is public (no access check).

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
Coming soon...