#include <netwidgets.h>

N::BookmarkTree:: BookmarkTree   (QWidget * parent,Plan * p)
                : TreeWidget     (          parent,       p)
                , Object         ( 0 , Types::URL          )
                , NetworkManager (                        p)
                , GroupItems     (                        p)
{
  WidgetClass   ;
  Configure ( ) ;
}

N::BookmarkTree::~BookmarkTree(void)
{
}

bool N::BookmarkTree::FocusIn(void)
{
  nKickOut          ( IsNull ( plan ) , true            ) ;
  DisableAllActions (                                   ) ;
  AssignAction      ( Label           , windowTitle ( ) ) ;
  LinkAction        ( Insert          , Insert      ( ) ) ;
  LinkAction        ( Delete          , Delete      ( ) ) ;
  LinkAction        ( Copy            , Copy        ( ) ) ;
  LinkAction        ( Paste           , Paste       ( ) ) ;
  return true                                             ;
}

void N::BookmarkTree::Configure(void)
{
  PassDragDrop = false                               ;
  StackSize    = 0                                   ;
  Reservation  = false                               ;
  AutoMap      = true                                ;
  ////////////////////////////////////////////////////
  setAccessibleName       ( "N::BookmarkTree"      ) ;
  setObjectName           ( "N::BookmarkTree"      ) ;
  setFocusPolicy          ( Qt::WheelFocus         ) ;
  setDragDropMode         ( DragDrop               ) ;
  setAlternatingRowColors ( true                   ) ;
  setRootIsDecorated      ( true                   ) ;
  setSelectionMode        ( SingleSelection        ) ;
  setColumnCount          ( 3                      ) ;
  ////////////////////////////////////////////////////
  NewTreeWidgetItem       ( header                 ) ;
  header -> setText       ( 0,tr("Name"   )        ) ;
  header -> setText       ( 1,tr("Content")        ) ;
  header -> setText       ( 2,tr("Update" )        ) ;
  setAllAlignments        ( header,Qt::AlignCenter ) ;
  setHeaderItem           ( header                 ) ;
  plan   -> setFont       ( this                   ) ;
  setWindowTitle          ( tr("Bookmarks")        ) ;
  setWindowIcon ( QIcon(":/images/bookmarks.png")  ) ;
  ////////////////////////////////////////////////////
  setDropFlag             ( DropText     , true    ) ;
  setDropFlag             ( DropUrls     , true    ) ;
  setDropFlag             ( DropImage    , true    ) ;
  setDropFlag             ( DropHtml     , true    ) ;
  setDropFlag             ( DropURIs     , true    ) ;
  setDropFlag             ( DropBookmark , true    ) ;
  ////////////////////////////////////////////////////
  MountClicked            ( 2                      ) ;
}

bool N::BookmarkTree::hasItem(void)
{
  return holdItem ( ) ;
}

bool N::BookmarkTree::startDrag(QMouseEvent * event)
{
  QTreeWidgetItem * item = currentItem ()        ;
  UUIDs Uuids                                    ;
  nKickOut ( IsNull(item) , false )              ;
  int mType = item->data(1,Qt::UserRole).toInt() ;
  if (mType==0)                                  {
    Uuids = UrlSelected ( )                      ;
    nKickOut ( Uuids.count() <= 0 , false )      ;
  }                                              ;
  dragPoint = event->pos()                       ;
  nKickOut ( !PassDragDrop , true )              ;
  return true                                    ;
}

bool N::BookmarkTree::fetchDrag(QMouseEvent * event)
{
  if (!IsMask(event->buttons(),Qt::LeftButton)) return false   ;
  QPoint pos = event->pos()                                    ;
  pos -= dragPoint                                             ;
  return ( pos.manhattanLength() > qApp->startDragDistance() ) ;
}

void N::BookmarkTree::dragDone(Qt::DropAction dropIt,QMimeData * mime)
{
}

bool N::BookmarkTree::finishDrag(QMouseEvent * event)
{
  return true ;
}

QMimeData * N::BookmarkTree::dragMime (void)
{
  QTreeWidgetItem * item = currentItem ()        ;
  UUIDs Uuids                                    ;
  SUID  uuid  = 0                                ;
  if (IsNull(item)) return NULL                  ;
  int mType = item->data(1,Qt::UserRole).toInt() ;
  if (mType==0)                                  {
    Uuids = UrlSelected ( )                      ;
    if (Uuids.count()<=0) return NULL            ;
  }                                              ;
  ////////////////////////////////////////////////
  QMimeData * mime = new QMimeData()             ;
  if (mType==1)                                  {
    uuid = nTreeUuid ( item , 0 )                ;
    setMime ( mime , "bookmark/uuid" , uuid  )   ;
  } else                                         {
    setMime ( mime , "uri/uuids" , Uuids )       ;
  }                                              ;
  return mime                                    ;
}

