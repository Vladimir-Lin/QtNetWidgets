#include <netwidgets.h>

N::WebDockBar:: WebDockBar     ( QWidget * parent , Plan * p )
              : ToolBar        (           parent ,        p )
              , NetworkManager (                           p )
              , GroupItems     (                           p )
              , Bookmark       ( 0                           )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::WebDockBar::~WebDockBar (void)
{
}

void N::WebDockBar::Configure(void)
{
  Loading = false                                                                          ;
  setWindowTitle  (tr("Web browser tools")                     )                           ;
  setAllowedAreas ( Qt::TopToolBarArea | Qt::BottomToolBarArea )                           ;
  setIconSize (QSize(24,24))                                                               ;
  QComboBox * combo = new QComboBox(this)                                                  ;
  combo             -> setEditable (true)                                                  ;
  combo             -> setMaxVisibleItems ( 30                     )                       ;
  combo             -> setInsertPolicy    ( QComboBox::InsertAtTop )                       ;
  Widgets[Edit    ]  = combo->lineEdit    (                        )                       ;
  Widgets[List    ]  = combo                                                               ;
  Actions[Save    ]  = addAction(QIcon(":/images/words.png"       ),tr("Save"           )) ;
  Actions[History ]  = addAction(QIcon(":/images/astrophysics.png"),tr("History"        )) ;
  Actions[Analysis]  = addAction(QIcon(":/images/menu.png"        ),tr("Analysis"       )) ;
  Actions[Engine  ]  = addAction(QIcon(":/images/engine.png"      ),tr("Search engine"  )) ;
  Actions[Desktop ]  = addAction(QIcon(":/images/server.png"      ),tr("Desktop browser")) ;
  Actions[Back    ]  = addAction(QIcon(":/images/projectup.png"   ),tr("Back"           )) ;
  Actions[Forward ]  = addAction(QIcon(":/images/right.png"       ),tr("Forward"        )) ;
  Actions[Stop    ]  = addAction(QIcon(":/images/flowoutput.png"  ),tr("Stop"           )) ;
  Actions[Reload  ]  = addAction(QIcon(":/images/refreshright.png"),tr("Reload"         )) ;
  Actions[Go      ]  = addAction(QIcon(":/images/zoomin.png"      ),tr("Go"             )) ;
  addWidget (combo)                                                                        ;
  combo->setVisible(false)                                                                 ;
}

bool N::WebDockBar::Relocation(void)
{
  nKickOut ( IsNull(plan) , false            ) ;
  QAction * a = Actions[Go]                    ;
  nKickOut ( !Widgets.contains(List) , false ) ;
  nKickOut ( IsNull(a)    , false            ) ;
  QWidget * d = widgetForAction(a)             ;
  nKickOut ( IsNull(d)    , false            ) ;
  //////////////////////////////////////////////
  QRect g =                     geometry ( )   ;
  QRect s = Widgets [ List ] -> geometry ( )   ;
  int   w = g . width  ( ) - s . x ( ) - 4     ;
  int   h = g . height ( ) - 8                 ;
  QRect S ( s . x ( ) , 4 , w , h )            ;
  Widgets [ List ] -> setVisible  ( true )     ;
  Widgets [ List ] -> setGeometry    ( S )     ;
  return false                                 ;
}

