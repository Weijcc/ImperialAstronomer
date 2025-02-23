#include "LoadingWidget.h"

#include <QGraphicsDropShadowEffect>

LoadingWidget::LoadingWidget(QWidget* parent) : QWidget(parent), ui(new Ui::LoadingInterface)
{
  this->ui->setupUi(this);

  this->setAttribute(Qt::WA_StaticContents, true);
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  this->setWindowFlags(this->windowFlags() | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  this->setWindowIcon(QIcon(":/ResourceFiles/BasicFiles/Icon.ico"));

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(0, 0, 0));
  this->ui->LogoLabel->setGraphicsEffect(dropShadowEffect);
}

LoadingWidget::~LoadingWidget()
{
  delete this->ui;
}
