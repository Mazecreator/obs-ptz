/* UART Interface
 *
 * Copyright 2021 Luuk Verhagen <developer@verhagenluuk.nl>
 *
 * SPDX-License-Identifier: GPLv2
 */

#include "UART.hpp"

std::map<QString, UART*> UART::interfaces;

UART::UART(QString& port_name, qint32 baudrate)
	:port_name(port_name), baud_rate(baudrate)
{
	connect(&uart, &QSerialPort::readyRead, this, &UART::poll);
	open();
}

void UART::open()
{
	uart.setPortName(port_name);
	uart.setBaudRate(baud_rate);

	if (!uart.open(QIODevice::ReadWrite))
	{
		blog(LOG_INFO, "Unable to open UART %s", qPrintable(port_name));
		return;
	}
	blog(LOG_INFO, "Serial connection with %s is opened", qPrintable(port_name));
}

void UART::close()
{
	if (uart.isOpen())
	{
		uart.close();
		blog(LOG_INFO, "Serial connection %s is closed", qPrintable(port_name));
	}
}

void UART::send(const QByteArray& packet)
{
	if (!uart.isOpen())
		return;
	blog(LOG_DEBUG, "%s --> %s", packet.toHex(':').data(), qPrintable(port_name));
	uart.write(packet);
}

void UART::receive_datagram(const QByteArray& packet)
{
	blog(LOG_DEBUG, "%s <-- %s", qPrintable(port_name), packet.toHex(':').data());

	emit receive(packet);
}

void UART::poll()
{
	const QByteArray data = uart.readAll();
	for (auto b : data) {
		rxbuffer += b;
		if ( (closeByte == NULL || closeByte =='\0') || ((b & closeByte) == closeByte)) {
			if (rxbuffer.size() >= messageLength) {
				receive_datagram(rxbuffer);
				rxbuffer.clear();
			}
		}
	}
}

UART* UART::get_interface(QString port_name)
{
	UART* iface;
	qDebug() << "Looking for UART object" << port_name;
	iface = interfaces[port_name];
	return iface;
}

UART* UART::add_interface(QString port_name, qint32 baudrate)
{
	UART* iface = get_interface(port_name);
	if (!iface) {
		qDebug() << "Creating new UART object" << port_name;
		iface = new UART(port_name, baudrate);
		interfaces[port_name] = iface;
		return iface;
	}
	return iface;
}
