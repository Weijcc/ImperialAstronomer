#include <QTime>
#include <QTimer>
#include <QTimeZone>
#include <QDateTime>
#include <QClipboard>
#include <QGraphicsDropShadowEffect>

#include "MasterWidget.h"
#include "MatchHistoryWidget.h"

using namespace std::chrono_literals;

MatchHistoryWidget::MatchHistoryWidget(QWidget* parent) : QWidget(parent), ui(new Ui::MatchHistoryInterface)
{
  this->ui->setupUi(this);
  this->setAttribute(Qt::WA_StaticContents, true);

  if (parent)
    this->setGeometry(parent->rect());

  this->leagueClientUxAPI = LeagueAPI::instance();

  this->gameDetailsListWidgetMenu = new QMenu(this->ui->GameDetailsListWidget);
  this->addFriendAction =
      new QAction(QIcon(this->leagueClientUxAPI->readProfileIcon(7)), "添加好友", this->ui->GameDetailsListWidget);
  this->monitorMatchHistoryAction =
      new QAction(QIcon(this->leagueClientUxAPI->readProfileIcon(10)), "查看信息", this->ui->GameDetailsListWidget);
  this->copySummonerFullNameAction =
      new QAction(QIcon(this->leagueClientUxAPI->readProfileIcon(9)), "复制昵称", this->ui->GameDetailsListWidget);
  this->gameDetailsListWidgetMenu->addAction(this->addFriendAction);
  this->gameDetailsListWidgetMenu->addAction(this->copySummonerFullNameAction);
  this->gameDetailsListWidgetMenu->addAction(this->monitorMatchHistoryAction);

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->MatchHistoryListWidget->setGraphicsEffect(dropShadowEffect);
  this->ui->MatchHistoryListWidget->setIconSize(QSize(42, 42));

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->GameDetailsListWidget->setGraphicsEffect(dropShadowEffect);
  this->ui->GameDetailsListWidget->hide();

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->SummonerInfoGroupBox->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->SummonerIconLabel->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->MatchHistoryListTextLabel->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->GameDetailsListTextLabel->setGraphicsEffect(dropShadowEffect);
  this->ui->GameDetailsListTextLabel->hide();

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->SummonerExpeProgressBar->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->FindSummonerTagLineEdit->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->FindSummonerNameLineEdit->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->FindSummonerPushButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->LocalSummonerPushButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->MasteryChampionTableWidget->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->RankedTableWidget->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->PreviousRankedTableWidget->setGraphicsEffect(dropShadowEffect);

  auto closePushButton = new QPushButton(this->ui->GameDetailsListWidget);
  closePushButton->setGeometry(763, 2, 26, 26);
  closePushButton->setObjectName("ClosePushButton");
  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  closePushButton->setGraphicsEffect(dropShadowEffect);

  auto intValidator = new QIntValidator();
  intValidator->setRange(0, 99999);
  this->ui->FindSummonerNameLineEdit->setPlaceholderText("玩家昵称");
  this->ui->FindSummonerTagLineEdit->setPlaceholderText("昵称编号");
  this->ui->FindSummonerTagLineEdit->setValidator(intValidator);

  this->ui->RankedTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  this->ui->RankedTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  this->ui->PreviousRankedTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  this->ui->PreviousRankedTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  connect(this->ui->FindSummonerPushButton, &QPushButton::clicked, this, [&] {
    auto name = this->ui->FindSummonerNameLineEdit->text();
    auto tagLine = this->ui->FindSummonerTagLineEdit->text();

    auto matchHistoryName = this->matchHistorySummonerDetails["gameName"].toString();
    auto matchHistoryTagLine = this->matchHistorySummonerDetails["tagLine"].toString();

    if (name.isEmpty() || tagLine.isEmpty())
    {
      Prompt::instance()->error("玩家昵称#编号不能为空");
      return;
    }

    if (name == matchHistoryName && tagLine == matchHistoryTagLine)
    {
      Prompt::instance()->warning("已就绪");
      return;
    }
    auto fullName = name + "#" + tagLine;
    this->leagueClientUxAPI->modifyMonitorSummoner(fullName.toUtf8());
    this->display(fullName.toUtf8());
  });

  connect(this->ui->LocalSummonerPushButton, &QPushButton::clicked, this, [&] {
    this->display(this->localSummonerDetails["puuid"].toString().toUtf8());
  });

  connect(this->ui->FindSummonerNameLineEdit, &QLineEdit::textEdited, this, [&](const QString& text) {
    auto tagLinePos = text.indexOf("#");
    if (tagLinePos != -1)
    {
      auto name = text.mid(0, tagLinePos);
      auto tagLine = text.mid(tagLinePos + 1);
      this->ui->FindSummonerNameLineEdit->setText(name);
      this->ui->FindSummonerTagLineEdit->setText(tagLine);
    }
  });

  connect(closePushButton, &QPushButton::clicked, this, [&] {
    this->ui->GameDetailsListWidget->hide();
    this->ui->GameDetailsListTextLabel->hide();
    this->ui->FindMatchHistoryGroupBox->show();
    auto item = this->ui->MatchHistoryListWidget->currentItem();
    if (item)
      item->setSelected(false);
  });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorMatchHistory,
      this,
      &MatchHistoryWidget::updateMatchHistoryListWidget);

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorSummoner,
      this,
      &MatchHistoryWidget::updateSummonerInfoGroupBox);

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorRankedStats,
      this,
      &MatchHistoryWidget::updateSummonerInfoGroupBox);

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessSummonerRankedList,
      this,
      &MatchHistoryWidget::updateRankedListLabel);

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorChampionMastery,
      this,
      &MatchHistoryWidget::updateMasteryChampionTableWidget);

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorRankedStats,
      this,
      &MatchHistoryWidget::updateRankedTableWidget);

  connect(this->ui->MatchHistoryListWidget, &QListWidget::itemClicked, this, [&] {
    auto index = this->ui->MatchHistoryListWidget->row(this->ui->MatchHistoryListWidget->currentItem());
    this->updateGameDetailsListWidget(index);
  });

  connect(this->ui->GameDetailsListWidget, &QListWidget::itemClicked, this, [&](QListWidgetItem* item) {
    item->setSelected(false);
  });

  connect(this->ui->GameDetailsListWidget, &QListWidget::customContextMenuRequested, this, [&](const QPoint& pos) {
    auto items = this->ui->GameDetailsListWidget->selectedItems();
    if (items.count() && items.at(0) && !items.at(0)->data(Qt::UserRole).toJsonObject().isEmpty())
    {
      auto object = items.at(0)->data(Qt::UserRole).toJsonObject();
      auto summonerId = object["summonerId"].toInteger();
      auto summonerFullName = object["fullName"].toString();
      auto currentSummonerId = this->matchHistorySummonerDetails.object()["summonerId"].toInteger();
      auto puuid = object["fullName"].toString();

      this->addFriendAction->setData(summonerFullName);
      this->addFriendAction->setEnabled(!summonerFullName.isEmpty());
      this->copySummonerFullNameAction->setData(puuid);
      this->copySummonerFullNameAction->setEnabled(summonerId);
      this->monitorMatchHistoryAction->setData(puuid);
      this->monitorMatchHistoryAction->setEnabled(summonerId && (summonerId != currentSummonerId));
      this->gameDetailsListWidgetMenu->exec(QCursor::pos());
    }
  });

  connect(this->gameDetailsListWidgetMenu, &QMenu::aboutToHide, this, [&] {
    auto items = this->ui->GameDetailsListWidget->selectedItems();
    if (items.count() && items.at(0))
      items.at(0)->setSelected(false);
  });

  connect(this->addFriendAction, &QAction::triggered, this, [&] {
    this->leagueClientUxAPI->friendRequest(this->addFriendAction->data().toByteArray());
  });

  connect(this->monitorMatchHistoryAction, &QAction::triggered, this, [&] {
    this->display(this->monitorMatchHistoryAction->data().toByteArray());
  });

  connect(this->copySummonerFullNameAction, &QAction::triggered, this, [&] {
    QApplication::clipboard()->setText(this->copySummonerFullNameAction->data().toString());
  });
}

