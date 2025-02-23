#pragma once

#include "ui_OtherFunctionInterface.h"
#include "LeagueAPI.h"

class OtherFunctionWidget : public QWidget
{
  Q_OBJECT
  LeagueAPI* leagueClientUxAPI = nullptr;

public:
  Ui::OtherFunctionInterface* ui = nullptr;

  OtherFunctionWidget(QWidget* parent = nullptr);
  ~OtherFunctionWidget() override;
};
