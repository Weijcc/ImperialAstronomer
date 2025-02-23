#include <QClipboard>
#include <QGraphicsDropShadowEffect>

#include "MasterWidget.h"

HomeWidget::HomeWidget(QWidget* parent) : QWidget(parent), ui(new Ui::HomeInterface)
{
  this->ui->setupUi(this);
  this->setAttribute(Qt::WA_StaticContents, true);

  if (parent)
  {
    this->masterWidget = parent->parent();
    this->setGeometry(parent->rect());
  }

  this->leagueClientUxAPI = LeagueAPI::instance();

  this->friendListWidgetMenu = new QMenu(this->ui->FriendListWidget);
  this->copySummonerNameAction =
      new QAction(QIcon(this->leagueClientUxAPI->readProfileIcon(23)), "复制昵称", this->ui->FriendListWidget);
  this->spectateAction =
      new QAction(QIcon(this->leagueClientUxAPI->readProfileIcon(9)), "观看对局", this->ui->FriendListWidget);
  this->monitorSummonerAction =
      new QAction(QIcon(this->leagueClientUxAPI->readProfileIcon(10)), "查看信息", this->ui->FriendListWidget);
  this->deleteFriendAction =
      new QAction(QIcon(this->leagueClientUxAPI->readProfileIcon(20)), "删除好友", this->ui->FriendListWidget);
  this->friendListWidgetMenu->addAction(this->copySummonerNameAction);
  this->friendListWidgetMenu->addAction(this->spectateAction);
  this->friendListWidgetMenu->addAction(this->monitorSummonerAction);
  this->friendListWidgetMenu->addAction(this->deleteFriendAction);

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
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
  this->ui->FriendListWidget->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->RankDataTableWidget->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->RankFightDataTableWidget->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->FriendListTextLabel->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(33, 33, 33));
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
  this->ui->SpectatePushButton->setGraphicsEffect(dropShadowEffect);

  dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->AddFriendPushButton->setGraphicsEffect(dropShadowEffect);

  auto intValidator = new QIntValidator();
  intValidator->setRange(0, 99999);
  this->ui->FindSummonerNameLineEdit->setPlaceholderText("玩家昵称");
  this->ui->FindSummonerTagLineEdit->setPlaceholderText("昵称编号");
  this->ui->FindSummonerTagLineEdit->setValidator(intValidator);

  this->ui->RankDataTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  this->ui->RankDataTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  this->ui->RankFightDataTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  this->ui->RankFightDataTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

  this->ui->FriendListWidget->setIconSize(QSize(42, 42));
  this->ui->FriendListWidget->setProperty("contextMenuPolicy", Qt::CustomContextMenu);

  connect(
      this->leagueClientUxAPI, &LeagueAPI::readyAccessMonitorLocalFriends, this, &HomeWidget::updateFriendListWidget);

  connect(this->leagueClientUxAPI, &LeagueAPI::readyAccessMonitorRankData, this, &HomeWidget::updateRankDataListWidget);

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorLocalRankedStats,
      this,
      &HomeWidget::updateSummonerInfoGroupBox);

  connect(this->ui->FriendListWidget, &QListWidget::customContextMenuRequested, this, [&](const QPoint& pos) {
    auto items = this->ui->FriendListWidget->selectedItems();
    if (items.count() && items.at(0))
    {
      auto friendDetails = items.at(0)->data(Qt::UserRole).toJsonObject();
      auto availability = friendDetails["availability"].toString();
      auto id = friendDetails["id"].toString();
      auto gameName = friendDetails["gameName"].toString();
      auto gameTag = friendDetails["gameTag"].toString();
      auto puuid = friendDetails["puuid"].toString();
      this->copySummonerNameAction->setData(gameName + "#" + gameTag);
      this->spectateAction->setData(puuid);
      this->monitorSummonerAction->setData(puuid);
      this->deleteFriendAction->setData(items.at(0)->data(Qt::UserRole).toJsonObject());
      this->spectateAction->setEnabled(availability == "dnd");
      this->friendListWidgetMenu->exec(QCursor::pos());
    }
  });

  connect(this->friendListWidgetMenu, &QMenu::aboutToHide, this, [&] {
    auto items = this->ui->FriendListWidget->selectedItems();
    if (items.count() && items.at(0))
      items.at(0)->setSelected(false);
  });

  connect(this->ui->FriendListWidget, &QListWidget::itemClicked, this, [&](QListWidgetItem* item) {
    item->setSelected(false);
  });

  connect(this->copySummonerNameAction, &QAction::triggered, this, [&] {
    QApplication::clipboard()->setText(this->copySummonerNameAction->data().toString());
  });

  connect(this->spectateAction, &QAction::triggered, this, [&] {
    this->leagueClientUxAPI->modifySpectateSummoner(this->spectateAction->data().toString().toUtf8());
  });

  connect(this->monitorSummonerAction, &QAction::triggered, this, [&] {
    if (this->masterWidget)
    {
      auto master = qobject_cast<MasterWidget*>(this->masterWidget);
      auto data = this->monitorSummonerAction->data().toByteArray();
      auto puuid = master->matchHistoryWidget->readMatchHistorySummonerDetails()["puuid"].toString();

      master->ui->HomePageRadioButton->setChecked(false);
      master->ui->MatchHistoryPageRadioButton->setChecked(true);
      master->ui->OtherFunctionPageRadioButton->setChecked(false);
      master->homeWidget->hide();
      master->otherFunctionWidget->hide();

      if (puuid == data)
      {
        master->matchHistoryWidget->show();
      }
      else
      {
        master->matchHistoryWidget->display(data);
      }
    }
  });

  connect(this->deleteFriendAction, &QAction::triggered, this, [&] {
    auto object = this->deleteFriendAction->data().toJsonObject();

    auto id = object["id"].toString().toUtf8();
    auto gameName = object["gameName"].toString();
    auto gameTag = object["gameTag"].toString();

    auto result = Prompt::instance()->warning("确定删除\n" + gameName + "#" + gameTag + "？");
    if (result == QDialog::Accepted)
    {
      this->leagueClientUxAPI->deleteFriend(id);
    }
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

  connect(this->ui->SpectatePushButton, &QPushButton::clicked, this, [&] {
    auto name = this->ui->FindSummonerNameLineEdit->text();
    auto tagLine = this->ui->FindSummonerTagLineEdit->text();
    if (name.isEmpty() || tagLine.isEmpty())
    {
      Prompt::instance()->error("玩家昵称#编号不能为空");
      return;
    }
    auto fullName = name + "#" + tagLine;
    this->leagueClientUxAPI->modifySpectateSummoner(fullName.toUtf8());
  });

  connect(this->ui->AddFriendPushButton, &QPushButton::clicked, this, [&] {
    auto name = this->ui->FindSummonerNameLineEdit->text();
    auto tagLine = this->ui->FindSummonerTagLineEdit->text();
    if (name.isEmpty() || tagLine.isEmpty())
    {
      Prompt::instance()->error("玩家昵称#编号不能为空");
      return;
    }
    auto fullName = name + "#" + tagLine;
    this->leagueClientUxAPI->friendRequest(fullName.toUtf8());
  });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessMonitorRankFightData,
      this,
      &HomeWidget::updateRankFightDataListWidget);
}

