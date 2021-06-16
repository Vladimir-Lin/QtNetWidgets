#include <netwidgets.h>

N::TldEditor:: TldEditor      ( QWidget * parent , Plan * p )
             : TreeWidget     (           parent ,        p )
             , NetworkManager (                           p )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::TldEditor::~TldEditor(void)
{
}

void N::TldEditor::Configure(void)
{
  setWindowTitle               ( tr("Top level domains") ) ;
  setDragDropMode              ( DragOnly                ) ;
  setRootIsDecorated           ( false                   ) ;
  setAlternatingRowColors      ( true                    ) ;
  setHorizontalScrollBarPolicy ( Qt::ScrollBarAsNeeded   ) ;
  setVerticalScrollBarPolicy   ( Qt::ScrollBarAsNeeded   ) ;
  setColumnCount               ( 6                       ) ;
  NewTreeWidgetItem            ( header                  ) ;
  header->setText              ( 0,tr("Top level domain")) ;
  header->setText              ( 1,tr("Country"         )) ;
  header->setText              ( 2,tr("NIC"             )) ;
  header->setText              ( 3,tr("SLDs"            )) ;
  header->setText              ( 4,tr("Sites"           )) ;
  header->setText              ( 5,tr("Comment"         )) ;
  setHeaderItem                ( header                  ) ;
  plan  -> setFont             ( this                    ) ;
}

bool N::TldEditor::FocusIn(void)
{
  LinkAction ( Refresh , List           () ) ;
  LinkAction ( Copy    , CopyToClipboard() ) ;
  return true                                ;
}

void N::TldEditor::List(void)
{
  SqlConnection SC ( plan->sql )                  ;
  QString CN = QtUUID::createUuidString ( )       ;
  if (SC.open("TldEditor",CN))                    {
    if (LoadDomainIndex(SC))                      {
      QString tld                                 ;
      foreach (tld,TLDs)                          {
        NewTreeWidgetItem ( IT )                  ;
        SUID    uuid    = TldUuids   [ tld     ]  ;
        int     country = TldNations [ uuid    ]  ;
        SUID    cuid    = Countries  [ country ]  ;
        QString nic     = NICs       [ uuid    ]  ;
        int     slds    = TldCounts  ( SC,uuid )  ;
        int     tlds    = TldTotal   ( SC,uuid )  ;
        QString comment = Comments   [ uuid    ]  ;
        IT -> setData (0,Qt::UserRole,uuid     )  ;
        IT -> setTextAlignment(3,Qt::AlignRight)  ;
        IT -> setTextAlignment(4,Qt::AlignRight)  ;
        IT -> setText (0,tld                   )  ;
        IT -> setText (1,Nations[cuid]         )  ;
        IT -> setText (2,nic                   )  ;
        IT -> setText (3,QString::number(slds) )  ;
        IT -> setText (4,QString::number(tlds) )  ;
        IT -> setText (5,comment               )  ;
        addTopLevelItem ( IT )                    ;
      }                                           ;
    }                                             ;
    SC . close ( )                                ;
  }                                               ;
  SC.remove()                                     ;
  SuitableColumns ( )                             ;
  Alert ( Done )                                  ;
}
