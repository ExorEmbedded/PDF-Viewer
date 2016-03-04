#ifndef EPADINTERFACE_H
#define EPADINTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface com.exor.FileBrowser
 */
class FileBrowserInterface: public QDBusAbstractInterface
{
	Q_OBJECT
public:
	static inline const char *staticInterfaceName()
	{ return "com.exor.FileBrowser"; }

public:
	FileBrowserInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

	~FileBrowserInterface();

public Q_SLOTS: // METHODS
	inline QDBusPendingReply<int> getOpenFileName(const QString& dir, const QStringList& filters, const QStringList& nameFilters,
											   const QString& topText, const QStringList& buttonLabels, const QString& stylesheet)
	{
		QList<QVariant> argumentList;
		argumentList << QVariant::fromValue(dir) << QVariant::fromValue(filters) << QVariant::fromValue(nameFilters)
					 << QVariant::fromValue(topText) << QVariant::fromValue(buttonLabels) << QVariant::fromValue(stylesheet);
		return asyncCallWithArgumentList(QLatin1String("getOpenFileName"), argumentList);
	}

	inline QDBusPendingReply<int> getSaveFileName(const QString& dir, const QString& topText, const QStringList& buttonLabels, const QString& stylesheet)
	{
		QList<QVariant> argumentList;
		argumentList << QVariant::fromValue(dir) << QVariant::fromValue(topText) << QVariant::fromValue(buttonLabels) << QVariant::fromValue(stylesheet);
		return asyncCallWithArgumentList(QLatin1String("getSaveFileName"), argumentList);
	}

Q_SIGNALS: // SIGNALS
	void done(const QString& path);
};

namespace com {
  namespace exor {
	typedef ::FileBrowserInterface FileBrowser;
  }
}

#endif // EPADINTERFACE_H
