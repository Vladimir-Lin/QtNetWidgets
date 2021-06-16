#include <netwidgets.h>

#if defined(QT_WEBENGINE_LIB)

N::WebEngineBrowser:: WebEngineBrowser ( QWidget * parent , Plan * p  )
                    : QWebEngineView   (           parent             )
                    , VirtualGui       (           this   ,        p  )
                    , Thread           (           0      ,     false )
                    , Progress         ( NULL                         )
                    , DockBar          ( NULL                         )
{
  WidgetClass   ;
  Configure ( ) ;
}

N::WebEngineBrowser::~WebEngineBrowser (void)
{
  if (NotNull(Progress))        {
    Progress -> hide        ( ) ;
    Progress -> deleteLater ( ) ;
    Progress  = NULL            ;
  }                             ;
}

void N::WebEngineBrowser::Configure(void)
{
//  QAction * action ;
  setAccessibleName ("nWebBrowser")                        ;
  setWindowIcon(QIcon(":/images/geography.png"))           ;
//  nConnect(this  ,SIGNAL(linkClicked     (const QUrl &))   ,
//           this  ,SLOT  (urlClicked      (const QUrl &)) ) ;
  nConnect(page(),SIGNAL(linkClicked     (const QUrl &))   ,
           this  ,SLOT  (urlClicked      (const QUrl &)) ) ;
  nConnect(this  ,SIGNAL(urlChanged      (const QUrl &))   ,
           this  ,SLOT  (urlAlter        (const QUrl &)) ) ;
  nConnect(page(),SIGNAL(aboutToLoadUrl  (const QUrl &))   ,
           this  ,SIGNAL(urlAlter        (const QUrl &)) ) ;
  nConnect(this  ,SIGNAL(loadStarted     ())               ,
           this  ,SLOT  (urlStarted      ())             ) ;
  nConnect(this  ,SIGNAL(loadProgress    (int))            ,
           this  ,SLOT  (urlProgress     (int))          ) ;
  nConnect(this  ,SIGNAL(loadFinished    (bool))           ,
           this  ,SLOT  (urlFinished     (bool))         ) ;
  nConnect(this  ,SIGNAL(statusBarMessage(QString))        ,
           this  ,SLOT  (urlMessage      (QString))      ) ;
  nConnect(this  ,SIGNAL(titleChanged    (QString))        ,
           this  ,SLOT  (setWindowTitle  (QString))      ) ;
  nConnect(page(),SIGNAL(downloadRequested(const QNetworkRequest &))   ,
           this  ,SLOT  (downloadRequested(const QNetworkRequest &)) ) ;
  settings()->setAttribute(QWebEngineSettings::JavascriptEnabled,true);
  settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows,true);
  settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard,true);
  settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
//  settings()->setLocalStoragePath (plan->Path("Temp/webcache")) ;
//  page()->setForwardUnsupportedContent(true);
//  action = pageAction(QWebPage::OpenLink) ;
//  nConnect(action,SIGNAL(triggered()),
//           this,SLOT(getLink()));
}

