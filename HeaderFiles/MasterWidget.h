#pragma once

#include <QTimer>
#include <QDialog>
#include <QSystemTrayIcon>
#include <QPropertyAnimation>

#include "ui_MasterInterface.h"
#include "ui_MessageBoxInterface.h"

#include "HomeWidget.h"
#include "MatchHistoryWidget.h"
#include "OtherFunctionWidget.h"
#include "AssistantWidget.h"
#include "LoadingWidget.h"

#include "LeagueAPI.h"

class Prompt : public QDialog
{
  Q_OBJECT

  QLabel* loadingLabel = nullptr;
  QMovie* loadingMovie = nullptr;

  Ui::PromptInterface* ui = nullptr;

  QPointF movePosition = {};
  bool movePositionFlag = false;

  Prompt();
  ~Prompt();

  static Prompt* handle;

  QTimer* timer = nullptr;
  QWidget* base = nullptr;

protected:
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

public:
  Prompt(Prompt&) = delete;
  Prompt(Prompt&&) = delete;
  Prompt(const Prompt&) = delete;
  const Prompt& operator=(const Prompt&) = delete;

  static Prompt* instance();
  static void release();

  void setBase(QWidget* base);

public slots:
  int loading();
  int error(const QString& text);
  int warning(const QString& text);
  int information(const QString& text);
};

class MasterWidget : public QWidget
{
  Q_OBJECT

  QPointF movePosition = {};
  bool movePositionFlag = false;
  QSystemTrayIcon* systemTrayIcon = nullptr;
  QPropertyAnimation* propertyAnimation = nullptr;

  AssistantWidget* assistantWidget = nullptr;
  LeagueAPI* leagueClientUxAPI = nullptr;
  LoadingWidget* loadingWidget = nullptr;
  QThread* leagueClientUxBaseThread = nullptr;

  const QMap<QByteArray, std::pair<QByteArray, QByteArray>> echoTextMap = {
      {"Mon", {"我看到了“生生不息”的激荡", "齐夏"}},
      {"Tue", {"我听到了“激发”的回响", "林擒"}},
      {"Wed", {"我听到了“强运”的回响", "云瑶"}},
      {"Thu", {"我听到了“巧物”的回响", "张丽娟"}},
      {"Fri", {"我听到了“破万法”的回响", "乔家劲"}},
      {"Sat", {"我听到了“忘忧”的回响", "罗十一"}},
      {"Sun", {"我听到了“天行健”的回响", "张山"}}};

protected:
  bool event(QEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  void leaveEvent(QEvent* event) override;

public:
  MasterWidget(QWidget* parent = nullptr);
  ~MasterWidget() override;

  void display();

  Ui::MasterInterface* ui = nullptr;

  HomeWidget* homeWidget = nullptr;
  MatchHistoryWidget* matchHistoryWidget = nullptr;
  OtherFunctionWidget* otherFunctionWidget = nullptr;
};