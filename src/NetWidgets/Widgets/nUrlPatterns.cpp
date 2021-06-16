#include <netwidgets.h>

N::UrlPatterns:: UrlPatterns    ( QWidget * parent , Plan * p )
               : TreeWidget     (           parent ,        p )
               , NetworkManager (                           p )
               , Object         ( 0 , Types::URL              )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::UrlPatterns::~UrlPatterns(void)
{
}

void N::UrlPatterns::Configure(void)
{
  NewTreeWidgetItem            ( head                        ) ;
  setWindowTitle               ( tr("Web search patterns"  ) ) ;
  setDragDropMode              ( NoDragDrop                  ) ;
  setRootIsDecorated           ( false                       ) ;
  setAlternatingRowColors      ( true                        ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded       ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded       ) ;
  setColumnCount               ( 2                           ) ;
  head -> setText              ( 0, tr("Name"   )            ) ;
  head -> setText              ( 1, tr("Pattern")            ) ;
  setAllAlignments             ( head , Qt::AlignCenter      ) ;
  setHeaderItem                ( head                        ) ;
  plan -> setFont              ( this                        ) ;
  //////////////////////////////////////////////////////////////
  MountClicked                 ( 2                           ) ;
}

bool N::UrlPatterns::FocusIn(void)
{
  LinkAction ( Refresh , startup () ) ;
  LinkAction ( Insert  , New     () ) ;
  LinkAction ( Rename  , Rename  () ) ;
  LinkAction ( Font    , setFont () ) ;
  LinkAction ( Delete  , Delete  () ) ;
  return true                         ;
}

bool N::UrlPatterns::FocusOut(void)
{
  return true ;
}

bool N::UrlPatterns::startup(void)
{
  clear    (               )                  ;
  EnterSQL ( SC ,plan->sql )                  ;
    QString Q                                 ;
    Q = SelectPatterns(SC,Qt::AscendingOrder) ;
    SqlLoopNow ( SC , Q )                     ;
      NewTreeWidgetItem(item)                 ;
      int     position = SC.Int   (0)         ;
      QString name     = SC.String(1)         ;
      QString pattern  = SC.String(2)         ;
      item->setData(0,Qt::UserRole,position)  ;
      item->setText(0,name                 )  ;
      item->setText(1,pattern              )  ;
      addTopLevelItem ( item               )  ;
    SqlLoopErr ( SC , Q )                     ;
    SqlLoopEnd ( SC , Q )                     ;
  LeaveSQL ( SC ,plan->sql )                  ;
  SuitableColumns ( )                         ;
  reportItems     ( )                         ;
  return true                                 ;
}

void N::UrlPatterns::New(void)
{
  NewTreeWidgetItem ( IT )      ;
  IT->setData(0,Qt::UserRole,0) ;
  addTopLevelItem ( IT     )    ;
  scrollToItem    ( IT     )    ;
  doubleClicked   ( IT , 0 )    ;
}

void N::UrlPatterns::Rename(void)
{
  QTreeWidgetItem * item                   ;
  item = currentItem()                     ;
  nDropOut ( IsNull(item) )                ;
  doubleClicked ( item , currentColumn() ) ;
}

void N::UrlPatterns::Delete(void)
{
  QTreeWidgetItem * item            ;
  item = currentItem()              ;
  nDropOut ( IsNull(item)  )        ;
  int id = nTreeInt(item,0 )        ;
  EnterSQL ( SC ,plan->sql )        ;
    QString name = item->text(0)    ;
    DeletePatterns(SC,uuid,id,name) ;
  LeaveSQL ( SC ,plan->sql )        ;
  takeItem ( item          )        ;
  Alert    ( Done          )        ;
}

void N::UrlPatterns::doubleClicked(QTreeWidgetItem * item,int column)
{
  QLineEdit * line                   ;
  removeOldItem(false,column)        ;
  line = setLineEdit                 (
         item                        ,
         column                      ,
         SIGNAL(editingFinished())   ,
         SLOT  (nameFinished   ()) ) ;
  line->setFocus(Qt::TabFocusReason) ;
}

void N::UrlPatterns::nameFinished(void)
{
  if (IsNull(ItemEditing)) return                         ;
  QLineEdit * line    = Casting  (QLineEdit  ,ItemWidget) ;
  if (IsNull(line       )) return                         ;
  ItemEditing -> setText    ( ItemColumn , line->text() ) ;
  /////////////////////////////////////////////////////////
  int     id      = nTreeInt(ItemEditing,0)               ;
  int     column  = ItemColumn                            ;
  QString name    = ItemEditing->text(0)                  ;
  QString pattern = ItemEditing->text(1)                  ;
  EnterSQL ( SC , plan->sql )                             ;
    switch (column)                                       {
      case 0                                              :
        if (id<=0)                                        {
          id = PatternId(SC,uuid,name)                    ;
          id ++                                           ;
          InsertPattern(SC,uuid,id,name)                  ;
          ItemEditing->setData(0,Qt::UserRole,id)         ;
        } else                                            {
          UpdatePattern(SC,uuid,id,name)                  ;
        }                                                 ;
      break                                               ;
      case 1                                              :
        UpdatePattern(SC,uuid,id,name,pattern)            ;
      break                                               ;
    }                                                     ;
  LeaveSQL      ( SC , plan->sql )                        ;
  removeOldItem ( false , column )                        ;
  Alert         ( Done           )                        ;
}

bool N::UrlPatterns::Menu(QPoint pos)
{ Q_UNUSED ( pos )           ;
  MenuManager mm (this)      ;
  QAction   * aa             ;
  mm.add(101,tr("New"    ))  ;
  mm.add(102,tr("Rename" ))  ;
  mm.add(103,tr("Delete" ))  ;
  mm.addSeparator (       )  ;
  mm.add(201,tr("Refresh"))  ;
  mm.setFont(plan)           ;
  aa = mm.exec()             ;
  nKickOut(IsNull(aa),false) ;
  switch (mm[aa])            {
    nFastCast(101,New    ()) ;
    nFastCast(102,Rename ()) ;
    nFastCast(103,Delete ()) ;
    nFastCast(201,startup()) ;
  }                          ;
  return true                ;
}