void N::WebEngineBrowser::focusInEvent(QFocusEvent * event)
{
  QWebEngineView::focusInEvent(event)                        ;
  if (IsNull(DockBar)) return                          ;
  if (!DockBar->isVisible()) DockBar->setVisible(true) ;
  disconnect(DockBar->Widgets[WebDockBar::Edit    ],SIGNAL(returnPressed(   )),0,0);
  disconnect(DockBar->Widgets[WebDockBar::List    ],SIGNAL(currentIndexChanged(int)),0,0);
  disconnect(DockBar->Actions[WebDockBar::Go      ],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::Stop    ],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::Reload  ],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::Back    ],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::Forward ],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::Analysis],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::History ],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::Desktop ],SIGNAL(triggered    (   )),0,0);
  disconnect(DockBar->Actions[WebDockBar::Save    ],SIGNAL(triggered    (   )),0,0);
  nConnect(DockBar->Widgets[WebDockBar::Edit    ],SIGNAL(returnPressed(   )),this,SLOT(returnPressed()));
  nConnect(DockBar->Widgets[WebDockBar::List    ],SIGNAL(currentIndexChanged(int)),this,SLOT(indexChanged(int)));
  nConnect(DockBar->Actions[WebDockBar::Go      ],SIGNAL(triggered    (   )),this,SLOT(Go      ()));
  nConnect(DockBar->Actions[WebDockBar::Stop    ],SIGNAL(triggered    (   )),this,SLOT(Stop    ()));
  nConnect(DockBar->Actions[WebDockBar::Reload  ],SIGNAL(triggered    (   )),this,SLOT(Reload  ()));
  nConnect(DockBar->Actions[WebDockBar::Back    ],SIGNAL(triggered    (   )),this,SLOT(Back    ()));
  nConnect(DockBar->Actions[WebDockBar::Forward ],SIGNAL(triggered    (   )),this,SLOT(Forward ()));
  nConnect(DockBar->Actions[WebDockBar::Analysis],SIGNAL(triggered    (   )),this,SLOT(Analysis()));
  nConnect(DockBar->Actions[WebDockBar::History ],SIGNAL(triggered    (   )),this,SLOT(History ()));
  nConnect(DockBar->Actions[WebDockBar::Desktop ],SIGNAL(triggered    (   )),this,SLOT(Desktop ()));
  nConnect(DockBar->Actions[WebDockBar::Save    ],SIGNAL(triggered    (   )),this,SLOT(Save    ()));
  DockBar->setUrl(url());
}