bool N::BookmarkTree::acceptDrop(QWidget * source,const QMimeData * mime)
{
  return dropHandler ( mime ) ;
}

QString N::BookmarkTree::MimeType(const QMimeData * mime)
{
  return AbstractGui :: MimeType        (
           mime                         ,
           "uri/uuid;uri/uuids;"
           "bookmark/uuid;bookmark/uuids"
         )                              ;
}

UUIDs N::BookmarkTree::MimeUuids(const QMimeData * mime,QString mimetype)
{
  UUIDs      Uuids                       ;
  QByteArray data = mime->data(mimetype) ;
  if (data.size()<=0) return Uuids       ;
  if (mimetype=="uri/uuid"      )        {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="uri/uuids"     )        {
    Uuids  = GetUuids ( data )           ;
  } else
  if (mimetype=="bookmark/uuid" )        {
    Uuids << GetUuid(data)               ;
  } else
  if (mimetype=="bookmark/uuids")        {
    Uuids  = GetUuids ( data )           ;
  }                                      ;
  return Uuids                           ;
}

QTreeWidgetItem * N::BookmarkTree::AppendItem (void)
{
  NewTreeWidgetItem (IT)        ;
  IT->setData(0,Qt::UserRole,0) ;
  addTopLevelItem   (IT)        ;
  return IT                     ;
}

void N::BookmarkTree::Insert(void)
{
  removeOldItem     (  )                   ;
  QTreeWidgetItem * IT = AppendItem ( )    ;
  scrollToItem(IT)                         ;
  QLineEdit * name = new QLineEdit(this)   ;
  ItemEditing = IT                         ;
  ItemColumn  = 0                          ;
  ItemWidget  = (QWidget *)name            ;
  IT->setData(0,Qt::UserRole  ,0         ) ;
  setItemWidget(IT,0,name)                 ;
  nConnect(name,SIGNAL(editingFinished())  ,
           this,SLOT  (editingFinished())) ;
  name->setFocus(Qt::TabFocusReason)       ;
}

void N::BookmarkTree::Delete(void)
{
  QTreeWidgetItem * item = currentItem() ;
  if (IsNull(item)) return               ;
  Delete ( item )                        ;
}

void N::BookmarkTree::doubleClicked(QTreeWidgetItem * item,int column)
{
  int mType = item->data(1,Qt::UserRole  ).toInt() ;
  int mStat = item->data(1,Qt::UserRole+1).toInt() ;
  if (mType==0)                                    {
    QUrl U ( item->text(1) )                       ;
    emit UrlClicked ( U , true )                   ;
  } else
  if (mType==1)                                    {
    if (mStat==0) Reload ( item )                  ;
  }                                                ;
}

void N::BookmarkTree::Rename(QTreeWidgetItem * item)
{
  QLineEdit * name = new QLineEdit(this)   ;
  ItemEditing = item                       ;
  ItemColumn  = 0                          ;
  ItemWidget  = (QWidget *)name            ;
  name -> setText (item->text(0))          ;
  setItemWidget(item,0,name)               ;
  nConnect(name,SIGNAL(editingFinished())  ,
           this,SLOT  (editingFinished())) ;
  name->setFocus(Qt::TabFocusReason)       ;
}

void N::BookmarkTree::removeOldItem(void)
{
  if (IsNull(ItemEditing)) return                ;
  SUID uuid = nTreeUuid(ItemEditing,ItemColumn)  ;
  removeItemWidget (ItemEditing,ItemColumn)      ;
  if (uuid<=0) takeItem(ItemEditing)             ;
  ItemEditing = NULL                             ;
  ItemWidget  = NULL                             ;
  ItemColumn  = -1                               ;
}

void N::BookmarkTree::editingFinished(void)
{
  SUID u = nTreeUuid(ItemEditing,ItemColumn)               ;
  QLineEdit * name = Casting(QLineEdit,ItemWidget)         ;
  if (NotNull(name))                                       {
    QString book = name->text()                            ;
    if (book.length()>0)                                   {
      Bustle ( )                                           ;
      Sql sql = plan->sql                                  ;
      SqlConnection SC(sql)                                ;
      if (SC.open("nBookmarkTree","editingFinished"))      {
        if (u<=0)                                          {
          u = NewBookmark ( SC , book )                    ;
          if (uuid>0)                                      {
            ItemEditing -> setData ( 0,Qt::UserRole  ,u  ) ;
            ItemEditing -> setData ( 1,Qt::UserRole  ,1  ) ;
            ItemEditing -> setData ( 1,Qt::UserRole+1,0  ) ;
            ItemEditing -> setText ( 0,book              ) ;
            if (uuid>0)                                    {
              GroupItems GI ( plan )                       ;
              UUIDs Buids                                  ;
              Buids << u                                   ;
              GI . AutoMap = true                          ;
              GI . GroupTable = GI.LookTable               (
                type                                       ,
                Types::Division                            ,
                Groups::Subordination                    ) ;
              GI . Join                                    (
                SC                                         ,
                uuid                                       ,
                type                                       ,
                Types::Division                            ,
                Groups::Subordination                      ,
                0                                          ,
                Buids                                    ) ;
            }                                              ;
          }                                                ;
        } else                                             {
          assureName(SC,u,vLanguageId,book)                ;
          ItemEditing -> setText ( 0,book                ) ;
        }                                                  ;
        SC . close ( )                                     ;
      }                                                    ;
      SC . remove  ( )                                     ;
      Vacancy      ( )                                     ;
    }                                                      ;
  }                                                        ;
  removeOldItem ( )                                        ;
}

