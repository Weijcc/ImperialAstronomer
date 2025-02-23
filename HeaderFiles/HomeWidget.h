#pragma once

#include <QMenu>

#include "ui_HomeInterface.h"
#include "LeagueAPI.h"

class HomeWidget : public QWidget
{
  Q_OBJECT
  QJsonDocument localSummonerDetails;
  Ui::HomeInterface* ui = nullptr;
  QObject* masterWidget = nullptr;
  LeagueAPI *leagueClientUxAPI = nullptr;
  const QMap<QByteArray, QColor> availabilityColorMap = {
      {"offline", QColor(48, 48, 48)},
      {"away", QColor(244, 67, 54)},
      {"dnd", QColor(38, 198, 218)},
      {"chat", QColor(0, 200, 83)},
      {"mobile", QColor(214, 204, 200)}};

  QMenu *friendListWidgetMenu = nullptr;
  QAction *spectateAction = nullptr;
  QAction *monitorSummonerAction = nullptr;
  QAction *deleteFriendAction = nullptr;
  QAction *copySummonerNameAction = nullptr;

public:
  HomeWidget(QWidget* parent = nullptr);
  ~HomeWidget() override;

  [[nodiscard]] QJsonDocument readLocalSummonerDetails();

public slots:
  void updateFriendListWidget(const QJsonDocument& details);
  void updateRankDataListWidget(const QJsonDocument& details);
  void updateRankFightDataListWidget(const QJsonDocument& details);
  void updateSummonerInfoGroupBox(const QJsonDocument& details);
};