void N::WebDockBar::Disconnect(void)
{
  disconnect(Actions[Save    ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[History ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[Back    ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[Forward ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[Reload  ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[Stop    ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[Analysis],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[Desktop ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Actions[Go      ],SIGNAL(triggered    (   )),NULL,NULL) ;
  disconnect(Widgets[Edit    ],SIGNAL(returnPressed(   )),NULL,NULL) ;
  disconnect(Widgets[List    ],SIGNAL(triggered    (int)),NULL,NULL) ;
}

void N::WebDockBar::setUrl(const QUrl & url)
{
  QString addr = url.toString ()                         ;
  Widgets[Edit]->blockSignals(true )                     ;
  Widgets[List]->blockSignals(true )                     ;
  QComboBox * history = Casting(QComboBox,Widgets[List]) ;
  QLineEdit * urledit = Casting(QLineEdit,Widgets[Edit]) ;
  if (NotNull(history) && history->findText(addr)<0)     {
    if (addr.length()>0)                                 {
      history->insertItem(0,addr)                        ;
      if (Bookmark>0)                                    {
        SqlConnection SC ( plan->sql                   ) ;
        QString CN = QtUUID::createUuidString()          ;
        if (SC.open("nBookmarkTree",CN))                 {
          QUrl U = url                                   ;
          SUID puid = NetworkManager::assurePage(SC,U)   ;
          UUIDs Buids                                    ;
          if (puid>0)                                    {
            Buids << puid                                ;
            GroupItems::Join                             (
              SC                                         ,
              Bookmark                                   ,
              Types::Division                            ,
              Types::URL                                 ,
              Groups::Subordination                      ,
              0                                          ,
              Buids                                    ) ;
          }                                              ;
          SC.close       (                       )       ;
        }                                                ;
        SC . remove      (                       )       ;
      }                                                  ;
    }                                                    ;
  }                                                      ;
  if (NotNull(urledit)) urledit -> setText ( addr )      ;
  Widgets[Edit]->blockSignals(false)                     ;
  Widgets[List]->blockSignals(false)                     ;
}

QUrl N::WebDockBar::Address(void)
{
  QLineEdit * urledit = Casting(QLineEdit,Widgets[Edit]) ;
  QString     addr = ""                                  ;
  if (NotNull(urledit)) addr = urledit -> text ( )       ;
  return QUrl ( addr )                                   ;
}

void N::WebDockBar::run(void)
{
  nDropOut ( Bookmark <= 0 )                           ;
  Loading = true                                       ;
  QComboBox * combo = Casting(QComboBox,Widgets[List]) ;
  nDropOut ( IsNull(combo) )                           ;
  combo->clear()                                       ;
  combo->blockSignals(true )                           ;
  SqlConnection SC(plan->sql                         ) ;
  QString CN = QtUUID::createUuidString()              ;
  if (SC.open("nBookmarkTree",CN))                     {
    UUIDs   Uuids                                      ;
    SUID    uuid                                       ;
    NetworkManager::LoadDomainIndex ( SC )             ;
    Uuids = GroupItems::Subordination                  (
              SC                                       ,
              Bookmark                                 ,
              Types::Division                          ,
              Types::URL                               ,
              Groups::Subordination                    ,
              SC.OrderByDesc("position")             ) ;
    foreach (uuid,Uuids)                               {
      QUrl U = NetworkManager::PageUrl ( SC , uuid )   ;
      combo->addItem(U.toString())                     ;
    }                                                  ;
    SC.close       (                       )           ;
  }                                                    ;
  SC . remove      (                       )           ;
  combo->blockSignals(false)                           ;
  Loading = false                                      ;
}

void N::WebDockBar::Load(void)
{
  nDropOut ( Loading ) ;
  start    (         ) ;
}

void N::WebDockBar::Patterns(SUID type,QString name,QString keyword)
{
  QComboBox * combo                        ;
  combo = Casting(QComboBox,Widgets[List]) ;
  nDropOut ( IsNull(combo) )               ;
  QStringList P                            ;
  EnterSQL(SC,plan->sql)                   ;
    NetworkManager::UrlPatterns            (
      SC                                   ,
      type                                 ,
      name                                 ,
      P                                  ) ;
  LeaveSQL(SC,plan->sql)                   ;
  nDropOut ( P.count() <= 0 )              ;
  nFullLoop(i,P.count())                   {
    QString g = P[i]                       ;
    QString p = QString(g).arg(keyword)    ;
    combo->insertItem(0,p)                 ;
  }                                        ;
  combo -> showPopup ( )                   ;
}