bool N::BookmarkTree::dropUrls    (
       QWidget *           source ,
       QPointF             psf    ,
       const QList<QUrl> & urls   )
{ Q_UNUSED ( source )                 ;
  QPoint pos = psf.toPoint()          ;
  QTreeWidgetItem * it = itemAt(pos)  ;
  if (IsNull(it)) return true         ;
  if (NotNull(it->parent()))          {
    it = it->parent()                 ;
  }                                   ;
  int mType                           ;
  mType = nTreeInt ( it , 1 )         ;
  nKickOut ( mType != 1 , true )      ;
  SUID uu = nTreeUuid ( it , 0 )      ;
  nKickOut ( uu <= 0  , true )        ;
  EnterSQL ( SC , plan->sql )         ;
    UUIDs Puids                       ;
    for (int i=0;i<urls.count();i++)  {
      QUrl u  = urls[i]               ;
      SUID ux = PageUuid(SC,u)        ;
      if ( ux > 0)                    {
        if (!Puids.contains(ux))      {
          Puids << ux                 ;
        }                             ;
      }                               ;
    }                                 ;
    GroupTable = LookTable            (
      Types::Division                 ,
      Types::URL                      ,
      Groups::Subordination         ) ;
    GroupItems::Join                  (
      SC                              ,
      uu                              ,
      Types::Division                 ,
      Types::URL                      ,
      Groups::Subordination           ,
      0                               ,
      Puids                         ) ;
  LeaveSQL ( SC , plan->sql )         ;
  Alert    ( Done           )         ;
  startup  (                )         ;
  return true                         ;
}

bool N::BookmarkTree::dropURIs (
       QWidget *      source   ,
       QPointF        psf      ,
       const UUIDs &  Uuids    )
{ Q_UNUSED ( source )                 ;
  QPoint pos = psf.toPoint()          ;
  QTreeWidgetItem * it = itemAt(pos)  ;
  if (IsNull(it)) return true         ;
  if (NotNull(it->parent()))          {
    it = it->parent()                 ;
  }                                   ;
  int mType                           ;
  mType = nTreeInt ( it , 1 )         ;
  nKickOut ( mType != 1 , true )      ;
  SUID uu = nTreeUuid ( it , 0 )      ;
  nKickOut ( uu <= 0  , true )        ;
  EnterSQL ( SC , plan->sql )         ;
    UUIDs Puids = Uuids               ;
    GroupTable = LookTable            (
      Types::Division                 ,
      Types::URL                      ,
      Groups::Subordination         ) ;
    GroupItems::Join                  (
      SC                              ,
      uu                              ,
      Types::Division                 ,
      Types::URL                      ,
      Groups::Subordination           ,
      0                               ,
      Puids                         ) ;
  LeaveSQL ( SC , plan->sql )         ;
  Alert    ( Done           )         ;
  startup  (                )         ;
  return true                         ;
}

bool N::BookmarkTree::dropBookmarks (
       QWidget     *  source        ,
       QPointF        psf           ,
       const UUIDs &  Uuids         )
{
  if (source==this) return true            ;
  QPoint pos = psf.toPoint()               ;
  QTreeWidgetItem * it = itemAt(pos)       ;
  if (IsNull(it)) return true              ;
  if (NotNull(it->parent()))               {
    it = it->parent()                      ;
  }                                        ;
  int mType                                ;
  mType = nTreeInt ( it , 1 )              ;
  nKickOut ( mType != 1 , true )           ;
  SUID uu = nTreeUuid ( it , 0 )           ;
  nKickOut ( uu <= 0  , true )             ;
  EnterSQL ( SC , plan->sql )              ;
    UUIDs Puids = Uuids                    ;
    GroupTable = LookTable                 (
                   Types::Division         ,
                   Types::URL              ,
                   Groups::Subordination ) ;
    GroupItems::Join                       (
      SC                                   ,
      uu                                   ,
      Types::Division                      ,
      Types::URL                           ,
      Groups::Subordination                ,
      0                                    ,
      Puids                              ) ;
  LeaveSQL ( SC , plan->sql )              ;
  Alert    ( Done           )              ;
  startup  (                )              ;
  return true                              ;
}

