#include <netwidgets.h>

N::TorrentLists:: TorrentLists ( QWidget * parent , Plan * p             )
                : TreeWidget   (           parent ,        p             )
                , Ownership    ( 0 , Types::None , Groups::Subordination )
                , GroupItems   (                           p             )
                , Depot        ( (int)Types::Video                       )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::TorrentLists::~TorrentLists (void)
{
}

QTreeWidgetItem * N::TorrentLists::uuidItem(SUID uuid)
{
  for (int i=0;i<topLevelItemCount();i++) {
    QTreeWidgetItem * item                ;
    item = topLevelItem(i)                ;
    if (uuid == nTreeUuid(item,0))        {
      return item                         ;
    }                                     ;
  }                                       ;
  return NULL                             ;
}

void N::TorrentLists::Configure(void)
{
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0 , " "               ) ;
  head -> setToolTip           ( 0 , tr("Downloading") ) ;
  head -> setText              ( 1 , " "               ) ;
  head -> setToolTip           ( 1 , tr("Sharing"    ) ) ;
  head -> setText              ( 2 , tr("Filename"   ) ) ;
  head -> setText              ( 3 , tr("Analyzed"   ) ) ;
  head -> setText              ( 4 , tr("Downloaded" ) ) ;
  head -> setText              ( 5 , ""                ) ;
  for (int i=0;i<4;i++)                                  {
    head -> setTextAlignment   ( i , Qt::AlignCenter   ) ;
  }                                                      ;
  setAccessibleName            ( "N::TorrentLists"     ) ;
  setObjectName                ( "N::TorrentLists"     ) ;
  setWindowTitle               ( tr("Torrents")        ) ;
  setDragDropMode              ( DragOnly              ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setSelectionMode             ( ExtendedSelection     ) ;
  setColumnCount               ( 6                     ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setHeaderItem                ( head                  ) ;
  MountClicked                 ( 2                     ) ;
  plan -> setFont              ( this                  ) ;
}

bool N::TorrentLists::FocusIn(void)
{
  nKickOut          ( IsNull(plan) , true       ) ;
  DisableAllActions (                           ) ;
  AssignAction      ( Label   , windowTitle ( ) ) ;
  LinkAction        ( Refresh , startup     ( ) ) ;
  LinkAction        ( Insert  , Insert      ( ) ) ;
  LinkAction        ( Import  , Import      ( ) ) ;
  if ( ! isStandby() )                            {
    LinkAction      ( Delete  , Delete      ( ) ) ;
  }                                               ;
  return true                                     ;
}

bool N::TorrentLists::hasItem(void)
{
  QTreeWidgetItem * item = currentItem () ;
  return NotNull ( item )                 ;
}

bool N::TorrentLists::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * atItem = itemAt(event->pos())            ;
  if (IsNull(atItem)) return false                           ;
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false ;
  dragPoint = event->pos()                                   ;
  if (!atItem->isSelected()) return false                    ;
  if (!PassDragDrop) return true                             ;
  return true                                                ;
}

bool N::TorrentLists::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

