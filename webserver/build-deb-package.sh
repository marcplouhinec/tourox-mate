#!/usr/bin/env bash

#
# Build a Debian package.
# Usage: ./build-deb-package.sh build_number
# Note: this script must be executed after the "mvn clean install" command.
#

if [ -z "$1" ]
  then
    echo "Usage: ./build-deb-package.sh build_number"
fi
BUILD_NUMBER=$1

cp -r ./debian ./target/debian
cd ./target

# Set the Debian package version
cp ./debian/DEBIAN/control.tpl ./debian/DEBIAN/control
sed -i "s/\${version}/1.0.0.${BUILD_NUMBER}/g" ./debian/DEBIAN/control

# Copy and rename the fat JAR
rm -rf ./debian/usr/local/lib/touroxwebserver/*
cp ./touroxwebserver-1.0.0-SNAPSHOT.jar ./debian/usr/local/lib/touroxwebserver/touroxwebserver.jar

# Adjust ownerships
fakeroot chown -R root:root debian
fakeroot chmod +x debian/etc/init.d/touroxwebserver
fakeroot chmod +x debian/DEBIAN/postinst

# build the package
fakeroot dpkg-deb --build debian .