MatchHistoryWidget::~MatchHistoryWidget()
{
  delete this->ui;
}

void MatchHistoryWidget::display(const QByteArray& details)
{
  this->hide();
  this->leagueClientUxAPI->modifyMonitorSummoner(details);
  Prompt::instance()->loading();
  this->ui->FindMatchHistoryGroupBox->show();
  this->ui->GameDetailsListWidget->hide();
  this->ui->GameDetailsListTextLabel->hide();
}

void MatchHistoryWidget::updated()
{
  if (!Prompt::instance()->isHidden())
  {
    if (this->updatedBits == 0x1F)
    {
      this->show();
      this->updatedBits = 0;
      Prompt::instance()->reject();
    }
  }
  else
  {
    this->updatedBits = 0;
  }
}

QJsonDocument MatchHistoryWidget::readMatchHistorySummonerDetails()
{
  return this->matchHistorySummonerDetails;
}

void MatchHistoryWidget::updateRankedListLabel(const QJsonDocument& details)
{
  auto rankedList = details.array();
  if (rankedList.count() && this->gameRankedFlexLabelList.count() && this->gameRankedSolo5LabelList.count())
  {
    QMap<qint64, std::pair<QString, QString>> rankedListDict;

    for (const auto& ranked : rankedList)
    {
      auto achievedTiers = ranked.toObject()["achievedTiers"].toArray();
      auto summonerId = ranked.toObject()["summonerId"].toInteger();
      rankedListDict[summonerId] = std::make_pair("", "");

      for (const auto& achievedTier : achievedTiers)
      {
        auto object = achievedTier.toObject();
        auto division = object["division"].toInt();
        auto tier = object["tier"].toString();
        auto queueType = object["queueType"].toString();
        auto divisionText = division ? QString::number(division) : "";

        if (!queueType.isEmpty())
        {
          if (queueType == "RANKED_SOLO_5x5")
          {
            rankedListDict[summonerId].first =
                this->leagueClientUxAPI->readRankedShortName(tier.toUtf8()) + divisionText;
          }
          else if (queueType == "RANKED_FLEX_SR")
          {
            rankedListDict[summonerId].second =
                this->leagueClientUxAPI->readRankedShortName(tier.toUtf8()) + divisionText;
          }
        }
        QApplication::processEvents();
      }

      for (auto index = 0; index < this->rankedListSummonerIdList.count(); index++)
      {
        if (rankedListDict.contains(this->rankedListSummonerIdList.at(index)))
        {
          auto value = rankedListDict[this->rankedListSummonerIdList.at(index)];
          if (!value.first.isEmpty())
            this->gameRankedSolo5LabelList.at(index)->setText("单双：" + value.first);

          if (!value.second.isEmpty())
            this->gameRankedFlexLabelList.at(index)->setText("灵活：" + value.second);
        }
        QApplication::processEvents();
      }
      QApplication::processEvents();
    }
  }
  this->ui->MatchHistoryListWidget->setEnabled(true);
}