UUIDs N::BookmarkTree::UrlSelected(void)
{
  UUIDs Uuids                                      ;
  for (int i=0;i<topLevelItemCount();i++)          {
    QTreeWidgetItem * item = topLevelItem(i)       ;
    int mType = item->data(1,Qt::UserRole).toInt() ;
    switch (mType)                                 {
      case 0                                       :
        if (item->checkState(0)==Qt::Checked)      {
          Uuids << nTreeUuid(item,0)               ;
        }                                          ;
      break                                        ;
      case 1                                       :
        for (int j=0;j<item->childCount();j++)     {
          QTreeWidgetItem * bItem                  ;
          bItem = item -> child ( j )              ;
          if (bItem->checkState(0)==Qt::Checked)   {
            Uuids << nTreeUuid(bItem,0)            ;
          }                                        ;
        }                                          ;
      break                                        ;
    }                                              ;
  }                                                ;
  return Uuids                                     ;
}

QTreeWidgetItem * N::BookmarkTree:: UrlItem    (
                    SqlConnection & Connection ,
                    SUID            uu         )
{
  QString Q                                               ;
  QString Title = ""                                      ;
  QUrl    Url                                             ;
  TUID    eTime = 0                                       ;
  Url   = PageUrl   ( Connection , uu     )               ;
  Title = PageTitle ( Connection , uu , 0 )               ;
  Q = Connection.sql.SelectFrom                           (
        "etime"                                           ,
        PlanTable(WebPages)                               ,
        QString("where uuid = %1").arg(uu)   )            ;
  if (Connection.Fetch(Q))                                {
    eTime = Connection . Tuid ( 0 )                       ;
  }                                                       ;
  StarDate T                                              ;
  T.Stardate = eTime                                      ;
  if (eTime>0) T . locate ( )                             ;
  NewTreeWidgetItem ( IT )                                ;
  IT    -> setCheckState( 0,Qt::Unchecked               ) ;
  IT    -> setData (0,Qt::UserRole  ,uu                 ) ;
  IT    -> setData (1,Qt::UserRole  ,0                  ) ;
  if (Title.length()<=0)                                  {
    IT  -> setText (0,Url.toString()                    ) ;
    IT  -> setData (1,Qt::UserRole+1,0                  ) ;
  } else                                                  {
    IT  -> setText (0,Title                             ) ;
    IT  -> setIcon (0,QIcon(":/images/bookmarks.png")   ) ;
    IT  -> setData (1,Qt::UserRole+1,1                  ) ;
  }                                                       ;
  IT    -> setText (1,Url.toString()                    ) ;
  if (eTime>0)                                            {
    QDateTime LT = T.toLocalTime()                        ;
    IT  -> setText (2,LT.toString("yyyy/MM/dd hh:mm:ss")) ;
  }                                                       ;
  return IT                                               ;
}

void N::BookmarkTree::run(int T,ThreadData * data)
{
  VarArgs V = data -> Arguments ;
  switch ( T )                  {
    case 10001                  :
      startLoading (          ) ;
      Reload       (     data ) ;
      stopLoading  (          ) ;
    break                       ;
    case 10002                  :
      startLoading (          ) ;
      ImportPages  ( V , data ) ;
      stopLoading  (          ) ;
    break                       ;
  }                             ;
}