QMimeData * N::TorrentLists::dragMime(void)
{
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  ////////////////////////////////////////////////
  for (int i=0;i<topLevelItemCount();i++)        {
    QTreeWidgetItem * it = topLevelItem(i)       ;
    if (it->isSelected())                        {
      Uuids << nTreeUuid(it,0)                   ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  if (Uuids.count()<=0 && uuid == 0) return NULL ;
  if (Uuids.count()==1)                          {
    uuid  = Uuids [0]                            ;
    Uuids . clear ( )                            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (Uuids.count()==0)                          {
    setMime ( mime , "torrent/uuid"  , uuid  )   ;
  } else                                         {
    setMime ( mime , "torrent/uuids" , Uuids )   ;
  }                                              ;
  return mime                                    ;
}

void N::TorrentLists::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::TorrentLists::finishDrag (QMouseEvent * event)
{
  return true ;
}

void N::TorrentLists::doubleClicked(QTreeWidgetItem * item,int)
{
  emit Clicked ( nTreeUuid ( item , 0 ) ) ;
}

void N::TorrentLists::run(int T,ThreadData * data)
{
  VarArgs V = data -> Arguments ;
  switch ( T )                  {
    case 10001                  :
      startLoading (          ) ;
      Refresh      ( data     ) ;
      stopLoading  (          ) ;
    break                       ;
    case 10002                  :
      startLoading (          ) ;
      Import       ( data , V ) ;
      stopLoading  (          ) ;
    break                       ;
    case 10003                  :
      startLoading (          ) ;
      Remove       (          ) ;
      stopLoading  (          ) ;
    break                       ;
    case 10004                  :
      startLoading (          ) ;
      Insert       ( data , V ) ;
      stopLoading  (          ) ;
    break                       ;
  }                             ;
}

UUIDs N::TorrentLists::Uuids(ThreadData * d,SqlConnection & SC)
{
  QString Q                                           ;
  UUIDs   uuids                                       ;
  if (ObjectUuid()>0)                                 {
    UUIDs tuids                                       ;
    tuids = Subordination                             (
      SC                                              ,
      ObjectUuid()                                    ,
      ObjectType()                                    ,
      Types::File                                     ,
      Connexion ( )                                   ,
      SC . OrderByAsc ( "position" )                ) ;
    foreach ( uuid , tuids )                          {
      if ( ! IsContinue ( d ) ) return uuids          ;
      Q = SC . sql . SelectFrom                       (
            "`type`,`extension`"                      ,
            PlanTable(FileManager)                    ,
            SC . WhereUuid ( uuid )                 ) ;
      if (SC.Fetch(Q))                                {
        int ft = SC.Int(0)                            ;
        int et = SC.Int(1)                            ;
        if ( ( ft == Types::File ) && ( et == 724 ) ) {
          uuids << uuid                               ;
        }                                             ;
      }                                               ;
    }                                                 ;
  } else                                              {
    if (Depot<=0)                                     {
      Q = SC . sql . SelectFrom                       (
        "uuid"                                        ,
        PlanTable(FileManager)                        ,
        QString                                       (
        "where `type` = %1 "
        "and `extension` = %2 "
        "order by `id` desc"                          )
        .arg(Types::File                              )
        .arg(724                                      )
      )                                               ;
      SqlLoopNow(SC,Q)                                ;
        uuids << SC.Uuid(0)                           ;
        if ( ! IsContinue ( d ) ) return uuids        ;
      SqlLoopErr(SC,Q)                                ;
      SqlLoopEnd(SC,Q)                                ;
    } else                                            {
      Q = SC . sql . SelectFrom                       (
        "`uuid`,`filename`"                           ,
        PlanTable(FileManager)                        ,
        QString                                       (
        "where `type` = %1 "
        "and `extension` = %2 "
        "and `depot` = %3 "
        "order by `id` desc"                          )
        .arg(Types::File                              )
        .arg(724                                      )
        .arg(Depot                                    )
      )                                               ;
      SqlLoopNow(SC,Q)                                ;
        uuids << SC.Uuid(0)                           ;
        if ( ! IsContinue ( d ) ) return uuids        ;
      SqlLoopErr(SC,Q)                                ;
      SqlLoopEnd(SC,Q)                                ;
    }                                                 ;
  }                                                   ;
  return uuids                                        ;
}

bool N::TorrentLists::List(ThreadData * d,SqlConnection & SC,UUIDs & uuids)
{
  SUID    uuid                                          ;
  QString Q                                             ;
  foreach ( uuid , uuids )                              {
    if ( ! IsContinue ( d ) ) return false              ;
    Q = SC . sql . SelectFrom                           (
          "filename"                                    ,
          PlanTable(FileManager)                        ,
          QString                                       (
          "where `uuid` = %1 "
          "and `type` = %2 "
          "and `extension` = %3 "
          "order by `id` desc"                          )
          . arg ( uuid                                  )
          . arg ( Types::File                           )
          . arg ( 724                                   )
        )                                               ;
    if ( SC . Fetch ( Q ) )                             {
      QString name = SC . String ( 0 )                  ;
      NewTreeWidgetItem     ( item                    ) ;
      item -> setData       ( 0 , Qt::UserRole , uuid ) ;
      item -> setCheckState ( 0 , Qt::Unchecked       ) ;
      item -> setCheckState ( 1 , Qt::Unchecked       ) ;
      item -> setText       ( 2 , name                ) ;
      addTopLevelItem       ( item                    ) ;
    }                                                   ;
  }                                                     ;
  Alert                     ( Done                    ) ;
  return true                                           ;
}

void N::TorrentLists::Refresh(ThreadData * d)
{
  EnterSQL              ( SC , plan -> sql ) ;
    UUIDs uuids = Uuids ( d , SC           ) ;
    List                ( d , SC , uuids   ) ;
  ErrorSQL              ( SC , plan -> sql ) ;
    Alert               ( Error            ) ;
  LeaveSQL              ( SC , plan -> sql ) ;
  emit AutoFit          (                  ) ;
}

bool N::TorrentLists::startup(void)
{
  clear (       ) ;
  start ( 10001 ) ;
  return true     ;
}

void N::TorrentLists::Insert(void)
{
  QStringList   files                                             ;
  QFileInfoList TFL                                               ;
  QString       filename = plan -> Temporary ( "" )               ;
  /////////////////////////////////////////////////////////////////
  if ( plan -> Variables . contains ( "TorrentPath" ) )           {
    filename = plan -> Variables [ "TorrentPath" ] . toString ( ) ;
  }                                                               ;
  files = QFileDialog::getOpenFileNames                           (
            this                                                  ,
            tr("Import torrents")                                 ,
            filename                                              ,
            tr("Torrents (*.torrent)") )                          ;
  foreach ( filename , files )                                    {
    TFL << QFileInfo(filename)                                    ;
  }                                                               ;
  if (TFL.count()<=0) return                                      ;
  /////////////////////////////////////////////////////////////////
  filename = TFL [ 0 ] . absolutePath ( )                         ;
  plan -> Variables [ "TorrentPath" ] = filename                  ;
  /////////////////////////////////////////////////////////////////
  VarArgs args                                                    ;
  for (int i=0;i<TFL.count();i++)                                 {
    args << TFL [ i ] . absoluteFilePath ( )                      ;
  }                                                               ;
  start ( 10004 , args )                                          ;
}

void N::TorrentLists::Insert(ThreadData * d,VarArgs & args)
{
  QFileInfoList TFL                                ;
  UUIDs         uuids                              ;
  for (int i = 0 ; i < args . count ( ) ; i++ )    {
    TFL << QFileInfo ( args [ i ] . toString ( ) ) ;
  }                                                ;
  if ( ! Import ( d , TFL , uuids ) ) return       ;
  if (   uuids . count ( ) <= 0     ) return       ;
  EnterSQL ( SC , plan->sql )                      ;
    Join   ( SC                                    ,
             ObjectUuid ( )                        ,
             ObjectType ( )                        ,
             Types::File                           ,
             Connexion  ( )                        ,
             0                                     ,
             uuids                                 ,
             false                               ) ;
    Alert  ( Done           )                      ;
  ErrorSQL ( SC , plan->sql )                      ;
    Alert  ( Error          )                      ;
  LeaveSQL ( SC , plan->sql )                      ;
  startup  (                )                      ;
}

void N::TorrentLists::Delete(void)
{
  start ( 10003 ) ;
}

void N::TorrentLists::Remove(void)
{
  if ( isStandby ( ) ) return          ;
  UUIDs uuids                          ;
  uuids = selectedUuids ( 0 )          ;
  if ( uuids . count ( ) <= 0 ) return ;
  EnterSQL ( SC , plan->sql )          ;
    Detach ( SC                        ,
             ObjectUuid ( )            ,
             ObjectType ( )            ,
             Types::File               ,
             Connexion  ( )            ,
             uuids                   ) ;
    Alert  ( Done             )        ;
  ErrorSQL ( SC , plan -> sql )        ;
    Alert  ( Error            )        ;
  LeaveSQL ( SC , plan -> sql )        ;
  startup  (                  )        ;
}

void N::TorrentLists::Import(void)
{
  QString filename = plan->Temporary("")                          ;
  if ( plan -> Variables . contains ( "TorrentPath" ) )           {
    filename = plan -> Variables [ "TorrentPath" ] . toString ( ) ;
  }                                                               ;
  filename = QFileDialog::getExistingDirectory                    (
             this                                                 ,
             tr("Import torrent directory")                       ,
             filename                                           ) ;
  if ( filename . length ( ) <= 0 ) return                        ;
  plan -> Variables [ "TorrentPath" ] = filename                  ;
  /////////////////////////////////////////////////////////////////
  QFileInfoList FIL                                               ;
  QFileInfoList TFL                                               ;
  QDir D(filename)                                                ;
  FIL = File::FileInfos(D)                                        ;
  for (int i=0;i<FIL.count();i++)                                 {
    QString suffix = FIL[i].suffix()                              ;
    suffix = suffix.toLower()                                     ;
    if (suffix=="torrent") TFL << FIL[i]                          ;
  }                                                               ;
  if (TFL.count()<=0) return                                      ;
  /////////////////////////////////////////////////////////////////
  VarArgs args                                                    ;
  for (int i=0;i<TFL.count();i++)                                 {
    args << TFL [ i ] . absoluteFilePath ( )                      ;
  }                                                               ;
  start ( 10002 , args )                                          ;
}

void N::TorrentLists::Import(ThreadData * d,VarArgs & args)
{
  QFileInfoList L                                ;
  UUIDs         U                                ;
  for (int i = 0 ; i < args . count ( ) ; i++ )  {
    L << QFileInfo ( args [ i ] . toString ( ) ) ;
  }                                              ;
  if ( Import ( d , L , U ) )                    {
    Alert ( Done )                               ;
  } else                                         {
    Alert ( Error )                              ;
  }                                              ;
}

bool N::TorrentLists::Import(UUIDs & uuids)
{
  QString filename = plan->Temporary("")                          ;
  if ( plan -> Variables . contains ( "TorrentPath" ) )           {
    filename = plan -> Variables [ "TorrentPath" ] . toString ( ) ;
  }                                                               ;
  filename = QFileDialog::getExistingDirectory                    (
             this                                                 ,
             tr("Import torrent directory")                       ,
             filename                                           ) ;
  if ( filename . length ( ) <= 0 ) return false                  ;
  plan -> Variables [ "TorrentPath" ] = filename                  ;
  /////////////////////////////////////////////////////////////////
  QFileInfoList FIL                                               ;
  QFileInfoList TFL                                               ;
  QDir D(filename)                                                ;
  FIL = File::FileInfos(D)                                        ;
  for (int i=0;i<FIL.count();i++)                                 {
    QString suffix = FIL[i].suffix()                              ;
    suffix = suffix.toLower()                                     ;
    if (suffix=="torrent") TFL << FIL[i]                          ;
  }                                                               ;
  if (TFL.count()<=0) return false                                ;
  /////////////////////////////////////////////////////////////////
  VarArgs args                                                    ;
  for (int i=0;i<TFL.count();i++)                                 {
    args << TFL [ i ] . absoluteFilePath ( )                      ;
  }                                                               ;
  start ( 10002 , args )                                          ;
  return true                                                     ;
}

bool N::TorrentLists::Import(ThreadData * d,QFileInfoList & List,UUIDs & uuids)
{
  bool   success = false                                                  ;
  bool   R       = true                                                   ;
  SUID   Tuid    = Types::File                                            ;
  int    Duid    = Types::Video                                           ;
  qint64 V       = 0                                                      ;
  int    TID     = 724                                                    ;
  int    PID = plan->Progress(tr("Torrent"),tr("%v/%m torrents") )        ;
  SqlConnection SC ( plan->sql )                                          ;
  CodeManager   CM ( plan      )                                          ;
  if ( SC . open ( FunctionString ) )                                     {
    QString filename                                                      ;
    QString Q                                                             ;
    plan -> Start    ( PID , &V , &R           )                          ;
    plan -> setRange ( PID , 0  , List.count() )                          ;
    for (int i = 0 ; IsContinue ( d ) && ( i < List . count ( ) ) ; i++ ) {
      V++                                                                 ;
      filename = List[i].absoluteFilePath()                               ;
      plan->ProgressText(PID,filename)                                    ;
      plan->processEvents()                                               ;
      QtTorrent Torrent                                                   ;
      if (Torrent.setFile(filename))                                      {
        SUID uuid = CM.assureFile(SC,filename)                            ;
        Q = SC.sql.SelectFrom                                             (
              "id",PlanTable(FileManager)                                 ,
              QString("where uuid = %1 and type = %2 "
                      "and extension = %3 and depot = %4"                 )
              .arg(uuid).arg(Tuid).arg(TID).arg(Duid)                   ) ;
        if (!SC.Fetch(Q))                                                 {
          QString path = List[i].absolutePath()                           ;
          QString name = List[i].fileName    ()                           ;
          Q = SC.sql.InsertInto                                           (
                PlanTable(FileManager)                                    ,
                6                                                         ,
                "uuid"                                                    ,
                "type"                                                    ,
                "extension"                                               ,
                "depot"                                                   ,
                "path"                                                    ,
                "filename"                                              ) ;
          SC . Prepare ( Q                                              ) ;
          SC . Bind    ( "uuid"      , uuid                             ) ;
          SC . Bind    ( "type"      , Tuid                             ) ;
          SC . Bind    ( "extension" , TID                              ) ;
          SC . Bind    ( "depot"     , Duid                             ) ;
          SC . Bind    ( "path"      , path.toUtf8()                    ) ;
          SC . Bind    ( "filename"  , name.toUtf8()                    ) ;
          SC . Exec    (                                                ) ;
        }                                                                 ;
        if ( ! uuids . contains ( uuid ) ) uuids << uuid                  ;
      }                                                                   ;
    }                                                                     ;
    plan -> Finish ( PID )                                                ;
    success = true                                                        ;
    SC . close ( )                                                        ;
  }                                                                       ;
  SC . remove ( )                                                         ;
  return success                                                          ;
}

void N::TorrentLists::Export(QTreeWidgetItem * item)
{
  QString Q                                      ;
  QString filename                               ;
  SUID    uuid     = nTreeUuid ( item , 0 )      ;
  ////////////////////////////////////////////////
  SqlConnection OC ( plan -> sql )               ;
  if (OC.open("nTorrentLists","Export"))         {
    Q = OC.sql.SelectFrom                        (
          "path,filename"                        ,
          PlanTable(FileManager)                 ,
          QString                                (
            "where `uuid` = %1 "
            "and `type` = %2 "
            "and `extension` = %3"               )
            . arg ( uuid                         )
            . arg ( Types::File                  )
            . arg ( 724 )                      ) ;
    if (OC.Fetch(Q))                             {
      QString path = OC.String(0)                ;
      QString name = OC.String(1)                ;
      QDir    d(path)                            ;
      if (d.exists())                            {
        filename = d.absoluteFilePath (name)     ;
      } else                                     {
        filename = plan->Temporary    (name)     ;
      }                                          ;
    }                                            ;
    OC.close()                                   ;
  }                                              ;
  OC.remove()                                    ;
  ////////////////////////////////////////////////
  filename = QFileDialog::getSaveFileName        (
               this                              ,
               tr("Export torrent into file")    ,
               filename                          ,
               tr("Torrent (*.torrent)")       ) ;
  if (filename.length()<=0) return               ;
  ////////////////////////////////////////////////
  QByteArray Body                                ;
  SqlConnection SC ( plan -> sql )               ;
  CodeManager   CM ( plan        )               ;
  if (SC.open("nTorrentLists","Filename"))       {
    CM . LoadFile ( SC , uuid , Body )           ;
    SC . close    (                  )           ;
  }                                              ;
  SC.remove()                                    ;
  ////////////////////////////////////////////////
  if (Body.size()>0)                             {
    File::toFile(filename,Body)                  ;
    Alert ( Done  )                              ;
  } else                                         {
    Alert ( Error )                              ;
  }                                              ;
}

bool N::TorrentLists::Menu(QPoint pos)
{
  nScopedMenu ( mm , this )                 ;
  QAction         * aa                      ;
  QTreeWidgetItem * it = itemAt(pos)        ;
  mm.add(101,tr("Refresh"))                 ;
  mm.add(102,tr("Insert" ))                 ;
  if ( NotNull(it) && ! isStandby ( ) )     {
    mm . add ( 103 , tr("Delete") )         ;
  }                                         ;
  mm.add(201,tr("Import" ))                 ;
  if (NotNull(it)) mm.add(202,tr("Export")) ;
  mm . setFont ( plan )                     ;
  aa = mm . exec ( )                        ;
  if (IsNull(aa)) return true               ;
  switch (mm[aa])                           {
    case 101                                :
      startup (    )                        ;
    break                                   ;
    case 102                                :
      Insert  (    )                        ;
    break                                   ;
    case 103                                :
      Delete  (    )                        ;
    break                                   ;
    case 201                                :
      Import  (    )                        ;
    break                                   ;
    case 202                                :
      Export  ( it )                        ;
    break                                   ;
  }                                         ;
  return true                               ;
}