void MatchHistoryWidget::updateSummonerInfoGroupBox(const QJsonDocument& details)
{
  if (details.object()["queueMap"].isObject())
  {
    auto object = details.object();
    auto solo5x5 = object["queueMap"].toObject()["RANKED_SOLO_5x5"].toObject();
    auto tier = solo5x5["tier"].toString().toUtf8();
    auto division = solo5x5["division"].toString().toUtf8();
    auto rankedName = this->leagueClientUxAPI->readRankedName(tier, division);
    if (rankedName.isEmpty())
    {
      this->ui->SummonerRankedLabel->setText("无段位");
    }
    else
    {
      this->ui->SummonerRankedLabel->setText(rankedName);
    }
    this->updatedBits |= (1 << 1);
    this->updated();
    return;
  }

  auto summonerDetails = details.object();
  auto fullName =
      summonerDetails["gameName"].toString().toUtf8() + "#" + summonerDetails["tagLine"].toString().toUtf8();
  auto profileIconId = summonerDetails["profileIconId"].toInt();
  auto region = this->leagueClientUxAPI->readLocalRegion();
  auto summonerLevel = summonerDetails["summonerLevel"].toInt();
  auto summonerId = summonerDetails["summonerId"].toInteger();
  profileIconId = (profileIconId < 29 ? profileIconId + 50 : profileIconId);
  auto profileIcon = this->leagueClientUxAPI->readProfileIcon(profileIconId);
  auto xpSinceLastLevel = summonerDetails["xpSinceLastLevel"].toInt();
  auto xpUntilNextLevel = summonerDetails["xpUntilNextLevel"].toInt();

  if (this->localSummonerDetails.isEmpty())
  {
    this->localSummonerDetails = details;
  }

  this->monitorPuuid = summonerDetails["puuid"].toString();

#if defined(DEMO)
  this->ui->SummonerNameLabel->setText("富士山下#00000");
#else
  this->ui->SummonerNameLabel->setText(fullName);
#endif

  this->ui->SummonerIconLabel->setPixmap(profileIcon);
  this->ui->SummonerRegionLabel->setText(region);
  this->ui->SummonerLevelLabel->setText("lv: " + QString::number(summonerLevel));
  this->ui->SummonerExpeProgressBar->setRange(0, xpUntilNextLevel);
  this->ui->SummonerExpeProgressBar->setValue(xpSinceLastLevel);
  this->ui->SummonerExpeProgressBar->setFormat("%v/%m");

  this->matchHistorySummonerDetails = QJsonDocument(summonerDetails);
  this->ui->FindSummonerNameLineEdit->clear();
  this->ui->FindSummonerTagLineEdit->clear();
  this->ui->LocalSummonerPushButton->setEnabled(
      !(summonerId == this->localSummonerDetails.object()["summonerId"].toInteger()));
  this->updatedBits |= (1 << 0);
  this->updated();
}

