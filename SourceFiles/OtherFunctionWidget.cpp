#include <QDesktopServices>
#include <QStandardPaths>
#include <QCompleter>
#include <QAbstractItemView>
#include <QFile>
#include <QGraphicsDropShadowEffect>
#include <QLineEdit>

#include "MasterWidget.h"
#include "OtherFunctionWidget.h"

OtherFunctionWidget::OtherFunctionWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::OtherFunctionInterface)
{
  this->ui->setupUi(this);
  this->setAttribute(Qt::WA_StaticContents, true);

  if (parent)
    this->setGeometry(parent->rect());

  this->leagueClientUxAPI = LeagueAPI::instance();

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->InfoChangePushButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->CreateLinkPushButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->GithubToolButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->BilibiliToolButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->AutoAcceptCheckBox->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->AssistantCheckBox->setGraphicsEffect(dropShadowEffect);

  this->ui->GithubToolButton->setIconSize(QSize(40, 40));
  this->ui->GithubToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  this->ui->GithubToolButton->setIcon(QIcon(":/ResourceFiles/BasicFiles/Github.png"));
  this->ui->GithubToolButton->setText("@Weijcc");

  this->ui->BilibiliToolButton->setIconSize(QSize(40, 40));
  this->ui->BilibiliToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  this->ui->BilibiliToolButton->setIcon(QIcon(":/ResourceFiles/BasicFiles/Bilibili.png"));
  this->ui->BilibiliToolButton->setText("@天狼西射zZ");

  this->ui->StateSelectComboBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->ui->StateSelectComboBox->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  this->ui->RankedSelectComboBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->ui->RankedSelectComboBox->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  this->ui->AutoBanChampionComboBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->ui->AutoBanChampionComboBox->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  this->ui->AutoPickChampionComboBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->ui->AutoPickChampionComboBox->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  this->ui->CareerBackgroundComboBox->setIconSize(QSize(128, 72));

  auto rankedDivisionList = this->leagueClientUxAPI->readRankedDivisionList();

  for (const auto& rankedDivision : rankedDivisionList)
  {
    this->ui->RankedSelectComboBox->addItem(
        this->leagueClientUxAPI->readRankedIcon(rankedDivision.second.first),
        rankedDivision.first,
        rankedDivision.first);
  }

  connect(
      this->ui->AutoBanChampionComboBox->lineEdit(),
      &QLineEdit::textChanged,
      this,
      [&](const QString& text) {
        if (text.isEmpty())
        {
          this->ui->AutoBanChampionComboBox->setCurrentIndex(-1);
        }
      });

  connect(
      this->ui->AutoPickChampionComboBox->lineEdit(),
      &QLineEdit::textChanged,
      this,
      [&](const QString& text) {
        if (text.isEmpty())
        {
          this->ui->AutoPickChampionComboBox->setCurrentIndex(-1);
        }
      });

  connect(
      this->ui->CareerBackgroundComboBox->lineEdit(),
      &QLineEdit::textChanged,
      this,
      [&](const QString& text) {
        if (text.isEmpty())
        {
          this->ui->CareerBackgroundComboBox->setCurrentIndex(-1);
        }
      });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessChampionDict,
      this,
      [&](const QMap<qint32, QByteArray>& dict) {
        QList<QString> values;
        auto keys = dict.keys();

        for (const auto& championId : keys)
        {
          values.push_back(dict[championId]);
          this->ui->AutoBanChampionComboBox->addItem(
              this->leagueClientUxAPI->readChampionIcon(championId),
              values.back(),
              championId);
          this->ui->AutoPickChampionComboBox->addItem(
              this->leagueClientUxAPI->readChampionIcon(championId),
              values.back(),
              championId);
        }
        auto completer = new QCompleter(this->ui->AutoBanChampionComboBox->model(), this->ui->OtherFunctionFrame);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        completer->popup()->setObjectName("CompleterItemView");
        this->ui->AutoBanChampionComboBox->setCompleter(completer);
        this->ui->AutoBanChampionComboBox->setCurrentIndex(-1);
        this->ui->AutoBanChampionComboBox->lineEdit()->setPlaceholderText("输入搜索");

        completer = new QCompleter(this->ui->AutoPickChampionComboBox->model(), this->ui->OtherFunctionFrame);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        completer->popup()->setObjectName("CompleterItemView");
        this->ui->AutoPickChampionComboBox->setCompleter(completer);
        this->ui->AutoPickChampionComboBox->setCurrentIndex(-1);
        this->ui->AutoPickChampionComboBox->lineEdit()->setPlaceholderText("输入搜索");
      },
      Qt::SingleShotConnection);

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessSkinsDict,
      this,
      [&](const QMap<qint32, std::pair<QByteArray, QByteArray>>& skinsDict) {
        for (const auto& skinDict : skinsDict)
        {
          auto id = skinsDict.key(skinDict);
          auto name = skinDict.first;
          auto file = skinDict.second;
          this->ui->CareerBackgroundComboBox->addItem(
              QPixmap(file),
              name,
              id);
        }
        auto completer = new QCompleter(this->ui->CareerBackgroundComboBox->model(), this->ui->OtherFunctionFrame);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        completer->setFilterMode(Qt::MatchContains);
        completer->popup()->setObjectName("CompleterItemView");
        this->ui->CareerBackgroundComboBox->setCompleter(completer);
        this->ui->CareerBackgroundComboBox->setCurrentIndex(-1);
        this->ui->CareerBackgroundComboBox->lineEdit()->setPlaceholderText("输入搜索");
      },
      Qt::SingleShotConnection);

  connect(
      this->ui->InfoChangePushButton,
      &QPushButton::clicked,
      this,
      [&] {
        const QMap<QByteArray, QByteArray> availabilityDict = {
            {"离线", "offline"}, {"离开", "away"}, {"游戏中", "dnd"}, {"在线", "chat"}, {"在线分组", "mobile"}};

        QJsonObject data;
        QJsonObject lol;

        auto stateText = this->ui->StateSelectComboBox->currentText().toUtf8();
        auto rankedName = this->ui->RankedSelectComboBox->currentText().toUtf8();
        auto autoBanId = this->ui->AutoBanChampionComboBox->currentData().toInt();
        auto autoPickId = this->ui->AutoPickChampionComboBox->currentData().toInt();
        auto plainText = this->ui->SignaturePlainTextEdit->toPlainText();
        auto skinId = this->ui->CareerBackgroundComboBox->currentData().toInt();

        QJsonObject background;
        background.insert("key", "backgroundSkinId");
        background.insert("value", skinId);

        if (autoBanId && autoPickId && autoBanId == autoPickId)
        {
          Prompt::instance()->error("禁用和选用不能相同");
          return;
        }

        stateText = (stateText == "不修改" ? QByteArray("chat") : availabilityDict[stateText]);
        data.insert("availability", QString(stateText));
        data.insert("statusMessage", plainText);
        lol.insert("rankedLeagueQueue", "RANKED_SOLO_5x5");
        lol.insert("rankedLeagueTier", QString(this->leagueClientUxAPI->readRankedPair(rankedName).first));
        lol.insert("rankedLeagueDivision", QString(this->leagueClientUxAPI->readRankedPair(rankedName).second));
        data.insert("lol", lol);

        this->leagueClientUxAPI->modifyChatMeInfo(QJsonDocument(data));
        if (autoBanId)
          this->leagueClientUxAPI->modifyAutoBanChampion(autoBanId);

        if (autoPickId)
          this->leagueClientUxAPI->modifyAutoPickChampion(autoPickId);

        if (skinId > 999)
          this->leagueClientUxAPI->modifyLocalSummonerProfile(QJsonDocument(background));
      });

  connect(
      this->ui->AutoAcceptCheckBox,
      &QCheckBox::checkStateChanged,
      this,
      [&](Qt::CheckState state) {
        this->leagueClientUxAPI->modifyMatchmakingAutoAccept(state == Qt::Checked);
      });

  connect(
      this->ui->CreateLinkPushButton,
      &QPushButton::clicked,
      this,
      [&]() {
        QFile(
            this->leagueClientUxAPI->readGameInstallPath() + "Launcher\\Client.exe").link(
            QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/League of Legends.lnk");
      });

  connect(
      this->ui->GithubToolButton,
      &QToolButton::clicked,
      this,
      [&] {
        QDesktopServices::openUrl(QUrl(QString("https://github.com/Weijcc")));
      });

  connect(
      this->ui->BilibiliToolButton,
      &QToolButton::clicked,
      this,
      [&] {
        QDesktopServices::openUrl(QUrl(QString("https://space.bilibili.com/1261262823")));
      });
}

OtherFunctionWidget::~OtherFunctionWidget()
{
  delete this->ui;
}