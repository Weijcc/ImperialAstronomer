#include <QPainter>
#include <QDateTime>
#include <QOperatingSystemVersion>
#include <QGraphicsDropShadowEffect>

#include "MasterWidget.h"
#include "LeagueAPI.h"
#include "WindowsAPI.h"

using namespace std::chrono_literals;

Prompt* Prompt::handle = nullptr;

Prompt::Prompt()
  : ui(new Ui::PromptInterface)
{
  this->ui->setupUi(this);
  this->setAttribute(Qt::WA_StaticContents, true);
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  this->setWindowFlags(this->windowFlags() | Qt::Dialog | Qt::FramelessWindowHint);
  this->setWindowIcon(QIcon(":/ResourceFiles/BasicFiles/Icon.ico"));
  this->setWindowTitle("司天监");

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->LogoLabel->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->ClosePushButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->ExitButton->setGraphicsEffect(dropShadowEffect);

  this->ui->TitleLabel->setStyleSheet(
      "color: #FAFAFA;"
      "font: 14px \"HarmonyOS Sans SC\";");

  this->timer = new QTimer(this);
  this->timer->setSingleShot(true);

  this->loadingLabel = new QLabel(this->ui->MessageBoxFrame);
  this->loadingLabel->setGeometry(100, 40, 100, 100);

  this->loadingMovie = new QMovie(":/ResourceFiles/BasicFiles/Loading.gif");
  this->loadingLabel->setMovie(this->loadingMovie);;

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->loadingLabel->setGraphicsEffect(dropShadowEffect);

  connect(this->timer, &QTimer::timeout, this, [&] {
    this->reject();
    QTimer::singleShot(100, [&] {
      this->warning("加载超时");
    });
  });

  connect(this->ui->ExitButton, &QPushButton::clicked, this, &QDialog::accept);
  connect(this->ui->ClosePushButton, &QPushButton::clicked, this, &QDialog::reject);
}

Prompt::~Prompt()
{
  delete this->ui;
}

Prompt* Prompt::instance()
{
  static std::once_flag once;

  std::call_once(
      once,
      [] {
        handle = new Prompt();
      });

  return handle;
}

void Prompt::release()
{
  delete handle;
}

void Prompt::setBase(QWidget* base)
{
  this->base = base;
}

int Prompt::loading()
{
  this->ui->IconLabel->hide();
  this->ui->TextLabel->hide();
  this->ui->ExitButton->hide();
  this->loadingMovie->start();
  this->loadingLabel->show();

  auto baseSize = this->base->size();
  auto basePos = this->base->mapToGlobal(QPoint(0, 0));
  auto size = this->size();
  this->setGeometry(
      basePos.x() + (baseSize.width() - size.width()) / 2,
      basePos.y() + (baseSize.height() - size.height()) / 2,
      size.width(),
      size.height());
  this->ui->TitleLabel->setText("加载中");

  this->timer->start(1000 * 10);
  auto result = this->exec();
  this->timer->stop();
  this->loadingLabel->hide();
  this->loadingMovie->stop();
  this->ui->ExitButton->show();
  this->ui->TextLabel->show();
  this->ui->IconLabel->show();

  return result;
}

int Prompt::error(const QString& text)
{
  auto baseSize = this->base->size();
  auto basePos = this->base->mapToGlobal(QPoint(0, 0));
  auto size = this->size();
  this->setGeometry(
      basePos.x() + (baseSize.width() - size.width()) / 2,
      basePos.y() + (baseSize.height() - size.height()) / 2,
      size.width(),
      size.height());
  this->ui->TitleLabel->setText("错误");
  this->ui->TextLabel->setText(text);
  this->ui->TextLabel->setStyleSheet(
      "color: #FAFAFA;"
      "font: 16px \"HarmonyOS Sans SC\";");
  this->ui->IconLabel->setPixmap({":/ResourceFiles/BasicFiles/Error.png"});
  this->activateWindow();
  this->setWindowState(this->windowState() | Qt::WindowActive);
  return this->exec();
}

int Prompt::warning(const QString& text)
{
  auto baseSize = this->base->size();
  auto basePos = this->base->mapToGlobal(QPoint(0, 0));
  auto size = this->size();
  this->setGeometry(
      basePos.x() + (baseSize.width() - size.width()) / 2,
      basePos.y() + (baseSize.height() - size.height()) / 2,
      size.width(),
      size.height());
  this->ui->TitleLabel->setText("警告");
  this->ui->TextLabel->setText(text);
  this->ui->TextLabel->setStyleSheet(
      "color: #FAFAFA;"
      "font: 16px \"HarmonyOS Sans SC\";");
  this->ui->IconLabel->setPixmap({":/ResourceFiles/BasicFiles/Warning.png"});
  return this->exec();
}

