#ifndef GLOBALACTIONCOLLECTION_H
#define GLOBALACTIONCOLLECTION_H

#include <QAction>
#include <QQuickItem>

class GlobalAction : public QAction
{
    Q_OBJECT
public:
    GlobalAction(QObject *parent = nullptr);
};

class GlobalActionCollection : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
public:
    GlobalActionCollection();

    QString name() const;
    void setName(const QString &name);

    QString displayName() const;
    void setDisplayName(const QString &displayName);

signals:
    void nameChanged();
    void displayNameChanged();

protected:
    virtual void componentComplete() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    QString m_name;
    QString m_displayName;
};

#endif // GLOBALACTIONCOLLECTION_H
