/****************************************************************************
 *                                                                          *
 * Copyright (C) 2015 Neutrino International Inc.                           *
 *                                                                          *
 * Author : Brian Lin <lin.foxman@gmail.com>, Skype: wolfram_lin            *
 *                                                                          *
 ****************************************************************************/

#ifndef QT_NETWIDGETS_H
#define QT_NETWIDGETS_H

#include <QtCore>
#include <QtGui>

#ifdef QT_WEBENGINE_LIB
#include <QtWebEngine>
#include <QtWebEngineWidgets>
#include <QtWebView>
#endif

#include <QtTorrent>
#include <QtManagers>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#    if defined(QT_BUILD_NETWIDGETS_LIB)
#      define Q_NETWIDGETS_EXPORT Q_DECL_EXPORT
#    else
#      define Q_NETWIDGETS_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define Q_NETWIDGETS_EXPORT
#endif

namespace Ui                             {
class Q_NETWIDGETS_EXPORT nFtpControl    ;
class Q_NETWIDGETS_EXPORT PadEditor      ;
class Q_NETWIDGETS_EXPORT PadBrowser     ;
class Q_NETWIDGETS_EXPORT PadFtp         ;
class Q_NETWIDGETS_EXPORT nTorrentWidget ;
}

namespace N              {

class Q_NETWIDGETS_EXPORT SldClassifier      ;
class Q_NETWIDGETS_EXPORT TldEditor          ;
class Q_NETWIDGETS_EXPORT SldEditor          ;
class Q_NETWIDGETS_EXPORT UrlPatterns        ;
class Q_NETWIDGETS_EXPORT PageTree           ;
class Q_NETWIDGETS_EXPORT BookmarkTree       ;
class Q_NETWIDGETS_EXPORT Bookmarks          ;
class Q_NETWIDGETS_EXPORT FtpControl         ;
class Q_NETWIDGETS_EXPORT PadFtp             ;
class Q_NETWIDGETS_EXPORT WebDockBar         ;
class Q_NETWIDGETS_EXPORT WebPageCapturer    ;
class Q_NETWIDGETS_EXPORT HtmlAnalyizer      ;
class Q_NETWIDGETS_EXPORT AndroidWebBrowser  ;
class Q_NETWIDGETS_EXPORT AppleWebBrowser    ;
class Q_NETWIDGETS_EXPORT DisguiseWebBrowser ;
#if defined ( QT_WEBENGINE_LIB          )
class Q_NETWIDGETS_EXPORT WebEngineBrowser   ;
#endif
class Q_NETWIDGETS_EXPORT TorrentLists       ;
class Q_NETWIDGETS_EXPORT TorrentWidget      ;

/*****************************************************************************
 *                                                                           *
 *                               Network widgets                             *
 *                                                                           *
 *****************************************************************************/

class Q_NETWIDGETS_EXPORT  SldClassifier : public QThread
                                         , public NetworkManager
{
  Q_OBJECT
  public:

    enum          {
      None  = 0   ,
      DoTLD = 1   ,
      DoSLD = 2 } ;

    int            Mode     ;
    int            Chunk    ;
    QToolButton  * Go       ;
    QProgressBar * Progress ;

    explicit SldClassifier (QObject * parent = NULL,Plan * plan = NULL) ;
    virtual ~SldClassifier (void);

    void DefaultUI         (void) ;

  protected:

    QDateTime Lastest ;
    Plan    * plan    ;

    void doTLDs            (SqlConnection & Connection) ;
    void doSLDs            (SqlConnection & Connection) ;
    bool OneSecond         (void) ;

    virtual void run       (void) ;

  private:

  public slots:

  protected slots:

    void showProgress      (int index,QString message) ;

  private slots:

  signals:

    void Finished          (void) ;
    void currentProgress   (int index,QString message) ;

};

class Q_NETWIDGETS_EXPORT  TldEditor : public TreeWidget
                , public NetworkManager
{
  Q_OBJECT
  public:

    explicit TldEditor     (StandardConstructor) ;
    virtual ~TldEditor     (void) ;

  protected:

    virtual bool FocusIn   (void) ;
    virtual void Configure (void) ;

  private:

  public slots:

    virtual void List      (void) ;

  protected slots:

  private slots:

  signals:

};

class Q_NETWIDGETS_EXPORT  SldEditor : public TreeWidget
                , public NetworkManager
{
  Q_OBJECT
  public:

    explicit SldEditor     (StandardConstructor) ;
    virtual ~SldEditor     (void) ;

  protected:

    virtual bool FocusIn   (void) ;
    virtual void Configure (void) ;

  private:

  public slots:

    virtual void List      (void) ;

  protected slots:

    virtual void RemoveOldItem (void) ;
    virtual void NameFinished  (void) ;

    virtual void singleClicked (QTreeWidgetItem * item,int column) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

};

class Q_NETWIDGETS_EXPORT  UrlPatterns : public TreeWidget
                  , public NetworkManager
                  , public Object
{
  Q_OBJECT
  public:

    explicit UrlPatterns      (StandardConstructor) ;
    virtual ~UrlPatterns      (void);

  protected:

    virtual void Configure     (void) ;
    virtual bool FocusIn       (void) ;
    virtual bool FocusOut      (void) ;

  private:

  public slots:

    virtual bool startup       (void) ;
    virtual void New           (void) ;
    virtual void Rename        (void) ;
    virtual void Delete        (void) ;

  protected slots:

    virtual bool Menu          (QPoint pos) ;
    virtual void nameFinished  (void) ;
    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

};

class Q_NETWIDGETS_EXPORT  PageTree : public TreeDock
               , public NetworkManager
{
  Q_OBJECT
  public:

    explicit PageTree          (StandardConstructor);
    virtual ~PageTree          (void);

  protected:

    virtual bool FocusIn       (void) ;
    virtual void Configure     (void) ;

  private:

    void   setStatusIcon       (QTreeWidgetItem * item,int retcode) ;

  public slots:

    virtual bool startup       (QUrl & url) ;
    virtual bool startup       (UUIDs & Pages) ;
    virtual void Export        (void) ;
    virtual void Delete        (void) ;

  protected slots:

    virtual void doubleClicked (QTreeWidgetItem *item, int column) ;
    virtual bool Menu          (QPoint pos) ;

  private slots:

  signals:

    void Web           (QUrl url,bool reuse) ;
    void OpenPlainText (QString title,QByteArray & Explain,int arrangement) ;

};

class Q_NETWIDGETS_EXPORT  BookmarkTree : public TreeWidget
                   , public Object
                   , public NetworkManager
                   , public GroupItems
{
  Q_OBJECT
  public:

    explicit BookmarkTree        (StandardConstructor) ;
    virtual ~BookmarkTree        (void) ;

  protected:

    QPoint          dragPoint ;
    TreeWidgetItems topItems  ;

    virtual bool FocusIn         (void) ;
    virtual void Configure       (void) ;

    virtual bool      hasItem    (void);
    virtual bool      startDrag  (QMouseEvent * event);
    virtual bool      fetchDrag  (QMouseEvent * event);
    virtual void      dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool      finishDrag (QMouseEvent * event);

    virtual QMimeData * dragMime (void) ;

    virtual QString MimeType     (const QMimeData * mime) ;
    virtual UUIDs   MimeUuids    (const QMimeData * mime,QString mimetype) ;

    virtual bool acceptDrop      (QWidget * source,const QMimeData * mime);

    virtual bool dropUrls        (nDeclDrops,const QList<QUrl> & urls) ;
    virtual bool dropURIs        (nDeclDrops,const UUIDs   & Uuids) ;
    virtual bool dropBookmarks   (nDeclDrops,const UUIDs   & Uuids) ;

    virtual QTreeWidgetItem * AppendItem (void) ;
    QTreeWidgetItem         * UrlItem    (SqlConnection & Connection,SUID uuid) ;

    UUIDs UrlSelected            (void) ;

    virtual void run             (int type,ThreadData * data) ;
    virtual void Reload          (ThreadData * data) ;
    virtual void ImportPages     (VarArgs & args,ThreadData * data) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual void Insert          (void) ;
    virtual void Delete          (void) ;
    virtual void Copy            (void) ;
    virtual void Paste           (void) ;

    virtual bool Join            (QUrl            & url ) ;
    virtual void Rename          (QTreeWidgetItem * item) ;
    virtual void Reload          (QTreeWidgetItem * item) ;
    virtual void Delete          (QTreeWidgetItem * item) ;
    virtual void ImportSites     (QTreeWidgetItem * item) ;
    virtual void ImportPages     (QTreeWidgetItem * item) ;
    virtual void ImportLinks     (QTreeWidgetItem * item) ;

  protected slots:

    virtual bool Menu            (QPoint pos) ;

    virtual void doubleClicked   (QTreeWidgetItem * item,int column) ;

    void removeOldItem           (void) ;
    void editingFinished         (void) ;

  private slots:

  signals:

    void UrlClicked              (QUrl url,bool reuse) ;
    void ImportRules             (QString name,SUID uuid,int type) ;
    void RunSets                 (VarArgLists & Operations) ;

};

class Q_NETWIDGETS_EXPORT  Bookmarks : public Widget
                                     , public Object
{
  Q_OBJECT
  public:

    BookmarkTree * bookmarks ;

    explicit Bookmarks           (StandardConstructor) ;
    virtual ~Bookmarks           (void) ;

    virtual bool canStop         (void) ;

  protected:

    LineEdit     * URL       ;
    QComboBox    * URLs      ;

    virtual void Configure       (void) ;

    virtual void resizeEvent     (QResizeEvent * event) ;

  private:

  public slots:

    virtual bool startup         (void) ;
    virtual bool Relocation      (void) ;

  protected slots:

    virtual void editingFinished (void) ;
    virtual void UrlClicked      (QUrl url,bool reuse) ;

  private slots:

  signals:

    void Web                     (QUrl url,bool reuse) ;
    void ImportRules             (QString name,SUID uuid,int type) ;
    void RunSets                 (VarArgLists & Operations) ;

};

class Q_NETWIDGETS_EXPORT  WebPageCapturer : public Thread
                                           , public Object
{
  public:

    PurePlan * FtpPlan   ;
    QUrl       Url       ;
    QDir       TempDir   ;
    KMAPs      Requests  ;
    KMAPs      mimeTypes ;
    QByteArray Header    ;
    QByteArray Body      ;

    explicit WebPageCapturer (void) ;
    virtual ~WebPageCapturer (void) ;

    bool          isRunning  (void) ;
    virtual void  started    (void) ;
    virtual void  fetch      (void) ;
    virtual void  finished   (void) ;
    virtual void  assureUuid (QUrl & url) ;

    HttpParser    Response   (void) ;

  protected:

    virtual void run         (void) ;

  private:

};

class Q_NETWIDGETS_EXPORT  HtmlAnalyizer : public TreeWidget
                                         , public WebPageCapturer
                                         , public XmlParser
{
  Q_OBJECT
  public:

    explicit HtmlAnalyizer     (StandardConstructor) ;
    virtual ~HtmlAnalyizer     (void);

  protected:

    QByteArray AnalyzeTitle ;

    virtual void Configure     (void) ;

    virtual void AppendTitle   (QString title,QByteArray origin  ) ;
    virtual void AppendLink    (QString name,QUrl origin,QUrl url) ;
    virtual void AppendImage   (QString name,QUrl origin,QUrl url) ;
    virtual void ReportError   (QString text                     ) ;
    virtual void AppendString  (QString text                     ) ;

    virtual void run           (void) ;

  private:

  public slots:

    virtual void addItem       (int type,QString key,QString name) ;

  protected slots:

    virtual void doubleClicked (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Web                   (QUrl url,bool reuse) ;

};

class Q_NETWIDGETS_EXPORT  FtpControl : public PanWidget
                                      , public FtpSettings
{
  Q_OBJECT
  public:

    explicit        FtpControl     (StandardConstructor) ;
    virtual        ~FtpControl     (void) ;

    virtual QSize   sizeHint       (void) const ;
    bool            AutoStartup    (void) ;

  protected:

    Ui::nFtpControl * ui             ;
    QTimer          * timer          ;
    QTimer          * searching      ;
    QTime             clock          ;
    EMAPs             folderNames    ;
    int               workingThreads ;
    qint64            prevSent       ;
    qint64            prevRecv       ;
    QList<double>     timeline       ;
    QList<qint64>     bytesOut       ;
    QList<qint64>     bytesIn        ;
    QList<qint64>     connections    ;

    virtual void    closeEvent     (QCloseEvent  * event) ;
    virtual void    resizeEvent    (QResizeEvent * event) ;
    virtual void    showEvent      (QShowEvent   * event) ;

    virtual void    Configure      (void) ;

    virtual bool    connectSharing (FileSharing * sharing) ;
    virtual bool    connectThread  (FtpThread   * thread ) ;
    virtual bool    Password       (QByteArray    & password) ;
    virtual QString Login          (void) ;

  private:

    QQueue< QPair<QString, QString> > queue;

  public slots:

    virtual void    relocation     (void) ;
    virtual void    startup        (void) ;
    virtual void    LoadFolder     (void) ;
    virtual void    SaveFolder     (void) ;
    virtual void    SearchFile     (void) ;
    virtual void    StopSearch     (void) ;
    virtual void    SearchResults  (void) ;
    virtual void    setBack        (bool toggle) ;
    virtual void    begin          (bool toggle) ;
    virtual void    addFolder      (void) ;
    virtual void    removeFolder   (void) ;
    virtual void    editFolder     (QTableWidgetItem * item) ;
    virtual void    editPath       (QTableWidgetItem * item) ;
    virtual void    openResult     (int row,int column) ;

  protected slots:

    virtual void    newThread      (void) ;
    virtual void    removeThread   (void) ;
    virtual void    closing        (void) ;
    virtual void    activity       (void) ;

    virtual void    Backward       (void) ;
    virtual void    Status         (void) ;
    virtual void    Chart          (void) ;

    virtual void    Accounts       (void) ;
    virtual void    PortChanged    (int port) ;
    virtual void    Options        (int state) ;

    virtual void    ThreadFinished (void) ;
    virtual void    addResult      (const QString & computer ,
                                    const QString & path   ) ;

  private slots:

  signals:

    void Return                    (void) ;
    void stoppingSearch            (void) ;

};

class Q_NETWIDGETS_EXPORT  PadFtp : public Splitter
                                  , public Thread
{
  Q_OBJECT
  public:

    FtpClient * ftp     ;
    QDir        Root    ;
    QDir        Current ;
    QIcon       Join    ;

    explicit PadFtp           (StandardConstructor) ;
    virtual ~PadFtp           (void) ;

    virtual QSize sizeHint    (void) const ;

  protected:

    Ui::PadFtp       *  ui               ;
    QMap<int,QAction *> Actions          ;
    QMap<int,BMAPs    > Displays         ;
    QStringList         RemoteDirs       ;
    QList<QUrlInfo>     Files            ;
    QStringList         Deletion         ;
    LMAPs               DeleteDirs       ;
    QStringList         Downloads        ;
    LMAPs               DownloadsDirs    ;
    ZMAPs               DownloadsSizes   ;
    bool                ContinueDownload ;
    QTimer              Timer            ;
    QTimer              HttpTimer        ;
    int                 TotalLines       ;
    qint64              FileIndex        ;
    int                 MaxFileLength    ;

    virtual void Configure    (void) ;

    virtual void run          (int type,ThreadData * data) ;

  private:

    void addAction            (int Id,QString text,QIcon icon) ;
    void setAlignments        (QTreeWidgetItem * item) ;

    void ListFiles            (void) ;
    void connectTo            (void) ;
    void FtpUp                (void) ;
    void FtpCd                (void) ;
    void FtpDelete            (void) ;
    void FtpUpload            (void) ;
    void FtpDownload          (void) ;
    void FtpCreate            (void) ;
    void HttpFetch            (void) ;

    void Download             (QDir & dir,QStringList & files,LMAPs & Dirs,ZMAPs & Sizes) ;
    void Download             (QString filename,QString remoteFile,quint64 size) ;

    void Upload               (QString filename,QString remoteFile,quint64 size) ;
    void Upload               (QDir & dir,QString pwd,QStringList & files) ;

    void AppendFiles          (QDir & root,QStringList & Exists,QStringList & newFiles) ;

  public slots:

    void Display              (int conf) ;

    void switchFtp            (void) ;
    void Edit                 (void) ;
    void BrowseFiles          (void) ;
    void RemoveFiles          (void) ;
    void ClearFiles           (void) ;
    void DropFiles            (QStringList files) ;
    void HttpDownload         (void) ;
    void HttpDownload         (QString message) ;
    void FetchHttp            (void) ;
    void ShowLogs             (void) ;

    void connectFtp           (void) ;
    void disconnectFtp        (void) ;
    void ftpUp                (void) ;
    void ftpCd                (void) ;
    void ftpDelete            (void) ;
    void ftpUpload            (void) ;
    void ftpDownload          (void) ;
    void ftpCreate            (void) ;

    void StopDownload         (void) ;
    void switchProgress       (void) ;
    void switchLogs           (void) ;

    void InitProfiles         (void) ;
    void Profiles             (void) ;
    void NewProfile           (void) ;
    void ProfileLists         (void) ;
    void LoadProfiles         (void) ;
    void SaveProfiles         (void) ;
    void DeleteProfiles       (void) ;

  protected slots:

    void ftpClicked           (QTreeWidgetItem * item,int column) ;
    void fileClicked          (QTreeWidgetItem * item,int column) ;
    void listInfo             (const QUrlInfo & url) ;
    void UrlInfos             (void) ;
    void CreateDirectory      (void) ;
    void ActualDownload       (void) ;
    void ActualUpload         (void) ;
    void RefreshProgress      (void) ;
    void RefreshHttp          (void) ;

  private slots:

    void organizeFtp          (void) ;
    void Logging              (QString message) ;
    void AddDownload          (QString filename) ;
    void assignFileSize       (quint64 size) ;
    void assignProgress       (quint64 index) ;

  signals:

    void Back                 (void) ;
    void BackTo               (void) ;
    void BackDownload         (void) ;
    void BackUpload           (void) ;
    void BackHttp             (QString message) ;
    void Picking              (void) ;
    void resizeFtp            (void) ;
    void ListUrlInfos         (void) ;
    void Message              (QString message) ;
    void DownloadFile         (QString filename) ;
    void setFileSize          (quint64 size) ;
    void setProgress          (quint64 index) ;

};

class Q_NETWIDGETS_EXPORT  TorrentLists : public TreeWidget
                                        , public Ownership
                                        , public GroupItems
{
  Q_OBJECT
  public:

    int  Depot ;

    explicit TorrentLists          (StandardConstructor) ;
    virtual ~TorrentLists          (void) ;

    QTreeWidgetItem *   uuidItem   (SUID uuid) ;

  protected:

    QPoint dragPoint ;

    virtual bool        FocusIn    (void) ;

    virtual bool        hasItem    (void);
    virtual bool        startDrag  (QMouseEvent * event);
    virtual bool        fetchDrag  (QMouseEvent * event);
    virtual QMimeData * dragMime   (void);
    virtual void        dragDone   (Qt::DropAction dropIt,QMimeData * mime);
    virtual bool        finishDrag (QMouseEvent * event);

    virtual void        Configure  (void) ;

    virtual void        run        (int type,ThreadData * data) ;

    UUIDs               Uuids      (ThreadData * data,SqlConnection & Connection) ;
    bool                List       (ThreadData * data,SqlConnection & Connection,UUIDs & Uuids) ;
    virtual void        Insert     (ThreadData * data,VarArgs & args) ;
    virtual void        Import     (ThreadData * data,VarArgs & args) ;
    virtual void        Refresh    (ThreadData * data) ;

  private:

  public slots:

    virtual bool startup           (void) ;
    virtual void Insert            (void) ;
    virtual void Delete            (void) ;
    virtual void Remove            (void) ;
    virtual void Import            (void) ;
    virtual bool Import            (UUIDs & uuids) ;
    virtual bool Import            (ThreadData * data,QFileInfoList & List,UUIDs & uuids) ;
    virtual void Export            (QTreeWidgetItem * item) ;

  protected slots:

    virtual bool Menu              (QPoint pos) ;
    virtual void doubleClicked     (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void Clicked                   (SUID uuid) ;
    void Details                   (SUID uuid) ;

};

class Q_NETWIDGETS_EXPORT  TorrentWidget : public TabWidget
                                         , public AttachDock
                                         , public QtTorrentManager
{
  Q_OBJECT
  public:

    QDir           directory ;
    TorrentLists * torrents  ;

    explicit TorrentWidget        (StandardConstructor) ;
    virtual ~TorrentWidget        (void);

    virtual bool LoadState        (void) ;
    virtual bool SaveState        (void) ;

  protected:

    Ui::nTorrentWidget * ui    ;
    QTimer             * Timer ;

    virtual void closeEvent       (QCloseEvent * event) ;
    virtual void contextMenuEvent (QContextMenuEvent * event) ;

    virtual void Configure        (void) ;
    virtual void run              (void) ;
    virtual void run              (int Type,ThreadData * data) ;

    virtual void List             (SUID uuid,QtTorrent & torrent) ;

    virtual void Monitor          (ThreadData * data) ;

  private:

    bool started ;
    bool inLoop  ;

  public slots:

    virtual bool startup          (void) ;
    virtual void stopIt           (void) ;
    virtual void Update           (void) ;
    virtual void Halt             (void) ;
    virtual void Delete           (SUID uuid) ;

    virtual void Docking          (QMainWindow       * Main    ,
                                   QString             title   ,
                                   Qt::DockWidgetArea  area    ,
                                   Qt::DockWidgetAreas areas ) ;
    virtual void DockIn           (bool shown) ;

  protected slots:

    void Visible                  (bool visible) ;
    virtual bool Menu             (QPoint pos) ;

    void Download                 (SUID uuid) ;
    void limitChanged             (int) ;

    void DHT                      (bool enabled) ;
    void Lookup                   (bool enabled) ;

    void torrentChanged           (QTreeWidgetItem * item,int column) ;

  private slots:

  signals:

    void attachDock  (QWidget * widget,QString title,Qt::DockWidgetArea area,Qt::DockWidgetAreas areas) ;
    void attachMdi   (QWidget * widget,int Direction) ;

};

/*****************************************************************************
 *                                                                           *
 *                                Web Browsers                               *
 *                                                                           *
 *****************************************************************************/

class Q_NETWIDGETS_EXPORT  WebDockBar : public ToolBar
                                      , public NetworkManager
                                      , public GroupItems
{
  Q_OBJECT
  public:

    enum              {
      Edit     =  1   ,
      List     =  2   ,
      Engine   =  3   ,
      Go       =  4   ,
      Stop     =  5   ,
      Reload   =  6   ,
      Back     =  7   ,
      Forward  =  8   ,
      History  =  9   ,
      Analysis = 10   ,
      Desktop  = 11   ,
      Save     = 12 } ;

    SUID                Bookmark ;
    QMap<int,QAction *> Actions  ;
    QMap<int,QWidget *> Widgets  ;

    explicit WebDockBar      (QWidget * parent,Plan * plan) ;
    virtual ~WebDockBar      (void) ;

    QUrl     Address         (void) ;

  protected:

    bool Loading ;

    virtual void Configure   (void) ;

    virtual void run         (void) ;

  private:

  public slots:

    virtual void Disconnect  (void) ;
    virtual bool Relocation  (void) ;

    virtual void setUrl      (const QUrl & url) ;
    virtual void Load        (void) ;

    virtual void Patterns    (SUID type,QString name,QString keyword) ;

  protected slots:

  private slots:

  signals:

};

class Q_NETWIDGETS_EXPORT  AndroidWebBrowser : public Widget
{
  Q_OBJECT
  public:

    WebDockBar * DockBar ;

    explicit AndroidWebBrowser (StandardConstructor) ;
    virtual ~AndroidWebBrowser (void) ;

  protected:

  private:

  public slots:

  protected slots:

  private slots:

  signals:

};

class Q_NETWIDGETS_EXPORT  AppleWebBrowser : public Widget
{
  Q_OBJECT
  public:

    WebDockBar * DockBar ;

    explicit AppleWebBrowser (StandardConstructor) ;
    virtual ~AppleWebBrowser (void) ;

  protected:

  private:

  public slots:

  protected slots:

  private slots:

  signals:

} ;

#if defined( QT_WEBENGINE_LIB)

class Q_NETWIDGETS_EXPORT  WebEngineBrowser : public QWebEngineView
                                            , public VirtualGui
                                            , public Thread
{
  Q_OBJECT
  public:

    WebDockBar * DockBar ;

    explicit WebEngineBrowser     (nStandardConstructor) ;
    virtual ~WebEngineBrowser     (void);

  protected:

    QProgressBar * Progress ;

    virtual void Configure        (void) ;

    virtual void focusInEvent     (QFocusEvent * event) ;
    virtual void contextMenuEvent (QContextMenuEvent *event);
    virtual bool runJavascript    (const QUrl & url) ;

  private:

  public slots:

    void setPath                  (QUrl url) ;

  protected slots:

    void urlClicked               (const QUrl & url) ;
    void urlAlter                 (const QUrl & url) ;
    void urlStarted               (void) ;
    void urlProgress              (int progress);
    void urlFinished              (bool ok);
    void urlMessage               (const QString & text);
    void downloadRequested        (const QNetworkRequest & request) ;

    void returnPressed            (void) ;
    void indexChanged             (int index) ;
    void Go                       (void) ;
    void Stop                     (void) ;
    void Reload                   (void) ;
    void Back                     (void) ;
    void Forward                  (void) ;
    void Analysis                 (void) ;
    void History                  (void) ;
    void Desktop                  (void) ;
    void Save                     (void) ;

  private slots:

  signals:

    void Analysis                 (QWebEngineView * view) ;
    void History                  (QWebEngineView * view) ;
    void Save                     (QWebEngineView * view) ;

};

#endif

class Q_NETWIDGETS_EXPORT  DisguiseWebBrowser : public Widget
{
  Q_OBJECT
  public:

    WebDockBar * DockBar ;

    explicit DisguiseWebBrowser (StandardConstructor) ;
    virtual ~DisguiseWebBrowser (void) ;

  protected:

  private:

  public slots:

  protected slots:

  private slots:

  signals:

};

#if   defined(Q_OS_ANDROID)
#define WebBrowser AndroidWebBrowser
#elif defined(Q_OS_IOS)
#define WebBrowser AppleWebBrowser
#elif defined(QT_WEBENGINE_LIB)
#define WebBrowser WebEngineBrowser
#else
#define WebBrowser DisguiseWebBrowser
#endif

}

QT_END_NAMESPACE

#endif
