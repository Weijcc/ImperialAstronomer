#pragma once

#include <QFile>
#include <QMovie>

#include "ui_AssistantInterface.h"
#include "LeagueAPI.h"

class AssistantWidget : public QWidget
{
  Q_OBJECT
  bool isEnable = false;

  QLabel* loadingLabel = nullptr;
  QMovie* loadingMovie = nullptr;

  Ui::AssistantInterface* ui = nullptr;
  LeagueAPI* leagueClientUxAPI = nullptr;

public:
  AssistantWidget(QWidget *parent = nullptr);
  ~AssistantWidget();

  void display(bool enable);

signals:
  void assistantClose();
  void loading(QWidget* base);
};
