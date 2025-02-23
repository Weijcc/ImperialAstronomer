#include <QApplication>
#include <QFile>
#include <QTranslator>
#include <QFontDatabase>
#include <QResource>

#include <QTimer>
#include <QSharedMemory>
#include <QSystemSemaphore>

#include "MasterWidget.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  QTranslator translator;

  if (translator.load(QApplication::applicationDirPath() + "/translations/qt_zh_CN.qm"))
  {
    QApplication::installTranslator(&translator);
  }

  QResource::registerResource(QApplication::applicationDirPath() + "/ResourceBinrary.rcc");

  QFontDatabase::addApplicationFont(":/ResourceFiles/BasicFiles/YSYRXK.ttf");
  QFontDatabase::addApplicationFont(":/ResourceFiles/BasicFiles/JetBrainsMono-Medium.ttf");
  QFontDatabase::addApplicationFont(":/ResourceFiles/BasicFiles/HarmonyOS_Sans_SC_Bold.ttf");
  QFile styleFile(":/ResourceFiles/BasicFiles/StyleSheet.css");
  if (styleFile.open(QIODevice::ReadOnly))
  {
    app.setStyleSheet(styleFile.readAll());
    styleFile.close();
  }

  QSystemSemaphore semaphore("IASemaphore", 1, QSystemSemaphore::Open);
  semaphore.acquire();

  QSharedMemory shared("IAShared");
  if (shared.attach())
  {
    semaphore.release();
    QSystemTrayIcon systemTrayIcon;
    systemTrayIcon.setIcon(QIcon(":/ResourceFiles/BasicFiles/Icon.ico"));
    systemTrayIcon.show();
    systemTrayIcon.showMessage("错误", "请不要重复运行", QSystemTrayIcon::MessageIcon::Critical);

    QTimer::singleShot(500, [&] {
      systemTrayIcon.hide();
      QApplication::exit(-1);
    });
    return QApplication::exec();
  }

  shared.create(1);
  semaphore.release();

  MasterWidget widget;
  return QApplication::exec();
}
