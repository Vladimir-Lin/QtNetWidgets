#include <netwidgets.h>
#include "ui_nFtpControl.h"

//#define FTPDEBUG

#ifdef FTPDEBUG
#define XDEBUG(msg) if (plan->Verbose>=90) plan -> Debug ( msg)
#else
#define XDEBUG(msg)
#endif

N::FtpControl:: FtpControl  (QWidget * parent,Plan * p)
              : PanWidget   (          parent,       p)
              , FtpSettings (                         )
              , ui          (new Ui::nFtpControl      )
{
  Configure ( ) ;
}

N::FtpControl::~FtpControl (void)
{
  delete ui ;
}

QSize N::FtpControl::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize (  640 ,  480 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::FtpControl::closeEvent(QCloseEvent  * event)
{
  if (FileSharing::serverRunning()) {
    FileSharing :: stopServer ( )   ;
  }                                 ;
  plan -> Ftp = NULL                ;
  QSplitter :: closeEvent ( event ) ;
}

void N::FtpControl::resizeEvent(QResizeEvent * event)
{
  QSplitter :: resizeEvent ( event ) ;
  relocation               (       ) ;
}

void N::FtpControl::showEvent(QShowEvent * event)
{
  QSplitter :: showEvent ( event ) ;
  relocation             (       ) ;
}

void N::FtpControl::Configure(void)
{
  ui -> setupUi    ( this            )               ;
  ////////////////////////////////////////////////////
  addWidget        ( ui->Security    )               ;
  addWidget        ( ui->Folders     )               ;
  addWidget        ( ui->Connections )               ;
  addWidget        ( ui->Stats       )               ;
  addWidget        ( ui->Search      )               ;
  setCurrentWidget ( ui->Security    )               ;
  ////////////////////////////////////////////////////
  ui->Folders     -> setEnabled(false)               ;
  ui->Connections -> setEnabled(false)               ;
  ui->Stats       -> setEnabled(false)               ;
  ui->Search      -> setEnabled(false)               ;
  ////////////////////////////////////////////////////
  ui->chart       -> setScene ( new QGraphicsScene ) ;
  ////////////////////////////////////////////////////
  QStringList labels                                 ;
  labels << tr ( "User"     )                        ;
  labels << tr ( "Address"  )                        ;
  labels << tr ( "Activity" )                        ;
  ui->Connections->setHorizontalHeaderLabels(labels) ;
  ////////////////////////////////////////////////////
  ui->Stacked->hide(                 )               ;
  setHandleSize    ( 16              )               ;
  ////////////////////////////////////////////////////
  FtpSettings::Gui         = true                    ;
  FtpSettings::Anonymous   = false                   ;
  FtpSettings::ReadOnly    = false                   ;
  FtpSettings::HiddenFiles = false                   ;
  FtpSettings::ServerPort  = 2100                    ;
  ////////////////////////////////////////////////////
  plan -> Ftp              = NULL                    ;
  ////////////////////////////////////////////////////
  timer = new QTimer ( this )                        ;
  nConnect ( timer     , SIGNAL ( timeout ( ) )      ,
             this      , SLOT   ( Chart   ( ) )    ) ;
  nConnect ( timer     , SIGNAL ( timeout ( ) )      ,
             ui->stats , SLOT   ( repaint ( ) )    ) ;
  timer -> setInterval   ( 250   )                   ;
  timer -> setSingleShot ( false )                   ;
  clock  . start         (       )                   ;
  prevSent = 0                                       ;
  prevRecv = 0                                       ;
  ////////////////////////////////////////////////////
  workingThreads = 0                                 ;
  searching      = new QTimer    ( this )            ;
  searching     -> setInterval   ( 500  )            ;
  searching     -> setSingleShot ( true )            ;
  nConnect ( searching , SIGNAL ( timeout      ())   ,
             this      , SLOT   ( SearchResults()) ) ;
}

void N::FtpControl::relocation(void)
{
  nDropOut ( IsNull(Stack) )                                                ;
  QSize s = Stack->size()                                                   ;
  int   w = s . width ( ) - 20                                              ;
  int   y                                                                   ;
  ///////////////////////////////////////////////////////////////////////////
  if (indexOf(ui->Security)>=0)                                             {
    ui -> Username      -> resize ( w , ui -> Username      -> height ( ) ) ;
    ui -> Password      -> resize ( w , ui -> Password      -> height ( ) ) ;
    ui -> Port          -> resize ( w , ui -> Port          -> height ( ) ) ;
    ui -> Start         -> resize ( w , ui -> Start         -> height ( ) ) ;
    ui -> Back          -> resize ( w , ui -> Back          -> height ( ) ) ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if (indexOf(ui->Folders)>=0)                                              {
    y   = s . height ( ) - 40                                               ;
    ui -> bAdd          -> move   ( ui -> bAdd -> x      ( ) , y          ) ;
    ui -> bAdd          -> resize ( ( w / 2 ) - 4 , ui->bAdd   ->height() ) ;
    ui -> bRemove       -> move   ( ( w / 2 ) + 18 , y                    ) ;
    ui -> bRemove       -> resize ( ( w / 2 ) - 4 , ui->bRemove->height() ) ;
    /////////////////////////////////////////////////////////////////////////
    y  -= 10                                                                ;
    y  -= ui -> sharedFolders -> y ( )                                      ;
    ui -> sharedFolders -> resize ( w , y                                 ) ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if (indexOf(ui->Stats)>=0)                                                {
    y   = s   . height     ( ) - 10                                         ;
    y  -= ui -> chart -> y ( )                                              ;
    ui -> stats         -> resize ( w , ui -> stats         -> height ( ) ) ;
    ui -> chart         -> resize ( w , y                                 ) ;
  }                                                                         ;
  ///////////////////////////////////////////////////////////////////////////
  if (indexOf(ui->Search)>=0)                                               {
    ui -> expression    -> resize ( w , ui -> expression    -> height ( ) ) ;
    ui -> bSearch       -> resize ( w / 2 , ui->bSearch->height()         ) ;
    ui -> bStop         -> move   ( ( w / 2 ) + 10 , ui->bStop->y()       ) ;
    ui -> bStop         -> resize ( w / 2 , ui->bSearch->height()         ) ;
    y   = s   . height     ( ) - 10                                         ;
    y  -= ui -> results -> y ( )                                            ;
    ui -> results       -> resize ( w , y                                 ) ;
  }                                                                         ;
}

void N::FtpControl::startup(void)
{
  QString user = User()                                                   ;
  QString pass = ""                                                       ;
  bool    Auto = false                                                    ;
  plan -> settings  . beginGroup   ( "FtpServer"                  )       ;
  if ( plan->settings.contains("Username"))                               {
    user = plan->settings.valueString("Username")                         ;
  }                                                                       ;
  if ( plan->settings.contains("Password"))                               {
    pass = plan->settings.valueString("Password")                         ;
  }                                                                       ;
  if ( plan->settings.contains("Port"))                                   {
    FtpSettings::ServerPort  = plan->settings.value("Port").toInt()       ;
  }                                                                       ;
  if ( plan->settings.contains("Anonymous"))                              {
    FtpSettings::Anonymous   = plan->settings.value("Anonymous").toBool() ;
  }                                                                       ;
  if ( plan->settings.contains("ReadOnly"))                               {
    FtpSettings::ReadOnly    = plan->settings.value("ReadOnly" ).toBool() ;
  }                                                                       ;
  if ( plan->settings.contains("Hidden"))                                 {
    FtpSettings::HiddenFiles = plan->settings.value("Hidden"   ).toBool() ;
  }                                                                       ;
  if ( plan->settings.contains("Startup"))                                {
    Auto                     = plan->settings.value("Startup"  ).toBool() ;
  }                                                                       ;
  /////////////////////////////////////////////////////////////////////////
  ui   -> Username  -> blockSignals ( true                     )          ;
  ui   -> Username  -> setText      ( user                     )          ;
  ui   -> Username  -> blockSignals ( false                    )          ;
  /////////////////////////////////////////////////////////////////////////
  ui   -> Password  -> blockSignals ( true                     )          ;
  ui   -> Password  -> setText      ( pass                     )          ;
  ui   -> Password  -> blockSignals ( false                    )          ;
  /////////////////////////////////////////////////////////////////////////
  ui   -> Port      -> blockSignals ( true                     )          ;
  ui   -> Port      -> setValue     ( FtpSettings::ServerPort  )          ;
  ui   -> Port      -> blockSignals ( false                    )          ;
  /////////////////////////////////////////////////////////////////////////
  ui   -> Anonymous -> blockSignals ( true                     )          ;
  ui   -> Anonymous -> setChecked   ( FtpSettings::Anonymous   )          ;
  ui   -> Anonymous -> blockSignals ( false                    )          ;
  /////////////////////////////////////////////////////////////////////////
  ui   -> ReadOnly  -> blockSignals ( true                     )          ;
  ui   -> ReadOnly  -> setChecked   ( FtpSettings::ReadOnly    )          ;
  ui   -> ReadOnly  -> blockSignals ( false                    )          ;
  /////////////////////////////////////////////////////////////////////////
  ui   -> Hidden    -> blockSignals ( true                     )          ;
  ui   -> Hidden    -> setChecked   ( FtpSettings::HiddenFiles )          ;
  ui   -> Hidden    -> blockSignals ( false                    )          ;
  /////////////////////////////////////////////////////////////////////////
  ui   -> Startup   -> blockSignals ( true                     )          ;
  ui   -> Startup   -> setChecked   ( Auto                     )          ;
  ui   -> Startup   -> blockSignals ( false                    )          ;
  /////////////////////////////////////////////////////////////////////////
  plan -> settings   . endGroup     (                          )          ;
  /////////////////////////////////////////////////////////////////////////
  bool    correct = true                                                  ;
  if (user.length()<=0) correct = false                                   ;
  if (pass.length()<=0) correct = false                                   ;
  ui -> Start       -> setEnabled ( correct )                             ;
}

void N::FtpControl::LoadFolder(void)
{
  QStringList        FLS                                                   ;
  QString            S                                                     ;
  QTableWidgetItem * item                                                  ;
  int                index = 0                                             ;
  FLS = plan -> settings . getStringList ( "FtpFolders" )                  ;
  if (indexOf(ui->Folders)>=0)                                             {
    ui -> Folders       -> setEnabled   ( true        )                    ;
    ui -> sharedFolders -> blockSignals ( true        )                    ;
    ui -> sharedFolders -> setRowCount  ( FLS.count() )                    ;
  }                                                                        ;
  foreach (S,FLS)                                                          {
    bool    w = false                                                      ;
    QString p = ""                                                         ;
    plan -> settings . beginGroup ( "FtpWritable" )                        ;
    if (plan->settings.contains(S))                                        {
      w = plan->settings.value(S).toBool()                                 ;
    }                                                                      ;
    plan -> settings . endGroup   (               )                        ;
    plan -> settings . beginGroup ( "FtpPathes"   )                        ;
    if (plan->settings.contains(S))                                        {
      p = plan->settings.valueString(S)                                    ;
    }                                                                      ;
    plan -> settings . endGroup   (               )                        ;
    if (p.length()>0 && S.length()>0)                                      {
      appendFolder ( p , S , w )                                           ;
      item  = new QTableWidgetItem ( QIcon(":/images/folder.png") , S )    ;
      item -> setFlags ( Qt::ItemIsEditable                                |
                         Qt::ItemIsEnabled                                 |
                         Qt::ItemIsSelectable                            ) ;
      if (indexOf(ui->Folders)>=0)                                         {
        folderNames [ index ] = S                                          ;
        ui -> sharedFolders -> setItem ( index , 0 , item )                ;
      }                                                                    ;
      //////////////////////////////////////////////////////////////////////
      item  = new QTableWidgetItem ( w ? tr("Allowed") : tr("Forbidden") ) ;
      item -> setFlags ( Qt::ItemIsUserCheckable                           |
                         Qt::ItemIsEnabled                                 |
                         Qt::ItemIsSelectable                            ) ;
      item -> setCheckState ( w ? Qt::Checked : Qt::Unchecked )            ;
      if (indexOf(ui->Folders)>=0)                                         {
        ui -> sharedFolders -> setItem ( index , 1 , item )                ;
      }                                                                    ;
      //////////////////////////////////////////////////////////////////////
      item  = new QTableWidgetItem ( p )                                   ;
      item -> setFlags ( Qt::ItemIsEditable                                |
                         Qt::ItemIsEnabled                                 |
                         Qt::ItemIsSelectable                            ) ;
      if (indexOf(ui->Folders)>=0)                                         {
        ui -> sharedFolders -> setItem ( index , 2 , item )                ;
      }                                                                    ;
      index++                                                              ;
    }                                                                      ;
  }                                                                        ;
  if (indexOf(ui->Folders)>=0)                                             {
    ui -> sharedFolders -> blockSignals ( false )                          ;
    ui -> Folders       -> setEnabled   ( false )                          ;
  }                                                                        ;
}

void N::FtpControl::SaveFolder(void)
{
  QMap<QString,FtpFolder> folders                                  ;
  QStringList             FLS                                      ;
  QString                 S                                        ;
  folders = FtpSharedFolders :: instance ( ) -> getFolderList ( )  ;
  nDropOut ( folders . count ( ) <= 0 )                            ;
  FLS     = folders  . keys  ( )                                   ;
  nDropOut ( FLS     . count ( ) <= 0 )                            ;
  plan -> settings . setStringList ( "FtpFolders" , FLS )          ;
  plan -> settings . beginGroup    ( "FtpWritable"  )              ;
  foreach (S,FLS)                                                  {
    plan -> settings . setValue    ( S , folders[S].isWritable() ) ;
  }                                                                ;
  plan -> settings . endGroup      (                )              ;
  plan -> settings . beginGroup    ( "FtpPathes"    )              ;
  foreach (S,FLS)                                                  {
    QString p = folders[S].getPath (                )              ;
    plan -> settings . setValue    ( S , p.toUtf8() )              ;
  }                                                                ;
  plan -> settings . endGroup      (                )              ;
}

void N::FtpControl::Backward(void)
{
  emit Return ( ) ;
}

bool N::FtpControl::AutoStartup(void)
{
  return ui->Startup->isChecked() ;
}

QString N::FtpControl::Login(void)
{
  return ui->Username->text() ;
}

bool N::FtpControl::Password(QByteArray & password)
{
  password . clear  (                               ) ;
  password . append ( ui->Password->text().toUtf8() ) ;
  return false                                        ;
}

bool N::FtpControl::connectSharing(FileSharing * sharing)
{
  nConnect(sharing , SIGNAL(newThread   ())   ,
           this    , SLOT  (newThread   ()) ) ;
  nConnect(sharing , SIGNAL(closing     ())   ,
           this    , SLOT  (closing     ()) ) ;
  nConnect(sharing , SIGNAL(removeThread())   ,
           this    , SLOT  (removeThread()) ) ;
  return false                                ;
}

bool N::FtpControl::connectThread(FtpThread * thread)
{
  nConnect ( thread , SIGNAL ( activity() )   ,
             this   , SLOT   ( activity() ) ) ;
  return true                                 ;
}

void N::FtpControl::newThread(void)
{
  Status ( ) ;
}

void N::FtpControl::removeThread(void)
{
  Status ( ) ;
}

void N::FtpControl::closing(void)
{
  Status ( ) ;
}

void N::FtpControl::activity(void)
{
  Status ( ) ;
  Chart  ( ) ;
}

void N::FtpControl::setBack(bool toggle)
{
  ui -> Back -> setVisible ( toggle ) ;
}

void N::FtpControl::begin(bool toggle)
{
  XDEBUG ( "N::FtpControl::begin" )            ;
  //////////////////////////////////////////////
  plan -> processEvents              (       ) ;
  ui   -> Start       -> setEnabled  ( false ) ;
  plan -> processEvents              (       ) ;
  #define ENABLEIT(II,BL)                      \
    if (indexOf(II)>=0) II->setEnabled(BL)
  if (toggle)                                  {
    QIcon ic = QIcon(":/images/close.png")     ;
    prevSent    = 0                            ;
    prevRecv    = 0                            ;
    timeline    . clear ( )                    ;
    bytesOut    . clear ( )                    ;
    bytesIn     . clear ( )                    ;
    connections . clear ( )                    ;
    if (!ui->Anonymous->isChecked())           {
      XDEBUG ( "N::FtpControl::begin@assureUser" ) ;
      //////////////////////////////////////////////
      server ( ) -> assureUser                 (
                      ui->Username->text()     ,
                      ui->Password->text()  )  ;
    }                                          ;
    if (ui->Username->text().length()>0)       {
      SystemUser = ui->Username->text()        ;
    }                                          ;
    XDEBUG ( "N::FtpControl::begin@startServer" ) ;
    ///////////////////////////////////////////////
    server ( ) -> startServer (             )  ;
    ui -> Start -> setText    ( tr("Stop" ) )  ;
    ui -> Start -> setIcon    ( ic          )  ;
//    ui -> Back  -> setEnabled ( false       )  ;
    ENABLEIT ( ui->Folders     , true  )       ;
    ENABLEIT ( ui->Connections , true  )       ;
    ENABLEIT ( ui->Stats       , true  )       ;
    ENABLEIT ( ui->Search      , true  )       ;
    ui -> Username  -> setEnabled ( false )    ;
    ui -> Password  -> setEnabled ( false )    ;
    ui -> Port      -> setEnabled ( false )    ;
    ui -> Hidden    -> setEnabled ( false )    ;
    ui -> Anonymous -> setEnabled ( false )    ;
    ui -> ReadOnly  -> setEnabled ( false )    ;
    timer -> start   ( )                       ;
    clock  . restart ( )                       ;
  } else                                       {
    QIcon ic = QIcon(":/images/yes.png"  )     ;
    server ( )        -> stopServer  (       ) ;
    ui -> Start -> setText    ( tr("Start") )  ;
    ui -> Start -> setIcon    ( ic          )  ;
//    ui -> Back  -> setEnabled ( true        )  ;
    ENABLEIT ( ui->Folders     , false )       ;
    ENABLEIT ( ui->Connections , false )       ;
    ENABLEIT ( ui->Stats       , false )       ;
    ENABLEIT ( ui->Search      , false )       ;
    ui -> Username  -> setEnabled ( true  )    ;
    ui -> Password  -> setEnabled ( true  )    ;
    ui -> Port      -> setEnabled ( true  )    ;
    ui -> Hidden    -> setEnabled ( true  )    ;
    ui -> Anonymous -> setEnabled ( true  )    ;
    ui -> ReadOnly  -> setEnabled ( true  )    ;
    timer -> stop    ( )                       ;
    clock  . restart ( )                       ;
  }                                            ;
  #undef  ENABLEIT
  ui -> Start       -> blockSignals ( true   ) ;
  ui -> Start       -> setDown      ( toggle ) ;
  ui -> Start       -> blockSignals ( false  ) ;
  ui -> Start       -> setEnabled   ( true   ) ;
}

void N::FtpControl::Accounts(void)
{
  QString user    = ui->Username->text()                         ;
  QString pass    = ui->Password->text()                         ;
  bool    correct = true                                         ;
  if (user.length()<=0) correct = false                          ;
  if (pass.length()<=0) correct = false                          ;
  ui -> Start       -> setEnabled ( correct )                    ;
  ////////////////////////////////////////////////////////////////
  plan -> settings . beginGroup ( "FtpServer"                  ) ;
  plan -> settings . setValue   ( "Username" , user . toUtf8() ) ;
  plan -> settings . setValue   ( "Password" , pass . toUtf8() ) ;
  plan -> settings . endGroup   (                              ) ;
}

void N::FtpControl::PortChanged(int port)
{
  FtpSettings::ServerPort = port                                          ;
  plan -> settings . beginGroup ( "FtpServer"                           ) ;
  plan -> settings . setValue   ( "Port"      , FtpSettings::ServerPort ) ;
  plan -> settings . endGroup   (                                       ) ;
}

void N::FtpControl::Options(int)
{
  FtpSettings::Anonymous   = ui->Anonymous->isChecked()                    ;
  FtpSettings::ReadOnly    = ui->ReadOnly ->isChecked()                    ;
  FtpSettings::HiddenFiles = ui->Hidden   ->isChecked()                    ;
  //////////////////////////////////////////////////////////////////////////
  bool autorun             = ui->Startup  ->isChecked()                    ;
  plan -> settings . beginGroup ( "FtpServer"                            ) ;
  plan -> settings . setValue   ( "Anonymous" , FtpSettings::Anonymous   ) ;
  plan -> settings . setValue   ( "ReadOnly"  , FtpSettings::ReadOnly    ) ;
  plan -> settings . setValue   ( "Hidden"    , FtpSettings::HiddenFiles ) ;
  plan -> settings . setValue   ( "Startup"   , autorun                  ) ;
  plan -> settings . endGroup   (                                        ) ;
}

void N::FtpControl::addFolder(void)
{
  QMutexLocker mLock ( &(FtpSettings::Mutex) )                             ;
  int                newIdx = ui->sharedFolders->rowCount()                ;
  QString            name   = tr("folder %1").arg(newIdx)                  ;
  QTableWidgetItem * item                                                  ;
  //////////////////////////////////////////////////////////////////////////
  ui -> sharedFolders -> blockSignals ( true  )                            ;
  ui -> sharedFolders -> setRowCount ( newIdx + 1 )                        ;
  //////////////////////////////////////////////////////////////////////////
  item  = new QTableWidgetItem ( QIcon(":/images/folder.png") , name )     ;
  item -> setFlags ( Qt::ItemIsEditable                                    |
                     Qt::ItemIsEnabled                                     |
                     Qt::ItemIsSelectable                                ) ;
  ui   -> sharedFolders -> setItem ( newIdx , 0 , item )                   ;
  //////////////////////////////////////////////////////////////////////////
  item  = new QTableWidgetItem ( tr("Forbidden") )                         ;
  item -> setFlags ( Qt::ItemIsUserCheckable                               |
                     Qt::ItemIsEnabled                                     |
                     Qt::ItemIsSelectable                                ) ;
  item -> setCheckState ( Qt::Unchecked )                                  ;
  ui   -> sharedFolders -> setItem ( newIdx , 1 , item )                   ;
  //////////////////////////////////////////////////////////////////////////
  item  = new QTableWidgetItem ( "" )                                      ;
  item -> setFlags ( Qt::ItemIsEditable                                    |
                     Qt::ItemIsEnabled                                     |
                     Qt::ItemIsSelectable                                ) ;
  ui   -> sharedFolders -> setItem ( newIdx , 2 , item )                   ;
  //////////////////////////////////////////////////////////////////////////
  ui   -> sharedFolders -> blockSignals ( false )                          ;
  FtpSettings :: addFolder ( "" , name , false )                           ;
  folderNames [ item->row() ] = name                                       ;
}

void N::FtpControl::removeFolder(void)
{
  QMutexLocker mLock ( &(FtpSettings::Mutex) )                             ;
  TableWidgetItems list = ui -> sharedFolders -> selectedItems ( )         ;
  if ( list.isEmpty() ) return                                             ;
  //////////////////////////////////////////////////////////////////////////
  TableWidgetItems removals                                                ;
  for (int i=0;i<list.count();i++)                                         {
    int                r  = list [ i ] -> row  ( )                         ;
    QTableWidgetItem * it = ui         -> sharedFolders -> item ( r , 0 )  ;
    QString            nn = it         -> text ( )                         ;
    FtpSharedFolders :: instance ( ) -> removeFolder ( nn )                ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  ui -> sharedFolders -> blockSignals ( true  )                            ;
  for (int i=0;i<removals.count();i++)                                     {
    int r = ui -> sharedFolders -> row ( removals [ i ] )                  ;
    if (r>=0) ui -> sharedFolders -> removeRow ( r )                       ;
  }                                                                        ;
  ui -> sharedFolders -> blockSignals ( false )                            ;
  //////////////////////////////////////////////////////////////////////////
  folderNames . clear ( )                                                  ;
  for (int i=0;i<ui->sharedFolders->rowCount();i++)                        {
    QTableWidgetItem * xi = ui->sharedFolders->item ( i , 0 )              ;
    folderNames [ i ] = xi->text()                                         ;
  }                                                                        ;
}

void N::FtpControl::editFolder(QTableWidgetItem * it)
{
  nDropOut           ( IsNull(it)            )                             ;
  nDropOut           ( it->row   () <  0     )                             ;
  nDropOut           ( it->column() == 2     )                             ;
  QMutexLocker mLock ( &(FtpSettings::Mutex) )                             ;
  //////////////////////////////////////////////////////////////////////////
  QMap<QString,FtpFolder> folders                                          ;
  QString                 name                                             ;
  QTableWidgetItem *      ix                                               ;
  ix      = ui -> sharedFolders -> item ( it->row() , 0 )                  ;
  folders = FtpSharedFolders :: instance ( ) -> getFolderList ( )          ;
  name    = folderNames[it->row()]                                         ;
  //////////////////////////////////////////////////////////////////////////
  FtpFolder folder = folders[ name ]                                       ;
  FtpSharedFolders :: instance ( ) -> removeFolder ( name )                ;
  //////////////////////////////////////////////////////////////////////////
  ui -> sharedFolders -> blockSignals ( true  )                            ;
  switch ( it -> column ( ) )                                              {
    case 0                                                                 :
      name                   = ix -> text ( )                              ;
      folderNames[it->row()] = name                                        ;
    break                                                                  ;
    case 1                                                                 :
      folder . setWritable ( it -> checkState ( ) == Qt::Checked )         ;
      it->setText(folder.isWritable() ? tr("Allowed") : tr("Forbidden"))   ;
    break                                                                  ;
    case 2                                                                 :
      folder . setPath ( it -> text ( ) )                                  ;
    break                                                                  ;
  }                                                                        ;
  ui -> sharedFolders -> blockSignals ( false )                            ;
  //////////////////////////////////////////////////////////////////////////
  FtpSharedFolders :: instance ( ) -> addFolder ( name , folder )          ;
  SaveFolder ( )                                                           ;
}

void N::FtpControl::editPath(QTableWidgetItem * it)
{
  nDropOut           ( IsNull(it)            )                             ;
  nDropOut           ( it->row   () <  0     )                             ;
  nDropOut           ( it->column() != 2     )                             ;
  QMutexLocker mLock ( &(FtpSettings::Mutex) )                             ;
  //////////////////////////////////////////////////////////////////////////
  QMap<QString,FtpFolder> folders                                          ;
  QString                 name                                             ;
  QTableWidgetItem *      ix                                               ;
  ix      = ui -> sharedFolders -> item ( it->row() , 0 )                  ;
  folders = FtpSharedFolders :: instance ( ) -> getFolderList ( )          ;
  if (folders.count()<0               ) return                             ;
  if (!folderNames.contains(it->row())) return                             ;
  name    = folderNames[it->row()]                                         ;
  if (!folders.contains(name)         ) return                             ;
  //////////////////////////////////////////////////////////////////////////
  FtpFolder folder = folders[ name ]                                       ;
  FtpSharedFolders :: instance ( ) -> removeFolder ( name )                ;
  name    = ix -> text ( )                                                 ;
  //////////////////////////////////////////////////////////////////////////
  ui -> sharedFolders -> blockSignals ( false )                            ;
  QString p = folder.getPath()                                             ;
  p = QFileDialog :: getExistingDirectory                                  (
                       Stack                                               ,
                       tr("Select '%1' folder").arg(name)                  ,
                       p                                                   ,
                       QFileDialog::ShowDirsOnly                         ) ;
  it     -> setText ( p )                                                  ;
  folder  . setPath ( p )                                                  ;
  ui -> sharedFolders -> blockSignals ( true  )                            ;
  //////////////////////////////////////////////////////////////////////////
  FtpSharedFolders :: instance ( ) -> addFolder ( name , folder )          ;
  SaveFolder ( )                                                           ;
}

void N::FtpControl::Status(void)
{
  nDropOut ( indexOf(ui->Connections) < 0 )                                    ;
  if ( ! server ( ) -> serverRunning ( ) )                                     {
    if (indexOf(ui->Connections)>=0)                                           {
      ui -> Connections -> setRowCount ( 0 )                                   ;
    }                                                                          ;
    return                                                                     ;
  }                                                                            ;
  //////////////////////////////////////////////////////////////////////////////
  setUpdatesEnabled ( false )                                                  ;
  QList<FileSharing::ConnectionInfo> lInfo                                     ;
  lInfo = server ( ) -> getClientsInfo ( )                                     ;
  if (indexOf(ui->Connections)>=0)                                             {
    ui -> Connections  -> setRowCount(lInfo.size())                            ;
    int i = 0                                                                  ;
    foreach ( FileSharing::ConnectionInfo info , lInfo )                       {
      ui -> Connections -> setItem ( i,0,new QTableWidgetItem(info.user    ) ) ;
      ui -> Connections -> setItem ( i,1,new QTableWidgetItem(info.address ) ) ;
      ui -> Connections -> setItem ( i,2,new QTableWidgetItem(info.activity) ) ;
      ++i                                                                      ;
    }                                                                          ;
  }                                                                            ;
  //////////////////////////////////////////////////////////////////////////////
  if (indexOf(ui->Stats)>=0)                                                   {
    QStringList sInfo = server() -> getServerInfo ( )                          ;
    for ( int i = 0 ; i < sInfo.size() ; ++i )                                 {
      if ( ! ui -> stats -> item ( i , 0 ) )                                   {
        ui -> stats -> setItem ( i , 0 , new QTableWidgetItem() )              ;
      }                                                                        ;
    }                                                                          ;
    ////////////////////////////////////////////////////////////////////////////
    quint64 size                                                               ;
    size  = sInfo [ 0 ] . toULongLong ( )                                      ;
    ui   -> stats -> item ( 0 , 0 ) -> setText ( DataFB ( (TUID)size ) )       ;
    size  = sInfo [ 1 ] . toULongLong ( )                                      ;
    ui   -> stats -> item ( 1 , 0 ) -> setText ( DataFB ( (TUID)size ) )       ;
    for (int i = 2 ; i < sInfo.size() ; ++i)                                   {
      ui -> stats -> item ( i , 0 ) -> setText ( sInfo[i]              )       ;
    }                                                                          ;
  }                                                                            ;
  //////////////////////////////////////////////////////////////////////////////
  setUpdatesEnabled ( true )                                                   ;
}

void N::FtpControl::Chart(void)
{
  nDropOut ( indexOf(ui->Stats) < 0 )                                      ;
  QStringList sInfo    = server ( ) -> getServerInfo ( )                   ;
  quint64     elapsed  = clock       . restart       ( )                   ;
  nDropOut ( elapsed <= 0           )                                      ;
  quint64     timeCoef = (quint64(250) << 32) / elapsed                    ;
  //////////////////////////////////////////////////////////////////////////
  if ( timeline.isEmpty() )                                                {
    timeline    . push_back ( 0.0 )                                        ;
    connections . push_back ( 0   )                                        ;
    bytesOut    . push_back ( 0   )                                        ;
    bytesIn     . push_back ( 0   )                                        ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  timeline    . push_back ( timeline.back() + elapsed / 250.0 )            ;
  connections . push_back ( sInfo[2].toInt()                  )            ;
  const quint64 vout = (sInfo[0].toLongLong() - prevSent) * timeCoef >> 32 ;
  const quint64 vin  = (sInfo[1].toLongLong() - prevRecv) * timeCoef >> 32 ;
  bytesOut . push_back ( ( bytesOut . back ( ) + vout ) >> 1 )             ;
  bytesIn  . push_back ( ( bytesIn  . back ( ) + vin  ) >> 1 )             ;
  //////////////////////////////////////////////////////////////////////////
  prevSent = sInfo [ 0 ] . toULongLong ( )                                 ;
  prevRecv = sInfo [ 1 ] . toULongLong ( )                                 ;
  //////////////////////////////////////////////////////////////////////////
  const int maxlen = ui -> chart -> width ( ) - 8                          ;
  //////////////////////////////////////////////////////////////////////////
  while ( timeline . back ( ) - timeline . front ( ) < maxlen )            {
    timeline    . push_front ( timeline . front ( ) - 1.0 )                ;
    bytesIn     . push_front ( 0                          )                ;
    bytesOut    . push_front ( 0                          )                ;
    connections . push_front ( 0                          )                ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  while ( timeline . back ( ) - timeline . front ( ) > maxlen )            {
    timeline    . pop_front ( )                                            ;
    bytesIn     . pop_front ( )                                            ;
    bytesOut    . pop_front ( )                                            ;
    connections . pop_front ( )                                            ;
  }                                                                        ;
  //////////////////////////////////////////////////////////////////////////
  setUpdatesEnabled ( false )                                              ;
  //////////////////////////////////////////////////////////////////////////
  ui -> chart -> scene ( ) -> clear ( )                                    ;
  const double start = timeline.front()                                    ;
  qint64 m = qMax<qint64>(1, maxitem(bytesOut))                                ;
  double coef = -0.667 * ui->chart->height() / double(m)                   ;
  QPolygonF polyOut                                                        ;
  for ( int i = 0 ; i < bytesOut.size() ; ++i )                            {
    polyOut << QPointF ( timeline[i] - start , bytesOut[i] * coef )        ;
  }                                                                        ;
  polyOut << QPointF ( timeline.back() - start, 0.0 ) << QPointF(0.0, 0.0) ;
  //////////////////////////////////////////////////////////////////////////
  m = qMax<qint64>(1, maxitem(bytesIn))                                        ;
  coef = -0.667 * ui->chart->height() / double(m)                          ;
  QPolygonF polyIn                                                         ;
  for ( int i = 0 ; i < bytesOut.size() ; ++i )                            {
    polyIn << QPointF ( timeline[i] - start , bytesIn[i] * coef )          ;
  }                                                                        ;
  polyIn << QPointF ( timeline.back() - start, 0.0 ) << QPointF(0.0, 0.0)  ;
  //////////////////////////////////////////////////////////////////////////
  m = qMax<qint64>(1, maxitem(connections))                                    ;
  coef = -0.667 * ui->chart->height() / double(m)                          ;
  QPolygonF polyCon                                                        ;
  for ( int i = 0 ; i < bytesOut.size() ; ++i )                            {
    polyCon << QPointF ( timeline[i] - start , connections[i] * coef )     ;
  }                                                                        ;
  polyCon << QPointF ( timeline.back() - start, 0.0 ) << QPointF(0.0, 0.0) ;
  //////////////////////////////////////////////////////////////////////////
  ui->chart->scene()->addPolygon(polyCon,QPen(QColor(0,255,0))                             ) ;
  ui->chart->scene()->addPolygon(polyOut,QPen(QColor(255,0,0)), QBrush(QColor(255,0,0,128))) ;
  ui->chart->scene()->addPolygon(polyIn ,QPen(QColor(0,0,255)), QBrush(QColor(0,0,255,128))) ;
  //////////////////////////////////////////////////////////////////////////
  setUpdatesEnabled ( true )                                               ;
}

void N::FtpControl::SearchFile(void)
{
  nDropOut ( workingThreads > 0 )                                  ;
  //////////////////////////////////////////////////////////////////
  ui -> bSearch    -> setEnabled  ( false )                        ;
  ui -> bStop      -> setEnabled  ( true  )                        ;
  ui -> expression -> setEnabled  ( false )                        ;
  ui -> results    -> setRowCount ( 0     )                        ;
  //////////////////////////////////////////////////////////////////
  QList<FtpComputer> serverList                                    ;
  QString request = ui->expression->text()                         ;
  serverList = NetDiscovery::instance()->getServerList()           ;
  workingThreads = 0                                               ;
  //////////////////////////////////////////////////////////////////
  for ( int i = 0 ; i < serverList.size() ; ++i )                  {
    FtpSearch * thread                                             ;
    thread = new FtpSearch(serverList[i].getName(), request, this) ;
    nConnect ( thread , SIGNAL (destroyed     ())                  ,
               this   , SLOT   (threadFinished())                ) ;
    nConnect ( this   , SIGNAL (stoppingSearch())                  ,
               thread , SLOT   (stopSearch    ())                ) ;
    nConnect ( thread , SIGNAL (found    (QString,QString))        ,
               this   , SLOT   (addResult(QString,QString))      ) ;
    ++workingThreads                                               ;
    thread -> start ( )                                            ;
  }                                                                ;
}

void N::FtpControl::StopSearch(void)
{
  emit stoppingSearch            (       ) ;
  ui -> bSearch    -> setEnabled ( true  ) ;
  ui -> bStop      -> setEnabled ( false ) ;
  ui -> expression -> setEnabled ( true  ) ;
}

void N::FtpControl::SearchResults(void)
{
  QMutexLocker mLock ( &(FtpSettings::Mutex) )                               ;
  int cur = ui -> results -> rowCount ( )                                    ;
  ui -> results -> setRowCount ( cur + queue . size ( ) )                    ;
  while ( !queue.isEmpty() )                                                 {
    QPair<QString,QString> file = queue . dequeue ( )                        ;
    QTableWidgetItem * item                                                  ;
    item  = new QTableWidgetItem(QIcon(":/images/computer.png"), file.first) ;
    ui   -> results -> setItem   ( cur , 0 , item )                          ;
    item -> setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled)               ;
    item  = new QTableWidgetItem ( file . second  )                          ;
    ui   -> results -> setItem   ( cur , 1 , item )                          ;
    item -> setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled)               ;
    ++cur                                                                    ;
  }                                                                          ;
  ui -> results -> sortItems ( 1 , Qt::AscendingOrder )                      ;
}

void N::FtpControl::ThreadFinished(void)
{
  QMutexLocker mLock ( &(FtpSettings::Mutex) ) ;
  --workingThreads                             ;
  if ( workingThreads <= 0 ) StopSearch ( )    ;
}

void N::FtpControl::addResult(const QString & computer,const QString & path)
{
  QMutexLocker mLock ( &(FtpSettings::Mutex) )           ;
  queue.enqueue(QPair<QString, QString>(computer, path)) ;
  if (!searching->isActive()) searching->start()         ;
}

void N::FtpControl::openResult(int row,int column)
{
  QString computerName = ui -> results -> item ( row , 0 ) -> text ( )       ;
  QString filename     = ui -> results -> item ( row , 1 ) -> text ( )       ;
  ////////////////////////////////////////////////////////////////////////////
  FtpComputer computer = NetDiscovery::instance()->getComputer(computerName) ;
  computer . open ( FtpSharedFolders :: getParentPath ( filename ) )         ;
}
