#!/bin/sh

if [ ! -e /etc/hildon-input-method.configured ]
then
	sudo /usr/bin/hildon-input-method-configurator
	sudo touch /etc/hildon-input-method.configured
fi

/usr/sbin/dsmetool -t "/usr/bin/hildon-input-method"