int Prompt::information(const QString& text)
{
  auto baseSize = this->base->size();
  auto basePos = this->base->mapToGlobal(QPoint(0, 0));
  auto size = this->size();
  this->setGeometry(
      basePos.x() + (baseSize.width() - size.width()) / 2,
      basePos.y() + (baseSize.height() - size.height()) / 2,
      size.width(),
      size.height());
  this->ui->TitleLabel->setText("提示");
  this->ui->TextLabel->setStyleSheet(
      "color: #FAFAFA;"
      "font: 16px \"HarmonyOS Sans SC\";");
  this->ui->IconLabel->setPixmap({":/ResourceFiles/BasicFiles/Information.png"});
  return this->exec();
}

void Prompt::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    this->movePositionFlag = true;
    this->movePosition = event->globalPosition() - this->pos();
    event->accept();
  }
}

void Prompt::mouseMoveEvent(QMouseEvent* event)
{
  if (this->movePositionFlag && (event->buttons() & Qt::LeftButton))
  {
    this->move((event->globalPosition() - this->movePosition).toPoint());
    event->accept();
  }
}

void Prompt::mouseReleaseEvent([[maybe_unused]] QMouseEvent* event)
{
  this->movePositionFlag = false;
}

MasterWidget::MasterWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::MasterInterface)
{
  this->ui->setupUi(this);
  this->setAttribute(Qt::WA_StaticContents, true);
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  this->setWindowFlags(this->windowFlags() | Qt::Window | Qt::FramelessWindowHint);
  this->setWindowIcon(QIcon(":/ResourceFiles/BasicFiles/Icon.ico"));

  HMODULE module = ::GetModuleHandle(L"user32.dll");
  if (module)
  {
    ACCENTPOLICY accent = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 0x000001E0, 0xB3424242, 0x00000000};
    WINDOWCOMPOSITIONATTRIBDATA attribute = {WCA_ACCENT_POLICY, &accent, sizeof(ACCENTPOLICY)};
    auto setWindowCompositionAttribute = reinterpret_cast<setWindowCompositionAttributeHandle*>(
      ::GetProcAddress(module, "SetWindowCompositionAttribute"));
    setWindowCompositionAttribute(reinterpret_cast<HWND>(this->winId()), &attribute);
    ::FreeLibrary(module);
  }

  Prompt::instance()->setBase(this);
  this->propertyAnimation = new QPropertyAnimation(this->ui->SideGroupBox, "pos", this);
  this->leagueClientUxAPI = LeagueAPI::instance();
  this->leagueClientUxBaseThread = new QThread();

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->SideFrame->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(66, 66, 66));
  this->ui->ClosePushButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->HomePageRadioButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->MatchHistoryPageRadioButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->OtherFunctionPageRadioButton->setGraphicsEffect(dropShadowEffect);

  auto key = QDateTime::currentDateTime().toString("ddd").toUtf8();
  this->ui->SideLabel->setPixmap({":/ResourceFiles/BasicFiles/Left.png"});
  this->ui->EchoTextLabel->setText(this->echoTextMap[key].first);
  this->ui->EchoerTextLabel->setText(this->echoTextMap[key].second);

  this->homeWidget = new HomeWidget(this->ui->MasterFrame);
  this->homeWidget->hide();
  this->matchHistoryWidget = new MatchHistoryWidget(this->ui->MasterFrame);
  this->matchHistoryWidget->hide();
  this->otherFunctionWidget = new OtherFunctionWidget(this->ui->MasterFrame);
  this->otherFunctionWidget->hide();
  this->assistantWidget = new AssistantWidget();
  this->loadingWidget = new LoadingWidget();
  this->loadingWidget->ui->LoadingProgressBar->setFormat("Loading...");
  this->loadingWidget->show();

  this->systemTrayIcon = new QSystemTrayIcon(this);
  this->systemTrayIcon->setToolTip("司天监");
  this->systemTrayIcon->setIcon(QIcon(":/ResourceFiles/BasicFiles/Icon.ico"));

  auto systemTrayMenu = new QMenu(this);

  auto systemTrayAction = new QAction(QIcon(LeagueAPI::readProfileIcon(9)), "显示", this);
  systemTrayMenu->addAction(systemTrayAction);

  connect(
      systemTrayAction,
      &QAction::triggered,
      this,
      [&]() {
        LeagueAPI::instance()->modifyWaitTime(200);
        this->show();
        this->activateWindow();
        this->setWindowState(this->windowState() | Qt::WindowActive);
      });

  systemTrayAction = new QAction(QIcon(LeagueAPI::readProfileIcon(10)), "退出", this);
  systemTrayMenu->addAction(systemTrayAction);

  connect(
      systemTrayAction,
      &QAction::triggered,
      this,
      [&]() {
        this->systemTrayIcon->hide();
        Prompt::release();
        LeagueAPI::instance()->modifyWaitTime(0);
        this->leagueClientUxAPI->exit();
        this->leagueClientUxBaseThread->wait();
        this->leagueClientUxBaseThread->deleteLater();
        QApplication::quit();
      });

  connect(
    this->leagueClientUxAPI,
    &LeagueAPI::summonerError,
    this,
    [&]() {
      this->hide();
      this->systemTrayIcon->showMessage("错误", "客户端错误出现错误，程序即将退出", QSystemTrayIcon::MessageIcon::Critical);
      QThread::msleep(1000);
      this->systemTrayIcon->hide();
      Prompt::release();
      LeagueAPI::instance()->modifyWaitTime(0);
      this->leagueClientUxAPI->exit();
      this->leagueClientUxBaseThread->wait();
      this->leagueClientUxBaseThread->deleteLater();
      QApplication::quit();
    });

  this->systemTrayIcon->setContextMenu(systemTrayMenu);

  connect(
      this->systemTrayIcon,
      &QSystemTrayIcon::activated,
      this,
      [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick)
        {
          LeagueAPI::instance()->modifyWaitTime(200);
          this->show();
          this->activateWindow();
          this->setWindowState(this->windowState() | Qt::WindowActive);
        }
      });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::initializeFail,
      this,
      [&] {
        this->hide();
        this->loadingWidget->hide();
        Prompt::instance()->error("请先登录联盟客户端！");
        Prompt::release();
        this->loadingWidget->deleteLater();
        this->leagueClientUxBaseThread->deleteLater();
        QApplication::quit();
      });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::errorString,
      this,
      [&](const QString& text) {
        Prompt::instance()->error(text);
      });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::updateProgressRate,
      this,
      [&](const QByteArray& text) {
        auto value = this->loadingWidget->ui->LoadingProgressBar->value() + 10;

        this->loadingWidget->ui->LoadingProgressBar->setFormat(text);
        this->loadingWidget->ui->LoadingProgressBar->setValue(value);

        if (value == 160)
        {
          disconnect(this->leagueClientUxAPI, &LeagueAPI::updateProgressRate, this, nullptr);
          for (auto index = value; index <= 180; index++)
          {
            this->loadingWidget->ui->LoadingProgressBar->setValue(index);
            QThread::msleep(6);
          }
          this->loadingWidget->deleteLater();
          this->display();
        }
      });

  connect(
      this->propertyAnimation,
      &QPropertyAnimation::finished,
      this,
      [&] {
        if (this->ui->SideGroupBox->pos().x() == (-85))
          this->ui->SideLabel->setPixmap({":/ResourceFiles/BasicFiles/Right.png"});
        else
          this->ui->SideLabel->setPixmap({":/ResourceFiles/BasicFiles/Left.png"});
        this->setAttribute(Qt::WA_Hover, true);
      });
  connect(
      this->ui->ClosePushButton,
      &QPushButton::clicked,
      this,
      [&] {
        this->hide();
        LeagueAPI::instance()->modifyWaitTime(1000);
      });

  connect(
      this->ui->HomePageRadioButton,
      &QRadioButton::clicked,
      this,
      [&](bool checked) {
        if (checked)
        {
          this->matchHistoryWidget->hide();
          this->otherFunctionWidget->hide();
          this->homeWidget->show();
        }
        else
        {
          this->homeWidget->hide();
        }
      });
  connect(
      this->ui->MatchHistoryPageRadioButton,
      &QRadioButton::clicked,
      this,
      [&](bool checked) {
        if (checked)
        {
          this->homeWidget->hide();
          this->otherFunctionWidget->hide();
          this->matchHistoryWidget->show();
        }
        else
        {
          this->matchHistoryWidget->hide();
        }
      });
  connect(
      this->ui->OtherFunctionPageRadioButton,
      &QRadioButton::clicked,
      this,
      [&](bool checked) {
        if (checked)
        {
          this->homeWidget->hide();
          this->matchHistoryWidget->hide();
          this->otherFunctionWidget->show();
        }
        else
        {
          this->otherFunctionWidget->hide();
        }
      });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorLocalSummoner,
      this,
      [&](const QJsonDocument& details) {
        this->homeWidget->updateSummonerInfoGroupBox(details);
      });

  connect(
      this->otherFunctionWidget->ui->AssistantCheckBox,
      &QCheckBox::checkStateChanged,
      this,
      [&](Qt::CheckState state) {
        this->assistantWidget->display(state == Qt::Checked);
      });

  connect(
      this->assistantWidget,
      &AssistantWidget::assistantClose,
      this,
      [&] {
        this->otherFunctionWidget->ui->AssistantCheckBox->setCheckState(Qt::Unchecked);
      });

  this->ui->SideGroupBox->raise();
  this->ui->ClosePushButton->raise();

  this->leagueClientUxAPI->moveToThread(this->leagueClientUxBaseThread);
  connect(this->leagueClientUxBaseThread, &QThread::started, this->leagueClientUxAPI, &LeagueAPI::exec);
  this->leagueClientUxBaseThread->start(QThread::HighestPriority);
  this->systemTrayIcon->show();
}

