NAME         = NetWidgets
TARGET       = $${NAME}

QT           = core
QT          += gui
QT          += network
QT          += sql
QT          += script
QT          += positioning
QT          += widgets
QT          += opengl
QT          += printsupport
QT          += multimedia
QT          += multimediawidgets
QT          += webchannel

!CONFIG(static,static|shared) {
QT          += webengine
QT          += webenginewidgets
QT          += webview
}

QT          += QtCUDA
QT          += QtOpenCV
QT          += QtTorrent
QT          += Essentials
QT          += QtCalendar
QT          += QtGMP
QT          += QtGSL
QT          += QtAlgebra
QT          += QtDiscrete
QT          += QtFFT
QT          += Mathematics
QT          += QtFuzzy
QT          += QtFLP
QT          += QtFoundation
QT          += QtGeometry
QT          += QtGadgets
QT          += QtComponents
QT          += QtManagers

load(qt_build_config)
load(qt_module)

INCLUDEPATH += $${PWD}/../../include/$${NAME}
HEADERS     += $${PWD}/../../include/$${NAME}/netwidgets.h

include ($${PWD}/Widgets/Widgets.pri)
include ($${PWD}/Torrent/Torrent.pri)
include ($${PWD}/Web/Web.pri)

OTHER_FILES += $${PWD}/../../include/$${NAME}/headers.pri

include ($${PWD}/../../doc/Qt/Qt.pri)

TRNAME       = $${NAME}
include ($${PWD}/../../Translations.pri)