void N::WebEngineBrowser::contextMenuEvent (QContextMenuEvent * event)
{
#ifdef XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

  QMenu *menu = new QMenu(this);

  QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

  if (!r.linkUrl().isEmpty()) {
      QAction *newWindowAction = menu->addAction(tr("Open in New &Window"), this, SLOT(openActionUrlInNewWindow()));
      newWindowAction->setData(r.linkUrl());
      QAction *newTabAction = menu->addAction(tr("Open in New &Tab"), this, SLOT(openActionUrlInNewTab()));
      newTabAction->setData(r.linkUrl());
      menu->addSeparator();
      menu->addAction(tr("Save Lin&k"), this, SLOT(downloadLinkToDisk()));
      menu->addAction(tr("&Bookmark This Link"), this, SLOT(bookmarkLink()))->setData(r.linkUrl());
      menu->addSeparator();
      if (!page()->selectedText().isEmpty())
          menu->addAction(pageAction(QWebPage::Copy));
      menu->addAction(tr("&Copy Link Location"), this, SLOT(copyLinkToClipboard()));
  }

  if (!r.imageUrl().isEmpty()) {
      if (!menu->isEmpty())
          menu->addSeparator();
      QAction *newWindowAction = menu->addAction(tr("Open Image in New &Window"), this, SLOT(openActionUrlInNewWindow()));
      newWindowAction->setData(r.imageUrl());
      QAction *newTabAction = menu->addAction(tr("Open Image in New &Tab"), this, SLOT(openActionUrlInNewTab()));
      newTabAction->setData(r.imageUrl());
      menu->addSeparator();
      menu->addAction(tr("&Save Image"), this, SLOT(downloadImageToDisk()));
      menu->addAction(tr("&Copy Image"), this, SLOT(copyImageToClipboard()));
      menu->addAction(tr("C&opy Image Location"), this, SLOT(copyImageLocationToClipboard()))->setData(r.imageUrl().toString());
      menu->addSeparator();
      menu->addAction(tr("Block Image"), this, SLOT(blockImage()))->setData(r.imageUrl().toString());
  }

  if (!page()->selectedText().isEmpty()) {
      if (menu->isEmpty()) {
          menu->addAction(pageAction(QWebPage::Copy));
      } else {
          menu->addSeparator();
      }
      QMenu *searchMenu = menu->addMenu(tr("Search with..."));

      QList<QString> list = ToolbarSearch::openSearchManager()->allEnginesNames();
      for (int i = 0; i < list.count(); ++i) {
          QString name = list.at(i);
          OpenSearchEngine *engine = ToolbarSearch::openSearchManager()->engine(name);
          QAction *action = new OpenSearchEngineAction(engine, searchMenu);
          searchMenu->addAction(action);
          action->setData(name);
      }

      connect(searchMenu, SIGNAL(triggered(QAction *)), this, SLOT(searchRequested(QAction *)));
  }

  QWebElement element = r.element();
  if (!element.isNull()
      && element.tagName().toLower() == QLatin1String("input")
      && element.attribute(QLatin1String("type"), QLatin1String("text")) == QLatin1String("text")) {
      if (menu->isEmpty()) {
          menu->addAction(pageAction(QWebPage::Copy));
      } else {
          menu->addSeparator();
      }

      QVariant variant;
      variant.setValue(element);
      menu->addAction(tr("Add to the toolbar search"), this, SLOT(addSearchEngine()))->setData(variant);
  }

  if (menu->isEmpty()) {
      delete menu;
      menu = page()->createStandardContextMenu();
  } else {
      if (page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
          menu->addAction(pageAction(QWebPage::InspectElement));
  }

  if (!menu->isEmpty()) {
      if (BrowserMainWindow::parentWindow(tabWidget())->menuBar()->isHidden()) {
          menu->addSeparator();
          menu->addAction(BrowserMainWindow::parentWindow(tabWidget())->showMenuBarAction());
      }

      menu->exec(mapToGlobal(event->pos()));
      delete menu;
      return;
  }
  delete menu;

#endif

  QWebEngineView::contextMenuEvent(event) ;
}

void N::WebEngineBrowser::setPath(QUrl url)
{
  setWindowTitle(url.toString()) ;
  if (runJavascript(url)) return ;
  load(url)                      ;
  if (IsNull(DockBar)) return    ;
  DockBar -> setUrl (url)        ;
}

void N::WebEngineBrowser::urlClicked(const QUrl & url)
{
    Notify(url.toString()) ;
  if (runJavascript(url)) return ;
  load(url)                      ;
  if (IsNull(DockBar)) return    ;
  DockBar -> setUrl (url)        ;
}

void N::WebEngineBrowser::urlAlter(const QUrl & url)
{
  if (NotNull(plan))                  {
    plan->showMessage(url.toString()) ;
  }                                   ;
//  if (runJavascript(url)) return ;
//  load(url)                      ;
  if (IsNull(DockBar)) return    ;
  DockBar -> setUrl (QUrl(url))  ;
}

void N::WebEngineBrowser::urlStarted(void)
{
  if (NotNull(Progress))              {
    Progress->setValue(0)             ;
    return                            ;
  }                                   ;
  Progress  = plan->Progress("%v%"  ) ;
  Progress -> setTextVisible(true   ) ;
  QSize m = Progress->maximumSize(  ) ;
  QSize l ( m.width()/2,m.height()  ) ;
  Progress -> setMinimumSize ( l    ) ;
  Progress -> setMaximumSize ( l    ) ;
  Progress -> show           (      ) ;
  Progress -> setRange       ( 0,100) ;
  Progress -> setValue       ( 0    ) ;
}

void N::WebEngineBrowser::urlProgress(int progress)
{
  if (IsNull(Progress)) return ;
  Progress->setValue(progress) ;
}

void N::WebEngineBrowser::urlFinished(bool ok)
{
  if (IsNull(Progress)) return ;
  Progress -> hide        (  ) ;
  Progress -> deleteLater (  ) ;
  Progress  = NULL             ;
  if (ok) Alert ( Done )       ;
}

void N::WebEngineBrowser::urlMessage(const QString & text)
{
  if (IsNull(plan)) return ;
  plan->showMessage(text)  ;
}

bool N::WebEngineBrowser::runJavascript(const QUrl & url)
{
  if (url.scheme() != QLatin1String("javascript")) return false ;
  QString scriptSource                                          ;
  scriptSource = QUrl::fromPercentEncoding                      (
                   url.toString                                 (
                    (QUrl::FormattingOptions)
                     QUrl::TolerantMode                         |
                     QUrl::RemoveScheme).toUtf8()             ) ;
//  QVariant result = page()->mainFrame          (                )
//                          ->evaluateJavaScript ( scriptSource ) ;
  if (NotNull(plan))                                            {
    plan->showMessage(url.toString())                           ;
  }                                                             ;
  return true                                                   ;
}

void N::WebEngineBrowser::downloadRequested(const QNetworkRequest & request)
{
//  Notify("WebBrowser","download request") ;
}

void N::WebEngineBrowser::returnPressed(void)
{
  Go () ;
}

void N::WebEngineBrowser::indexChanged(int index)
{
  Go () ;
}

void N::WebEngineBrowser::Go(void)
{
  if (IsNull(DockBar)) return   ;
  setUrl ( DockBar->Address() ) ;
  Alert ( Click )               ;
}

void N::WebEngineBrowser::Stop(void)
{
  stop    () ;
}

void N::WebEngineBrowser::Reload(void)
{
  reload  () ;
}

void N::WebEngineBrowser::Back(void)
{
  back    () ;
}

void N::WebEngineBrowser::Forward(void)
{
  forward () ;
}

void N::WebEngineBrowser::Analysis(void)
{
  emit Analysis ( this ) ;
}

void N::WebEngineBrowser::History(void)
{
  emit History ( this ) ;
}

void N::WebEngineBrowser::Desktop(void)
{
  if (IsNull(DockBar)) return                           ;
  QDesktopServices :: openUrl ( DockBar -> Address () ) ;
}

void N::WebEngineBrowser::Save(void)
{
  emit Save( this ) ;
}

#ifdef NOT_WORKING_NOW

void N::WebEngineBrowser::getLink(void)
{
  QAction *action = qobject_cast<QAction *>(sender()) ;
  load( QUrl ( QString::fromUtf8( action->data().toUrl().toEncoded() ) ) ) ;
}

void WebView::applyZoom()
{
    setZoomFactor(qreal(m_currentZoom) / 100.0);
}

void WebView::downloadLinkToDisk()
{
    pageAction(QWebPage::DownloadLinkToDisk)->trigger();
}

void WebView::copyLinkToClipboard()
{
    pageAction(QWebPage::CopyLinkToClipboard)->trigger();
}

void WebView::openActionUrlInNewTab()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QWebPage *page = tabWidget()->getView(TabWidget::NewNotSelectedTab, this)->page();
        QNetworkRequest request(action->data().toUrl());
        request.setRawHeader("Referer", url().toEncoded());
        page->mainFrame()->load(request);
    }
}

void WebView::openActionUrlInNewWindow()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QWebPage *page = tabWidget()->getView(TabWidget::NewWindow, this)->page();
        QNetworkRequest request(action->data().toUrl());
        request.setRawHeader("Referer", url().toEncoded());
        page->mainFrame()->load(request);
    }
}

void WebView::openImageInNewWindow()
{
    pageAction(QWebPage::OpenImageInNewWindow)->trigger();
}

void WebView::downloadImageToDisk()
{
    pageAction(QWebPage::DownloadImageToDisk)->trigger();
}

void WebView::copyImageToClipboard()
{
    pageAction(QWebPage::CopyImageToClipboard)->trigger();
}

void WebView::copyImageLocationToClipboard()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        BrowserApplication::clipboard()->setText(action->data().toString());
    }
}

void WebView::blockImage()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        QString imageUrl = action->data().toString();
        AdBlockDialog *dialog = AdBlockManager::instance()->showDialog();
        dialog->addCustomRule(imageUrl);
    }
}

void WebView::bookmarkLink()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        AddBookmarkDialog dialog;
        dialog.setUrl(QString::fromUtf8(action->data().toUrl().toEncoded()));
        dialog.exec();
    }
}

#endif

#endif
