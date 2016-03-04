#include "filebrowserinterface.h"

/*
 * Implementation of interface class FileBrowserInterface
 */

FileBrowserInterface::FileBrowserInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
	: QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

FileBrowserInterface::~FileBrowserInterface()
{
}
