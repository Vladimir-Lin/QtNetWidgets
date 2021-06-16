#include <netwidgets.h>

N::SldClassifier:: SldClassifier  (QObject * parent,Plan * p)
                 : QThread        (          parent         )
                 , NetworkManager (                        p)
                 , plan           (                        p)
                 , Go             (NULL                     )
                 , Progress       (NULL                     )
                 , Mode           (None                     )
                 , Chunk          (10000                    )
{
  connect(this,SIGNAL(currentProgress(int,QString))   ,
          this,SLOT  (showProgress   (int,QString)) ) ;
}

N::SldClassifier::~SldClassifier(void)
{
}

void N::SldClassifier::DefaultUI(void)
{
  Go        = new QToolButton(plan->status)                 ;
  Go       -> setAutoRaise (true)                           ;
  Go       -> setCheckable (true)                           ;
  Go       -> setIcon      (QIcon(":/images/StopPlay.png")) ;
  Go       -> setToolTip   (tr("Stop analyizing")         ) ;
  plan     -> status  -> addPermanentWidget ( Go      )     ;
  Progress  = plan    -> Progress           ( "%v/%m" )     ;
  Progress -> setTextVisible                (  true   )     ;
}

void N::SldClassifier::showProgress(int index,QString message)
{
  Progress -> setValue    ( index   ) ;
  plan     -> showMessage ( message ) ;
}

bool N::SldClassifier::OneSecond(void)
{
  QDateTime D = Lastest                      ;
  QDateTime S = QDateTime::currentDateTime() ;
  time_t    d = D.toTime_t()                 ;
  time_t    s = S.toTime_t()                 ;
  return (d!=s)                              ;
}

void N::SldClassifier::doTLDs(SqlConnection & Connection)
{
  bool    done  = false                                       ;
  QString Limit = QString ( " limit 0,%1 ;" ) . arg(Chunk)    ;
  TUID    TOTAL = SiteTotal  ( Connection )                   ;
  SUID    LUID  = LastestTld ( Connection )                   ;
  TUID    FUID  = 0                                           ;
  TUID    AUID  = 0                                           ;
  UUIDs   Uuids                                               ;
  NAMEs   URLs                                                ;
  QMap<SUID,TUID> IDs                                         ;
  QString         Q                                           ;
  QString         Options                                     ;
  QDateTime       Start                                       ;
  if (LUID>0) FUID = SiteId( Connection , LUID )              ;
  Go       -> setChecked ( true        )                      ;
  Progress -> setRange   ( 0,(int)TOTAL)                      ;
  Limit   = " order by id asc" + Limit                        ;
  Lastest = QDateTime::currentDateTime ( )                    ;
  Start   = QDateTime::currentDateTime ( )                    ;
  AUID    = FUID                                              ;
  while (!done)                                               {
    Uuids . clear ( )                                         ;
    URLs  . clear ( )                                         ;
    IDs   . clear ( )                                         ;
    Options = QString(" where id > %1").arg(FUID)             ;
    Q = Connection.sql.SelectFrom                             (
          "id,uuid,url"                                       ,
          PlanTable(SiteDepot)                                ,
          Options + Limit                                   ) ;
    SqlLoopNow ( Connection , Q )                             ;
      TUID    fuid = Connection . Value ( 0 ) . toLongLong () ;
      SUID    uuid = Connection . Uuid  ( 1 )                 ;
      QString url  = Connection . String( 2 )                 ;
      Uuids        << uuid                                    ;
      IDs  [ uuid ] = fuid                                    ;
      URLs [ uuid ] = url                                     ;
    SqlLoopErr ( Connection , Q )                             ;
    SqlLoopEnd ( Connection , Q )                             ;
    if (Uuids.count()<=0) done = true; else                   {
      for (int i=0;Go->isChecked() && i<Uuids.count();i++)    {
        SUID uuid = Uuids [ i    ]                            ;
        FUID      = IDs   [ uuid ]                            ;
        QUrl url                                              ;
        url . setHost (URLs[uuid])                            ;
        SUID tld  = UrlTLD ( url )                            ;
        if (tld>0) InsertTldMap ( Connection , uuid , tld )   ;
        if (OneSecond())                                      {
          TUID      INDEX = FUID  - AUID                      ;
          TUID      STOTA = TOTAL - AUID                      ;
          QString   msg                                       ;
          QDateTime ETA                                       ;
          ETA = Time::ETA(Start,(int)INDEX,(int)STOTA)        ;
          msg = tr("Estimated complete time : %1"             )
                .arg(ETA.toString("yyyy/MM/dd hh:mm:ss")    ) ;
          emit currentProgress( (int)FUID , msg )             ;
          Lastest = QDateTime::currentDateTime ( )            ;
        }                                                     ;
      }                                                       ;
    }                                                         ;
    if (!Go->isChecked()) done = true                         ;
  }                                                           ;
}

