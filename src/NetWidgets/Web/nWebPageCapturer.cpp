#include <netwidgets.h>

N::WebPageCapturer:: WebPageCapturer (void        )
                   : Thread          (            )
                   , Object          (0,Types::URL)
                   , FtpPlan         (NULL        )
{
}

N::WebPageCapturer::~WebPageCapturer (void)
{
}

bool N::WebPageCapturer::isRunning(void)
{
  return ( running < 2 ) ;
}

void N::WebPageCapturer::run(void)
{
  if (IsNull(FtpPlan)) return ;
  if (uuid<=0        ) return ;
  started  (                ) ;
  fetch    (                ) ;
  finished (                ) ;
}

void N::WebPageCapturer::assureUuid(QUrl & url)
{
  Url = url                                     ;
  SqlConnection  SC ( FtpPlan->sql )            ;
  NetworkManager NM ( FtpPlan      )            ;
  if (SC.open("nWebPageCapturer","assureUuid")) {
    uuid = NM . PageUuid ( SC , Url           ) ;
    SC . close  ( )                             ;
  }                                             ;
  SC   . remove ( )                             ;
}

N::HttpParser N::WebPageCapturer::Response(void)
{
  return HttpParser(Header) ;
}

void N::WebPageCapturer::started(void)
{
}

void N::WebPageCapturer::finished(void)
{
}

void N::WebPageCapturer::fetch(void)
{
  Ftp ftp                                           ;
  Header . clear        (     )                     ;
  Body   . clear        (     )                     ;
  if (Requests.count()>0) ftp.Requests = Requests   ;
  Header = ftp . header ( Url )                     ;
  if (Header.size()<=0) return                      ;
  HttpParser HR = Response()                        ;
  int sc = HR.statusCode()                          ;
  if (sc!=200) return                               ;
  if (!HR.hasContentType()) return                  ;
  QString CT  = HR.contentType()                    ;
  if (!mimeTypes.contains(CT)) return               ;
  QString FMT = mimeTypes [ CT ]                    ;
  QString fx  = QString("%1.%2").arg(uuid).arg(FMT) ;
  QString filename = TempDir.absoluteFilePath (fx ) ;
  QFile::remove(filename)                           ;
  QFile F(filename)                                 ;
  ftp . download ( Url , F )                        ;
  if (F.open(QIODevice::ReadOnly))                  {
    Body = F.readAll ( )                            ;
    F    . close     ( )                            ;
  }                                                 ;
  QFile::remove(filename)                           ;
}
