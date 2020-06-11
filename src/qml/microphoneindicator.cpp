/*
    Copyright 2019 Kai Uwe Broulik <kde@privat.broulik.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "microphoneindicator.h"

#include <QAction>
#include <QGuiApplication>
#include <QIcon>
#include <QMenu>
#include <QTimer>
#include <QStyleHints>

#include <KGlobalAccel>
#include <KLocalizedString>
#include <KNotification>
#include <KStatusNotifierItem>

#include "client.h"
#include "context.h"
#include "pulseaudio.h"
#include "source.h"

#include "volumeosd.h"

using namespace QPulseAudio;

MicrophoneIndicator::MicrophoneIndicator(QObject *parent)
    : QObject(parent)
    , m_sourceModel(new SourceModel(this))
    , m_sourceOutputModel(new SourceOutputModel(this))
    , m_updateTimer(new QTimer(this))
{

    connect(m_sourceModel, &QAbstractItemModel::rowsInserted, this, &MicrophoneIndicator::scheduleUpdate);
    connect(m_sourceModel, &QAbstractItemModel::rowsRemoved, this, &MicrophoneIndicator::scheduleUpdate);
    connect(m_sourceModel, &QAbstractItemModel::dataChanged, this, &MicrophoneIndicator::scheduleUpdate);

    connect(m_sourceOutputModel, &QAbstractItemModel::rowsInserted, this, &MicrophoneIndicator::scheduleUpdate);
    connect(m_sourceOutputModel, &QAbstractItemModel::rowsRemoved, this, &MicrophoneIndicator::scheduleUpdate);

    m_updateTimer->setInterval(0);
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, &QTimer::timeout, this, &MicrophoneIndicator::update);

    scheduleUpdate();
}

MicrophoneIndicator::~MicrophoneIndicator() = default;

void MicrophoneIndicator::init()
{
    // does nothing, just prompts QML engine to create an instance of the singleton
}

void MicrophoneIndicator::scheduleUpdate()
{
    if (!m_updateTimer->isActive()) {
        m_updateTimer->start();
    }
}

void MicrophoneIndicator::update()
{
    const QStringList names = appNames();
    if (names.isEmpty()) {
        m_showOsdOnUpdate = false;
        delete m_sni;
        m_sni = nullptr;
        return;
    }

    if (!m_sni) {
        m_sni = new KStatusNotifierItem(QStringLiteral("microphone"));
        m_sni->setTitle(i18n("Microphone"));
        m_sni->setCategory(KStatusNotifierItem::Hardware);
        // always Active since it is completely removed when microphone isn't in use
        m_sni->setStatus(KStatusNotifierItem::Active);

        connect(m_sni, &KStatusNotifierItem::activateRequested, this, [this] {
            setPushToTalkMode(!m_pushToTalkMode);
        });
        // but also middle click to be consistent with volume icon
        connect(m_sni, &KStatusNotifierItem::secondaryActivateRequested, this, &MicrophoneIndicator::toggleMuted);

        connect(m_sni, &KStatusNotifierItem::scrollRequested, this, [this](int delta, Qt::Orientation orientation) {
            if (orientation != Qt::Vertical) {
                return;
            }

            m_wheelDelta += delta;

            while (m_wheelDelta >= 120) {
                m_wheelDelta -= 120;
                adjustVolume(+1);
            }
            while (m_wheelDelta <= -120) {
                m_wheelDelta += 120;
                adjustVolume(-1);
            }
        });

        QMenu *menu = m_sni->contextMenu();

        m_muteAction = menu->addAction(QIcon::fromTheme(QStringLiteral("microphone-sensitivity-muted")), i18n("Mute"));
        m_muteAction->setCheckable(true);
        connect(m_muteAction.data(), &QAction::triggered, this, &MicrophoneIndicator::setMuted);

        m_pushToTalkModeAction = menu->addAction(QIcon::fromTheme(QStringLiteral("irc-voice")), i18n("Push to Talk"));
        m_pushToTalkModeAction->setCheckable(true);
        connect(m_pushToTalkModeAction, &QAction::triggered, this, &MicrophoneIndicator::setPushToTalkMode);

        // don't let it quit plasmashell
        m_sni->setStandardActionsEnabled(false);
    }

    const bool allMuted = muted();

    QString iconName;
    if (allMuted) {
        if (m_pushToTalkMode) {
            iconName = QStringLiteral("irc-unvoice"); // TODO proper icon
        } else {
            iconName = QStringLiteral("microphone-sensitivity-muted");
        }
    } else {
        if (m_pushToTalkMode) {
            iconName = QStringLiteral("irc-voice"); // TODO proper icon
        } else if (Source *defaultSource = m_sourceModel->defaultSource()) {
            const int percent = volumePercent(defaultSource);
            iconName = QStringLiteral("microphone-sensitivity");
            // it deliberately never shows the "muted" icon unless *all* microphones are muted
            if (percent <= 25) {
                iconName.append(QStringLiteral("-low"));
            } else if (percent <= 75) {
                iconName.append(QStringLiteral("-medium"));
            } else {
                iconName.append(QStringLiteral("-high"));
            }
        } else {
            iconName = QStringLiteral("microphone-sensitivity-high");
        }
    }

    m_sni->setIconByName(iconName);

    QString tooltip = i18nc("App is using mic", "%1 is using the microphone", names.constFirst());
    if (names.count() > 1) {
        tooltip = i18nc("List of apps is using mic", "%1 are using the microphone", names.join(i18nc("list separator", ", ")));
    }

    m_sni->setToolTip(QIcon::fromTheme(iconName),
                      m_pushToTalkMode ? i18n("Microphone (Push to talk)") : i18n("Microphone"),
                      tooltip);

    if (m_muteAction) {
        m_muteAction->setChecked(allMuted);
    }

    if (m_showOsdOnUpdate) {
        showOsd();
        m_showOsdOnUpdate = false;
    }
}

bool MicrophoneIndicator::muted() const
{
    static const int s_mutedRole = m_sourceModel->role(QByteArrayLiteral("Muted"));
    Q_ASSERT(s_mutedRole > -1);

    for (int row = 0; row < m_sourceModel->rowCount(); ++row) {
        const QModelIndex idx = m_sourceModel->index(row);
        if (!idx.data(s_mutedRole).toBool()) {
            // this is deliberately checking if *all* microphones are muted rather than the preferred one
            return false;
        }
    }

    return true;
}

void MicrophoneIndicator::setMuted(bool muted)
{
    static const int s_mutedRole = m_sourceModel->role(QByteArrayLiteral("Muted"));
    Q_ASSERT(s_mutedRole > -1);

    if (!m_pushToTalkMode) {
        m_showOsdOnUpdate = true;
    }

    if (muted) {
        for (int row = 0; row < m_sourceModel->rowCount(); ++row) {
            const QModelIndex idx = m_sourceModel->index(row);
            if (!idx.data(s_mutedRole).toBool()) {
                m_sourceModel->setData(idx, true, s_mutedRole);
                m_mutedIndices.append(QPersistentModelIndex(idx));
                continue;
            }
        }
        return;
    }

    // If we didn't mute it, unmute all
    if (m_mutedIndices.isEmpty()) {
        for (int i = 0; i < m_sourceModel->rowCount(); ++i) {
            m_sourceModel->setData(m_sourceModel->index(i), false, s_mutedRole);
        }
        return;
    }

    // Otherwise unmute the devices we muted
    for (auto &idx : qAsConst(m_mutedIndices)) {
        if (!idx.isValid()) {
            continue;
        }
        m_sourceModel->setData(idx, false, s_mutedRole);
    }
    m_mutedIndices.clear();

    // no update() needed as the model signals a change
}

void MicrophoneIndicator::toggleMuted()
{
    setMuted(!muted());
}

void MicrophoneIndicator::adjustVolume(int direction)
{
    Source *source = m_sourceModel->defaultSource();
    if (!source) {
        return;
    }

    const int step = qRound(5 * Context::NormalVolume / 100.0);

    const auto newVolume = qBound(Context::MinimalVolume,
                                  source->volume() + direction * step,
                                  Context::NormalVolume);

    source->setVolume(newVolume);
    source->setMuted(newVolume == Context::MinimalVolume);

    m_showOsdOnUpdate = true;
}

int MicrophoneIndicator::volumePercent(Source *source)
{
    return source->isMuted() ? 0 : qRound(source->volume() / static_cast<qreal>(Context::NormalVolume) * 100);
}

VolumeOSD *MicrophoneIndicator::osd()
{
    if (!m_osd) {
        m_osd = new VolumeOSD(this);
    }
    return m_osd;
}

void MicrophoneIndicator::showOsd()
{
    auto *preferredSource = m_sourceModel->defaultSource();
    if (!preferredSource) {
        return;
    }

    osd()->showMicrophone(volumePercent(preferredSource));

}

QStringList MicrophoneIndicator::appNames() const
{
    // If there are no microphones present, there's nothing to record
    if (m_sourceModel->rowCount() == 0) {
        return QStringList();
    }

    static const int s_nameRole = m_sourceOutputModel->role(QByteArrayLiteral("Name"));
    Q_ASSERT(s_nameRole > -1);
    static const int s_clientRole = m_sourceOutputModel->role(QByteArrayLiteral("Client"));
    Q_ASSERT(s_clientRole > -1);
    static const int s_virtualStreamRole = m_sourceOutputModel->role(QByteArrayLiteral("VirtualStream"));
    Q_ASSERT(s_virtualStreamRole > -1);

    QStringList names;
    names.reserve(m_sourceOutputModel->rowCount());

    for (int i = 0; i < m_sourceOutputModel->rowCount(); ++i) {
        const QModelIndex idx = m_sourceOutputModel->index(i);

        if (idx.data(s_virtualStreamRole).toBool()) {
            continue;
        }

        Client *client = qobject_cast<Client *>(idx.data(s_clientRole).value<QObject *>());

        const QString name = client ? client->name() : idx.data(s_nameRole).toString();
        if (!names.contains(name)) {
            names.append(name);
        }
    }

    return names;
}

void MicrophoneIndicator::setPushToTalkMode(bool pushToTalkMode)
{
    m_pushToTalkMode = pushToTalkMode;

    if (!pushToTalkMode) {
        setMuted(m_wasMutedBeforePushToTalkMode);
        return;
    }

    // FIXME take this from GlobalActionCollection thingie in main.qml, maybe add a Q_INVOKABLE to get current shortcut
    const auto shortcuts = KGlobalAccel::self()->globalShortcut(QStringLiteral("kmix"), QStringLiteral("mic_push_to_talk"));
    // FIXME proper notifyrc
    if (shortcuts.isEmpty()) {
        KNotification *notification = KNotification::event(KNotification::Notification, i18n("No Push to Talk Shortcut"),
                                                   i18n("set a shortcut to use it blabla"),
                                                   QStringLiteral("irc-voice"));
        notification->setActions({i18n("Configure Shortcuts...")});
        connect(notification, &KNotification::action1Activated, this, [this] {
            // FIXME open global shortcuts kcm
        });
        return;
    }

    if (!m_pushToTalkDisableTimer) {
        m_pushToTalkDisableTimer = new QTimer(this);
        m_pushToTalkDisableTimer->setSingleShot(true);
        connect(m_pushToTalkDisableTimer, &QTimer::timeout, this, [this] {
            setMuted(true);
            // TODO better text for when mic is muted again
            osd()->showText(QStringLiteral("irc-unvoice"), i18nc("Push to talk button released, microphone muted again", "Don't Speak"));
        });
    }

    KNotification *notification = KNotification::event(KNotification::Notification,
                                                       i18n("Push to Talk Enabled"),
                                                       i18nc("Placeholder is keyboard shortcut",
                                                             "Press and hold '%1' when you want to speak. Click the microphone icon to disable push to talk.",
                                                             shortcuts.first().toString(QKeySequence::NativeText)),
                                                       // TODO better icon
                                                       QStringLiteral("irc-voice"));
    // Mark as response to explicit user action ("enabling push to talk mode")
    notification->setHint(QStringLiteral("x-kde-user-action-feedback"), true);

    m_wasMutedBeforePushToTalkMode = muted();
    setMuted(true);
}

void MicrophoneIndicator::pushToTalk()
{
    if (!m_pushToTalkMode) {
        // FIXME store push to talk setting
        setPushToTalkMode(true);
        return;
    }

    QStyleHints *styleHints = qApp->styleHints();

    if (!m_pushToTalkDisableTimer->isActive()) {
        setMuted(false);
        // TODO proper icon
        osd()->showText(QStringLiteral("irc-voice"), i18nc("Push to talk button depressed, microphone unmuted", "Speak..."));
        // First key repeat comes after a long delay
        // multiply by two to have some margin in case Plasma is busy
        m_pushToTalkDisableTimer->start(styleHints->keyboardInputInterval() * 2);
        return;
    }

    // subsequent invocations are in quick succession
    m_pushToTalkDisableTimer->start(styleHints->keyboardAutoRepeatRate() * 2); // restart
}