void MatchHistoryWidget::updateGameDetailsListWidget(qint32 index)
{
  this->ui->GameDetailsListWidget->hide();
  this->ui->GameDetailsListTextLabel->hide();
  this->ui->GameDetailsListWidget->clear();
  this->ui->MatchHistoryListWidget->setEnabled(false);

  this->gameRankedFlexLabelList.clear();
  this->gameRankedSolo5LabelList.clear();
  this->rankedListSummonerIdList.clear();

  QByteArray version;
  QMap<int, std::pair<bool, QJsonObject>> teamItemDetails;
  auto game = this->monitorGameList.at(index).toObject();
  auto json = game["json"].toObject();

  auto gameCreation = json["gameCreation"].toInteger();
  auto gameDuration = json["gameDuration"].toInteger();
  auto gameVersion = json["gameVersion"].toString();
  auto teams = json["teams"].toArray();
  auto participants = json["participants"].toArray();
  auto queueId = json["queueId"].toInt();

  for (const auto& team : teams)
  {
    auto object = team.toObject();
    auto teamId = object["teamId"].toInt();
    teamItemDetails[teamId] = std::make_pair(false, object);
    QApplication::processEvents();
  }

  for (const auto& participant : participants)
  {
    auto object = participant.toObject();
    auto riotIdGameName = object["riotIdGameName"].toString();
    auto riotIdTagline = object["riotIdTagline"].toString();
    auto summonerId = object["summonerId"].toInteger();
    auto puuid = object["puuid"].toString();
    auto participantName = riotIdGameName + "#" + (riotIdTagline == "BOT" ? "人机" : riotIdTagline);
    auto championId = object["championId"].toInt();
    auto spell1Id = object["spell1Id"].toInt();
    auto spell2Id = object["spell2Id"].toInt();
    auto assists = object["assists"].toInt();
    auto deaths = object["deaths"].toInt();
    auto kills = object["kills"].toInt();
    auto champLevel = object["champLevel"].toInt();
    auto goldEarned = object["goldEarned"].toInt();
    auto totalMinionsKilled = object["totalMinionsKilled"].toInt();
    auto neutralMinionsKilled = object["neutralMinionsKilled"].toInt();
    auto totalDamageTaken = object["totalDamageTaken"].toInt();
    auto totalDamageDealtToChampions = object["totalDamageDealtToChampions"].toInt();
    auto teamId = object["teamId"].toInt();
    auto perks = object["perks"].toObject();

    if (!teamItemDetails[teamId].first)
    {
      auto team = teamItemDetails[teamId].second;
      auto win = team["win"].toBool();
      auto bans = team["bans"].toArray();
      auto objectives = team["objectives"].toObject();

      auto item = new QListWidgetItem();
      auto itemWidget = new QFrame();
      auto itemLabel = new QLabel(itemWidget);
      auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
      auto queueName = this->leagueClientUxAPI->readQueueName(queueId);

      dropShadowEffect->setBlurRadius(2);
      dropShadowEffect->setOffset(0, 0);
      dropShadowEffect->setColor(QColor(0, 0, 0));

      item->setSizeHint(QSize(0, 44));
      itemLabel->setAlignment(Qt::AlignCenter);
      itemLabel->setGeometry(2, 2, 120, 40);
      itemLabel->setGraphicsEffect(dropShadowEffect);
      itemWidget->setStyleSheet("background: #F2B39DDB;");

      auto gameMinute = gameDuration / 60;
      auto gameSecond = gameDuration % 60;

      if (win)
      {
        QString text = gameMinute < 6 ? QString("：重开\n") : QString("：胜出\n");
        itemLabel->setText(this->teamColorTextDict[teamId] + text + queueName);
        itemLabel->setStyleSheet(
            "color: #303030;"
            "background: #A5D6A7;"
            "font: 14px \"HarmonyOS Sans SC\";");
      }
      else
      {
        QString text = gameMinute < 6 ? QString("：重开\n") : QString("：败北\n");
        itemLabel->setText(this->teamColorTextDict[teamId] + text + queueName);
        itemLabel->setStyleSheet(
            "color: #303030;"
            "background: #EF9A9A;"
            "font: 14px \"HarmonyOS Sans SC\";");
      }

      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      itemLabel->setGeometry(126, 3, 18, 18);
      itemLabel->setPixmap(this->leagueClientUxAPI->readOtherIcon("baron-" + QByteArray::number(teamId)));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(150, 3, 20, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 14px \"HarmonyOS Sans SC\";");
      itemLabel->setText(QString::number(objectives["baron"].toObject()["kills"].toInt()));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      itemLabel->setGeometry(126, 23, 18, 18);
      itemLabel->setPixmap(this->leagueClientUxAPI->readOtherIcon("herald-" + QByteArray::number(teamId)));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(150, 23, 20, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 14px \"HarmonyOS Sans SC\";");
      itemLabel->setText(QString::number(objectives["riftHerald"].toObject()["kills"].toInt()));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      itemLabel->setGeometry(174, 3, 18, 18);
      itemLabel->setPixmap(this->leagueClientUxAPI->readOtherIcon("vilemaw-" + QByteArray::number(teamId)));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(198, 3, 20, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 14px \"HarmonyOS Sans SC\";");
      itemLabel->setText(QString::number(objectives["horde"].toObject()["kills"].toInt()));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      itemLabel->setGeometry(174, 23, 18, 18);
      itemLabel->setPixmap(this->leagueClientUxAPI->readOtherIcon("dragon-" + QByteArray::number(teamId)));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(198, 23, 20, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 14px \"HarmonyOS Sans SC\";");
      itemLabel->setText(QString::number(objectives["dragon"].toObject()["kills"].toInt()));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      itemLabel->setGeometry(228, 4, 18, 18);
      itemLabel->setPixmap(this->leagueClientUxAPI->readOtherIcon("tower-" + QByteArray::number(teamId)));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(252, 4, 20, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 12px \"HarmonyOS Sans SC\";");
      itemLabel->setText(QString::number(objectives["tower"].toObject()["kills"].toInt()));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      itemLabel->setGeometry(228, 22, 18, 18);
      itemLabel->setPixmap(this->leagueClientUxAPI->readOtherIcon("inhibitor-" + QByteArray::number(teamId)));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(252, 22, 20, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 12px \"HarmonyOS Sans SC\";");
      itemLabel->setText(QString::number(objectives["inhibitor"].toObject()["kills"].toInt()));

      auto gameBeginTime = QDateTime::fromMSecsSinceEpoch(gameCreation);
      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(276, 4, 190, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 12px \"HarmonyOS Sans SC\";");
      itemLabel->setText("开始时间：" + gameBeginTime.toString("yyyy-MM-dd HH:mm:ss"));

      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(276, 22, 110, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 12px \"HarmonyOS Sans SC\";");
      itemLabel->setText("游戏时长：" + QString::asprintf("%02lld:%02lld", gameMinute, gameSecond));

      auto endPos = gameVersion.indexOf(".");
      endPos = gameVersion.indexOf(".", endPos + 1);
      version = gameVersion.mid(0, endPos).toUtf8();
      itemLabel = new QLabel(itemWidget);
      itemLabel->setGeometry(386, 22, 80, 18);
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 12px \"HarmonyOS Sans SC\";");
      itemLabel->setText("版本：" + version);

      for (auto banIdx = 0; banIdx < bans.count(); banIdx++)
      {
        if (!banIdx)
        {
          itemLabel = new QLabel(itemWidget);
          itemLabel->setGeometry(466, 13, 30, 18);
          itemLabel->setStyleSheet(
              "color: #303030;"
              "font: 12px \"HarmonyOS Sans SC\";");
          itemLabel->setText("禁用：");
        }

        auto championIcon = this->leagueClientUxAPI->readChampionIcon(bans[banIdx].toObject()["championId"].toInt());
        itemLabel = new QLabel(itemWidget);
        itemLabel->setScaledContents(true);
        itemLabel->setGeometry(500 + banIdx * 32, 7, 30, 30);
        itemLabel->setStyleSheet("border: 2px solid #78909C;");
        itemLabel->setPixmap(championIcon);
        QApplication::processEvents();
      }

      teamItemDetails[teamId].first = true;
      this->ui->GameDetailsListWidget->addItem(item);
      this->ui->GameDetailsListWidget->setItemWidget(item, itemWidget);
    }

    auto item = new QListWidgetItem();
    auto itemWidget = new QWidget();
    auto itemLabel = new QLabel(itemWidget);

    item->setSizeHint(QSize(0, 44));
    itemLabel->setScaledContents(true);
    itemLabel->setGeometry(2, 2, 40, 40);
    itemLabel->setPixmap(this->leagueClientUxAPI->readChampionIcon(championId));

    itemLabel = new QLabel(itemWidget);
    itemLabel->setAlignment(Qt::AlignCenter);
    itemLabel->setGeometry(44, 12, 20, 20);
    itemLabel->setStyleSheet(
        "color: #303030;"
        "border-radius: 10px;"
        "background: #78909C;"
        "font: 11px \"HarmonyOS Sans SC\";");
    itemLabel->setText(QString::asprintf("%02d", champLevel));

    itemLabel = new QLabel(itemWidget);
    itemLabel->setGeometry(66, 2, 148, 40);
    auto localSummonerFullName = this->localSummonerDetails.object()["gameName"].toString() + "#"
        + this->localSummonerDetails.object()["tagLine"].toString();
    if (this->ui->SummonerNameLabel->text().toUtf8() == participantName)
    {
      itemLabel->setStyleSheet(
          "color: #673AB7;"
          "font: 12px \"HarmonyOS Sans SC\";");
    }
    else if (localSummonerFullName.toUtf8() == participantName)
    {
      itemLabel->setStyleSheet(
          "color: #FF9100;"
          "font: 12px \"HarmonyOS Sans SC\";");
    }
    else
    {
      itemLabel->setStyleSheet(
          "color: #303030;"
          "font: 12px \"HarmonyOS Sans SC\";");
    }
#if defined(DEMO)
    itemLabel->setText(
        "爱情转移#00000\nK/D/A：" + QString::number(kills) + "/" + QString::number(deaths) + "/"
        + QString::number(assists));
#else
    itemLabel->setText(
        participantName + "\nK/D/A：" + QString::number(kills) + "/" + QString::number(deaths) + "/"
        + QString::number(assists));
#endif

    itemLabel = new QLabel(itemWidget);
    itemLabel->setStyleSheet(
        "color: #303030;"
        "font: 11px \"HarmonyOS Sans SC\";");
    itemLabel->setGeometry(214, 4, 70, 18);
    itemLabel->setText(QString::asprintf("经济：%.1fK", (float(goldEarned) / 1000.0f)));

    itemLabel = new QLabel(itemWidget);
    itemLabel->setStyleSheet(
        "color: #303030;"
        "font: 11px \"HarmonyOS Sans SC\";");
    itemLabel->setGeometry(214, 22, 70, 18);
    itemLabel->setText(QString::asprintf("伤害：%.1fK", (float(totalDamageDealtToChampions) / 1000.0f)));

    itemLabel = new QLabel(itemWidget);
    itemLabel->setStyleSheet(
        "color: #303030;"
        "font: 11px \"HarmonyOS Sans SC\";");
    itemLabel->setGeometry(284, 4, 70, 18);
    itemLabel->setText(QString::asprintf("承伤：%.1fK", (float(totalDamageTaken) / 1000.0f)));

    itemLabel = new QLabel(itemWidget);
    itemLabel->setStyleSheet(
        "color: #303030;"
        "font: 11px \"HarmonyOS Sans SC\";");
    itemLabel->setGeometry(284, 22, 70, 18);
    itemLabel->setText(QString::asprintf("补刀：%d", totalMinionsKilled + neutralMinionsKilled));

    itemLabel = new QLabel(itemWidget);
    itemLabel->setStyleSheet(
        "color: #FF6F00;"
        "font: 11px \"HarmonyOS Sans SC\";");
    itemLabel->setGeometry(354, 4, 70, 18);
    this->gameRankedSolo5LabelList.push_back(itemLabel);

    itemLabel = new QLabel(itemWidget);
    itemLabel->setStyleSheet(
        "color: #006064;"
        "font: 11px \"HarmonyOS Sans SC\";");
    itemLabel->setGeometry(354, 22, 70, 18);
    this->gameRankedFlexLabelList.push_back(itemLabel);
    this->rankedListSummonerIdList.push_back(summonerId);

    itemLabel = new QLabel(itemWidget);
    itemLabel->setScaledContents(true);
    itemLabel->setGeometry(428, 2, 20, 20);
    itemLabel->setStyleSheet("border: 1px solid #78909C");
    itemLabel->setPixmap(this->leagueClientUxAPI->readSpellIcon(spell1Id));

    itemLabel = new QLabel(itemWidget);
    itemLabel->setScaledContents(true);
    itemLabel->setGeometry(428, 22, 20, 20);
    itemLabel->setStyleSheet("border: 1px solid #78909C");
    itemLabel->setPixmap(this->leagueClientUxAPI->readSpellIcon(spell2Id));

    for (auto itemIndex = 0; itemIndex < 7; itemIndex++)
    {
      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      itemLabel->setGeometry(452 + itemIndex * 32, 7, 30, 30);
      itemLabel->setStyleSheet("QLabel {border: 2px solid #78909C;}");
      auto itemId = object["item" + QString::number(itemIndex)].toInt();
      auto itemIcon = this->leagueClientUxAPI->readItemIcon(itemId);
      if (itemId)
      {
        itemLabel->setToolTipDuration(60 * 1000);
        itemLabel->setToolTip(this->leagueClientUxAPI->readItemData(itemId, version));
      }
      itemLabel->setPixmap(itemIcon);
      QApplication::processEvents();
    }

    QList<int> perkIdList;
    auto styles = perks["styles"].toArray();
    for (const auto& style : styles)
    {
      auto selections = style.toObject()["selections"].toArray();
      for (const auto& selection : selections)
      {
        perkIdList.push_back(selection.toObject()["perk"].toInt());
      }
      QApplication::processEvents();
    }

    for (auto perkIdx = 0; perkIdx < 6; perkIdx++)
    {
      itemLabel = new QLabel(itemWidget);
      itemLabel->setScaledContents(true);
      if (!perkIdx)
      {
        itemLabel->setGeometry(678 + perkIdx * 34, 6, 32, 32);
        itemLabel->setStyleSheet(
            "QLabel {border-radius: 16px;"
            "background: #212121;"
            "border: 2px solid #78909C;}");
      }
      else if (perkIdx < 4)
      {
        itemLabel->setGeometry(712 + (perkIdx - 1) * 22, 2, 20, 20);
        itemLabel->setStyleSheet(
            "QLabel {border-radius: 10;"
            "background: #212121;"
            "border: 1px solid #78909C;}");
      }
      else
      {
        itemLabel->setGeometry(723 + (perkIdx - 4) * 22, 20, 20, 20);
        itemLabel->setStyleSheet(
            "QLabel {border-radius: 10px;"
            "background: #212121;"
            "border: 1px solid #78909C;}");
      }
      auto perkIcon = this->leagueClientUxAPI->readRuneIcon(perkIdList.at(perkIdx));
      itemLabel->setToolTipDuration(60 * 1000);
      itemLabel->setToolTip(this->leagueClientUxAPI->readRuneData(perkIdList.at(perkIdx), version));
      itemLabel->setPixmap(perkIcon);
      QApplication::processEvents();
    }

    if (riotIdTagline != "BOT")
    {
      QJsonObject data;
      data.insert("puuid", puuid);
      data.insert("summonerId", summonerId);
      data.insert("fullName", participantName);
      item->setData(Qt::UserRole, data);
    }

    this->ui->GameDetailsListWidget->addItem(item);
    this->ui->GameDetailsListWidget->setItemWidget(item, itemWidget);
    QApplication::processEvents();
  }

  if (this->rankedListSummonerIdList.count())
    this->leagueClientUxAPI->modifySummonerRankedList(
        this->rankedListSummonerIdList, {"RANKED_SOLO_5x5", "RANKED_FLEX_SR"});

  this->ui->GameDetailsListWidget->show();
  this->ui->FindMatchHistoryGroupBox->hide();
  this->ui->GameDetailsListTextLabel->show();
  QApplication::processEvents();
}

