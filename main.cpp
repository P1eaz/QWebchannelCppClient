#include <QtCore/QCoreApplication>

#include <qwebsocket.h>

#include "WebChannelClient.h"



int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	QWebSocket socket;
	socket.open(QUrl("ws://localhost:12345"));

	CWebChannelClient client(&socket);

	if (!WaitForSignal(&socket, &QWebSocket::connected))
	{
		qDebug() << "can not connect.";
	}
	client.initialize();

	if (WaitForSignal(&client, &CWebChannelClient::initialized))
	{
		qDebug() << "Initialized";
	}

	//while (1)
	//{
	//	auto resopnse1 = client.invokeMethod("server", "setArguments", { 100, "Hello World." });
	//	qApp->processEvents();
	//	resopnse1->connect(resopnse1, &CWebChannelResponse::result, [](const QJsonValue& message)
	//		{
	//			static int count = 0;
	//			qDebug() << message << count++;
	//		});
	//};

	QTimer timer;
	timer.connect(&timer, &QTimer::timeout, [&client]() {
		const auto result1 = client.invokeMethodBlocking("server", "getQString");
		qDebug() << result1;

		auto resopnse1 = client.invokeMethod("server", "setArguments", { 100, "Hello World." });
		//qApp->processEvents();
		resopnse1->connect(resopnse1, &CWebChannelResponse::result, [](const QJsonValue& message)
			{
				static int count = 0;
				qDebug() << message << count++;
			});
		});

	timer.setInterval(1000);
	timer.start();


	auto signal1 = client.connect("server", "signals1");
	signal1->connect(signal1, &CWebChannelConnection::signal, [&client](const QJsonArray& message)
		{

			static int count = 0;
			qDebug() << message << count;
			count++;

			if (count == 10)
			{
				client.disconnect("server", "signals1");
			}
		});

	auto signal2 = client.connect("server", "signals2");
	signal2->connect(signal2, &CWebChannelConnection::signal, [&client](const QJsonArray& message)
		{

			static int count = 0;
			qDebug() << message << count;
			count++;
		});

	return a.exec();
}
