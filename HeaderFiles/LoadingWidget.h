#pragma once

#include "ui_LoadingInterface.h"
#include "LeagueAPI.h"

class LoadingWidget : public QWidget
{
  Q_OBJECT
public:
  Ui::LoadingInterface* ui = nullptr;

  LoadingWidget(QWidget* parent = nullptr);
  ~LoadingWidget();
};
