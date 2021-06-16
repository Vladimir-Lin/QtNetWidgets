#include <netwidgets.h>
#include "ui_PadFtp.h"

N::PadFtp:: PadFtp           ( QWidget      * parent , Plan * p )
          : Splitter         ( Qt::Vertical , parent ,        p )
          , Thread           ( 0 , false                        )
          , ftp              ( NULL                             )
          , Root             ( QDir::currentPath()              )
          , Current          ( QDir::currentPath()              )
          , Join             ( QIcon(":/icons/join.png" )       )
          , ui               ( new Ui::PadFtp                   )
          , ContinueDownload ( false                            )
          , TotalLines       ( 0                                )
          , FileIndex        ( 0                                )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::PadFtp::~PadFtp(void)
{
  delete ui ;
}

QSize N::PadFtp::sizeHint(void) const
{
  if ( plan  -> Booleans [ "Desktop" ] ) {
    return QSize ( 1280 ,  900 )         ;
  } else
  if ( plan  -> Booleans [ "Pad"     ] ) {
    return QSize ( 1024 ,  720 )         ;
  } else
  if ( plan  -> Booleans [ "Phone"   ] ) {
    return QSize (  320 ,  480 )         ;
  }                                      ;
  return   QSize ( 1024 ,  720 )         ;
}