void N::BookmarkTree::Reload(ThreadData * d)
{
  emit assignToolTip        ( tr("Loading bookmarks...") )     ;
  emit assignSortingEnabled ( false                      )     ;
  emit assignEnabling       ( false                      )     ;
  int divisions = 0                                            ;
  int bookmarks = 0                                            ;
  EnterSQL          ( SC , plan->sql             )             ;
    UUIDs   Uuids                                              ;
    SUID    uu                                                 ;
    QString Q                                                  ;
    ////////////////////////////////////////////////////////////
    LoadDomainIndex ( SC )                                     ;
    if (uuid<=0)                                               {
      Uuids = SC.Uuids                                         (
                PlanTable(Divisions)                           ,
                "uuid"                                         ,
                QString("where type = %1 order by position asc")
                .arg(Types::URL)                             ) ;
    } else                                                     {
      GroupTable = LookTable                                   (
        type                                                   ,
        Types::Division                                        ,
        Groups::Subordination                                ) ;
      Uuids = Subordination                                    (
                SC                                             ,
                uuid                                           ,
                type                                           ,
                Types::Division                                ,
                Groups::Subordination                          ,
                SC.OrderByAsc("position")                    ) ;
    }                                                          ;
    divisions = Uuids.count()                                  ;
    ////////////////////////////////////////////////////////////
    foreach (uu,Uuids)                                         {
      int uType = 0                                            ;
      Q = SC.sql.SelectFrom                                    (
            "type"                                             ,
            PlanTable(Divisions)                               ,
            SC.WhereUuid(uu)                                 ) ;
      if (SC.Fetch(Q)) uType = SC . Int ( 0 )                  ;
      if (uType==Types::URL)                                   {
        int total = 0                                          ;
        GroupTable = LookTable                                 (
          Types::Division                                      ,
          Types::URL                                           ,
          Groups::Subordination                              ) ;
        total = Count                                          (
                  SC                                           ,
                  uu                                           ,
                  Types::Division                              ,
                  Types::URL                                   ,
                  Groups::Subordination                      ) ;
        ////////////////////////////////////////////////////////
        QTreeWidgetItem * item = AppendItem ( )                ;
        QString name = SC.getName                              (
                         PlanTable(Names)                      ,
                         "uuid"                                ,
                         vLanguageId                           ,
                         uu                                  ) ;
        item -> setData (0,Qt::UserRole  ,uu     )             ;
        item -> setData (1,Qt::UserRole  ,1      )             ;
        item -> setData (1,Qt::UserRole+1,0      )             ;
        item -> setText (0,name                  )             ;
        item -> setText (1,QString::number(total))             ;
      }                                                        ;
    }                                                          ;
    ////////////////////////////////////////////////////////////
    GroupTable = LookTable                                     (
      type                                                     ,
      Types::URL                                               ,
      Groups::Subordination                                  ) ;
    Uuids = Subordination                                      (
              SC                                               ,
              uuid                                             ,
              type                                             ,
              Types::URL                                       ,
              Groups::Subordination                            ,
              SC.OrderByAsc("position")                      ) ;
    bookmarks += Uuids.count()                                 ;
    foreach (uu,Uuids)                                         {
      QTreeWidgetItem * IT = UrlItem(SC,uu)                    ;
      addTopLevelItem (IT)                                     ;
    }                                                          ;
  LeaveSQL               ( SC , plan->sql )                    ;
  if (divisions>0 || bookmarks>0)                              {
    QString d = tr("%1 bookmark folders").arg(divisions)       ;
    QString b = tr("%1 bookmarks").arg(bookmarks)              ;
    QString s                                                  ;
    if (divisions>0) s = d                                     ;
    if (bookmarks>0)                                           {
      if (divisions>0) s = tr("%1, %2").arg(d).arg(b)          ;
                  else s = b                                   ;
    }                                                          ;
    emit assignToolTip      ( s                 )              ;
  } else                                                       {
    emit assignToolTip      ( tr("No bookmark") )              ;
  }                                                            ;
  Alert                     ( Done              )              ;
  emit assignEnabling       ( true              )              ;
  emit assignSortingEnabled ( true              )              ;
  emit AutoFit              (                   )              ;
}

bool N::BookmarkTree::startup(void)
{
  emit clearItems (       ) ;
  start           ( 10001 ) ;
  return true               ;
}

bool N::BookmarkTree::Join(QUrl & url)
{
  QString host = url . host ( ) . toLower ( )      ;
  QString path = URI ( url )                       ;
  QString page                                     ;
  if (path.length()<=0) path = "/"                 ;
  page = host + path                               ;
  if (page . length ( ) <= 0) return false         ;
  Bustle            (                            ) ;
  SqlConnection SC  ( plan -> sql                ) ;
  if ( SC . open ( FunctionString ) )              {
    SUID    uu    = assurePage(SC,url)             ;
    QString title = ""                             ;
    if (uu>0) title = PageTitle(SC,uu,0)           ;
    if (uu>0)                                      {
      QTreeWidgetItem * pItem = currentItem ( )    ;
      if ( NotNull ( pItem ) )                     {
        if ( NotNull ( pItem -> parent ( ) ) )     {
          pItem = pItem -> parent ( )              ;
        }                                          ;
      }                                            ;
      int  Mode = 0                                ;
      SUID Buid = 0                                ;
      if ( IsNull ( pItem ) )                      {
        if (uuid>0) Mode = 1                       ;
               else Mode = 2                       ;
      } else Buid = nTreeUuid(pItem,0)             ;
      UUIDs Buids                                  ;
      Buids << uu                                  ;
      if (Mode==0)                                 {
        QTreeWidgetItem * IT = UrlItem(SC,uu)      ;
        pItem -> addChild (IT)                     ;
        GroupTable = LookTable                     (
                       Types::Division             ,
                       Types::URL                  ,
                       Groups::Subordination     ) ;
        GroupItems::Join                           (
          SC                                       ,
          Buid                                     ,
          Types::Division                          ,
          Types::URL                               ,
          Groups::Subordination                    ,
          0,Buids                                ) ;
        int cc = pItem -> childCount (           ) ;
        if (cc>pItem->text(1).toInt())             {
          pItem->setText( 1,QString::number(cc) )  ;
        }                                          ;
      } else
      if (Mode==1)                                 {
        QTreeWidgetItem * IT = UrlItem(SC,uu)      ;
        addTopLevelItem (IT)                       ;
        GroupTable = LookTable                     (
                       ObjectType ( )              ,
                       Types::URL                  ,
                       Groups::Subordination     ) ;
        GroupItems::Join                           (
          SC                                       ,
          ObjectUuid()                             ,
          ObjectType()                             ,
          Types ::URL                              ,
          Groups::Subordination                    ,
          0                                        ,
          Buids                                  ) ;
      }                                            ;
    }                                              ;
    SC . close      (                         )    ;
  }                                                ;
  SC   . remove     (                         )    ;
  Vacancy           (                         )    ;
  Alert             ( Done                    )    ;
  return true                                      ;
}