void MatchHistoryWidget::updateMatchHistoryListWidget(const QJsonDocument& details)
{
  this->ui->MatchHistoryListWidget->hide();
  this->ui->MatchHistoryListWidget->clear();
  this->ui->MatchHistoryListWidget->scrollToTop();

  auto matchHistoryDetails = details.object();
  auto games = matchHistoryDetails["games"].toArray();
  if (this->monitorGameList == games)
  {
    return;
  }

  this->monitorGameList = games;

  if (!this->monitorGameList.count())
  {
    auto item = new QListWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setSizeHint(this->ui->MatchHistoryListWidget->size());
    item->setText("没有找到这个玩家的任何对局\n可能是因为该玩家\n半年内没有进行过任何对局");
    this->ui->MatchHistoryListWidget->addItem(item);
    this->ui->MatchHistoryListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->ui->MatchHistoryListWidget->setEnabled(false);
    this->ui->MatchHistoryListWidget->show();
    this->ui->FindMatchHistoryGroupBox->show();
    this->updatedBits |= (1 << 2);
    this->updated();
    return;
  }

  for (const auto& matchHistory : this->monitorGameList)
  {
    auto json = matchHistory.toObject()["json"].toObject();
    auto metadata = matchHistory.toObject()["metadata"].toObject();
    auto participants = metadata["participants"].toArray();
    auto monitorIndex = -1;

    for (auto index = 0; index < participants.count(); index++)
    {
      if (this->monitorPuuid == participants.at(index).toString())
        monitorIndex = index;
      QApplication::processEvents();
    }

    if (monitorIndex != -1)
    {
      auto queueId = json["queueId"].toInt();
      auto queueName = this->leagueClientUxAPI->readQueueName(queueId);
      if (!queueName.isEmpty())
      {
        auto monitorParticipant = json["participants"].toArray().at(monitorIndex);
        auto win = monitorParticipant["win"].toBool();
        auto kills = monitorParticipant["kills"].toInt();
        auto deaths = monitorParticipant["deaths"].toInt();
        auto assists = monitorParticipant["assists"].toInt();
        auto gameId = json["gameId"].toInteger();

        auto item = new QListWidgetItem();
        auto championId = monitorParticipant["championId"].toInt();
        auto championIcon = this->leagueClientUxAPI->readChampionIcon(championId);
        auto championName = this->leagueClientUxAPI->readChampionName(championId);

        item->setForeground(this->winColorDict[win]);
        item->setIcon(championIcon);
        item->setText(
            queueName + "（" + this->winTextDict[win] + "）\n" + championName + "（" + QByteArray::number(kills) + "/"
            + QByteArray::number(deaths) + "/" + QByteArray::number(assists) + "）");
        this->ui->MatchHistoryListWidget->addItem(item);
      }
    }
    QApplication::processEvents();
  }

  this->ui->MatchHistoryListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->ui->MatchHistoryListWidget->setEnabled(true);
  this->ui->MatchHistoryListWidget->show();
  this->ui->FindMatchHistoryGroupBox->show();
  this->updatedBits |= (1 << 2);
  this->updated();
}

