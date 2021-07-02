/* UART Interface
 *
 * Copyright 2021 Luuk Verhagen <developer@verhagenluuk.nl>
 *
 * SPDX-License-Identifier: GPLv2
 */

#pragma once
#include <QObject>
/*#include <QTimer>*/
#include <QSerialPort>
#include <QDebug>
#include <QStringListModel>
#include <QtGlobal>
#include <util/base.h>

/*
* General Serial UART class
*/
class PelcoPUART : public QObject {
	Q_OBJECT

private:
	static std::map<QString, PelcoPUART*> interfaces;

	QString port_name;
	qint32 baud_rate;
	QSerialPort uart;
	QByteArray rxbuffer;

signals:
	void receive(const QByteArray& packet); 
	void reset();

public:
	char closeByte = '\0'; //Some protcols have a close byte to indicate the end of the message
	int messageLength = 0; //Some protocols have a fixed number of bytes and no closing byte

	PelcoPUART(QString& port_name, qint32 baudrate = 9600);
	void open();
	void close();
	void send(const QByteArray& packet);
	void receive_datagram(const QByteArray& packet);
	QString portName() { return port_name; }


	static PelcoPUART* get_interface(QString port_name);
	static PelcoPUART* add_interface(QString port_name, qint32 baudrate = 9600);

public slots:
	void poll();
};