void N::BookmarkTree::Reload(QTreeWidgetItem * item)
{
  SUID vuid  = nTreeUuid (item,0          )         ;
  int  mType = item->data(1,Qt::UserRole  ).toInt() ;
  if (mType!=1) return                              ;
  UUIDs Buids                                       ;
  for (int i=0;i<item->childCount();i++)            {
    QTreeWidgetItem * bItem = item->child(i)        ;
    SUID buid = nTreeUuid ( bItem , 0 )             ;
    Buids << buid                                   ;
  }                                                 ;
  Bustle            (                             ) ;
  SqlConnection SC  ( plan -> sql                 ) ;
  if ( SC . open ( FunctionString ) )               {
    GroupTable = LookTable                          (
                   Types::Division                  ,
                   Types::URL                       ,
                   Groups::Subordination          ) ;
    SUID  uu                                        ;
    UUIDs Uuids = Subordination                     (
                   SC                               ,
                   vuid                             ,
                   Types::Division                  ,
                   Types::URL                       ,
                   Groups::Subordination            ,
                   SC.OrderByAsc("position")      ) ;
    UUIDs Muids                                     ;
    foreach (uu,Uuids)                              {
      if (!Buids.contains(uu)) Muids << uu          ;
    }                                               ;
    foreach (uu,Muids)                              {
      QTreeWidgetItem * bItem = UrlItem  ( SC,uu )  ;
      if (NotNull(bItem)) item->addChild ( bItem )  ;
    }                                               ;
    item -> setData ( 1 , Qt::UserRole+1 ,1   )     ;
    SC . close      (                         )     ;
  }                                                 ;
  SC   . remove     (                         )     ;
  Vacancy           (                         )     ;
  int cc = item -> childCount (               )     ;
  item -> setText   ( 1,QString::number(cc)   )     ;
  Alert             ( Done                    )     ;
}

void N::BookmarkTree::Copy(void)
{
  QTreeWidgetItem * item = currentItem ( ) ;
  UUIDs Uuids                              ;
  nDropOut ( IsNull ( item ) )             ;
  int mType = nTreeInt ( item , 1 )        ;
  if (mType==0)                            {
    Uuids = UrlSelected ( )                ;
    nDropOut ( Uuids.count() <= 0 )        ;
  } else return                            ;
  //////////////////////////////////////////
  QMimeData * mime = new QMimeData()       ;
  setMime ( mime , "uri/uuids" , Uuids )   ;
  nSetClipboardMime ( mime )               ;
}

void N::BookmarkTree::Paste(void)
{
  QTreeWidgetItem * item = currentItem ( )      ;
  QMimeData       * mime = nClipboardMime       ;
  nDropOut ( IsNull(item) )                     ;
  nDropOut ( IsNull(mime) )                     ;
  int   mType = nTreeInt( item , 1 )            ;
  UUIDs Uuids                                   ;
  Uuids = MimeUuids (mime,"uri/uuids")          ;
  if (Uuids.count()<=0)                         {
    Uuids = MimeUuids (mime,"uri/uuid")         ;
  }                                             ;
  if (Uuids.count()<=0) return                  ;
  if (mType==0)                                 {
    nDropOut ( IsNull( item->parent() ) )       ;
    item = item -> parent ( )                   ;
  }                                             ;
  nDropOut ( IsNull(item) )                     ;
  ///////////////////////////////////////////////
  SUID uu = nTreeUuid ( item , 0 )              ;
  Bustle            (                         ) ;
  SqlConnection SC  ( plan -> sql             ) ;
  if ( SC . open ( FunctionString ) )           {
    GroupTable = LookTable                      (
                   Types::Division              ,
                   Types::URL                   ,
                   Groups::Subordination      ) ;
    GroupItems::Join                            (
          SC                                    ,
          uu                                    ,
          Types::Division                       ,
          Types::URL                            ,
          Groups::Subordination                 ,
          0                                     ,
          Uuids                               ) ;
    SC . close      (                         ) ;
  }                                             ;
  SC   . remove     (                         ) ;
  Vacancy           (                         ) ;
  Reload            ( item                    ) ;
}

