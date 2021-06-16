#include <netwidgets.h>
#include "ui_nTorrentWidget.h"

#pragma message("TorrentWidget is in need of re-organize")

N::TorrentWidget:: TorrentWidget    ( QWidget * parent , Plan * p )
                 : TabWidget        (           parent ,        p )
                 , AttachDock       (                           p )
                 , QtTorrentManager (                             )
                 , Timer            ( new QTimer ( this )         )
                 , ui               ( new Ui::nTorrentWidget      )
                 , started          ( false                       )
                 , inLoop           ( false                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::TorrentWidget::~TorrentWidget(void)
{
  delete ui ;
}

void N::TorrentWidget::closeEvent(QCloseEvent * e)
{
  started = false              ;
  Timer -> stop ( )            ;
  QTabWidget::closeEvent ( e ) ;
}

void N::TorrentWidget::contextMenuEvent(QContextMenuEvent * event)
{
  if (Menu(event->pos())) event->accept()  ;
  else QTabWidget::contextMenuEvent(event) ;
}

void N::TorrentWidget::Configure(void)
{
  ui   -> setupUi   ( this               )         ;
  plan -> setFont   ( this               )         ;
  setAccessibleName ( "N::TorrentWidget" )         ;
  setObjectName     ( "N::TorrentWidget" )         ;
  //////////////////////////////////////////////////
  QFont f = plan -> fonts [ Fonts::TreeView ]      ;
  f . setPixelSize    ( f.pixelSize() - 4 )        ;
  tabBar() -> setFont ( f                 )        ;
  //////////////////////////////////////////////////
  QStringList DH                                   ;
  DH << tr ( "Name"     )                          ;
  DH << tr ( "Size"     )                          ;
  DH << tr ( "Progress" )                          ;
  DH << tr ( "Start"    )                          ;
  DH << tr ( "ETA"      )                          ;
  ui->Download->setHeaderLabels(DH)                ;
  //////////////////////////////////////////////////
  torrents = new TorrentLists ( this , plan )      ;
  insertTab ( 0 , torrents , tr("Torrents") )      ;
  //////////////////////////////////////////////////
  setCurrentWidget  ( torrents )                   ;
  Timer -> setParent ( this     )                  ;
  nConnect ( torrents , SIGNAL (Clicked (SUID))    ,
             this     , SLOT   (Download(SUID))  ) ;
  nConnect ( Timer    , SIGNAL (timeout())         ,
             this     , SLOT   (Update ())       ) ;
}

void N::TorrentWidget::Docking     (
       QMainWindow       * Main  ,
       QString             title ,
       Qt::DockWidgetArea  area  ,
       Qt::DockWidgetAreas areas )
{
  AttachDock::Docking(Main,this,title,area,areas) ;
  QObject::connect(Dock,SIGNAL(visibilityChanged(bool)),this,SLOT(Visible(bool)));
}

void N::TorrentWidget::Visible(bool visible)
{
  Visiblity(visible) ;
}

void N::TorrentWidget::DockIn(bool shown)
{
  Show(shown);
}

bool N::TorrentWidget::Menu(QPoint)
{
  nScopedMenu ( mm , this )                                     ;
  QMdiSubWindow   * mdi  = Casting ( QMdiSubWindow , parent() ) ;
  QDockWidget     * dock = Casting ( QDockWidget   , parent() ) ;
  QWidget         * cw   = currentWidget ( )                    ;
  QTreeWidgetItem * item = ui->Download->currentItem()          ;
  QAction         * aa                                          ;
  if (cw==ui->Download)                                         {
    if (item->parent()==NULL)                                   {
      mm.add(201,tr("Delete item"))                             ;
    }                                                           ;
  } else                                                        {
    mm.add(101,tr("Refresh torrents"))                          ;
  }                                                             ;
  if (NotNull(dock)) mm.add(1000001,tr("Move to window area"))  ;
  if (NotNull(mdi )) mm.add(1000002,tr("Move to dock area"  ))  ;
  mm . setFont ( plan    )                                      ;
  aa = mm.exec()                                                ;
  if (IsNull(aa)) return true                                   ;
  switch (mm[aa])                                               {
    case 101                                                    :
      torrents->clear()                                         ;
      torrents->start()                                         ;
    break                                                       ;
    case 201                                                    :
      Delete ( nTreeUuid ( item , 0 ) )                         ;
    break                                                       ;
    case 1000001                                                :
      emit attachMdi  ( this , 0             )                  ;
    break                                                       ;
    case 1000002                                                :
      emit attachDock                                           (
        this , windowTitle()                                    ,
        Qt::BottomDockWidgetArea                                ,
        Qt::LeftDockWidgetArea  | Qt::RightDockWidgetArea       |
        Qt::TopDockWidgetArea   | Qt::BottomDockWidgetArea    ) ;
    break                                                       ;
    default                                                     :
    break                                                       ;
  }                                                             ;
  return true                                                   ;
}

bool N::TorrentWidget::startup(void)
{
  torrents -> clear (       ) ;
  torrents -> start (       ) ;
  started  = true             ;
  #ifndef QT_DEBUG
  Listen            (       ) ;
  #endif
  start             ( 10001 ) ;
  Timer -> start    ( 1000  ) ;
  return true                 ;
}

void N::TorrentWidget::limitChanged(int)
{
  int mul = ui->UploadLimit  ->value() ;
  int mdl = ui->DownloadLimit->value() ;
  setLimit(mul,mdl)                    ;
}

void N::TorrentWidget::List(SUID uuid,QtTorrent & torrent)
{
  QString name                                ;
  long long ts = 0                            ;
  name = torrent.Item("Name").toString()      ;
  ts   = torrent.Size()                       ;
  /////////////////////////////////////////////
  NewTreeWidgetItem(item)                     ;
  item->setText(0,name)                       ;
  item->setText(1,QString::number(ts))        ;
  item->setData(0,Qt::UserRole,uuid)          ;
  item->setTextAlignment(1,Qt::AlignRight     |
                           Qt::AlignVCenter ) ;
  ui->Download->blockSignals   (true)         ;
  ui->Download->addTopLevelItem(item)         ;
  /////////////////////////////////////////////
  for (int i=0;i<torrent.Files();i++)         {
    NewTreeWidgetItem(ftem)                   ;
    QString f                                 ;
    TUID s                                    ;
    f = torrent.File(i,"File").toString   ()  ;
    s = torrent.File(i,"Size").toLongLong()   ;
    if (f.length()>0 && s!=0)                 {
      ftem->setCheckState (0,Qt::Checked)     ;
      ftem->setText(0,f                 )     ;
      ftem->setText(1,QString::number(s))     ;
      ftem->setTextAlignment(1,Qt::AlignRight |
                           Qt::AlignVCenter ) ;
      item->addChild(ftem)                    ;
    }                                         ;
  }                                           ;
  /////////////////////////////////////////////
  ui->Download->blockSignals(false)           ;
  setCurrentWidget(ui->Download)              ;
}

void N::TorrentWidget::Download(SUID uuid)
{
  nDropOut ( uuid <= 0 )                      ;
  QByteArray B                                ;
  QString    filename                         ;
  QString    path                             ;
  CodeManager CM ( plan )                     ;
  EnterSQL(SC,plan->sql)                      ;
    QString Q                                 ;
    CM . LoadFile ( SC , uuid , B )           ;
    Q = SC.sql.SelectFrom                     (
          "filename"                          ,
          PlanTable(FileManager)              ,
          QString                             (
            "where uuid = %1 "
            "and type = %2 "
            "and extension = %3 "
            "and depot = %4"                  )
          .arg(uuid                           )
          .arg(Types::File                    )
          .arg(724                            )
          .arg(torrents->Depot                )
        )                                     ;
    if (SC.Fetch(Q))                          {
      filename = SC.String(0)                 ;
    }                                         ;
  LeaveSQL(SC,plan->sql)                      ;
  /////////////////////////////////////////////
  if (filename.length()<=0) return            ;
  filename = plan->Temporary(filename)        ;
  File::toFile(filename,B)                    ;
  QtTorrent torrent                           ;
  torrent.setFile(filename)                   ;
  QString name                                ;
  name = torrent.Item("Name").toString()      ;
  if (name.length()<=0) return                ;
  /////////////////////////////////////////////
  path = directory.absoluteFilePath(name)     ;
  if (path.length()<=0) return                ;
  directory.mkdir(path)                       ;
  torrent.Directory = path                    ;
  QtTorrentManager::Download(torrent)         ;
  /////////////////////////////////////////////
  List  ( uuid , torrent )                    ;
  /////////////////////////////////////////////
  QTreeWidgetItem * mtem                      ;
  mtem = torrents->uuidItem(uuid)             ;
  if (mtem!=NULL)                             {
    mtem->setCheckState(0,Qt::Checked)        ;
    mtem->setCheckState(1,Qt::Checked)        ;
  }                                           ;
  Alert ( Done           )                    ;
}

void N::TorrentWidget::stopIt(void)
{
  started = false             ;
  Timer -> stop ( )           ;
  while ( inLoop )            {
    Time::msleep ( 50 )       ;
    qApp -> processEvents ( ) ;
  }                           ;
}

void N::TorrentWidget::DHT(bool enabled)
{
  if (enabled) Resume ( ) ;
          else Pause  ( ) ;
}

void N::TorrentWidget::Halt(void)
{
  Pause                   (       ) ;
  ui->Start->blockSignals ( true  ) ;
  ui->Start->setChecked   ( false ) ;
  ui->Start->blockSignals ( false ) ;
}

void N::TorrentWidget::Update(void)
{
  QString b  = tr("Torrent manager")             ;
  int     us = Value("Upload"  ).toInt()         ;
  int     ds = Value("Download").toInt()         ;
  QString p  = tr("%1 / %2").arg(us).arg(ds)     ;
  QString m  = b + " : "  + p                    ;
  setWindowTitle(m)                              ;
  QWidget * pw = parentWidget()                  ;
  if (NotNull(pw)) pw->setWindowTitle(m)         ;
  int total = ui->Download->topLevelItemCount()  ;
  int index = -1                                 ;
  QString name                                   ;
  QList<float> progress                          ;
  QList<long long> filesizes                     ;
  ui->Download->blockSignals(true )              ;
  for (int i=0;i<total;i++)                      {
    if (Progress(i,name,progress,filesizes))     {
      QTreeWidgetItem * item                     ;
      QTreeWidgetItem * ctem                     ;
      int ps = progress.count()                  ;
      index = -1                                 ;
      for (int j=0;index<0 && j<total;j++)       {
        item = ui->Download->topLevelItem(j)     ;
        if (item->text(0)==name)                 {
          if (item->childCount()==ps)            {
            index = j                            ;
          }                                      ;
        }                                        ;
      }                                          ;
      if (index>=0)                              {
        item = ui->Download->topLevelItem(index) ;
        if (item->childCount()==ps)              {
          for (int j=0;j<ps;j++)                 {
            float     v  = progress [j]          ;
            long long fs = filesizes[j]          ;
            QString   FS = QString::number(fs)   ;
            QString   ST                         ;
            v *= 100                             ;
            QString pm                           ;
            pm  = QString::number(v,'f',3)       ;
            pm += "%"                            ;
            ctem = item->child(j)                ;
            ctem->setText(2,pm)                  ;
            ctem->setToolTip(2,FS)               ;
            ST = ctem->text(3)                   ;
            if (ST.length()<=0 && (v>0.000001))  {
              QDateTime DT                       ;
              DT = QDateTime::currentDateTime()  ;
              QString VT = "yyyy/MM/dd hh:mm:ss" ;
              VT = DT.toString(VT)               ;
              ctem->setText(3,VT)                ;
            }                                    ;
          }                                      ;
        }                                        ;
      }                                          ;
    }                                            ;
  }                                              ;
  ui->Download->blockSignals(false)              ;
  plan->Variables["Upload"  ] = QVariant(us)     ;
  plan->Variables["Download"] = QVariant(ds)     ;
}

bool N::TorrentWidget::LoadState(void)
{
  Sql sql                                               ;
  sql.SqlMode = "SQLITE"                                ;
  sql.dbName  = directory.absoluteFilePath(".torrents") ;
  ui -> Download -> blockSignals ( true  )              ;
  SqlConnection SC ( sql )                              ;
  if ( SC . open ( FunctionString ) )                   {
    QString Q                                           ;
    UUIDs Uuids                                         ;
    SUID  uuid                                          ;
    Q = SC.sql.SelectFrom("uuid","torrent"              ,
                          "order by id asc"           ) ;
    SqlLoopNow ( SC , Q )                               ;
      Uuids << SC.Uuid(0)                               ;
    SqlLoopErr ( SC , Q )                               ;
    SqlLoopEnd ( SC , Q )                               ;
    foreach (uuid,Uuids)                                {
      QTreeWidgetItem * it = NULL                       ;
      int total                                         ;
      Download (uuid)                                   ;
      it    = NULL                                      ;
      total = ui->Download->topLevelItemCount()         ;
      for (int i=0;it==NULL && i<total;i++)             {
        QTreeWidgetItem * xt                            ;
        xt = ui->Download->topLevelItem(i)              ;
        if (uuid==nTreeUuid(xt,0)) it = xt              ;
      }                                                 ;
      if (NotNull(it))                                  {
        Q = SC.sql.SelectFrom                           (
            "position,download,file,stime" , "tlist"    ,
        QString("where uuid = %1 order by position asc" )
            .arg(uuid)                               )  ;
        SqlLoopNow ( SC , Q )                           ;
          int     idx  = SC . Int    ( 0 )              ;
          int     down = SC . Int    ( 1 )              ;
          QString file = SC . String ( 2 )              ;
          QString stim = SC . String ( 3 )              ;
          QString tnam = it-> text   ( 0 )              ;
          if (down==0)                                  {
            QTreeWidgetItem * xt                        ;
            xt = it->child(idx)                         ;
            xt->setCheckState(0,Qt::Unchecked)          ;
            xt->setText(3,stim)                         ;
            QtTorrentManager::setPriority(tnam,idx,down) ;
          }                                             ;
        SqlLoopErr ( SC , Q )                           ;
        SqlLoopEnd ( SC , Q )                           ;
      }                                                 ;
    }                                                   ;
    SC.close()                                          ;
  }                                                     ;
  SC.remove()                                           ;
  ui -> Download -> blockSignals ( false )              ;
  return true                                           ;
}

bool N::TorrentWidget::SaveState(void)
{
  Sql sql                                               ;
  sql.SqlMode = "SQLITE"                                ;
  sql.dbName  = directory.absoluteFilePath(".torrents") ;
  SqlConnection SC ( sql )                              ;
  if (SC.open(FunctionString))                          {
    QString Q                                           ;
    Q = "create table torrent ("
        "id integer primary key autoincrement,"
        "uuid bigint not null,"
        "name blob"
        ") ;"                                           ;
    SC.Query(Q)                                         ;
    Q = "create table tlist ("
        "id integer primary key autoincrement,"
        "uuid bigint not null,"
        "position integer default -1,"
        "download integer default 0,"
        "name blob,"
        "file blob,"
        "stime blob"
        ") ;"                                           ;
    SC.Query(Q)                                         ;
    Q = "delete from torrent ;"                         ;
    SC.Query(Q)                                         ;
    Q = "delete from tlist ;"                           ;
    SC.Query(Q)                                         ;
    int total = ui->Download->topLevelItemCount()       ;
    nFullLoop ( i , total )                             {
      QTreeWidgetItem * it                              ;
      it = ui->Download->topLevelItem(i)                ;
      SUID    uuid = nTreeUuid( it , 0 )                ;
      QString name = it->text ( 0      )                ;
      Q = SC.sql.InsertInto("torrent",2,"uuid","name")  ;
      SC . Prepare ( Q                      )           ;
      SC . Bind    ( "uuid" , uuid          )           ;
      SC . Bind    ( "name" , name.toUtf8() )           ;
      SC . Exec    (                        )           ;
      nFullLoop ( n , it->childCount() )                {
        QTreeWidgetItem * ft                            ;
        ft = it->child(n)                               ;
        int d = ft->checkState(0)==Qt::Checked ? 1 : 0  ;
        QString file = ft->text(0)                      ;
        QString stim = ft->text(3)                      ;
        Q = SC.sql.InsertInto                           (
              "tlist"                                   ,
              6                                         ,
              "uuid"                                    ,
              "position"                                ,
              "download"                                ,
              "name"                                    ,
              "file"                                    ,
              "stime"                                 ) ;
        SC . Prepare ( Q                              ) ;
        SC . Bind    ( "uuid"     , uuid              ) ;
        SC . Bind    ( "position" , n                 ) ;
        SC . Bind    ( "download" , d                 ) ;
        SC . Bind    ( "name"     , name.toUtf8()     ) ;
        SC . Bind    ( "file"     , file.toUtf8()     ) ;
        SC . Bind    ( "stime"    , stim.toUtf8()     ) ;
        SC . Exec    (                                ) ;
      }                                                 ;
    }                                                   ;
    SC.close()                                          ;
  }                                                     ;
  SC.remove()                                           ;
  return true                                           ;
}

void N::TorrentWidget::run(int T,ThreadData * d)
{
  switch ( T )      {
    case 10001      :
      Monitor ( d ) ;
    break           ;
  }                 ;
}

void N::TorrentWidget::run(void)
{
  Monitor ( &Data ) ;
}

void N::TorrentWidget::Monitor(ThreadData * d)
{
  inLoop = true                         ;
  LoadState ( )                         ;
  while ( IsContinue ( d ) && started ) {
    Look         (    )                 ;
    Time::msleep ( 10 )                 ;
    if ( ! IsContinue ( d ) )           {
      started = false                   ;
    }                                   ;
  }                                     ;
  SaveState ( )                         ;
  inLoop = false                        ;
}

void N::TorrentWidget::torrentChanged (
       QTreeWidgetItem * item       ,
       int               column     )
{
  if (column!=0) return                     ;
  QTreeWidgetItem * ptem                    ;
  ptem = item->parent()                     ;
  if (IsNull(ptem)) return                  ;
  QString n = ptem->text(0)                 ;
  bool b = item->checkState(0)==Qt::Checked ;
  int i = ptem->indexOfChild(item)          ;
  if (i<0) return                           ;
  int p = b ? 1 : 0                         ;
  QtTorrentManager::setPriority(n,i,p)      ;
}

void N::TorrentWidget::Delete(SUID uuid)
{
  QTreeWidgetItem * it = NULL                       ;
  int total                                         ;
  it    = NULL                                      ;
  total = ui->Download->topLevelItemCount()         ;
  for (int i=0;it==NULL && i<total;i++)             {
    QTreeWidgetItem * xt                            ;
    xt = ui->Download->topLevelItem(i)              ;
    if (uuid==nTreeUuid(xt,0)) it = xt              ;
  }                                                 ;
  if (IsNull(it)) return                            ;
  int index = ui->Download->indexOfTopLevelItem(it) ;
  if (index<0) return                               ;
  QString name = it->text(0)                        ;
  if (name.length()>0) Remove(name)                 ;
  ui->Download->takeTopLevelItem(index)             ;
}

void N::TorrentWidget::Lookup(bool enabled)
{
}