HomeWidget::~HomeWidget()
{
  delete this->ui;
}

QJsonDocument HomeWidget::readLocalSummonerDetails()
{
  return this->localSummonerDetails;
}

void HomeWidget::updateFriendListWidget(const QJsonDocument& details)
{
  auto friendDetailsList = details.array();
  this->ui->FriendListWidget->clear();
  this->ui->FriendListWidget->scrollToTop();
  if (!friendDetailsList.count())
  {
    auto item = new QListWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setSizeHint(this->ui->FriendListWidget->size());
    item->setText("没有添加任何好友\n添加好友一起进行游戏吧");
    this->ui->FriendListWidget->addItem(item);
    this->ui->FriendListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->ui->FriendListWidget->setEnabled(false);
    this->ui->FriendListWidget->show();
    return;
  }
  for (const auto& friendDetails : friendDetailsList)
  {
    auto object = friendDetails.toObject();
    auto icon = object["icon"].toInt();
    icon = (icon < 29 ? icon + 50 : icon);
    auto gameTag = object["gameTag"].toString().toUtf8();
    auto gameName = object["gameName"].toString().toUtf8();
    auto fullName = gameName + "#" + gameTag;
    auto availability = object["availability"].toString().toUtf8();
    auto playerCredentials = object["playerCredentials"].toObject();
    auto availabilityColor = this->availabilityColorMap[availability];

    auto queueId = playerCredentials["queueId"].toInt();
    auto queueName = this->leagueClientUxAPI->readQueueName(queueId);

    if (!queueName.isEmpty())
      queueName.insert(0, '-');

    auto queueType = queueId == 0 ? QByteArray() : queueName;
    availability = this->leagueClientUxAPI->readAvailability(availability);
    auto item = new QListWidgetItem();
    auto profileIcon = this->leagueClientUxAPI->readProfileIcon(icon);

    item->setForeground(availabilityColor);
    item->setIcon(profileIcon);
#if defined(DEMO)
    item->setText("明年今日#00000\n" + availability + queueType);
#else
    item->setText(fullName + "\n" + availability + queueType);
#endif
    item->setData(Qt::UserRole, object);
    this->ui->FriendListWidget->addItem(item);
    QApplication::processEvents();
  }
}