void N::BookmarkTree::Delete(QTreeWidgetItem * item)
{
  int mType = item->data(1,Qt::UserRole).toInt() ;
  if (mType!=0) return                           ;
  QTreeWidgetItem * group = item->parent()       ;
  int  Mode  = 0                                 ;
  int  gType = Types::Division                   ;
  SUID uu    = 0                                 ;
  SUID guid  = 0                                 ;
  if (IsNull(group))                             {
    if (uuid>0)                                  {
      Mode  = 1                                  ;
      guid  = uuid                               ;
      gType = type                               ;
    } else Mode = 2                              ;
  } else guid = nTreeUuid ( group , 0 )          ;
  if (Mode  >  1) return                         ;
  if (gType <= 0) return                         ;
  uu = nTreeUuid ( item , 0 )                    ;
  Bustle            (                         )  ;
  SqlConnection SC  ( plan -> sql             )  ;
  if ( SC . open ( FunctionString ) )            {
    UUIDs Uuids                                  ;
    Uuids << uu                                  ;
    GroupTable = LookTable                       (
                   gType                         ,
                   Types::URL                    ,
                   Groups::Subordination       ) ;
    GroupItems::Detach                           (
      SC                                         ,
      guid                                       ,
      gType                                      ,
      Types::URL                                 ,
      Groups::Subordination                      ,
      Uuids                                   )  ;
    SC . close      (                         )  ;
  }                                              ;
  SC   . remove     (                         )  ;
  Vacancy           (                         )  ;
  if (NotNull(group))                            {
    group -> removeChild ( item )                ;
    int cc = group -> childCount (            )  ;
    group -> setText( 1,QString::number(cc)   )  ;
  } else                                         {
    takeTopLevelItem(indexOfTopLevelItem(item))  ;
  }                                              ;
}

void N::BookmarkTree::ImportSites(QTreeWidgetItem * item)
{
  QString filename                                      ;
  filename = QFileDialog::getOpenFileName               (
               this                                     ,
               tr("Import sites from file")             ,
               plan->Temporary("")                      ,
               "*.txt\n*.*"                 )           ;
  nDropOut ( filename.length() <= 0 )                   ;
  ///////////////////////////////////////////////////////
  QStringList L                                         ;
  L = File::StringList(filename,"\n")                   ;
  nDropOut ( L.count() <= 0 )                           ;
  L = File::PurifyLines(L)                              ;
  ///////////////////////////////////////////////////////
  QList<QUrl> URLs                                      ;
  nDropOut ( ! NetworkManager :: toURLs ( L , URLs ) )  ;
  ///////////////////////////////////////////////////////
  QTreeWidgetItem * mark = item                         ;
  if (NotNull(mark->parent())) mark = mark->parent()    ;
  ///////////////////////////////////////////////////////
  VarArgLists Operations                                ;
  SUID DUID = nTreeUuid(mark,0)                         ;
  if (DUID<=0) return                                   ;
  for (int i=0;i<URLs.count();i++)                      {
    QUrl u = URLs[i]                                    ;
    if (u.isValid())                                    {
      VarArgs V                                         ;
      V  . clear ( )                                    ;
      V << "SiteTo"                                     ;
      V << u.host()                                     ;
      V << DUID                                         ;
      V << Types::Division                              ;
      V << Groups::Subordination                        ;
      Operations << V                                   ;
    }                                                   ;
  }                                                     ;
  emit RunSets ( Operations )                           ;
}

void N::BookmarkTree::ImportPages(QTreeWidgetItem * item)
{
  QString filename                                      ;
  filename = QFileDialog::getOpenFileName               (
               this                                     ,
               tr("Import pages from file")             ,
               plan->Temporary("")                      ,
               "*.txt\n*.*"                 )           ;
  nDropOut ( filename.length() <= 0 )                   ;
  ///////////////////////////////////////////////////////
  QTreeWidgetItem * mark = item                         ;
  if (NotNull(mark->parent())) mark = mark->parent()    ;
  SUID DUID = nTreeUuid ( mark , 0 )                    ;
  ///////////////////////////////////////////////////////
  VarArgs V                                             ;
  V << filename                                         ;
  V << DUID                                             ;
  start ( 10002 , V )                                   ;
}

