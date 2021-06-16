#include <netwidgets.h>

N::PageTree :: PageTree       ( QWidget * parent , Plan * p )
             : TreeDock       (           parent ,        p )
             , NetworkManager (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PageTree ::~PageTree(void)
{
}

void N::PageTree::Configure(void)
{
  NewTreeWidgetItem       ( head                   ) ;
  head->setText           ( 0 ,tr("Page")          ) ;
  plan -> setFont         ( this                   ) ;
  setRootIsDecorated      ( false                  ) ;
  setAlternatingRowColors ( true                   ) ;
  setColumnCount          ( 1                      ) ;
  setSelectionMode        ( ExtendedSelection      ) ;
  setAllAlignments        ( head , Qt::AlignCenter ) ;
  setHeaderItem           ( head                   ) ;
  setWindowTitle          ( tr("Page analyizer")   ) ;
  MountClicked            ( 2                      ) ;
}

bool N::PageTree::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , false               ) ;
  DisableAllActions (                                    ) ;
  AssignAction      ( Label        , windowTitle     ( ) ) ;
  LinkAction        ( Copy         , CopyToClipboard ( ) ) ;
  LinkAction        ( Export       , Export          ( ) ) ;
  LinkAction        ( Delete       , Delete          ( ) ) ;
  return true                                              ;
}

void N::PageTree::setStatusIcon(QTreeWidgetItem * item,int retcode)
{
  switch (retcode)                                      {
    case 0                                              :
      item->setIcon(0,QIcon(":/images/flowoutput.png")) ;
    break                                               ;
    case 10                                             :
      item->setIcon(0,QIcon(":/images/refreshtop.png")) ;
    break                                               ;
    case 200                                            :
      item->setIcon(0,QIcon(":/images/yes.png"))        ;
    break                                               ;
    case 302                                            :
      item->setIcon(0,QIcon(":/images/exchange.png"))   ;
    break                                               ;
    case 404                                            :
      item->setIcon(0,QIcon(":/images/flowend.png"))    ;
    break                                               ;
    default                                             :
      item->setIcon(0,QIcon(":/images/help.png"     ) ) ;
    break                                               ;
  }                                                     ;
}

bool N::PageTree::startup(QUrl & url)
{
  clear           (                )            ;
  setWindowTitle  ( url.toString() )            ;
  EnterSQL        ( SC , plan->sql )            ;
    UUIDs   Uuids                               ;
    SUID    uuid                                ;
    SUID    Ruid = PageUuid(SC,url)             ;
    if (Ruid>0)                                 {
      Uuids << Ruid                             ;
      FromPage ( SC   , Ruid , Uuids )          ;
      foreach  ( uuid ,        Uuids )          {
        QUrl u       = PageUrl    ( SC , uuid ) ;
        int  retcode = HttpStatus ( SC , uuid ) ;
        NewTreeWidgetItem (item               ) ;
        item->setData     (0,Qt::UserRole,uuid) ;
        item->setText     (0,u.toString()     ) ;
        setStatusIcon     (item,retcode       ) ;
        addTopLevelItem   (item               ) ;
      }                                         ;
    }                                           ;
  LeaveSQL        ( SC , plan->sql )            ;
  SuitableColumns (                )            ;
  Alert           ( Done           )            ;
  return true                                   ;
}

bool N::PageTree::startup(UUIDs & Uuids)
{
  clear           (                )          ;
  EnterSQL        ( SC , plan->sql )          ;
    QString Q                                 ;
    SUID    uuid                              ;
    foreach (uuid,Uuids)                      {
      QUrl u       = PageUrl    ( SC , uuid ) ;
      int  retcode = HttpStatus ( SC , uuid ) ;
      NewTreeWidgetItem (item               ) ;
      item->setData     (0,Qt::UserRole,uuid) ;
      item->setText     (0,u.toString()     ) ;
      setStatusIcon     (item,retcode       ) ;
      addTopLevelItem   (item               ) ;
    }                                         ;
  LeaveSQL        ( SC , plan->sql )          ;
  SuitableColumns (                )          ;
  Alert           ( Done           )          ;
  return true                                 ;
}

void N::PageTree::Delete(void)
{
  TreeWidgetItems Items = selectedItems() ;
  nDropOut ( Items . count ( ) <= 0 )     ;
  nFullLoop ( i , Items.count() )         {
    takeItem(Items[i])                    ;
  }                                       ;
}