void MatchHistoryWidget::updateRankedTableWidget(const QJsonDocument& details)
{
  auto rankedDetails = details.object();
  auto queues = rankedDetails["queues"].toArray();

  this->ui->RankedTableWidget->clear();
  this->ui->RankedTableWidget->scrollToTop();
  this->ui->RankedTableWidget->setColumnCount(4);
  this->ui->RankedTableWidget->setHorizontalHeaderLabels({"类型", "段位", "点数", "段位胜场"});
  this->ui->RankedTableWidget->setColumnWidth(0, 120);
  this->ui->RankedTableWidget->setColumnWidth(1, 80);
  this->ui->RankedTableWidget->setColumnWidth(2, 80);
  this->ui->RankedTableWidget->setColumnWidth(3, 80);

  this->ui->PreviousRankedTableWidget->clear();
  this->ui->PreviousRankedTableWidget->scrollToTop();
  this->ui->PreviousRankedTableWidget->setColumnCount(3);
  this->ui->PreviousRankedTableWidget->setHorizontalHeaderLabels({"类型", "最高段位", "结算段位"});
  this->ui->PreviousRankedTableWidget->setColumnWidth(0, 120);
  this->ui->PreviousRankedTableWidget->setColumnWidth(1, 120);
  this->ui->PreviousRankedTableWidget->setColumnWidth(2, 120);

  QJsonArray rankedList;
  for (const auto& queue : queues)
  {
    auto object = queue.toObject();
    auto queueType = object["queueType"].toString().toUtf8();
    if (!this->rankedTypeDict[queueType].isEmpty())
    {
      rankedList.append(object);
    }
    QApplication::processEvents();
  }

  QList<QString> headers;
  this->ui->RankedTableWidget->setRowCount(static_cast<int>(rankedList.count()));

  for (auto index = 0; index < rankedList.count(); index++)
  {
    auto object = rankedList.at(index).toObject();
    auto highestTier = object["highestTier"].toString().toUtf8();
    auto highestDivision = object["highestDivision"].toString().toUtf8();
    auto leaguePoints = object["leaguePoints"].toInt();
    auto wins = object["wins"].toInt();
    auto queueType = object["queueType"].toString().toUtf8();

    auto item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(this->rankedTypeDict[queueType]);
    this->ui->RankedTableWidget->setItem(index, 0, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(
        highestTier.isEmpty() ? QByteArray("无段位")
                              : this->leagueClientUxAPI->readRankedName(highestTier, highestDivision));
    this->ui->RankedTableWidget->setItem(index, 1, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(leaguePoints));
    this->ui->RankedTableWidget->setItem(index, 2, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(wins));
    this->ui->RankedTableWidget->setItem(index, 3, item);

    this->ui->RankedTableWidget->setRowHeight(index, 36);
    headers.append(QString::asprintf("#%d", index + 1));
    QApplication::processEvents();
  }
  this->ui->RankedTableWidget->setVerticalHeaderLabels(headers);

  headers.clear();
  this->ui->PreviousRankedTableWidget->setRowCount(static_cast<int>(rankedList.count()));
  for (auto index = 0; index < rankedList.count(); index++)
  {
    auto object = rankedList.at(index).toObject();
    auto previousSeasonEndTier = object["previousSeasonEndTier"].toString().toUtf8();
    auto previousSeasonEndDivision = object["previousSeasonEndDivision"].toString().toUtf8();
    auto previousSeasonHighestTier = object["previousSeasonHighestTier"].toString().toUtf8();
    auto previousSeasonHighestDivision = object["previousSeasonHighestDivision"].toString().toUtf8();

    auto queueType = object["queueType"].toString().toUtf8();

    auto item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(this->rankedTypeDict[queueType]);
    this->ui->PreviousRankedTableWidget->setItem(index, 0, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(
        previousSeasonHighestTier.isEmpty()
            ? QByteArray("无段位")
            : this->leagueClientUxAPI->readRankedName(previousSeasonHighestTier, previousSeasonHighestDivision));
    this->ui->PreviousRankedTableWidget->setItem(index, 1, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(
        previousSeasonEndTier.isEmpty()
            ? QByteArray("无段位")
            : this->leagueClientUxAPI->readRankedName(previousSeasonEndTier, previousSeasonEndDivision));
    this->ui->PreviousRankedTableWidget->setItem(index, 2, item);

    this->ui->PreviousRankedTableWidget->setRowHeight(index, 36);
    headers.append(QString::asprintf("#%d", index + 1));
    QApplication::processEvents();
  }
  this->ui->PreviousRankedTableWidget->setVerticalHeaderLabels(headers);
  this->updatedBits |= (1 << 3);
  this->updated();
}

void MatchHistoryWidget::updateMasteryChampionTableWidget(const QJsonDocument& details)
{
  auto masteryChampionDetailsList = details.array();

  this->ui->MasteryChampionTableWidget->clear();
  this->ui->MasteryChampionTableWidget->scrollToTop();
  this->ui->MasteryChampionTableWidget->setColumnCount(4);
  this->ui->MasteryChampionTableWidget->setHorizontalHeaderLabels({"英雄", "成就等级", "成就点数", "最佳评分"});
  this->ui->MasteryChampionTableWidget->setColumnWidth(0, 120);
  this->ui->MasteryChampionTableWidget->setColumnWidth(1, 80);
  this->ui->MasteryChampionTableWidget->setColumnWidth(2, 80);
  this->ui->MasteryChampionTableWidget->setColumnWidth(3, 80);

  QList<QString> headers;
  auto maxIdx = masteryChampionDetailsList.count() > 50 ? 50 : masteryChampionDetailsList.count();
  this->ui->MasteryChampionTableWidget->setRowCount(static_cast<int>(maxIdx));

  for (auto index = 0; index < maxIdx; index++)
  {
    auto object = masteryChampionDetailsList.at(index).toObject();
    auto championId = object["championId"].toInt();
    auto highestGrade = object["highestGrade"].toString();
    auto championLevel = object["championLevel"].toInt();
    auto championPoints = object["championPoints"].toInt();

    auto item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setIcon(this->leagueClientUxAPI->readChampionIcon(championId));
    item->setText(this->leagueClientUxAPI->readChampionName(championId));
    this->ui->MasteryChampionTableWidget->setItem(index, 0, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(championLevel));
    this->ui->MasteryChampionTableWidget->setItem(index, 1, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.2fK", static_cast<double>(championPoints) / 1000.0));
    this->ui->MasteryChampionTableWidget->setItem(index, 2, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(highestGrade.isEmpty() ? "-" : highestGrade);
    this->ui->MasteryChampionTableWidget->setItem(index, 3, item);
    this->ui->MasteryChampionTableWidget->setRowHeight(index, 36);
    headers.append(QString::asprintf("%02d", index + 1));
    QApplication::processEvents();
  }
  this->ui->MasteryChampionTableWidget->setVerticalHeaderLabels(headers);
  this->updatedBits |= (1 << 4);
  this->updated();
}