void N::BookmarkTree::ImportPages(VarArgs & args,ThreadData * d)
{
  QString filename                                      ;
  SUID    DUID = 0                                      ;
  if ( args . count ( ) <= 0 ) return                   ;
  filename = args [ 0 ] . toString ( )                  ;
  if ( args . count ( ) >  1 )                          {
    DUID = args [ 1 ] . toULongLong ( )                 ;
  }                                                     ;
  ///////////////////////////////////////////////////////
  QFileInfo FXI ( filename )                            ;
  if ( ! FXI . exists ( ) ) return                      ;
  ///////////////////////////////////////////////////////
  QStringList L                                         ;
  L = File::StringList(filename,"\n")                   ;
  nDropOut ( L.count() <= 0 )                           ;
  L = File::PurifyLines(L)                              ;
  ///////////////////////////////////////////////////////
  QList<QUrl> URLs                                      ;
  nDropOut ( ! NetworkManager :: toURLs ( L , URLs ) )  ;
  ///////////////////////////////////////////////////////
  if ( DUID <= 0 ) return                               ;
  Bustle           (           )                        ;
  SqlConnection SC ( plan->sql )                        ;
  if ( SC . open ( FunctionString ) )                   {
    UUIDs Uuids                                         ;
    if (NetworkManager::PageUuids(SC,URLs,Uuids,true))  {
      GroupTable = LookTable                            (
                     Types::Division                    ,
                     Types::URL                         ,
                     Groups::Subordination            ) ;
      GroupItems::Join                                  (
        SC                                              ,
        DUID                                            ,
        Types::Division                                 ,
        Types::URL                                      ,
        Groups::Subordination                           ,
        0                                               ,
        Uuids                                         ) ;
    }                                                   ;
    SC . close  ( )                                     ;
  }                                                     ;
  SC   . remove ( )                                     ;
  Vacancy  (      )                                     ;
  Alert    ( Done )                                     ;
}

void N::BookmarkTree::ImportLinks(QTreeWidgetItem * item)
{
  nDropOut ( IsNull( item->parent() ) )   ;
  QTreeWidgetItem * mark = item->parent() ;
  SUID DUID = nTreeUuid(mark,0)           ;
  if (DUID<=0) return                     ;
  QUrl url(item->text(0))                 ;
  VarArgLists Operations                  ;
  VarArgs     V                           ;
  V << "PageTo"                           ;
  V << url.host()                         ;
  V << DUID                               ;
  V << Types::Division                    ;
  V << Groups::Subordination              ;
  Operations << V                         ;
  emit RunSets ( Operations )             ;
}

bool N::BookmarkTree::Menu(QPoint pos)
{
  MenuManager       mm (this)                         ;
  QAction         * a                                 ;
  QTreeWidgetItem * item  = itemAt(pos)               ;
  int               mType = -1                        ;
  int               mLoad = -1                        ;
  /////////////////////////////////////////////////////
  if (NotNull(item))                                  {
    mType = item->data (1,Qt::UserRole  ) . toInt ( ) ;
    mLoad = item->data (1,Qt::UserRole+1) . toInt ( ) ;
  }                                                   ;
  /////////////////////////////////////////////////////
  if (IsNull(item))                                   {
    mm . add   ( 1001 , tr("New")                   ) ;
  } else
  if (mType==1)                                       {
    if (mLoad==0)                                     {
      mm . add ( 2001 , tr ( "Load"         )    )    ;
    } else                                            {
      mm . add ( 2001 , tr ( "Reload"       )    )    ;
    }                                                 ;
    mm . add   ( 2002 , tr ( "Rename"       )       ) ;
    mm . add   ( 2003 , tr ( "Delete"       )       ) ;
    mm . add   ( 2004 , tr ( "Select all"   )       ) ;
    mm . add   ( 2005 , tr ( "Select none"  )       ) ;
    mm . add   ( 2101 , tr ( "Import rules" )       ) ;
    mm . add   ( 2102 , tr ( "Import sites" )       ) ;
    mm . add   ( 2103 , tr ( "Import pages" )       ) ;
  } else                                              {
    mm . add   ( 2111 , tr ( "Import page links" )  ) ;
  }                                                   ;
  mm . setFont ( plan->fonts[Fonts::Menu]           ) ;
  a = mm . exec( QCursor::pos()                     ) ;
  switch (mm[a])                                      {
    case 1001                                         :
      Insert (      )                                 ;
    break                                             ;
    case 2001                                         :
      Reload ( item )                                 ;
    break                                             ;
    case 2002                                         :
      Rename ( item )                                 ;
    break                                             ;
    case 2003                                         :
      Delete ( item )                                 ;
    break                                             ;
    case 2004                                         :
      for (int i=0;i<item->childCount();i++)          {
        QTreeWidgetItem * it = item->child(i)         ;
        it->setCheckState(0,Qt::Checked)              ;
      }                                               ;
    break                                             ;
    case 2005                                         :
      for (int i=0;i<item->childCount();i++)          {
        QTreeWidgetItem * it = item->child(i)         ;
        it->setCheckState(0,Qt::Unchecked)            ;
      }                                               ;
    break                                             ;
    case 2101                                         :
      emit ImportRules                                (
             item->text(0)                            ,
             nTreeUuid(item,0)                        ,
             Types::Division                        ) ;
    break                                             ;
    case 2102                                         :
      ImportSites(item)                               ;
    break                                             ;
    case 2103                                         :
      ImportPages(item)                               ;
    break                                             ;
    case 2111                                         :
      ImportLinks(item)                               ;
    break                                             ;
  }                                                   ;
  return true                                         ;
}