void N::SldClassifier::doSLDs(SqlConnection & Connection)
{
  bool    done  = false                                       ;
  QString Limit = QString ( " limit 0,%1 ;" ) . arg(Chunk)    ;
  TUID    TOTAL = SiteTotal  ( Connection )                   ;
  SUID    LUID  = LastestSld ( Connection )                   ;
  TUID    FUID  = 0                                           ;
  TUID    AUID  = 0                                           ;
  UUIDs   Uuids                                               ;
  NAMEs   URLs                                                ;
  QMap<SUID,TUID> IDs                                         ;
  QString         Q                                           ;
  QString         Options                                     ;
  QDateTime       Start                                       ;
  if (LUID>0) FUID = SiteId( Connection , LUID )              ;
  Go       -> setChecked ( true        )                      ;
  Progress -> setRange   ( 0,(int)TOTAL)                      ;
  Limit   = " order by id asc" + Limit                        ;
  Lastest = nTimeNow                                          ;
  Start   = nTimeNow                                          ;
  AUID    = FUID                                              ;
  while (!done)                                               {
    Uuids . clear ( )                                         ;
    URLs  . clear ( )                                         ;
    IDs   . clear ( )                                         ;
    Options = QString(" where id > %1").arg(FUID)             ;
    Q = Connection.sql.SelectFrom                             (
          "id,uuid,url"                                       ,
          PlanTable(SiteDepot)                                ,
          Options + Limit                                   ) ;
    SqlLoopNow ( Connection , Q )                             ;
      TUID    fuid = Connection . Value ( 0 ) . toLongLong () ;
      SUID    uuid = Connection . Uuid  ( 1 )                 ;
      QString url  = Connection.String( 2 )                   ;
      Uuids        << uuid                                    ;
      IDs  [ uuid ] = fuid                                    ;
      URLs [ uuid ] = url                                     ;
    SqlLoopErr ( Connection , Q )                             ;
    SqlLoopEnd ( Connection , Q )                             ;
    if (Uuids.count()<=0) done = true; else                   {
      for (int i=0;Go->isChecked() && i<Uuids.count();i++)    {
        SUID uuid = Uuids [ i    ]                            ;
        FUID      = IDs   [ uuid ]                            ;
        QUrl url                                              ;
        url . setHost (URLs[uuid])                            ;
        SUID sld  = UrlSLD ( url )                            ;
        if (sld>0) InsertSldMap ( Connection , uuid , sld )   ;
        if (OneSecond())                                      {
          TUID      INDEX = FUID  - AUID                      ;
          TUID      STOTA = TOTAL - AUID                      ;
          QString   msg                                       ;
          QDateTime ETA                                       ;
          ETA = Time::ETA(Start,(int)INDEX,(int)STOTA)        ;
          msg = tr("Estimated complete time : %1"             )
                .arg(ETA.toString("yyyy/MM/dd hh:mm:ss")    ) ;
          emit currentProgress( (int)FUID , msg )             ;
          Lastest = QDateTime::currentDateTime ( )            ;
        }                                                     ;
      }                                                       ;
    }                                                         ;
    if (!Go->isChecked()) done = true                         ;
  }                                                           ;
}

void N::SldClassifier::run(void)
{
  SqlConnection SC ( NetPlan->sql )                  ;
  if (SC.open("nSldClassifier","run"))               {
    if (LoadDomainIndex(SC))                         {
      PrepareDomainMap (  )                          ;
      switch (Mode)                                  {
        case None                                    :
        break                                        ;
        case DoTLD                                   :
          plan->showMessage(tr("Counting sites...")) ;
          plan->processEvents ( )                    ;
          doTLDs(SC)                                 ;
        break                                        ;
        case DoSLD                                   :
          plan->showMessage(tr("Counting sites...")) ;
          plan->processEvents ( )                    ;
          doSLDs(SC)                                 ;
        break                                        ;
      }                                              ;
    }                                                ;
    SC . close ( )                                   ;
  }                                                  ;
  SC.remove()                                        ;
  Go       -> hide        ( )                        ;
  Progress -> hide        ( )                        ;
  Go       -> deleteLater ( )                        ;
  Progress -> deleteLater ( )                        ;
  Go        = NULL                                   ;
  Progress  = NULL                                   ;
  emit Finished ( )                                  ;
}
