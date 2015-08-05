#ifndef QCANBERRAPRIVATE_CONTEXT_H
#define QCANBERRAPRIVATE_CONTEXT_H

#include <QObject>

#include <canberra.h>

namespace QCanberraPrivate {

class Q_DECL_EXPORT PendingEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint32 id READ id CONSTANT)
    Q_PROPERTY(bool finished READ finished NOTIFY finishedChanged)
//    Q_PROPERTY(ErrorType error READ error NOTIFY errorChanged)
public:
    enum ErrorType {
        NoError,
        NotSupportedError,
        InvalidError,
        StateError,
        OOMError,
        NoDriverError,
// FIXME... rests
    };

    explicit PendingEvent(QObject *parent = nullptr);
    ~PendingEvent();

    quint32 id() const;
    bool finished() const;
    void hadError() const;
//    ErrorType error() const;

public slots:
    void onFinished(int errorCode);

signals:
    void finishedChanged();
    void errorChanged();

private:
    quint32 m_id = 0;
    bool m_finished = false;
    ErrorType m_error = NoError;
};

class Q_DECL_EXPORT Context : public QObject
{
    Q_OBJECT
public:
    explicit Context(QObject *parent = nullptr);
    ~Context();

    bool isValid() const;

    void play(quint32 index);

private:
    ca_context *m_context = nullptr;
};

} // namespace QCanberraPrivate

#endif // QCANBERRAPRIVATE_CONTEXT_H
