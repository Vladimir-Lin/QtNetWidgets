#include <netwidgets.h>

N::Bookmarks:: Bookmarks  ( QWidget * parent , Plan * p )
             : Widget     (           parent ,        p )
             , Object     ( 0 , Types::None             )
             , URL        ( NULL                        )
             , URLs       ( NULL                        )
             , bookmarks  ( NULL                        )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::Bookmarks::~Bookmarks (void)
{
}

bool N::Bookmarks::canStop(void)
{
  if ( IsNull ( bookmarks ) ) return true ;
  return bookmarks -> canStop ( )         ;
}

void N::Bookmarks::Configure (void)
{
  URLs      = new QComboBox      ( this            )             ;
  URL       = new LineEdit       ( URLs , plan     )             ;
  bookmarks = new BookmarkTree   ( this , plan     )             ;
  URLs     -> setLineEdit        ( URL             )             ;
  URLs     -> setMaxVisibleItems ( 30              )             ;
  plan     -> setFont            ( this            )             ;
  URL      -> setToolTip         ( tr("http://"  ) )             ;
  URL      -> setPlaceholderText ( tr("http://"  ) )             ;
  ////////////////////////////////////////////////////////////////
  setAccessibleName ( "N::Bookmarks"                  )          ;
  setObjectName     ( "N::Bookmarks"                  )          ;
  setWindowTitle    ( tr("Bookmarks")                 )          ;
  setWindowIcon     ( QIcon(":/images/bookmarks.png") )          ;
  ////////////////////////////////////////////////////////////////
  nConnect ( bookmarks , SIGNAL(UrlClicked(QUrl,bool))           ,
             this      , SLOT  (UrlClicked(QUrl,bool))         ) ;
  nConnect ( bookmarks , SIGNAL(ImportRules(QString,SUID,int))   ,
             this      , SIGNAL(ImportRules(QString,SUID,int)) ) ;
  nConnect ( bookmarks , SIGNAL(RunSets(VarArgLists&))           ,
             this      , SIGNAL(RunSets(VarArgLists&))         ) ;
  nConnect ( URL       , SIGNAL(returnPressed  ())               ,
             this      , SLOT  (editingFinished())             ) ;
}

void N::Bookmarks::resizeEvent(QResizeEvent * e)
{
  QWidget::resizeEvent ( e ) ;
  Relocation           (   ) ;
}

bool N::Bookmarks::Relocation(void)
{
  nKickOut ( IsNull(URLs     ) , false )         ;
  nKickOut ( IsNull(bookmarks) , false )         ;
  QFont f = plan->fonts[Fonts::ComboBox]         ;
  int   H = f . pixelSize ( ) + 12               ;
  URLs      -> move   (       0 ,            0 ) ;
  URLs      -> resize ( width() ,            H ) ;
  bookmarks -> move   (       0 ,            H ) ;
  bookmarks -> resize ( width() , height() - H ) ;
  return true                                    ;
}

bool N::Bookmarks::startup(void)
{
  if ( IsNull ( bookmarks ) ) return false                  ;
  bookmarks -> setOwner ( ObjectUuid ( ) , ObjectType ( ) ) ;
  bookmarks -> startup  (                                 ) ;
  return true                                               ;
}

void N::Bookmarks::editingFinished(void)
{
  if (IsNull(URL)) return      ;
  QString url  = URL->text ( ) ;
  if (url.length()<=0) return  ;
  QString http = url.left(4)   ;
  http = http . toLower ( )    ;
  if (http!="http")            {
    url = "http://" + url      ;
  }                            ;
  QUrl U(url)                  ;
  bookmarks -> Join ( U )      ;
}

void N::Bookmarks::UrlClicked(QUrl url,bool reuse)
{
  emit Web ( url , reuse ) ;
}
