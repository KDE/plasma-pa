/*
  Copyright (C) 2012-2014 Harald Sitter <apachelogger@ubuntu.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "module.h"
#include "ui_module.h"

#include <KAboutData>
#include <KColorScheme>
#include <KDeclarative/KDeclarative>
#include <KLocalizedString>

#include <context.h>
#include <pulseaudio.h>

#include "version.h"



#include <QQmlContext>
#include <QQuickItem>
#include <QQuickView>
#include <QQuickWindow>
#include <QStandardPaths>

class QuickView : public QQuickView
{
Q_OBJECT
public:
    QuickView(const QString &path,
                  QObject *context,
                  QObject *model,
                  QWindow *parent = nullptr)
        : QQuickView(parent)
    {
        KDeclarative::KDeclarative kdeclarative;
        kdeclarative.setDeclarativeEngine(engine());
        // FIXME: i18n
//        kdeclarative.setTranslationDomain(QStringLiteral(TRANSLATION_DOMAIN));
        kdeclarative.setupBindings();

        rootContext()->setContextProperty("pulseContext", context);
        rootContext()->setContextProperty("dataModel", model);
        rootContext()->setContextProperty("backgroundColor", KColorScheme(QPalette::Active, KColorScheme::View, KSharedConfigPtr(0)).background(KColorScheme::NormalBackground).color());
        QString mainFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation, path, QStandardPaths::LocateFile);
        setResizeMode(QQuickView::SizeRootObjectToView);
        setSource(QUrl(mainFile));
//            connect(rootObject(), SIGNAL(changed()), this, SIGNAL(changed()));
        setMinimumSize(initialSize());
        connect(rootObject(), SIGNAL(implicitWidthChanged()), this, SLOT(slotImplicitSizeChanged()));
        connect(rootObject(), SIGNAL(implicitHeightChanged()), this, SLOT(slotImplicitSizeChanged()));
    }

public slots:
    void onImplicitSizeChanged()
    {
        setMinimumSize(QSize(rootObject()->property("implicitWidth").toInt(),
                             rootObject()->property("implicitHeight").toInt()));
    }
};



Module::Module(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
    , ui(new Ui::Module)
    , m_context(new Context(this))
{
    KAboutData *aboutData = new KAboutData("kcm-about-distro",
                                           i18nc("@title", "About Distribution"),
                                           global_s_versionStringFull,
                                           QStringLiteral(""),
                                           KAboutLicense::LicenseKey::GPL_V3,
                                           i18nc("@info:credit", "Copyright 2012-2014 Harald Sitter"));

    setMinimumSize(800,600);

    aboutData->addAuthor(i18nc("@info:credit", "Harald Sitter"),
                        i18nc("@info:credit", "Author"),
                        QStringLiteral("apachelogger@kubuntu.org"));

    setAboutData(aboutData);

    ui->setupUi(this);


    QuickView *view = new QuickView(QStringLiteral("pulseaudio/qml/SinkInputs.qml"),
                                    m_context,
                                    new SinkInputModel(m_context, this));
    QWidget *w = QWidget::createWindowContainer(view, this);
    connect(view, &QWindow::minimumWidthChanged, w, &QWidget::setMinimumWidth);
    connect(view, &QWindow::minimumHeightChanged, w, &QWidget::setMinimumHeight);
    w->setMinimumSize(view->initialSize());
    //    vl->addWidget(w);
    //    setLayout(vl);
    //    connect(view.data(), &KWin::Compositing::EffectView::changed, [this]{
    //        emit changed(true);
    //    });
    w->setFocusPolicy(Qt::StrongFocus);

    QuickView *view1 = new QuickView(QStringLiteral("pulseaudio/qml/Sinks.qml"),
                                    m_context,
                                    new SinkModel(m_context, this));
    QWidget *w1 = QWidget::createWindowContainer(view1, this);
    connect(view1, &QWindow::minimumWidthChanged, w1, &QWidget::setMinimumWidth);
    connect(view1, &QWindow::minimumHeightChanged, w1, &QWidget::setMinimumHeight);
    w1->setMinimumSize(view->initialSize());
    w1->setFocusPolicy(Qt::StrongFocus);

    QuickView *view4 = new QuickView(QStringLiteral("pulseaudio/qml/Cards.qml"),
                                    m_context,
                                    new CardModel(m_context, this));
    QWidget *w4 = QWidget::createWindowContainer(view4, this);
    connect(view4, &QWindow::minimumWidthChanged, w4, &QWidget::setMinimumWidth);
    connect(view4, &QWindow::minimumHeightChanged, w4, &QWidget::setMinimumHeight);
    w4->setMinimumSize(view->initialSize());
    w4->setFocusPolicy(Qt::StrongFocus);

#warning i18n
    ui->tabWidget->insertTab(0, w, QString("Applications"));
    ui->tabWidget->insertTab(1, new QWidget, "Recording");
    ui->tabWidget->insertTab(2, w1, "Output Devices");
    ui->tabWidget->insertTab(3, new QWidget, "Input Devices");
    ui->tabWidget->insertTab(4, w4, "Configuration");

    // We have no help so remove the button from the buttons.
    setButtons(buttons() ^ KCModule::Help ^ KCModule::Default ^ KCModule::Apply);
}

Module::~Module()
{
    delete ui;
}

void Module::load()
{
}

void Module::save()
{
}

void Module::defaults()
{
}

#include "module.moc"