MasterWidget::~MasterWidget()
{
  this->assistantWidget->deleteLater();
  delete this->ui;
}

void MasterWidget::display()
{
  LeagueAPI::instance()->modifyWaitTime(200);
  this->show();
  this->activateWindow();
  this->setWindowState(this->windowState() | Qt::WindowActive);
  this->propertyAnimation->setDuration(300);
  this->propertyAnimation->setStartValue(this->ui->SideGroupBox->pos());
  this->propertyAnimation->setEndValue(QPoint((-85), 128));
  this->propertyAnimation->start();
}

bool MasterWidget::event(QEvent* event)
{
  auto type = event->type();
  if (type == QEvent::HoverEnter || type == QEvent::HoverLeave || type == QEvent::HoverMove)
  {
    auto hover = reinterpret_cast<QHoverEvent*>(event);
    auto pos = this->mapFromGlobal(hover->globalPosition());
    auto sidePos = this->ui->SideGroupBox->pos();

    this->propertyAnimation->setDuration(300);
    this->propertyAnimation->setStartValue(sidePos);

    if (sidePos.x() == -10)
      this->propertyAnimation->setEndValue(QPoint((-85), sidePos.y()));
    else
      this->propertyAnimation->setEndValue(QPoint((-10), sidePos.y()));

    if ((pos.x() < 70 && pos.y() > 133 && pos.y() < 433 && sidePos.x() == (-85))
      || ((pos.x() > 90 || pos.y() < 133 || pos.y() > 433) && sidePos.x() == (-10)))
    {
      this->propertyAnimation->start();
      this->setAttribute(Qt::WA_Hover, false);
    }
  }
  return QWidget::event(event);
}

void MasterWidget::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    this->movePositionFlag = true;
    this->movePosition = event->globalPosition() - this->pos();
    event->accept();
  }
}

void MasterWidget::mouseMoveEvent(QMouseEvent* event)
{
  if (this->movePositionFlag && (event->buttons() & Qt::LeftButton))
  {
    this->move((event->globalPosition() - this->movePosition).toPoint());
    event->accept();
  }
}

void MasterWidget::mouseReleaseEvent([[maybe_unused]] QMouseEvent* event)
{
  this->movePositionFlag = false;
}

void MasterWidget::closeEvent(QCloseEvent* event)
{
  this->leagueClientUxAPI->exit();
  this->leagueClientUxBaseThread->wait();
  this->leagueClientUxBaseThread->deleteLater();
  event->accept();
}

void MasterWidget::leaveEvent(QEvent* event)
{
  auto sidePos = this->ui->SideGroupBox->pos();
  if (sidePos.x() == -10)
  {
    this->propertyAnimation->setEndValue(QPoint((-85), sidePos.y()));
    this->propertyAnimation->start();
    this->setAttribute(Qt::WA_Hover, false);
  }
  event->accept();
}