void HomeWidget::updateRankDataListWidget(const QJsonDocument& details)
{
  auto rankDataList = details["jData"]["res"]["championdetails"].toString().split("#");
  this->ui->RankDataTableWidget->clear();
  this->ui->RankDataTableWidget->scrollToTop();
  this->ui->RankDataTableWidget->setColumnCount(5);
  this->ui->RankDataTableWidget->setHorizontalHeaderLabels({"T级", "英雄", "胜率", "登场率", "禁用率"});
  this->ui->RankDataTableWidget->setColumnWidth(0, 60);
  this->ui->RankDataTableWidget->setColumnWidth(1, 120);
  this->ui->RankDataTableWidget->setColumnWidth(2, 60);
  this->ui->RankDataTableWidget->setColumnWidth(3, 60);
  this->ui->RankDataTableWidget->setColumnWidth(4, 60);

  QList<QString> headers;
  auto maxIdx = rankDataList.count() > 50 ? 50 : rankDataList.count();
  this->ui->RankDataTableWidget->setRowCount(maxIdx);
  for (auto index = 0; index < maxIdx; index++)
  {
    auto dataList = rankDataList.at(index).split("_");
    if (dataList.count() < 6)
      continue;

    auto item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("T%d", dataList.at(2).toInt()));
    this->ui->RankDataTableWidget->setItem(index, 0, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setIcon(this->leagueClientUxAPI->readChampionIcon(dataList.at(1).toInt()));
    item->setText(this->leagueClientUxAPI->readChampionName(dataList.at(1).toInt()));
    this->ui->RankDataTableWidget->setItem(index, 1, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.1f%%", dataList.at(3).toDouble() * 100.0));
    this->ui->RankDataTableWidget->setItem(index, 2, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.1f%%", dataList.at(5).toDouble() * 100.0));
    this->ui->RankDataTableWidget->setItem(index, 3, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.1f%%", dataList.at(4).toDouble() * 100.0));
    this->ui->RankDataTableWidget->setItem(index, 4, item);
    this->ui->RankDataTableWidget->setRowHeight(index, 36);
    headers.append(QString::asprintf("%02d", index + 1));
    QApplication::processEvents();
  }
  this->ui->RankDataTableWidget->setVerticalHeaderLabels(headers);
}

void HomeWidget::updateRankFightDataListWidget(const QJsonDocument& details)
{
  auto rankFightDataText = details["data"]["result"].toString();
  rankFightDataText.replace(R"("})", "");
  rankFightDataText.replace(R"({"listcollect":")", "");
  auto rankFightDataList = rankFightDataText.split("#");
  this->ui->RankFightDataTableWidget->clear();
  this->ui->RankFightDataTableWidget->scrollToTop();
  this->ui->RankFightDataTableWidget->setColumnCount(5);
  this->ui->RankFightDataTableWidget->setHorizontalHeaderLabels({"英雄", "胜率", "参团率", "场均伤害", "场均承伤"});
  this->ui->RankFightDataTableWidget->setColumnWidth(0, 120);
  this->ui->RankFightDataTableWidget->setColumnWidth(1, 58);
  this->ui->RankFightDataTableWidget->setColumnWidth(2, 60);
  this->ui->RankFightDataTableWidget->setColumnWidth(3, 60);
  this->ui->RankFightDataTableWidget->setColumnWidth(4, 60);

  QList<QString> headers;
  auto maxIdx = rankFightDataList.count() > 50 ? 50 : rankFightDataList.count();
  this->ui->RankFightDataTableWidget->setRowCount(maxIdx);
  for (auto index = 0; index < maxIdx; index++)
  {
    auto dataList = rankFightDataList[index].split("_");
    if (dataList.count() < 10)
      continue;
    dataList.removeAt(5);

    auto item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setIcon(this->leagueClientUxAPI->readChampionIcon(dataList.at(0).toInt()));
    item->setText(this->leagueClientUxAPI->readChampionName(dataList.at(0).toInt()));
    this->ui->RankFightDataTableWidget->setItem(index, 0, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.1f%%", dataList.at(3).toDouble() * 100.0));
    this->ui->RankFightDataTableWidget->setItem(index, 1, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.1f%%", dataList.at(6).toDouble() * 100.0));
    this->ui->RankFightDataTableWidget->setItem(index, 2, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.1f%%", dataList.at(7).toDouble() * 100.0));
    this->ui->RankFightDataTableWidget->setItem(index, 3, item);

    item = new QTableWidgetItem();
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::asprintf("%.1f%%", dataList.at(8).toDouble() * 100.0));
    this->ui->RankFightDataTableWidget->setItem(index, 4, item);
    this->ui->RankFightDataTableWidget->setRowHeight(index, 36);
    headers.append(QString::asprintf("%02d", index + 1));
    QApplication::processEvents();
  }
  this->ui->RankFightDataTableWidget->setVerticalHeaderLabels(headers);
}

void HomeWidget::updateSummonerInfoGroupBox(const QJsonDocument& details)
{
  if (details.object()["queueMap"].isObject())
  {
    auto object = details.object();
    auto solo5x5 = object["queueMap"].toObject()["RANKED_SOLO_5x5"].toObject();
    auto tier = solo5x5["tier"].toString().toUtf8();
    auto division = solo5x5["division"].toString().toUtf8();
    auto rankedName = this->leagueClientUxAPI->readRankedName(tier, division);
    if (rankedName.isEmpty())
      this->ui->SummonerRankedLabel->setText("无段位");
    else
      this->ui->SummonerRankedLabel->setText(rankedName);
    return;
  }
  auto summonerDetails = details.object();
  auto fullName =
      summonerDetails["gameName"].toString().toUtf8() + "#" + summonerDetails["tagLine"].toString().toUtf8();
  auto profileIconId = summonerDetails["profileIconId"].toInt();
  auto region = this->leagueClientUxAPI->readLocalRegion();
  auto summonerLevel = summonerDetails["summonerLevel"].toInt();
  profileIconId = (profileIconId < 29 ? profileIconId + 50 : profileIconId);
  auto profileIcon = this->leagueClientUxAPI->readProfileIcon(profileIconId);
  auto xpSinceLastLevel = summonerDetails["xpSinceLastLevel"].toInt();
  auto xpUntilNextLevel = summonerDetails["xpUntilNextLevel"].toInt();
  this->ui->SummonerNameLabel->setText(fullName);
  this->ui->SummonerIconLabel->setPixmap(profileIcon);
  this->ui->SummonerRegionLabel->setText(region);
  this->ui->SummonerLevelLabel->setText("lv: " + QString::number(summonerLevel));
  this->ui->SummonerExpeProgressBar->setRange(0, xpUntilNextLevel);
  this->ui->SummonerExpeProgressBar->setValue(xpSinceLastLevel);
  this->ui->SummonerExpeProgressBar->setFormat("%v/%m");
  this->localSummonerDetails = QJsonDocument(details);
}
