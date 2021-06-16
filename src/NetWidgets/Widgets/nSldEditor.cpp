#include <netwidgets.h>

N::SldEditor:: SldEditor      ( QWidget * parent , Plan * p )
             : TreeWidget     (           parent ,        p )
             , NetworkManager (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::SldEditor::~SldEditor(void)
{
}

void N::SldEditor::Configure(void)
{
  setWindowTitle               ( tr("Second level domains")  ) ;
  setDragDropMode              ( DragOnly                    ) ;
  setRootIsDecorated           ( false                       ) ;
  setAlternatingRowColors      ( true                        ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded       ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded       ) ;
  setColumnCount               ( 4                           ) ;
  NewTreeWidgetItem            ( header                      ) ;
  header->setText              ( 0,tr("Second level domain") ) ;
  header->setText              ( 1,tr("Top level domain"   ) ) ;
  header->setText              ( 2,tr("Name"               ) ) ;
  header->setText              ( 3,tr("Statistics"         ) ) ;
  setHeaderItem                ( header                      ) ;
  plan  -> setFont             ( this                        ) ;
  MountClicked                 ( 1                           ) ;
  MountClicked                 ( 2                           ) ;
}

bool N::SldEditor::FocusIn(void)
{
  LinkAction ( Refresh , List           () ) ;
  LinkAction ( Copy    , CopyToClipboard() ) ;
  return true                                ;
}

void N::SldEditor::List(void)
{
  SqlConnection SC ( plan->sql )                  ;
  QString CN = QtUUID::createUuidString ( )       ;
  if (SC.open("SldEditor",CN))                    {
    if (LoadDomainIndex(SC))                      {
      QString sld                                 ;
      foreach (sld,SLDs)                          {
        NewTreeWidgetItem ( IT )                  ;
        SUID    uuid    = SldUuids  [ sld  ]      ;
        SUID    tld     = SldTLDs   [ uuid ]      ;
        int     Sites   = SldCounts ( SC , uuid ) ;
        QString name    = Name ( SC , uuid      ) ;
        IT -> setData (0,Qt::UserRole,uuid      ) ;
        IT -> setData (1,Qt::UserRole,tld       ) ;
        IT -> setTextAlignment(0,Qt::AlignRight ) ;
        IT -> setTextAlignment(1,Qt::AlignRight ) ;
        IT -> setTextAlignment(3,Qt::AlignRight ) ;
        IT -> setText (0,sld                    ) ;
        IT -> setText (1,TldNames[tld]          ) ;
        IT -> setText (2,name                   ) ;
        IT -> setText (3,QString::number(Sites) ) ;
        addTopLevelItem ( IT                    ) ;
      }                                           ;
    }                                             ;
    SC . close ( )                                ;
  }                                               ;
  SC.remove()                                     ;
  SuitableColumns(2)                              ;
  Alert ( Done )                                  ;
}

void N::SldEditor::singleClicked(QTreeWidgetItem * item,int column)
{
  if ( ItemEditing == item && ItemColumn  == column ) return ;
  RemoveOldItem ( )                                          ;
}

void N::SldEditor::doubleClicked(QTreeWidgetItem * item,int column)
{
  RemoveOldItem ( )                           ;
  nDropOut ( column != 2 )                    ;
  QLineEdit * le                              ;
  le          = setLineEdit                   (
                  item                        ,
                  column                      ,
                  SIGNAL(editingFinished())   ,
                  SLOT  (NameFinished   ()) ) ;
  ItemEditing = item                          ;
  ItemWidget  = le                            ;
  ItemColumn  = column                        ;
}

void N::SldEditor::RemoveOldItem(void)
{
  if (IsNull(ItemEditing)) return               ;
  removeItemWidget ( ItemEditing , ItemColumn ) ;
  ItemEditing = NULL                            ;
  ItemWidget  = NULL                            ;
  ItemColumn  = -1                              ;
}

void N::SldEditor::NameFinished(void)
{
  if (IsNull(ItemEditing)) return                ;
  SUID uuid = nTreeUuid(ItemEditing,0)           ;
  QLineEdit * le = Casting(QLineEdit,ItemWidget) ;
  if (NotNull(le))                               {
    QString name = le -> text ( )                ;
    SqlConnection SC(plan->sql)                  ;
    QString CN = QtUUID::createUuidString ( )    ;
    if (SC.open("SldEditor",CN))                 {
      SC.assureName                              (
        PlanTable(Names)                         ,
        uuid                                     ,
        vLanguageId                              ,
        name                                   ) ;
      ItemEditing->setText(ItemColumn,name)      ;
      SC.close()                                 ;
    }                                            ;
    SC.remove()                                  ;
  }                                              ;
  RemoveOldItem ( )                              ;
}