bool N::PageTree::Menu(QPoint pos)
{ Q_UNUSED ( pos )                                              ;
  QMdiSubWindow * mdi    = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget   * dock   = Casting ( QDockWidget   , parent() ) ;
  MenuManager     mm(this)                                      ;
  QAction       * aa                                            ;
  QTreeWidgetItem * item = currentItem()                        ;
  if (NotNull(item)) mm.add(101,tr("Go"))                       ;
  if (NotNull(item)) mm.add(102,tr("Header" ))                  ;
  if (NotNull(item)) mm.add(103,tr("Content"))                  ;
  mm.add(201,tr("Export to ..."))                               ;
  mm.addSeparator()                                             ;
  mm.add(301,tr("Sorting"),true,isSortingEnabled())             ;
  if (NotNull(dock) || NotNull(mdi )) mm.addSeparator (      )  ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm.setFont ( plan )                                           ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 101                                                    :
      doubleClicked(item,0)                                     ;
    break                                                       ;
    case 102                                                    :
      if (NotNull(item))                                        {
        SUID       uuid = nTreeUuid ( item , 0 )                ;
        QByteArray B                                            ;
        bool       got  = false                                 ;
        EnterSQL ( SC , plan->sql )                             ;
          QString    Q                                          ;
          Q = SC.sql.SelectFrom                                 (
                "header"                                        ,
                PlanTable(WebContents)                          ,
                SC.WhereUuid(uuid)                            ) ;
          if (SC.Fetch(Q)) B = SC.ByteArray(0)                  ;
          got = ( B.size() > 0 )                                ;
        LeaveSQL ( SC , plan->sql )                             ;
        if (got)                                                {
          emit OpenPlainText ( item->text(0) , B , 0 )          ;
        } else                                                  {
          Alert ( Error )                                       ;
        }                                                       ;
      }                                                         ;
    break                                                       ;
    case 103                                                    :
      if (NotNull(item))                                        {
        SUID       uuid = nTreeUuid ( item , 0 )                ;
        QByteArray B                                            ;
        bool       got  = false                                 ;
        DocumentManager DM ( plan )                             ;
        EnterSQL ( SC , plan->sql )                             ;
          if (!DM.GetXml(SC,uuid,B))                            {
            QString    Q                                        ;
            Q = SC.sql.SelectFrom                               (
                  "html"                                        ,
                  PlanTable(WebContents)                        ,
                  SC.WhereUuid(uuid)                          ) ;
            if (SC.Fetch(Q)) B = SC.ByteArray(0)                ;
          }                                                     ;
          got = ( B.size() > 0 )                                ;
        LeaveSQL ( SC , plan->sql )                             ;
        if (got)                                                {
          emit OpenPlainText ( item->text(0) , B , 0 )          ;
        } else                                                  {
          Alert ( Error )                                       ;
        }                                                       ;
      }                                                         ;
    break                                                       ;
    case 201                                                    :
      Export ( )                                                ;
    break                                                       ;
    case 301                                                    :
      setSortingEnabled(aa->isChecked())                        ;
    break                                                       ;
    case 1000001                                                :
      emit attachMdi ( this , 0 )                               ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this                                                    ,
        windowTitle()                                           ,
        Qt::BottomDockWidgetArea                                ,
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

void N::PageTree::doubleClicked(QTreeWidgetItem * item,int column)
{
  QUrl url ( item->text(0) ) ;
  emit Web ( url , true    ) ;
}

void N::PageTree::Export(void)
{
  QString filename                        ;
  filename = QFileDialog::getSaveFileName (
               this                       ,
               tr("Export pages to file") ,
               plan->Temporary("")        ,
               "*.txt\n*.*"             ) ;
  nDropOut ( filename.length() <= 0 )     ;
  QStringList L                           ;
  QString     M                           ;
  QByteArray  B                           ;
  for (int i=0;i<topLevelItemCount();i++) {
    QTreeWidgetItem * item                ;
    item = topLevelItem(i)                ;
    L << item->text(0)                    ;
  }                                       ;
  if (L.count()<=0) return                ;
  M = L . join   ( "\r\n" )               ;
  B = M . toUtf8 (        )               ;
  File::toFile ( filename , B )           ;
  Alert        ( Done         )           ;
}
