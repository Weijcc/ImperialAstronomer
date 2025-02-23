#pragma once

#include <bitset>

#include <QMenu>
#include "LeagueAPI.h"
#include "ui_MatchHistoryInterface.h"

class MatchHistoryWidget : public QWidget
{
  Q_OBJECT

  quint8 updatedBits = 0;
  qint32 updatedIndex = 0;
  bool monitorFlag = false;

  QJsonDocument localSummonerDetails;
  QJsonDocument matchHistorySummonerDetails;
  Ui::MatchHistoryInterface* ui = nullptr;
  LeagueAPI* leagueClientUxAPI = nullptr;
  const QMap<bool, QByteArray> winTextDict = {{true, "胜出"}, {false, "败北"}};
  const QMap<bool, QColor> winColorDict = {{true, QColor(56, 142, 60)}, {false, QColor(191, 54, 12)}};
  const QMap<int, QByteArray> teamColorTextDict = {
      {100, "蓝色方"},
      {200, "红色方"},
  };

  const QMap<QByteArray, QByteArray> rankedTypeDict = {
      {"RANKED_SOLO_5x5", "排位（单/双）"},
      {"RANKED_FLEX_SR", "排位（灵活）"},
      {"RANKED_TFT", "云顶之弈"},
      {"RANKED_TFT_TURBO", "云顶之弈（狂暴）"},
      {"RANKED_TFT_DOUBLE_UP", "云顶之弈（双人）"},
      {"RANKED_TFT_DOUBLE_UP", "云顶之弈（双人）"},
  };

  QString monitorPuuid;
  QMenu* gameDetailsListWidgetMenu = nullptr;
  QAction* addFriendAction = nullptr;
  QAction* monitorMatchHistoryAction = nullptr;
  QAction* copySummonerFullNameAction = nullptr;

  QList<QLabel*> gameRankedFlexLabelList;
  QList<QLabel*> gameRankedSolo5LabelList;
  QList<qint64> rankedListSummonerIdList;

  QJsonArray monitorGameList;

  void updated();

private slots:
  void updateGameDetailsListWidget(qint32 index);

public:
  MatchHistoryWidget(QWidget* parent = nullptr);
  ~MatchHistoryWidget() override;

  void display(const QByteArray& details);

  [[nodiscard]] QJsonDocument readMatchHistorySummonerDetails();

public slots:
  void updateRankedTableWidget(const QJsonDocument& details);
  void updateRankedListLabel(const QJsonDocument& details);
  void updateSummonerInfoGroupBox(const QJsonDocument& details);
  void updateMatchHistoryListWidget(const QJsonDocument& details);
  void updateMasteryChampionTableWidget(const QJsonDocument& details);
};
