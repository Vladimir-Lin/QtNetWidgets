#include <netwidgets.h>

N::HtmlAnalyizer:: HtmlAnalyizer   (QWidget * parent,Plan * p)
                 : TreeWidget      (          parent,       p)
                 , WebPageCapturer (                         )
                 , XmlParser       (                         )
{
  WidgetClass    ;
  FtpPlan = plan ;
  Configure ( )  ;
}

N::HtmlAnalyizer::~HtmlAnalyizer (void)
{
}

void N::HtmlAnalyizer::Configure(void)
{
  setDragDropMode              ( DragDrop              ) ;
  setRootIsDecorated           ( false                 ) ;
  setAlternatingRowColors      ( true                  ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded ) ;
  setColumnCount               ( 2                     ) ;
  NewTreeWidgetItem            ( head                  ) ;
  head -> setText              ( 0,tr("Item" )         ) ;
  head -> setText              ( 1,tr("Value")         ) ;
  setHeaderItem                ( head                  ) ;
  MountClicked                 ( 2                     ) ;
}

void N::HtmlAnalyizer::AppendTitle(QString title,QByteArray origin)
{
  AnalyzeTitle = origin ;
}

void N::HtmlAnalyizer::AppendLink(QString name,QUrl origin,QUrl url)
{
  QUrl O = origin ;
  if (url.host().length()<=0) O = O.resolved(url) ; else O = url ;
  if (!O.isValid()) return ;
  addItem(3,name,O.toString()) ;
}

void N::HtmlAnalyizer::AppendImage(QString name,QUrl origin,QUrl url)
{
  QUrl O = origin ;
  if (url.host().length()<=0) O = O.resolved(url) ; else O = url ;
  if (!O.isValid()) return ;
  addItem(4,name,O.toString()) ;
}

void N::HtmlAnalyizer::ReportError(QString text)
{
  QTreeWidgetItem * IT = topLevelItem(1) ;
  if (IsNull(IT)) return                 ;
  IT->setText(1,text)                    ;
}

void N::HtmlAnalyizer::AppendString(QString text)
{
}

void N::HtmlAnalyizer::addItem (int type,QString key,QString name)
{
  NewTreeWidgetItem(IT           ) ;
  IT->setData(0,Qt::UserRole,type) ;
  IT->setText(0,key              ) ;
  IT->setText(1,name             ) ;
  addTopLevelItem  (IT           ) ;
}

void N::HtmlAnalyizer::doubleClicked(QTreeWidgetItem * item,int column)
{
  int mType = nTreeInt(item,0)     ;
  if (mType!=3 && mType!=4) return ;
  QUrl U (item->text(1))           ;
  emit Web ( U , true )            ;
}

void N::HtmlAnalyizer::run(void)
{
  if (IsNull(FtpPlan)) return                    ;
  if (uuid<=0        ) return                    ;
  addItem    (0,tr("Title"),"")                  ;
  addItem    (1,tr("Error"),"")                  ;
  started    (                )                  ;
  fetch      (                )                  ;
  QStringList keys                               ;
  QString     key                                ;
  HttpParser  HR                                 ;
  HR   = Response()                              ;
  keys = HR.keys ()                              ;
  foreach (key,keys)                             {
    QString v                                    ;
    v = HR.value(key)                            ;
    addItem ( 2 , key , v )                      ;
  }                                              ;
  ParseLinks ( Url , Body     )                  ;
  ////////////////////////////////////////////////
  QString title = Codec->toUnicode(AnalyzeTitle) ;
  setWindowTitle(title)                          ;
  QTreeWidgetItem * IT = topLevelItem(0)         ;
  if (NotNull(IT))                               {
    IT->setText(1,title)                         ;
  }                                              ;
  ////////////////////////////////////////////////
  finished (      )                              ;
  Alert    ( Done )                              ;
}
