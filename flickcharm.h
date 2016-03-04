#ifndef FLICKCHARM_H
#define FLICKCHARM_H

#include <QObject>
#include <QTreeWidgetItem>

class FlickCharmPrivate;
class QWidget;

class FlickCharm: public QObject
{
	Q_OBJECT
public:
	FlickCharm(QObject *parent = 0);
	~FlickCharm();
	void activateOn(QWidget *widget);
	void activateSubWidgetOn(QWidget* subWidget, QWidget *widget);
    //void activateSubWidgetOn(QTreeWidgetItem *subWidget, QWidget *widget);
	bool eventFilter(QObject *object, QEvent *event);

protected:
	void timerEvent(QTimerEvent *event);

private:
	FlickCharmPrivate *d;
};

#endif // FLICKCHARM_H