void N::PadFtp::Configure(void)
{
  NewTreeWidgetItem           ( head                )                         ;
  QFont f = plan -> fonts     [ Fonts :: ListView   ]                         ;
  ui -> setupUi               ( this                )                         ;
  head -> setText             ( 0 , tr("Name")      )                         ;
  head -> setText             ( 1 , tr("Type")      )                         ;
  head -> setText             ( 2 , tr("Size")      )                         ;
  head -> setText             ( 3 , ""              )                         ;
  for (int i=0;i<4;i++)                                                       {
    head -> setTextAlignment  ( i , Qt::AlignCenter )                         ;
    head -> setFont           ( i , f               )                         ;
  }                                                                           ;
  ui->Ftp->setHeaderItem      ( head                )                         ;
  /////////////////////////////////////////////////////////////////////////////
  head = new QTreeWidgetItem  (                     )                         ;
  head -> setText             ( 0 , tr("Name")      )                         ;
  head -> setText             ( 1 , tr("Size")      )                         ;
  head -> setText             ( 2 , ""              )                         ;
  for (int i=0;i<3;i++)                                                       {
    head -> setTextAlignment  ( i , Qt::AlignCenter )                         ;
    head -> setFont           ( i , f               )                         ;
  }                                                                           ;
  ui->Files->setHeaderItem    ( head                )                         ;
  /////////////////////////////////////////////////////////////////////////////
  head = new QTreeWidgetItem  (                     )                         ;
  head -> setText             ( 0 , tr("Transfer")  )                         ;
  head -> setTextAlignment    ( 0 , Qt::AlignCenter )                         ;
  head -> setFont             ( 0 , f               )                         ;
  ui->Transfer->setHeaderItem ( head                )                         ;
  /////////////////////////////////////////////////////////////////////////////
  head = new QTreeWidgetItem  (                     )                         ;
  head -> setText             ( 0 , tr("Profile")   )                         ;
  head -> setTextAlignment    ( 0 , Qt::AlignCenter )                         ;
  head -> setFont             ( 0 , f               )                         ;
  ui->FtpDepot->setHeaderItem ( head                )                         ;
  /////////////////////////////////////////////////////////////////////////////
  addAction (  0 , tr( "Back"       ) , QIcon(":/icons/cdup.png"      ) )     ;
  addAction (  1 , tr( "Connect"    ) , QIcon(":/icons/ftp.png"       ) )     ;
  addAction (  2 , tr( "Ftp"        ) , QIcon(":/icons/ftp.png"       ) )     ;
  addAction (  3 , tr( "Http"       ) , QIcon(":/images/bookmarks.png") )     ;
  addAction (  4 , tr( "Files"      ) , QIcon(":/icons/list.png"      ) )     ;
  addAction (  5 , tr( "Edit"       ) , QIcon(":/icons/edit.png"      ) )     ;
  addAction (  6 , tr( "Disconnect" ) , QIcon(":/icons/disconnect.png") )     ;
  addAction (  7 , tr( "Up"         ) , QIcon(":/icons/cdup.png"      ) )     ;
  addAction (  8 , tr( "cd"         ) , QIcon(":/icons/next.png"      ) )     ;
  addAction (  9 , tr( "Download"   ) , QIcon(":/icons/download.png"  ) )     ;
  addAction ( 10 , tr( "Upload"     ) , QIcon(":/icons/upload.png"    ) )     ;
  addAction ( 11 , tr( "Delete"     ) , QIcon(":/icons/remove.png"    ) )     ;
  addAction ( 12 , tr( "Create"     ) , QIcon(":/icons/mkdir.png"     ) )     ;
  addAction ( 13 , tr( "Create"     ) , QIcon(":/icons/mkdir.png"     ) )     ;
  addAction ( 14 , tr( "Download"   ) , QIcon(":/icons/download.png"  ) )     ;
  addAction ( 15 , tr( "Pick"       ) , QIcon(":/icons/join.png"      ) )     ;
  addAction ( 16 , tr( "Remove"     ) , QIcon(":/icons/remove.png"    ) )     ;
  addAction ( 17 , tr( "Clear"      ) , QIcon(":/icons/delete.png"    ) )     ;
  addAction ( 18 , tr( "Upload"     ) , QIcon(":/icons/upload.png"    ) )     ;
  addAction ( 19 , tr( "Download"   ) , QIcon(":/icons/download.png"  ) )     ;
  addAction ( 20 , tr( "Logs"       ) , QIcon(":/icons/text.png"      ) )     ;
  addAction ( 21 , tr( "Stop"       ) , QIcon(":/icons/remove.png"    ) )     ;
  addAction ( 22 , tr( "Progress"   ) , QIcon(":/icons/progress.png"  ) )     ;
  addAction ( 23 , tr( "Logs"       ) , QIcon(":/icons/text.png"      ) )     ;
  addAction ( 24 , tr( "Sites"      ) , QIcon(":/icons/sites.png"     ) )     ;
  addAction ( 25 , tr( "New"        ) , QIcon(":/icons/new.png"       ) )     ;
  addAction ( 26 , tr( "Profiles"   ) , QIcon(":/icons/sites.png"     ) )     ;
  addAction ( 27 , tr( "Load"       ) , QIcon(":/icons/load.png"      ) )     ;
  addAction ( 28 , tr( "Save"       ) , QIcon(":/icons/save.png"      ) )     ;
  addAction ( 29 , tr( "Delete"     ) , QIcon(":/icons/remove.png"    ) )     ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 0 ] [  0 ] = true                                                ;
  Displays [ 0 ] [  1 ] = true                                                ;
  Displays [ 0 ] [  2 ] = false                                               ;
  Displays [ 0 ] [  3 ] = true                                                ;
  Displays [ 0 ] [  4 ] = true                                                ;
  Displays [ 0 ] [  5 ] = false                                               ;
  Displays [ 0 ] [  6 ] = false                                               ;
  Displays [ 0 ] [  7 ] = false                                               ;
  Displays [ 0 ] [  8 ] = false                                               ;
  Displays [ 0 ] [  9 ] = false                                               ;
  Displays [ 0 ] [ 10 ] = false                                               ;
  Displays [ 0 ] [ 11 ] = false                                               ;
  Displays [ 0 ] [ 12 ] = false                                               ;
  Displays [ 0 ] [ 13 ] = false                                               ;
  Displays [ 0 ] [ 14 ] = false                                               ;
  Displays [ 0 ] [ 15 ] = false                                               ;
  Displays [ 0 ] [ 16 ] = false                                               ;
  Displays [ 0 ] [ 17 ] = false                                               ;
  Displays [ 0 ] [ 18 ] = false                                               ;
  Displays [ 0 ] [ 19 ] = false                                               ;
  Displays [ 0 ] [ 20 ] = false                                               ;
  Displays [ 0 ] [ 21 ] = false                                               ;
  Displays [ 0 ] [ 22 ] = false                                               ;
  Displays [ 0 ] [ 23 ] = false                                               ;
  Displays [ 0 ] [ 24 ] = true                                                ;
  Displays [ 0 ] [ 25 ] = false                                               ;
  Displays [ 0 ] [ 26 ] = false                                               ;
  Displays [ 0 ] [ 27 ] = false                                               ;
  Displays [ 0 ] [ 28 ] = false                                               ;
  Displays [ 0 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 1 ] [  0 ] = false                                               ;
  Displays [ 1 ] [  1 ] = false                                               ;
  Displays [ 1 ] [  2 ] = false                                               ;
  Displays [ 1 ] [  3 ] = false                                               ;
  Displays [ 1 ] [  4 ] = false                                               ;
  Displays [ 1 ] [  5 ] = true                                                ;
  Displays [ 1 ] [  6 ] = true                                                ;
  Displays [ 1 ] [  7 ] = true                                                ;
  Displays [ 1 ] [  8 ] = true                                                ;
  Displays [ 1 ] [  9 ] = true                                                ;
  Displays [ 1 ] [ 10 ] = true                                                ;
  Displays [ 1 ] [ 11 ] = true                                                ;
  Displays [ 1 ] [ 12 ] = true                                                ;
  Displays [ 1 ] [ 13 ] = false                                               ;
  Displays [ 1 ] [ 14 ] = false                                               ;
  Displays [ 1 ] [ 15 ] = false                                               ;
  Displays [ 1 ] [ 16 ] = false                                               ;
  Displays [ 1 ] [ 17 ] = false                                               ;
  Displays [ 1 ] [ 18 ] = false                                               ;
  Displays [ 1 ] [ 19 ] = false                                               ;
  Displays [ 1 ] [ 20 ] = true                                                ;
  Displays [ 1 ] [ 21 ] = false                                               ;
  Displays [ 1 ] [ 22 ] = false                                               ;
  Displays [ 1 ] [ 23 ] = false                                               ;
  Displays [ 1 ] [ 24 ] = false                                               ;
  Displays [ 1 ] [ 25 ] = false                                               ;
  Displays [ 1 ] [ 26 ] = false                                               ;
  Displays [ 1 ] [ 27 ] = false                                               ;
  Displays [ 1 ] [ 28 ] = false                                               ;
  Displays [ 1 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 2 ] [  0 ] = false                                               ;
  Displays [ 2 ] [  1 ] = false                                               ;
  Displays [ 2 ] [  2 ] = true                                                ;
  Displays [ 2 ] [  3 ] = false                                               ;
  Displays [ 2 ] [  4 ] = false                                               ;
  Displays [ 2 ] [  5 ] = false                                               ;
  Displays [ 2 ] [  6 ] = false                                               ;
  Displays [ 2 ] [  7 ] = false                                               ;
  Displays [ 2 ] [  8 ] = false                                               ;
  Displays [ 2 ] [  9 ] = false                                               ;
  Displays [ 2 ] [ 10 ] = false                                               ;
  Displays [ 2 ] [ 11 ] = false                                               ;
  Displays [ 2 ] [ 12 ] = false                                               ;
  Displays [ 2 ] [ 13 ] = true                                                ;
  Displays [ 2 ] [ 14 ] = false                                               ;
  Displays [ 2 ] [ 15 ] = false                                               ;
  Displays [ 2 ] [ 16 ] = false                                               ;
  Displays [ 2 ] [ 17 ] = false                                               ;
  Displays [ 2 ] [ 18 ] = false                                               ;
  Displays [ 2 ] [ 19 ] = false                                               ;
  Displays [ 2 ] [ 20 ] = false                                               ;
  Displays [ 2 ] [ 21 ] = false                                               ;
  Displays [ 2 ] [ 22 ] = false                                               ;
  Displays [ 2 ] [ 23 ] = false                                               ;
  Displays [ 2 ] [ 24 ] = false                                               ;
  Displays [ 2 ] [ 25 ] = false                                               ;
  Displays [ 2 ] [ 26 ] = false                                               ;
  Displays [ 2 ] [ 27 ] = false                                               ;
  Displays [ 2 ] [ 28 ] = false                                               ;
  Displays [ 2 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 3 ] [  0 ] = false                                               ;
  Displays [ 3 ] [  1 ] = false                                               ;
  Displays [ 3 ] [  2 ] = true                                                ;
  Displays [ 3 ] [  3 ] = false                                               ;
  Displays [ 3 ] [  4 ] = false                                               ;
  Displays [ 3 ] [  5 ] = false                                               ;
  Displays [ 3 ] [  6 ] = false                                               ;
  Displays [ 3 ] [  7 ] = false                                               ;
  Displays [ 3 ] [  8 ] = false                                               ;
  Displays [ 3 ] [  9 ] = false                                               ;
  Displays [ 3 ] [ 10 ] = false                                               ;
  Displays [ 3 ] [ 11 ] = false                                               ;
  Displays [ 3 ] [ 12 ] = false                                               ;
  Displays [ 3 ] [ 13 ] = false                                               ;
  Displays [ 3 ] [ 14 ] = true                                                ;
  Displays [ 3 ] [ 15 ] = false                                               ;
  Displays [ 3 ] [ 16 ] = false                                               ;
  Displays [ 3 ] [ 17 ] = false                                               ;
  Displays [ 3 ] [ 18 ] = false                                               ;
  Displays [ 3 ] [ 19 ] = false                                               ;
  Displays [ 3 ] [ 20 ] = true                                                ;
  Displays [ 3 ] [ 21 ] = false                                               ;
  Displays [ 3 ] [ 22 ] = false                                               ;
  Displays [ 3 ] [ 23 ] = false                                               ;
  Displays [ 3 ] [ 24 ] = false                                               ;
  Displays [ 3 ] [ 25 ] = false                                               ;
  Displays [ 3 ] [ 26 ] = false                                               ;
  Displays [ 3 ] [ 27 ] = false                                               ;
  Displays [ 3 ] [ 28 ] = false                                               ;
  Displays [ 3 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 4 ] [  0 ] = false                                               ;
  Displays [ 4 ] [  1 ] = false                                               ;
  Displays [ 4 ] [  2 ] = false                                               ;
  Displays [ 4 ] [  3 ] = false                                               ;
  Displays [ 4 ] [  4 ] = false                                               ;
  Displays [ 4 ] [  5 ] = true                                                ;
  Displays [ 4 ] [  6 ] = false                                               ;
  Displays [ 4 ] [  7 ] = false                                               ;
  Displays [ 4 ] [  8 ] = false                                               ;
  Displays [ 4 ] [  9 ] = false                                               ;
  Displays [ 4 ] [ 10 ] = false                                               ;
  Displays [ 4 ] [ 11 ] = false                                               ;
  Displays [ 4 ] [ 12 ] = false                                               ;
  Displays [ 4 ] [ 13 ] = false                                               ;
  Displays [ 4 ] [ 14 ] = false                                               ;
  Displays [ 4 ] [ 15 ] = true                                                ;
  Displays [ 4 ] [ 16 ] = true                                                ;
  Displays [ 4 ] [ 17 ] = true                                                ;
  Displays [ 4 ] [ 18 ] = false                                               ;
  Displays [ 4 ] [ 19 ] = false                                               ;
  Displays [ 4 ] [ 20 ] = false                                               ;
  Displays [ 4 ] [ 21 ] = false                                               ;
  Displays [ 4 ] [ 22 ] = false                                               ;
  Displays [ 4 ] [ 23 ] = false                                               ;
  Displays [ 4 ] [ 24 ] = false                                               ;
  Displays [ 4 ] [ 25 ] = false                                               ;
  Displays [ 4 ] [ 26 ] = false                                               ;
  Displays [ 4 ] [ 27 ] = false                                               ;
  Displays [ 4 ] [ 28 ] = false                                               ;
  Displays [ 4 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 5 ] [  0 ] = false                                               ;
  Displays [ 5 ] [  1 ] = false                                               ;
  Displays [ 5 ] [  2 ] = true                                                ;
  Displays [ 5 ] [  3 ] = false                                               ;
  Displays [ 5 ] [  4 ] = false                                               ;
  Displays [ 5 ] [  5 ] = false                                               ;
  Displays [ 5 ] [  6 ] = false                                               ;
  Displays [ 5 ] [  7 ] = false                                               ;
  Displays [ 5 ] [  8 ] = false                                               ;
  Displays [ 5 ] [  9 ] = false                                               ;
  Displays [ 5 ] [ 10 ] = false                                               ;
  Displays [ 5 ] [ 11 ] = false                                               ;
  Displays [ 5 ] [ 12 ] = false                                               ;
  Displays [ 5 ] [ 13 ] = false                                               ;
  Displays [ 5 ] [ 14 ] = false                                               ;
  Displays [ 5 ] [ 15 ] = true                                                ;
  Displays [ 5 ] [ 16 ] = true                                                ;
  Displays [ 5 ] [ 17 ] = true                                                ;
  Displays [ 5 ] [ 18 ] = true                                                ;
  Displays [ 5 ] [ 19 ] = false                                               ;
  Displays [ 5 ] [ 20 ] = true                                                ;
  Displays [ 5 ] [ 21 ] = false                                               ;
  Displays [ 5 ] [ 22 ] = false                                               ;
  Displays [ 5 ] [ 23 ] = false                                               ;
  Displays [ 5 ] [ 24 ] = false                                               ;
  Displays [ 5 ] [ 25 ] = false                                               ;
  Displays [ 5 ] [ 26 ] = false                                               ;
  Displays [ 5 ] [ 27 ] = false                                               ;
  Displays [ 5 ] [ 28 ] = false                                               ;
  Displays [ 5 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 6 ] [  0 ] = false                                               ;
  Displays [ 6 ] [  1 ] = false                                               ;
  Displays [ 6 ] [  2 ] = false                                               ;
  Displays [ 6 ] [  3 ] = false                                               ;
  Displays [ 6 ] [  4 ] = false                                               ;
  Displays [ 6 ] [  5 ] = true                                                ;
  Displays [ 6 ] [  6 ] = false                                               ;
  Displays [ 6 ] [  7 ] = false                                               ;
  Displays [ 6 ] [  8 ] = false                                               ;
  Displays [ 6 ] [  9 ] = false                                               ;
  Displays [ 6 ] [ 10 ] = false                                               ;
  Displays [ 6 ] [ 11 ] = false                                               ;
  Displays [ 6 ] [ 12 ] = false                                               ;
  Displays [ 6 ] [ 13 ] = false                                               ;
  Displays [ 6 ] [ 14 ] = false                                               ;
  Displays [ 6 ] [ 15 ] = false                                               ;
  Displays [ 6 ] [ 16 ] = false                                               ;
  Displays [ 6 ] [ 17 ] = false                                               ;
  Displays [ 6 ] [ 18 ] = false                                               ;
  Displays [ 6 ] [ 19 ] = true                                                ;
  Displays [ 6 ] [ 20 ] = false                                               ;
  Displays [ 6 ] [ 21 ] = false                                               ;
  Displays [ 6 ] [ 22 ] = false                                               ;
  Displays [ 6 ] [ 23 ] = false                                               ;
  Displays [ 6 ] [ 24 ] = false                                               ;
  Displays [ 6 ] [ 25 ] = false                                               ;
  Displays [ 6 ] [ 26 ] = false                                               ;
  Displays [ 6 ] [ 27 ] = false                                               ;
  Displays [ 6 ] [ 28 ] = false                                               ;
  Displays [ 6 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 7 ] [  0 ] = false                                               ;
  Displays [ 7 ] [  1 ] = false                                               ;
  Displays [ 7 ] [  2 ] = true                                                ;
  Displays [ 7 ] [  3 ] = false                                               ;
  Displays [ 7 ] [  4 ] = false                                               ;
  Displays [ 7 ] [  5 ] = false                                               ;
  Displays [ 7 ] [  6 ] = false                                               ;
  Displays [ 7 ] [  7 ] = false                                               ;
  Displays [ 7 ] [  8 ] = false                                               ;
  Displays [ 7 ] [  9 ] = false                                               ;
  Displays [ 7 ] [ 10 ] = false                                               ;
  Displays [ 7 ] [ 11 ] = false                                               ;
  Displays [ 7 ] [ 12 ] = false                                               ;
  Displays [ 7 ] [ 13 ] = false                                               ;
  Displays [ 7 ] [ 14 ] = false                                               ;
  Displays [ 7 ] [ 15 ] = false                                               ;
  Displays [ 7 ] [ 16 ] = false                                               ;
  Displays [ 7 ] [ 17 ] = false                                               ;
  Displays [ 7 ] [ 18 ] = false                                               ;
  Displays [ 7 ] [ 19 ] = false                                               ;
  Displays [ 7 ] [ 20 ] = false                                               ;
  Displays [ 7 ] [ 21 ] = false                                               ;
  Displays [ 7 ] [ 22 ] = false                                               ;
  Displays [ 7 ] [ 23 ] = false                                               ;
  Displays [ 7 ] [ 24 ] = false                                               ;
  Displays [ 7 ] [ 25 ] = false                                               ;
  Displays [ 7 ] [ 26 ] = false                                               ;
  Displays [ 7 ] [ 27 ] = false                                               ;
  Displays [ 7 ] [ 28 ] = false                                               ;
  Displays [ 7 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 8 ] [  0 ] = false                                               ;
  Displays [ 8 ] [  1 ] = false                                               ;
  Displays [ 8 ] [  2 ] = false                                               ;
  Displays [ 8 ] [  3 ] = false                                               ;
  Displays [ 8 ] [  4 ] = false                                               ;
  Displays [ 8 ] [  5 ] = false                                               ;
  Displays [ 8 ] [  6 ] = false                                               ;
  Displays [ 8 ] [  7 ] = false                                               ;
  Displays [ 8 ] [  8 ] = false                                               ;
  Displays [ 8 ] [  9 ] = false                                               ;
  Displays [ 8 ] [ 10 ] = false                                               ;
  Displays [ 8 ] [ 11 ] = false                                               ;
  Displays [ 8 ] [ 12 ] = false                                               ;
  Displays [ 8 ] [ 13 ] = false                                               ;
  Displays [ 8 ] [ 14 ] = false                                               ;
  Displays [ 8 ] [ 15 ] = false                                               ;
  Displays [ 8 ] [ 16 ] = false                                               ;
  Displays [ 8 ] [ 17 ] = false                                               ;
  Displays [ 8 ] [ 18 ] = false                                               ;
  Displays [ 8 ] [ 19 ] = false                                               ;
  Displays [ 8 ] [ 20 ] = false                                               ;
  Displays [ 8 ] [ 21 ] = true                                                ;
  Displays [ 8 ] [ 22 ] = true                                                ;
  Displays [ 8 ] [ 23 ] = true                                                ;
  Displays [ 8 ] [ 24 ] = false                                               ;
  Displays [ 8 ] [ 25 ] = false                                               ;
  Displays [ 8 ] [ 26 ] = false                                               ;
  Displays [ 8 ] [ 27 ] = false                                               ;
  Displays [ 8 ] [ 28 ] = false                                               ;
  Displays [ 8 ] [ 29 ] = false                                               ;
  /////////////////////////////////////////////////////////////////////////////
  Displays [ 9 ] [  0 ] = false                                               ;
  Displays [ 9 ] [  1 ] = false                                               ;
  Displays [ 9 ] [  2 ] = false                                               ;
  Displays [ 9 ] [  3 ] = false                                               ;
  Displays [ 9 ] [  4 ] = false                                               ;
  Displays [ 9 ] [  5 ] = true                                                ;
  Displays [ 9 ] [  6 ] = false                                               ;
  Displays [ 9 ] [  7 ] = false                                               ;
  Displays [ 9 ] [  8 ] = false                                               ;
  Displays [ 9 ] [  9 ] = false                                               ;
  Displays [ 9 ] [ 10 ] = false                                               ;
  Displays [ 9 ] [ 11 ] = false                                               ;
  Displays [ 9 ] [ 12 ] = false                                               ;
  Displays [ 9 ] [ 13 ] = false                                               ;
  Displays [ 9 ] [ 14 ] = false                                               ;
  Displays [ 9 ] [ 15 ] = false                                               ;
  Displays [ 9 ] [ 16 ] = false                                               ;
  Displays [ 9 ] [ 17 ] = false                                               ;
  Displays [ 9 ] [ 18 ] = false                                               ;
  Displays [ 9 ] [ 19 ] = false                                               ;
  Displays [ 9 ] [ 20 ] = false                                               ;
  Displays [ 9 ] [ 21 ] = false                                               ;
  Displays [ 9 ] [ 22 ] = false                                               ;
  Displays [ 9 ] [ 23 ] = false                                               ;
  Displays [ 9 ] [ 24 ] = false                                               ;
  Displays [ 9 ] [ 25 ] = true                                                ;
  Displays [ 9 ] [ 26 ] = true                                                ;
  Displays [ 9 ] [ 27 ] = true                                                ;
  Displays [ 9 ] [ 28 ] = true                                                ;
  Displays [ 9 ] [ 29 ] = true                                                ;
  /////////////////////////////////////////////////////////////////////////////
  nConnect ( this      , SIGNAL ( resizeFtp   (                      ) )      ,
             this      , SLOT   ( organizeFtp (                      ) )    ) ;
  nConnect ( this      , SIGNAL ( BackTo      (                      ) )      ,
             this      , SLOT   ( Edit        (                      ) )    ) ;
  nConnect ( this      , SIGNAL ( BackDownload(                      ) )      ,
             this      , SLOT   ( ftpDownload (                      ) )    ) ;
  nConnect ( this      , SIGNAL ( BackUpload  (                      ) )      ,
             this      , SLOT   ( ftpUpload   (                      ) )    ) ;
  nConnect ( this      , SIGNAL ( BackHttp    (QString               ) )      ,
             this      , SLOT   ( HttpDownload(QString               ) )    ) ;
  nConnect ( this      , SIGNAL ( Message     (QString               ) )      ,
             this      , SLOT   ( Logging     (QString               ) )    ) ;
  nConnect ( this      , SIGNAL ( ListUrlInfos(                      ) )      ,
             this      , SLOT   ( UrlInfos    (                      ) )    ) ;
  nConnect ( this      , SIGNAL ( DownloadFile    ( QString          ) )      ,
             this      , SLOT   ( AddDownload     ( QString          ) )    ) ;
  nConnect ( this      , SIGNAL ( setFileSize     ( quint64          ) )      ,
             this      , SLOT   ( assignFileSize  ( quint64          ) )    ) ;
  nConnect ( this      , SIGNAL ( setProgress     ( quint64          ) )      ,
             this      , SLOT   ( assignProgress  ( quint64          ) )    ) ;
  nConnect ( ui->Line  , SIGNAL ( returnPressed   (                  ) )      ,
             this      , SLOT   ( CreateDirectory (                  ) )    ) ;
  nConnect ( ui->Ftp   , SIGNAL ( itemClicked ( QTreeWidgetItem*,int ) )      ,
             this      , SLOT   ( ftpClicked  ( QTreeWidgetItem*,int ) )    ) ;
  nConnect ( ui->Files , SIGNAL ( itemClicked ( QTreeWidgetItem*,int ) )      ,
             this      , SLOT   ( fileClicked ( QTreeWidgetItem*,int ) )    ) ;
  /////////////////////////////////////////////////////////////////////////////
  nConnect ( Actions [  0 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SIGNAL ( Back            ( ) )                ) ;
  nConnect ( Actions [  1 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( connectFtp      ( ) )                ) ;
  nConnect ( Actions [  2 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( switchFtp       ( ) )                ) ;
  nConnect ( Actions [  3 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( HttpDownload    ( ) )                ) ;
  nConnect ( Actions [  4 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( BrowseFiles     ( ) )                ) ;
  nConnect ( Actions [  5 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( Edit            ( ) )                ) ;
  nConnect ( Actions [  6 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( disconnectFtp   ( ) )                ) ;
  nConnect ( Actions [  7 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ftpUp           ( ) )                ) ;
  nConnect ( Actions [  8 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ftpCd           ( ) )                ) ;
  nConnect ( Actions [  9 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ftpDownload     ( ) )                ) ;
  nConnect ( Actions [ 10 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ftpUpload       ( ) )                ) ;
  nConnect ( Actions [ 11 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ftpDelete       ( ) )                ) ;
  nConnect ( Actions [ 12 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ftpCreate       ( ) )                ) ;
  nConnect ( Actions [ 13 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( CreateDirectory ( ) )                ) ;
  nConnect ( Actions [ 14 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ActualDownload  ( ) )                ) ;
  nConnect ( Actions [ 15 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SIGNAL ( Picking         ( ) )                ) ;
  nConnect ( Actions [ 16 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( RemoveFiles     ( ) )                ) ;
  nConnect ( Actions [ 17 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ClearFiles      ( ) )                ) ;
  nConnect ( Actions [ 18 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ActualUpload    ( ) )                ) ;
  nConnect ( Actions [ 19 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( FetchHttp       ( ) )                ) ;
  nConnect ( Actions [ 20 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ShowLogs        ( ) )                ) ;
  nConnect ( Actions [ 21 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( StopDownload    ( ) )                ) ;
  nConnect ( Actions [ 22 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( switchProgress  ( ) )                ) ;
  nConnect ( Actions [ 23 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( switchLogs      ( ) )                ) ;
  nConnect ( Actions [ 24 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( Profiles        ( ) )                ) ;
  nConnect ( Actions [ 25 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( NewProfile      ( ) )                ) ;
  nConnect ( Actions [ 26 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( ProfileLists    ( ) )                ) ;
  nConnect ( Actions [ 27 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( LoadProfiles    ( ) )                ) ;
  nConnect ( Actions [ 28 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( SaveProfiles    ( ) )                ) ;
  nConnect ( Actions [ 29 ] , SIGNAL ( triggered       ( ) )                  ,
             this           , SLOT   ( DeleteProfiles  ( ) )                ) ;
  /////////////////////////////////////////////////////////////////////////////
  Timer     . setParent ( this )                                              ;
  HttpTimer . setParent ( this )                                              ;
  nConnect ( &Timer     , SIGNAL ( timeout         ( ) )                      ,
             this       , SLOT   ( RefreshProgress ( ) )                    ) ;
  nConnect ( &HttpTimer , SIGNAL ( timeout         ( ) )                      ,
             this       , SLOT   ( RefreshHttp     ( ) )                    ) ;
  /////////////////////////////////////////////////////////////////////////////
  InitProfiles         (             )                                        ;
  plan    -> setFont   ( ui -> Label )                                        ;
  plan    -> setFont   ( ui -> Edit  )                                        ;
  InitProfiles         (             )                                        ;
  Display              ( 0           )                                        ;
}

void N::PadFtp::addAction(int Id,QString text,QIcon icon)
{
  Actions [ Id ] = new QAction ( icon , text , ui->Tools ) ;
  ui -> Tools    -> addAction  ( Actions [ Id ]          ) ;
  Actions [ Id ] -> setFont    ( ui -> Tools -> font ( ) ) ;
}

void N::PadFtp::Display(int conf)
{
  BMAPs D   = Displays [ conf ]               ;
  CUIDs IDs = D . keys (      )               ;
  int   id                                    ;
  foreach (id,IDs)                            {
    Actions [ id ] -> setVisible ( D [ id ] ) ;
  }                                           ;
}

void N::PadFtp::setAlignments(QTreeWidgetItem * item)
{
  item -> setTextAlignment ( 2 , Qt::AlignRight | Qt::AlignVCenter ) ;
}

void N::PadFtp::ftpClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( column )                                              ;
  int flag = item->data(0,Qt::UserRole).toInt()                    ;
  int type = item->data(1,Qt::UserRole).toInt()                    ;
  flag    ^= 1                                                     ;
  item    -> setData ( 0 , Qt::UserRole , flag )                   ;
  //////////////////////////////////////////////////////////////////
  switch (flag)                                                    {
    case 0                                                         :
      item -> setIcon ( 0 , QIcon(":/icons/empty.png") )           ;
    break                                                          ;
    case 1                                                         :
      item -> setIcon ( 0 , Join                       )           ;
    break                                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  switch (type)                                                    {
    case 0                                                         :
    break                                                          ;
    case 1                                                         :
    break                                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  Alert ( Click )                                                  ;
}

void N::PadFtp::fileClicked(QTreeWidgetItem * item,int column)
{ Q_UNUSED ( column )                                              ;
  int flag = item->data(0,Qt::UserRole).toInt()                    ;
  int type = item->data(1,Qt::UserRole).toInt()                    ;
  flag    ^= 1                                                     ;
  item    -> setData ( 0 , Qt::UserRole , flag )                   ;
  //////////////////////////////////////////////////////////////////
  switch (flag)                                                    {
    case 0                                                         :
      item -> setIcon ( 0 , QIcon(":/icons/empty.png") )           ;
    break                                                          ;
    case 1                                                         :
      item -> setIcon ( 0 , Join                       )           ;
    break                                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  switch (type)                                                    {
    case 0                                                         :
    break                                                          ;
    case 1                                                         :
    break                                                          ;
  }                                                                ;
  //////////////////////////////////////////////////////////////////
  Alert ( Click )                                                  ;
}

void N::PadFtp::AddDownload(QString filename)
{
  NewTreeWidgetItem                      ( it           ) ;
  it   -> setText                        ( 0 , filename ) ;
  ui   -> Transfer -> insertTopLevelItem ( 0 , it       ) ;
  plan -> processEvents                  (              ) ;
}

void N::PadFtp::assignFileSize(quint64 size)
{
  ui   -> Progress -> setRange ( 0 , (int)size ) ;
  plan -> processEvents        (               ) ;
}

void N::PadFtp::assignProgress(quint64 index)
{
  ui   -> Progress -> setValue ( (int)index ) ;
  plan -> processEvents        (            ) ;
}

void N::PadFtp::RefreshProgress(void)
{
  if (IsNull(ftp)) return               ;
  assignProgress ( ftp->ProgressIndex ) ;
  QStringList logs                      ;
  ftp -> LogMutex . lock   (      )     ;
  logs = ftp     -> Loggings            ;
  ftp -> Loggings . clear  (      )     ;
  ftp -> LogMutex . unlock (      )     ;
  QString L = logs.join    ( "\n" )     ;
  QStringList SS = L.split ( "\n" )     ;
  foreach (L,SS)                        {
    L = L . replace   ( "\r" , "" )     ;
    L = L . replace   ( "\n" , "" )     ;
    if (L.length()>0)                   {
      ui -> Logs -> append ( L    )     ;
    }                                   ;
  }                                     ;
}

void N::PadFtp::organizeFtp(void)
{
  QString t                                                ;
  QString s                                                ;
  QString host = ui -> Hostname -> text  ( )               ;
  int     port = ui -> Port     -> value ( )               ;
  //////////////////////////////////////////////////////////
  host = host.replace("ftp://","")                         ;
  t = RemoteDirs.join("/")                                 ;
  s = QString("ftp://%1:%2/%3").arg(host).arg(port).arg(t) ;
  ui -> Message -> setText ( s )                           ;
  for (int i=0;i<4;i++)                                    {
    ui -> Ftp -> resizeColumnToContents ( i )              ;
  }                                                        ;
  ui -> Menu -> setEnabled ( true )                        ;
}

void N::PadFtp::Logging(QString message)
{
  if (TotalLines>2000)              {
    ui -> Logs -> clear (         ) ;
    TotalLines = 0                  ;
  }                                 ;
  TotalLines++                      ;
  ui -> Logs -> append  ( message ) ;
  plan -> processEvents (         ) ;
}

void N::PadFtp::switchFtp(void)
{
  ui -> Label -> setCurrentWidget   ( ui->Message ) ;
  if (NotNull(ftp))                                 {
    ui -> Edit  -> setCurrentWidget ( ui->Ftp     ) ;
    Display                         ( 1           ) ;
  } else                                            {
    ui -> Edit  -> setCurrentWidget ( ui->Login   ) ;
    Display                         ( 0           ) ;
  }                                                 ;
  Alert ( Error )                                   ;
}

void N::PadFtp::Edit(void)
{
  ui -> Label -> setCurrentWidget   ( ui->Message ) ;
  if (IsNull(ftp))                                  {
    ui -> Edit  -> setCurrentWidget ( ui->Login   ) ;
  }                                                 ;
  Display                           ( 0           ) ;
  Alert ( Error )                                   ;
}

void N::PadFtp::InitProfiles(void)
{
  QString Q                             ;
  SqlConnection SC ( plan -> sql )      ;
  if (SC.open("PadFtp","InitProfiles")) {
    if (SC.sql.isMySQL())               {
      Q = "create table `ftpprofiles` (\n"
          "`id` integer primary key auto_increment,\n"
          "`uuid` bigint unsigned not null,\n"
          "`name` blob not null,\n"
          "`body` blob not null,\n"
          "`ltime` timestamp default CURRENT_TIMESTAMP,\n"
          "unique (`uuid`)\n"
          ") ;"                         ;
    } else
    if (SC.sql.isSQLITE())              {
      Q = "create table `ftpprofiles` (\n"
          "`id` integer primary key autoincrement,\n"
          "`uuid` bigint unsigned not null,\n"
          "`name` blob not null,\n"
          "`body` blob not null,\n"
          "`ltime` timestamp default CURRENT_TIMESTAMP,\n"
          "unique (`uuid`)\n"
          ") ;"                         ;
    }                                   ;
    SC . Query  ( Q )                   ;
    SC . close  (   )                   ;
  }                                     ;
  SC   . remove (   )                   ;
}

void N::PadFtp::Profiles(void)
{
  ui -> FtpDepot -> clear            (                )       ;
  ui -> Label    -> setCurrentWidget ( ui -> Message  )       ;
  ui -> Edit     -> setCurrentWidget ( ui -> FtpDepot )       ;
  Display                            ( 9              )       ;
  /////////////////////////////////////////////////////////////
  QString Q                                                   ;
  SqlConnection SC ( plan -> sql )                            ;
  if (SC.open("PadFtp","Profiles"))                           {
    Q = "select uuid,name from ftpprofiles order by id asc ;" ;
    SqlLoopNow  ( SC , Q )                                    ;
      SUID    u = SC . Uuid   ( 0 )                           ;
      QString n = SC . String ( 1 )                           ;
      NewTreeWidgetItem ( it )                                ;
      it -> setText ( 0 , n                )                  ;
      it -> setData ( 0 , Qt::UserRole , u )                  ;
      ui -> FtpDepot -> addTopLevelItem ( it )                ;
    SqlLoopErr  ( SC , Q )                                    ;
    SqlLoopEnd  ( SC , Q )                                    ;
    SC . close  (   )                                         ;
  }                                                           ;
  SC   . remove (   )                                         ;
  /////////////////////////////////////////////////////////////
  Alert ( Error )                                             ;
}

void N::PadFtp::NewProfile(void)
{
  ui -> Edit    -> setCurrentWidget ( ui->Login ) ;
  ui -> Profile -> clear            (           ) ;
  /////////////////////////////////////////////////
  Alert ( Error )                                 ;
}

void N::PadFtp::ProfileLists(void)
{
  ui -> Edit -> setCurrentWidget ( ui->FtpDepot ) ;
  /////////////////////////////////////////////////
  Alert ( Error )                                 ;
}

typedef struct {
  char Hostname [ 256 ] ;
  char Username [ 256 ] ;
  char Password [ 256 ] ;
  int  Port             ;
  int  Anonymous        ;
  char Pending  [ 248 ] ;
} FtpSiteConf           ;

void N::PadFtp::LoadProfiles(void)
{
  QTreeWidgetItem * item                                  ;
  item = ui->FtpDepot->currentItem()                      ;
  if (IsNull(item)) return                                ;
  SUID u = nTreeUuid ( item , 0 )                         ;
  if (u<=0) return                                        ;
  /////////////////////////////////////////////////////////
  QString    Q                                            ;
  QByteArray FSC                                          ;
  SqlConnection SC ( plan -> sql )                        ;
  if (SC.open("PadFtp","Profiles"))                       {
    Q = QString("select body from ftpprofiles where uuid = %1 ;").arg(u) ;
    if (SC.Fetch(Q)) FSC = SC.ByteArray(0)                ;
    SC . close  (   )                                     ;
  }                                                       ;
  SC   . remove (   )                                     ;
  /////////////////////////////////////////////////////////
  if (FSC.size()<=0 || FSC.size() != sizeof(FtpSiteConf)) {
    QString m                                             ;
    m = tr("Ftp configuration is missing.")               ;
    ui -> Message -> setText ( m )                        ;
    return                                                ;
  }                                                       ;
  /////////////////////////////////////////////////////////
  FtpSiteConf * fsc = (FtpSiteConf *)FSC.data()           ;
  ui -> Profile   -> setText    ( item->text(0)         ) ;
  ui -> Hostname  -> setText    ( fsc->Hostname         ) ;
  ui -> Username  -> setText    ( fsc->Username         ) ;
  ui -> Password  -> setText    ( fsc->Password         ) ;
  ui -> Port      -> setValue   ( fsc->Port             ) ;
  ui -> Anonymous -> setChecked ( (fsc->Anonymous==1 )  ) ;
  /////////////////////////////////////////////////////////
  Edit ( )                                                ;
}

void N::PadFtp::SaveProfiles(void)
{
  QString profile = ui->Profile->text()                      ;
  if (profile.length()<=0) return                            ;
  ////////////////////////////////////////////////////////////
  QString hostname = ui->Hostname->text()                    ;
  QString username = ui->Username->text()                    ;
  QString password = ui->Password->text()                    ;
  ////////////////////////////////////////////////////////////
  QString m                                                  ;
  if (hostname.length()<=0 )                                 {
    m   = tr("Ftp site requires a hostname.")                ;
    ui -> Message -> setText ( m )                           ;
    Alert ( Click )                                          ;
    return                                                   ;
  }                                                          ;
  if (!ui->Anonymous->isChecked())                           {
    if (username.length()<=0 )                               {
      m   = tr("Ftp site requires a username.")              ;
      ui -> Message -> setText ( m )                         ;
      Alert ( Click )                                        ;
      return                                                 ;
    }                                                        ;
    if (password.length()<=0 )                               {
      m   = tr("Ftp site requires a password.")              ;
      ui -> Message -> setText ( m )                         ;
      Alert ( Click )                                        ;
      return                                                 ;
    }                                                        ;
  }                                                          ;
  ////////////////////////////////////////////////////////////
  QByteArray    FSC                                          ;
  FtpSiteConf * fsc                                          ;
  FSC . resize ( sizeof(FtpSiteConf) )                       ;
  fsc = (FtpSiteConf *)FSC.data()                            ;
  memset ( fsc , 0 , sizeof(FtpSiteConf) )                   ;
  if (hostname.length()<255)                                 {
    strcpy ( fsc->Hostname , hostname.toUtf8().constData() ) ;
  }                                                          ;
  if (username.length()<255)                                 {
    strcpy ( fsc->Username , username.toUtf8().constData() ) ;
  }                                                          ;
  if (password.length()<255)                                 {
    strcpy ( fsc->Password , password.toUtf8().constData() ) ;
  }                                                          ;
  fsc -> Port      = ui -> Port      -> value     ()         ;
  fsc -> Anonymous = ui -> Anonymous -> isChecked () ? 1 : 0 ;
  ////////////////////////////////////////////////////////////
  QString Q                                                  ;
  SqlConnection SC ( plan -> sql )                           ;
  if (SC.open("PadFtp","Profiles"))                          {
    SUID u = SC.Unique ( "ftpprofiles" , "uuid" , 1322 )     ;
    if (u>0)                                                 {
      Q = SC . sql . InsertInto                              (
            "ftpprofiles"                                    ,
            3                                                ,
            "uuid"                                           ,
            "name"                                           ,
            "body"                                         ) ;
      SC . Prepare ( Q )                                     ;
      SC . Bind    ( "uuid" , u                )             ;
      SC . Bind    ( "name" , profile.toUtf8() )             ;
      SC . Bind    ( "body" , FSC              )             ;
      if ( SC . Exec ( ) )                                   {
        NewTreeWidgetItem ( it )                             ;
        it -> setText ( 0 , profile          )               ;
        it -> setData ( 0 , Qt::UserRole , u )               ;
        ui -> FtpDepot -> addTopLevelItem ( it )             ;
      }                                                      ;
    }                                                        ;
    SC . close  (   )                                        ;
  }                                                          ;
  SC   . remove (   )                                        ;
  ////////////////////////////////////////////////////////////
  ui -> Edit -> setCurrentWidget ( ui->FtpDepot )            ;
  ////////////////////////////////////////////////////////////
  Alert ( Error )                                            ;
}

void N::PadFtp::DeleteProfiles(void)
{
  QTreeWidgetItem * item                                          ;
  item = ui->FtpDepot->currentItem()                              ;
  if (IsNull(item)) return                                        ;
  SUID u = nTreeUuid ( item , 0 )                                 ;
  if (u<=0) return                                                ;
  /////////////////////////////////////////////////////////////////
  QString Q                                                       ;
  SqlConnection SC ( plan -> sql )                                ;
  if (SC.open("PadFtp","Profiles"))                               {
    Q = QString("delete from ftpprofiles where uuid = %1").arg(u) ;
    if ( SC . Query  ( Q ) )                                      {
      int index = ui->FtpDepot->indexOfTopLevelItem(item)         ;
      if (index>=0)                                               {
        ui->FtpDepot->takeTopLevelItem(index)                     ;
        Alert ( Error )                                           ;
      }                                                           ;
    }                                                             ;
    SC . close  (   )                                             ;
  }                                                               ;
  SC   . remove (   )                                             ;
  /////////////////////////////////////////////////////////////////
  Alert ( Error )                                                 ;
}

void N::PadFtp::BrowseFiles(void)
{
  ui -> Label -> setCurrentWidget ( ui->Message ) ;
  ui -> Edit  -> setCurrentWidget ( ui->Files   ) ;
  /////////////////////////////////////////////////
  QString cp = Current . absoluteFilePath ( ""  ) ;
  QString rp                                      ;
  QString m                                       ;
  rp = Root . relativeFilePath     ( cp         ) ;
  m  = tr("Current path : %1").arg ( rp         ) ;
  ui -> Message -> setText         ( m          ) ;
  /////////////////////////////////////////////////
  Display                          ( 4          ) ;
  Alert ( Error )                                 ;
}

void N::PadFtp::RemoveFiles(void)
{
  TreeWidgetItems items                              ;
  ////////////////////////////////////////////////////
  for (int i=0;i<ui->Files->topLevelItemCount();i++) {
    QTreeWidgetItem * it                             ;
    int               tt                             ;
    it = ui->Files->topLevelItem(i)                  ;
    tt = it->data(0,Qt::UserRole).toInt()            ;
    if (1 == tt) items << it                         ;
  }                                                  ;
  ////////////////////////////////////////////////////
  for (int i=0;i<items.count();i++)                  {
    int tt                                           ;
    tt = ui->Files->indexOfTopLevelItem(items[i])    ;
    if (tt>=0) ui->Files->takeTopLevelItem(tt)       ;
  }                                                  ;
  ////////////////////////////////////////////////////
  Alert ( Error )                                    ;
}

void N::PadFtp::ClearFiles(void)
{
  ui -> Files -> clear ( ) ;
  Alert ( Error )          ;
}

void N::PadFtp::DropFiles(QStringList files)
{
  QStringList Exists                                 ;
  ////////////////////////////////////////////////////
  for (int i=0;i<ui->Files->topLevelItemCount();i++) {
    QTreeWidgetItem * it                             ;
    QString           ss                             ;
    it = ui->Files->topLevelItem(i)                  ;
    ss = it->text(0)                                 ;
    ss = Root.absoluteFilePath(ss)                   ;
    if (Exists.contains(ss))                         {
      Exists << ss                                   ;
    }                                                ;
  }                                                  ;
  ////////////////////////////////////////////////////
  AppendFiles ( Root , Exists , files )              ;
  for (int i=0;i<3;i++)                              {
    ui -> Files -> resizeColumnToContents ( i )      ;
  }                                                  ;
  ////////////////////////////////////////////////////
  QString cp = Current . absoluteFilePath ( "" )     ;
  QString rp                                         ;
  QString m                                          ;
  rp = Root . relativeFilePath     ( cp        )     ;
  m  = tr("Current path : %1").arg ( rp        )     ;
  ui -> Message -> setText         ( m         )     ;
  ////////////////////////////////////////////////////
  Alert ( Error )                                    ;
}

void N::PadFtp::AppendFiles(QDir & root,QStringList & Exists,QStringList & newFiles)
{
  QString S                                              ;
  QString rp                                             ;
  rp = root . absoluteFilePath ( "" )                    ;
  ////////////////////////////////////////////////////////
  foreach (S,newFiles) if (!Exists.contains(S))          {
    if (S.contains(rp))                                  {
      QFileInfo F(S)                                     ;
      if (F.isDir())                                     {
        NewTreeWidgetItem ( it )                         ;
        QString sp                                       ;
        sp = Root.relativeFilePath(S)                    ;
        Exists << S                                      ;
        it -> setText ( 0 , sp                         ) ;
        it -> setText ( 1 , tr("Directory")            ) ;
        it -> setIcon ( 0 , QIcon(":/icons/empty.png") ) ;
        it -> setData ( 0 , Qt::UserRole , 0           ) ;
        it -> setData ( 1 , Qt::UserRole , 1           ) ;
        ui -> Files -> addTopLevelItem ( it )            ;
        //////////////////////////////////////////////////
        QDir          D(S)                               ;
        QFileInfoList FIL                                ;
        QStringList   fil                                ;
        FIL = D . entryInfoList ( QDir::Dirs             |
                                  QDir::Files            |
                                  QDir::NoDotAndDotDot   |
                                  QDir::NoSymLinks     ) ;
        for (int i=0;i<FIL.count();i++)                  {
          fil << FIL [ i ] . absoluteFilePath ( )        ;
        }                                                ;
        AppendFiles ( D , Exists , fil )                 ;
      } else                                             {
        NewTreeWidgetItem ( it )                         ;
        QString sp                                       ;
        sp = Root.relativeFilePath(S)                    ;
        Exists << S                                      ;
        it -> setText ( 0 , sp                         ) ;
        it -> setText ( 1 , QString::number(F.size())  ) ;
        it -> setTextAlignment ( 1 , Qt::AlignRight      |
                                     Qt::AlignVCenter  ) ;
        it -> setIcon ( 0 , QIcon(":/icons/empty.png") ) ;
        it -> setData ( 0 , Qt::UserRole , 0           ) ;
        it -> setData ( 1 , Qt::UserRole , 0           ) ;
        ui -> Files -> addTopLevelItem ( it )            ;
      }                                                  ;
    }                                                    ;
  }                                                      ;
}

void N::PadFtp::ShowLogs(void)
{
  ui -> Label -> setCurrentWidget ( ui->Message ) ;
  ui -> Edit  -> setCurrentWidget ( ui->Logs    ) ;
  Display                         ( 7           ) ;
  Alert ( Error )                                 ;
}

void N::PadFtp::RefreshHttp(void)
{
  if (MaxFileLength<0) return                      ;
  ui -> Progress -> setRange ( 0 , MaxFileLength ) ;
  ui -> Progress -> setValue ( FileIndex         ) ;
}

void N::PadFtp::HttpDownload(QString message)
{
  ui -> Message ->setText          ( message     ) ;
  ui -> Label -> setCurrentWidget  ( ui->Message ) ;
  ui -> Edit  -> setCurrentWidget  ( ui->Http    ) ;
  ui -> Menu  -> setEnabled        ( true        ) ;
  Display                          ( 6           ) ;
  HttpTimer . stop                 (             ) ;
  Alert ( Error )                                  ;
}

void N::PadFtp::HttpDownload(void)
{
  QString cp = Current . absoluteFilePath ( "" ) ;
  QString rp                                     ;
  QString m                                      ;
  rp = Root . relativeFilePath     ( cp        ) ;
  m  = tr("Current path : %1").arg ( rp        ) ;
  HttpDownload                     ( m         ) ;
}

void N::PadFtp::FetchHttp(void)
{
  QString m = ui->URL->text()                       ;
  QUrl u ( m )                                      ;
  if (!u.isValid())                                 {
    m = tr("Download address is not valid.")        ;
    ui -> Message -> setText ( m )                  ;
    Alert ( Click )                                 ;
    return                                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  QString p = ui->LocalFile->text()                 ;
  p = Current.absoluteFilePath ( p )                ;
  QFileInfo F(p)                                    ;
  QDir      D = F . absoluteDir ( )                 ;
  if (!D.exists())                                  {
    m = tr("Local file directory does not exists.") ;
    ui -> Message -> setText ( m )                  ;
    Alert ( Click )                                 ;
    return                                          ;
  }                                                 ;
  ///////////////////////////////////////////////////
  ui -> Menu  -> setEnabled       ( false        )  ;
  ui -> Label -> setCurrentWidget ( ui->Progress )  ;
  ///////////////////////////////////////////////////
  MaxFileLength = -1                                ;
  FileIndex     = 0                                 ;
  HttpTimer . start ( 200 )                         ;
  ///////////////////////////////////////////////////
  start ( 10008 )                                   ;
}

void N::PadFtp::HttpFetch(void)
{
  QString address  = ui -> URL       -> text ( )            ;
  QString filename = ui -> LocalFile -> text ( )            ;
  QUrl    url ( address )                                   ;
  ///////////////////////////////////////////////////////////
  filename = Current . absoluteFilePath ( filename )        ;
  ///////////////////////////////////////////////////////////
  Ftp        FTP                                            ;
  HttpParser HR                                             ;
  QString    Agent      = XML::UserAgents[rand()%40]        ;
  QByteArray Header                                         ;
  QByteArray Body                                           ;
  int        dlTime     = 10 * 60 * 1000                    ;
  int        cnTime     = 60 * 1000                         ;
  FTP.connectionTimeout = cnTime                            ;
  ///////////////////////////////////////////////////////////
  Header . clear ( )                                        ;
  Body   . clear ( )                                        ;
  ///////////////////////////////////////////////////////////
  FTP.Requests["user-agent"] = Agent                        ;
  Header = FTP . header ( url , cnTime )                    ;
  int sc = 0                                                ;
  if (Header.size()>0)                                      {
    HR . setHeader     ( Header )                           ;
    sc = HR.statusCode (        )                           ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  if ( ( sc == 200 ) && HR.hasContentLength())              {
    FileIndex     = 0                                       ;
    MaxFileLength = HR.contentLength()                      ;
    FTP.Written   = &FileIndex                              ;
  } else                                                    {
    MaxFileLength = -1                                      ;
    FileIndex     = 0                                       ;
  }                                                         ;
  QFile::remove(filename)                                   ;
  ///////////////////////////////////////////////////////////
  QString result = ""                                       ;
  QFile F(filename)                                         ;
  FTP.Requests["user-agent"] = Agent                        ;
  if (FTP . download ( url , F , dlTime ) )                 {
    result = tr("Download successful")                      ;
  } else                                                    {
    QFile :: remove ( filename )                            ;
    result = tr("Please check your address by web browser") ;
  }                                                         ;
  ///////////////////////////////////////////////////////////
  emit BackHttp ( result )                                  ;
}

void N::PadFtp::connectFtp(void)
{
  QString host      = ui -> Hostname  -> text      ( )           ;
  int     port      = ui -> Port      -> value     ( )           ;
  if (host.length()<=0) return                                   ;
  bool    anonymous = ui -> Anonymous -> isChecked ( )           ;
  QString username  = ui -> Username  -> text      ( )           ;
  QString password  = ui -> Password  -> text      ( )           ;
  bool    correct   = false                                      ;
  ////////////////////////////////////////////////////////////////
  host = host.replace("ftp://","")                               ;
  if (anonymous) correct = true; else                            {
    correct = true                                               ;
    if (username.length()<=0) correct = false                    ;
    if (password.length()<=0) correct = false                    ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  if (!correct)                                                  {
    Alert ( Menu )                                               ;
    return                                                       ;
  }                                                              ;
  ////////////////////////////////////////////////////////////////
  QString s                                                      ;
  s    = QString ( "ftp://%1:%2" ) . arg ( host ) . arg ( port ) ;
  ui -> Message -> setText          ( s             )            ;
  ui -> Ftp     -> clear            (               )            ;
  ui -> Label   -> setCurrentWidget ( ui -> Message )            ;
  ui -> Edit    -> setCurrentWidget ( ui -> Ftp     )            ;
  Display                           ( 1             )            ;
  ////////////////////////////////////////////////////////////////
  Displays [ 0 ] [  1 ] = false                                  ;
  Displays [ 0 ] [  2 ] = true                                   ;
  RemoteDirs            . clear ( )                              ;
  ////////////////////////////////////////////////////////////////
  ftp   = new FtpClient    ( this                              ) ;
  ////////////////////////////////////////////////////////////////
  nConnect ( ftp  , SIGNAL ( DataProgress   (quint64))           ,
             this , SLOT   ( assignProgress (quint64))         ) ;
  nConnect ( ftp  , SIGNAL ( Message        (QString))           ,
             this , SLOT   ( Logging        (QString))         ) ;
  ////////////////////////////////////////////////////////////////
  plan -> processEvents    (                                   ) ;
  ui -> Menu -> setEnabled ( false                             ) ;
  start                    ( 10001                             ) ;
}

void N::PadFtp::disconnectFtp(void)
{
  if (IsNull(ftp)) return                         ;
  ftp -> Quit ( )                                 ;
  delete ftp                                      ;
  ftp  = NULL                                     ;
  Displays [ 0 ] [  1 ] = true                    ;
  Displays [ 0 ] [  2 ] = false                   ;
  ui -> Label -> setCurrentWidget ( ui->Message ) ;
  ui -> Edit  -> setCurrentWidget ( ui->Login   ) ;
  Display                         ( 0           ) ;
  Alert ( Error )                                 ;
}

void N::PadFtp::FtpUp(void)
{
  QString cdUp = ".."             ;
  int index = RemoteDirs.count()  ;
  if (index<=0)                   {
    emit resizeFtp ( )            ;
    return                        ;
  }                               ;
  index--                         ;
  RemoteDirs . takeAt ( index )   ;
  ftp -> ChangeDirectory ( cdUp ) ;
  ListFiles              (      ) ;
}

void N::PadFtp::ftpUp(void)
{
  if (IsNull(ftp)) return            ;
  ui -> Ftp -> clear ( )             ;
  ui -> Menu -> setEnabled ( false ) ;
  start ( 10002 )                    ;
}

void N::PadFtp::FtpCd(void)
{
  if (IsNull(ftp)) return        ;
  int index = RemoteDirs.count() ;
  if (index<=0)                  {
    emit resizeFtp ( )           ;
    return                       ;
  }                              ;
  index--                        ;
  QString cd = RemoteDirs[index] ;
  ftp -> ChangeDirectory ( cd  ) ;
  ListFiles              (     ) ;
}

void N::PadFtp::ftpCd(void)
{
  if (IsNull(ftp)) return                           ;
  QTreeWidgetItem * item                            ;
  item = ui->Ftp->currentItem()                     ;
  if (IsNull(item)) return                          ;
  if (item->data(1,Qt::UserRole).toInt()!=1) return ;
  RemoteDirs << item->text(0)                       ;
  ui -> Ftp -> clear ( )                            ;
  ui -> Menu -> setEnabled ( false )                ;
  start ( 10003 )                                   ;
}

void N::PadFtp::FtpDelete(void)
{
  if (IsNull(ftp)) return          ;
  QString k                        ;
  foreach (k,Deletion)             {
    bool d = DeleteDirs[k]         ;
    QString m                      ;
    m = tr("Delete %1").arg(k)     ;
    emit Message ( m )             ;
    if (d)                         {
      ftp -> RemoveDirectory ( k ) ;
    } else                         {
      ftp -> Delete          ( k ) ;
    }                              ;
  }                                ;
  ListFiles ( )                    ;
}

void N::PadFtp::ftpDelete(void)
{
  if (IsNull(ftp)) return                           ;
  ///////////////////////////////////////////////////
  Deletion   . clear ( )                            ;
  DeleteDirs . clear ( )                            ;
  for (int i=0;i<ui->Ftp->topLevelItemCount();i++)  {
    QTreeWidgetItem * item                          ;
    item = ui->Ftp->topLevelItem(i)                 ;
    if (item->data(0,Qt::UserRole).toInt()==1)      {
      int type = item->data(1,Qt::UserRole).toInt() ;
      QString n = item->text(0)                     ;
      Deletion  << n                                ;
      DeleteDirs [ n ] = ( 1 == type )              ;
    }                                               ;
  }                                                 ;
  if (Deletion.count()<=0) return                   ;
  ///////////////////////////////////////////////////
  ui -> Ftp -> clear ( )                            ;
  ui -> Menu -> setEnabled ( false )                ;
  start ( 10004 )                                   ;
}

void N::PadFtp::Upload(QString filename,QString remoteFile,quint64 size)
{
  emit DownloadFile ( remoteFile     ) ;
  QFile F           ( filename       ) ;
  ftp -> ProgressIndex = 0             ;
  emit setFileSize  ( size           ) ;
  ftp -> Put        ( F , remoteFile ) ;
}

void N::PadFtp::Upload(QDir & dir,QString pwd,QStringList & files)
{
  QStringList L = files                  ;
  QStringList D                          ;
  QStringList F                          ;
  QStringList R                          ;
  QString     S                          ;
  ////////////////////////////////////////
  if (!ContinueDownload) return          ;
  if (! ftp -> ChangeDirectory ( pwd ) ) {
    return                               ;
  }                                      ;
  ////////////////////////////////////////
  foreach (S,L)                          {
    QString n                            ;
    n = dir.relativeFilePath(S)          ;
    if (n.contains('/'))                 {
      D << S                             ;
    } else                               {
      F << S                             ;
      R << n                             ;
    }                                    ;
    if (!ContinueDownload) return        ;
  }                                      ;
  ////////////////////////////////////////
  if (F.count()>0)                       {
    for (int i=0;i<F.count();i++)        {
      QString   f = F[i]                 ;
      QString   r = R[i]                 ;
      QFileInfo O ( f )                  ;
      if (O.isDir())                     {
        ftp -> MakeDirectory ( r )       ;
      } else                             {
        int z = O.size()                 ;
        Upload ( f , r, z )              ;
      }                                  ;
      if (!ContinueDownload) return      ;
    }                                    ;
  }                                      ;
  ////////////////////////////////////////
  if (!ContinueDownload) return          ;
  if (D.count()<=0) return               ;
  ////////////////////////////////////////
  QStringList DIRs                       ;
  foreach (S,D)                          {
    QString n                            ;
    n = dir.relativeFilePath(S)          ;
    if (n.contains('/'))                 {
      QStringList B                      ;
      B = n.split('/')                   ;
      if (B.count()>0)                   {
        QString t = B[0]                 ;
        if (!DIRs.contains(t))           {
          DIRs << t                      ;
        }                                ;
      }                                  ;
    }                                    ;
    if (!ContinueDownload) return        ;
  }                                      ;
  ////////////////////////////////////////
  foreach (S,DIRs)                       {
    QDir    X = dir                      ;
    QString xp                           ;
    QString z                            ;
    X  . cd                   ( S  )     ;
    xp = X . absoluteFilePath ( "" )     ;
    F  . clear                (    )     ;
    foreach (z,D)                        {
      if (z.contains(xp))                {
        F << z                           ;
      } else                             {
        R << z                           ;
      }                                  ;
    }                                    ;
    D = R                                ;
    if (F.count()>0)                     {
      QString p                          ;
      p  = pwd                           ;
      p += '/'                           ;
      p += S                             ;
      Upload ( X , p , F )               ;
    }                                    ;
    if (!ContinueDownload) return        ;
  }                                      ;
}

void N::PadFtp::FtpUpload(void)
{
  if (IsNull(ftp)) return                            ;
  ////////////////////////////////////////////////////
  QStringList files                                  ;
  for (int i=0;i<ui->Files->topLevelItemCount();i++) {
    QTreeWidgetItem * item                           ;
    QString           S                              ;
    item = ui->Files->topLevelItem(i)                ;
    S    = item -> text             ( 0 )            ;
    S    = Root  . absoluteFilePath ( S )            ;
    QFileInfo F ( S )                                ;
    if (F.exists(S)) files << S                      ;
  }                                                  ;
  ////////////////////////////////////////////////////
  QStringList CurrentFiles                           ;
  QStringList RootFiles                              ;
  QString cp = Current . absoluteFilePath ( "" )     ;
  QString S                                          ;
  foreach (S,files)                                  {
    if (S.contains(cp))                              {
      CurrentFiles << S                              ;
    } else                                           {
      RootFiles    << S                              ;
    }                                                ;
  }                                                  ;
  ////////////////////////////////////////////////////
  QString pwd                                        ;
  pwd = ftp ->PWD()                                  ;
  ContinueDownload  = true                           ;
  ftp -> UserAnswer = 1                              ;
  ////////////////////////////////////////////////////
  if (ContinueDownload)                              {
    Upload ( Current , pwd , CurrentFiles )          ;
  }                                                  ;
  if (ContinueDownload)                              {
    Upload ( Root    , pwd , RootFiles    )          ;
  }                                                  ;
  ////////////////////////////////////////////////////
  emit BackUpload ( )                                ;
}

void N::PadFtp::ActualUpload(void)
{
  if (IsNull(ftp)) return                            ;
  if (ui->Files->topLevelItemCount()<=0) return      ;
  ////////////////////////////////////////////////////
  ui -> Transfer -> clear         (                ) ;
  ui -> Logs     -> clear         (                ) ;
  ui -> Label -> setCurrentWidget ( ui -> Progress ) ;
  ui -> Edit  -> setCurrentWidget ( ui -> Transfer ) ;
  Actions [ 21 ] -> setEnabled    ( true           ) ;
  Display ( 8 )                                      ;
  Timer        . start            ( 200            ) ;
  start ( 10007 )                                    ;
}

void N::PadFtp::ftpUpload(void)
{
  QString cp = Current . absoluteFilePath ( ""    ) ;
  QString rp                                        ;
  rp  = Root     . relativeFilePath ( cp          ) ;
  ui -> Message -> setText          ( rp          ) ;
  ui -> Label   -> setCurrentWidget ( ui->Message ) ;
  ui -> Edit    -> setCurrentWidget ( ui->Files   ) ;
  ContinueDownload = true                           ;
  Display                           ( 5           ) ;
  Alert ( Error )                                   ;
}

void N::PadFtp::Download(QString filename,QString remoteFile,quint64 size)
{
  emit DownloadFile ( remoteFile     ) ;
  QFile F           ( filename       ) ;
  ftp -> ProgressIndex = 0             ;
  emit setFileSize  ( size           ) ;
  ftp -> Get        ( F , remoteFile ) ;
}

void N::PadFtp::Download(QDir & dir,QStringList & files,LMAPs & Dirs,ZMAPs & Sizes)
{
  QString S                                 ;
  QString P = ftp ->PWD()                   ;
  QString U = ui -> Username -> text ( )    ;
  foreach (S,files)                         {
    if (!ContinueDownload) return           ;
    ftp -> ChangeDirectory ( P )            ;
    if (Dirs[S])                            {
      QDir D = dir                          ;
      D . mkdir ( S )                       ;
      if ( D . cd ( S ) )                   {
        if ( ftp -> ChangeDirectory(S) )    {
          QList<QUrlInfo> F                 ;
          ///////////////////////////////////
          if (ftp->List("",F,U))            {
            if (F.count()>0)                {
              QStringList L                 ;
              LMAPs       A                 ;
              ZMAPs       B                 ;
              for (int i=0;i<F.count();i++) {
                QString n                   ;
                n = F[i].name()             ;
                L << n                      ;
                A  [ n ] = F[i] . isDir ( ) ;
                B  [ n ] = F[i] . size  ( ) ;
              }                             ;
              Download ( D , L , A , B )    ;
            }                               ;
          }                                 ;
        }                                   ;
      }                                     ;
    } else                                  {
      QString filename                      ;
      quint64 size                          ;
      filename = dir.absoluteFilePath(S)    ;
      size     = Sizes[S]                   ;
      Download ( filename , S , size )      ;
    }                                       ;
  }                                         ;
}

void N::PadFtp::StopDownload(void)
{
  ContinueDownload = false               ;
  Actions [ 21 ] -> setEnabled ( false ) ;
  Alert ( Error )                        ;
}

void N::PadFtp::switchProgress(void)
{
  ui -> Edit -> setCurrentWidget ( ui->Transfer ) ;
  Alert ( Error )                                 ;
}

void N::PadFtp::switchLogs(void)
{
  ui -> Edit -> setCurrentWidget ( ui->Logs ) ;
  Alert ( Error )                             ;
}

void N::PadFtp::FtpDownload(void)
{
  if (IsNull(ftp)) return            ;
  QDir        D     = Current        ;
  QStringList DL    = Downloads      ;
  LMAPs       DD    = DownloadsDirs  ;
  ZMAPs       DS    = DownloadsSizes ;
  ContinueDownload  = true           ;
  ftp -> UserAnswer = 1              ;
  Download ( D , DL , DD , DS )      ;
  Timer . stop ( )                   ;
  emit BackDownload ( )              ;
}

void N::PadFtp::ActualDownload(void)
{
  if (IsNull(ftp)) return                            ;
  ////////////////////////////////////////////////////
  Downloads  . clear ( )                             ;
  DeleteDirs . clear ( )                             ;
  for (int i=0;i<ui->Ftp->topLevelItemCount();i++)   {
    QTreeWidgetItem * item                           ;
    item = ui->Ftp->topLevelItem(i)                  ;
    if (item->data(0,Qt::UserRole).toInt()==1)       {
      int type = item->data(1,Qt::UserRole).toInt()  ;
      QString n = item->text(0)                      ;
      Downloads    << n                              ;
      DownloadsDirs [ n ] = ( 1 == type )            ;
      if ( 0 == type )                               {
        DownloadsSizes [ n ] = item->text(2).toInt() ;
      } else                                         {
        DownloadsSizes [ n ] = 0                     ;
      }                                              ;
    }                                                ;
  }                                                  ;
  if (Downloads.count()<=0) return                   ;
  ////////////////////////////////////////////////////
  ui -> Transfer -> clear         (                ) ;
  ui -> Logs     -> clear         (                ) ;
  ui -> Label -> setCurrentWidget ( ui -> Progress ) ;
  ui -> Edit  -> setCurrentWidget ( ui -> Transfer ) ;
  Actions [ 21 ] -> setEnabled    ( true           ) ;
  Display ( 8 )                                      ;
  Timer        . start            ( 200            ) ;
  start ( 10006 )                                    ;
}

void N::PadFtp::ftpDownload(void)
{
  ui -> Label -> setCurrentWidget         ( ui -> Message ) ;
  ui -> Edit  -> setCurrentWidget         ( ui -> Ftp     ) ;
  Display                                 ( 3             ) ;
  QString cp = Current . absoluteFilePath ( ""            ) ;
  QString rp                                                ;
  rp = Root.relativeFilePath              ( cp            ) ;
  ui -> Message -> setText                ( rp            ) ;
  ContinueDownload = true                                   ;
  Alert ( Error )                                           ;
}

void N::PadFtp::FtpCreate(void)
{
  if (IsNull(ftp)) return    ;
  QString n                  ;
  n = ui -> Line -> text ( ) ;
  if (n.length()>0)          {
    ftp->MakeDirectory(n)    ;
  }                          ;
  ListFiles ( )              ;
}

void N::PadFtp::CreateDirectory(void)
{
  if (IsNull(ftp)) return                           ;
  QString n                                         ;
  n = ui -> Line -> text ( )                        ;
  ui -> Label -> setCurrentWidget ( ui -> Message ) ;
  Display ( 1 )                                     ;
  if (n.length()<=0) return                         ;
  ui -> Menu -> setEnabled ( false )                ;
  start ( 10005 )                                   ;
}

void N::PadFtp::ftpCreate(void)
{
  ui -> Label -> setCurrentWidget ( ui->Line           ) ;
  ui -> Line  -> blockSignals     ( true               ) ;
  ui -> Line  -> setText          ( ""                 ) ;
  ui -> Line  -> blockSignals     ( false              ) ;
  Display                         ( 2                  ) ;
  ui -> Line  -> setFocus         ( Qt::TabFocusReason ) ;
  Alert ( Error )                                        ;
}

void N::PadFtp::connectTo(void)
{
  QString host      = ui -> Hostname  -> text      ( ) ;
  int     port      = ui -> Port      -> value     ( ) ;
  bool    anonymous = ui -> Anonymous -> isChecked ( ) ;
  QString username  = ui -> Username  -> text      ( ) ;
  QString password  = ui -> Password  -> text      ( ) ;
  //////////////////////////////////////////////////////
  host = host.replace("ftp://","")                     ;
  //////////////////////////////////////////////////////
  if ( ! ftp -> connectFtp ( host , port ) )           {
    delete ftp                                         ;
    ftp = NULL                                         ;
    Displays [ 0 ] [  1 ] = true                       ;
    Displays [ 0 ] [  2 ] = false                      ;
    emit resizeFtp ( )                                 ;
    emit BackTo    ( )                                 ;
    return                                             ;
  }                                                    ;
  //////////////////////////////////////////////////////
  if (anonymous)                                       {
    username = "anonymous"                             ;
    password = ""                                      ;
  }                                                    ;
  //////////////////////////////////////////////////////
  if (!ftp->Login(username,password))                  {
    delete ftp                                         ;
    ftp = NULL                                         ;
    Displays [ 0 ] [  1 ] = true                       ;
    Displays [ 0 ] [  2 ] = false                      ;
    emit BackTo ( )                                    ;
    return                                             ;
  }                                                    ;
  //////////////////////////////////////////////////////
  ListFiles ( )                                        ;
}

void N::PadFtp::ListFiles(void)
{
  QString username  = ui -> Username -> text ( ) ;
  ////////////////////////////////////////////////
  Files . clear ( )                              ;
  if (ftp->List("",Files,username))              {
    emit ListUrlInfos ( )                        ;
  }                                              ;
  ////////////////////////////////////////////////
  QString pwd = ftp->PWD()                       ;
  if (pwd.length()>0)                            {
    QString delimiter = "/"                      ;
    if (pwd.contains('/'))                       {
      delimiter = "/"                            ;
    } else
    if (pwd.contains('\\'))                      {
      delimiter = "\\"                           ;
    }                                            ;
    RemoteDirs = pwd . split ( delimiter )       ;
    if (RemoteDirs.count()>0)                    {
      if (RemoteDirs[0].length()<=0)             {
        RemoteDirs . takeAt ( 0 )                ;
      }                                          ;
    }                                            ;
  }                                              ;
  ////////////////////////////////////////////////
  emit resizeFtp ( )                             ;
  Alert ( Error )                                ;
}

void N::PadFtp::UrlInfos(void)
{
  for (int i=0;i<Files.count();i++)                    {
    if (   Files[i].isDir() ) listInfo ( Files [ i ] ) ;
  }                                                    ;
  for (int i=0;i<Files.count();i++)                    {
    if ( ! Files[i].isDir() ) listInfo ( Files [ i ] ) ;
  }                                                    ;
  organizeFtp ( )                                      ;
}

void N::PadFtp::listInfo(const QUrlInfo & url)
{
  if (url.isDir ())                                    {
    NewTreeWidgetItem ( it )                           ;
    it -> setIcon   ( 0 , QIcon(":/icons/empty.png") ) ;
    it -> setText   ( 0 , url.name()                 ) ;
    it -> setData   ( 0 , Qt::UserRole , 0           ) ;
    it -> setText   ( 1 , tr("Directory")            ) ;
    it -> setData   ( 1 , Qt::UserRole , 1           ) ;
    it -> setText   ( 2 , ""                         ) ;
    setAlignments   ( it                             ) ;
    ui   -> Ftp -> addTopLevelItem ( it              ) ;
  } else
  if (url.isFile())                                    {
    NewTreeWidgetItem ( it )                           ;
    it -> setIcon   ( 0 , QIcon(":/icons/empty.png") ) ;
    it -> setText   ( 0 , url.name()                 ) ;
    it -> setData   ( 0 , Qt::UserRole , 0           ) ;
    it -> setText   ( 1 , tr("File")                 ) ;
    it -> setData   ( 1 , Qt::UserRole , 0           ) ;
    it -> setText   ( 2 , QString::number(url.size())) ;
    setAlignments   ( it                             ) ;
    ui   -> Ftp -> addTopLevelItem ( it              ) ;
  }                                                    ;
}

void N::PadFtp::run(int type,ThreadData * data)
{ Q_UNUSED ( data )   ;
  switch (type)       {
    case 10001        :
      connectTo   ( ) ;
    break             ;
    case 10002        :
      FtpUp       ( ) ;
    break             ;
    case 10003        :
      FtpCd       ( ) ;
    break             ;
    case 10004        :
      FtpDelete   ( ) ;
    break             ;
    case 10005        :
      FtpCreate   ( ) ;
    break             ;
    case 10006        :
      FtpDownload ( ) ;
    break             ;
    case 10007        :
      FtpUpload   ( ) ;
    break             ;
    case 10008        :
      HttpFetch   ( ) ;
    break             ;
  }                   ;
}
