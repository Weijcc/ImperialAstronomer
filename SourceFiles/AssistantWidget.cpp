#include "AssistantWidget.h"

#include <QObject>
#include <QListWidget>
#include <QHeaderView>
#include <QTableWidget>
#include <QGraphicsDropShadowEffect>

#include "WindowsAPI.h"

AssistantWidget::AssistantWidget(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::AssistantInterface)
{
  this->ui->setupUi(this);
  this->setAttribute(Qt::WA_StaticContents, true);
  this->setAttribute(Qt::WA_TranslucentBackground, true);
  this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

  HMODULE module = ::GetModuleHandle(L"user32.dll");
  if (module)
  {
    ACCENTPOLICY accent = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 0x000001E0, 0xB3424242, 0x00000000};
    WINDOWCOMPOSITIONATTRIBDATA attribute = {WCA_ACCENT_POLICY, &accent, sizeof(ACCENTPOLICY)};
    auto setWindowCompositionAttribute = reinterpret_cast<setWindowCompositionAttributeHandle*>(
      ::GetProcAddress(module, "SetWindowCompositionAttribute"));
    setWindowCompositionAttribute(reinterpret_cast<HWND>(this->winId()), &attribute);
    ::FreeLibrary(module);
  }

  this->hide();
  this->leagueClientUxAPI = LeagueAPI::instance();

  this->ui->AssistantFrame->hide();

  this->loadingLabel = new QLabel(this);
  this->loadingLabel->setGeometry(100, 238, 100, 100);
  this->loadingMovie = new QMovie(":/ResourceFiles/BasicFiles/Loading.gif");
  this->loadingLabel->setMovie(this->loadingMovie);

  auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
  dropShadowEffect->setBlurRadius(2);
  dropShadowEffect->setOffset(0, 0);
  dropShadowEffect->setColor(QColor(66, 66, 66));
  this->ui->ClosePushButton->setGraphicsEffect(dropShadowEffect);

  connect(this->ui->ClosePushButton, &QPushButton::clicked, [&] {
    this->isEnable = false;
    this->hide();
    emit this->assistantClose();
  });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::readyAccessWindowGeometry,
      this,
      [&](bool changed, const QRect& rect) {
        auto geometry = this->rect();

        if (rect.isEmpty())
        {
          this->ui->ChampionDetailsTabWidget->clear();
          this->isEnable = false;
          this->hide();
          return;
        }

        if (this->isEnable && this->ui->ChampionDetailsTabWidget->count())
        {
          this->setGeometry(
              rect.x() + rect.width(),
              (rect.height() - geometry.height()) / 2 + rect.y(),
              geometry.width(),
              geometry.height());
          this->show();
        }

        if (changed)
        {
          this->loadingMovie->start();
          this->loadingLabel->show();
          this->ui->AssistantFrame->hide();
        }
      });

  connect(
      this->leagueClientUxAPI,
      &LeagueAPI::pickChampionChange,
      this,
      [&](const QJsonDocument& document) {
        if (!this->isEnable)
        {
          return;
        }

        this->ui->ChampionDetailsTabWidget->clear();

        std::once_flag championFlag;
        auto championLane = document["championLane"].toObject();
        auto championFight = document["championFight"].toObject();
        auto width = this->ui->ChampionDetailsTabWidget->width();

        for (const auto& key : championLane.keys())
        {
          auto position = championLane[key].toObject();
          auto winRate = position["lanewinrate"].toString().toDouble() / 100.0;
          auto pickRate = position["lanshowrate"].toString().toDouble() / 100.0;
          auto kda = championLane[key].toObject()["kda"].toString().toDouble() / 100.0;
          if (winRate != 0.0 && pickRate != 0.0)
          {
            std::call_once(
                championFlag,
                [&] {
                  auto championId = championLane[key].toObject()["championid"].toString().toInt();
                  this->ui->ChampionNameLabel->setText(this->leagueClientUxAPI->readChampionName(championId));
                  this->ui->ChampionIconLabel->setPixmap(this->leagueClientUxAPI->readChampionIcon(championId));
                });
            auto positionTabWidget = new QWidget(this->ui->ChampionDetailsTabWidget);
            auto positionInfoLabel = new QLabel(positionTabWidget);
            positionInfoLabel->setAlignment(Qt::AlignCenter);
            positionInfoLabel->setStyleSheet(
                "color: #FAFAFA;"
                "font: 14px \"HarmonyOS Sans SC\";");
            positionInfoLabel->setText(
                QString::asprintf("胜率：%.2f%%\n登场率：%.2f%%ㅤKDA：%.2f%%", winRate, pickRate, kda));
            positionInfoLabel->setGeometry(0, 0, width, 50);

            auto alignmentTabWidget = new QTabWidget(positionTabWidget);
            alignmentTabWidget->setObjectName("AlignmentTabWidget");
            alignmentTabWidget->setGeometry(0, 50, width, 180);

            std::once_flag advantagesFlag;
            QList<QString> advantagesHeaders;
            auto advantagesTableWidget = new QTableWidget(alignmentTabWidget);
            advantagesTableWidget->setShowGrid(false);
            advantagesTableWidget->setIconSize({30, 30});
            advantagesTableWidget->setObjectName("AdvantagesTableWidget");
            advantagesTableWidget->setColumnCount(3);
            advantagesTableWidget->setHorizontalHeaderLabels({"英雄", "胜率", "登场率"});
            advantagesTableWidget->setColumnWidth(0, 120);
            advantagesTableWidget->setColumnWidth(1, 70);
            advantagesTableWidget->setColumnWidth(2, 70);
            advantagesTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            advantagesTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
            advantagesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
            advantagesTableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

            std::once_flag disadvantagesFlag;
            QList<QString> disadvantagesHeaders;
            auto disadvantagesTableWidget = new QTableWidget(alignmentTabWidget);
            disadvantagesTableWidget->setShowGrid(false);
            disadvantagesTableWidget->setIconSize({30, 30});
            disadvantagesTableWidget->setObjectName("DisadvantagesTableWidget");
            disadvantagesTableWidget->setColumnCount(3);
            disadvantagesTableWidget->setHorizontalHeaderLabels({"英雄", "胜率", "登场率"});
            disadvantagesTableWidget->setColumnWidth(0, 120);
            disadvantagesTableWidget->setColumnWidth(1, 70);
            disadvantagesTableWidget->setColumnWidth(2, 70);
            disadvantagesTableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            disadvantagesTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
            disadvantagesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
            disadvantagesTableWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

            auto championList = championFight[key].toArray();
            for (const auto& champion : championList)
            {
              auto object = champion.toObject();
              auto cwinRate = object["winrate"].toString().toInt() / 100.0;
              auto cpickRate = object["champion2showrate"].toString().toInt() / 100.0;
              auto championId = object["championid2"].toString().toInt();

              if (cwinRate < 50.0)
              {
                std::call_once(
                    disadvantagesFlag,
                    [&] {
                      alignmentTabWidget->addTab(disadvantagesTableWidget, "劣势");
                    });
                auto rowIndex = disadvantagesTableWidget->rowCount();
                disadvantagesTableWidget->insertRow(rowIndex);

                auto item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setIcon(this->leagueClientUxAPI->readChampionIcon(championId));
                item->setText(this->leagueClientUxAPI->readChampionName(championId));
                disadvantagesTableWidget->setItem(rowIndex, 0, item);

                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(QString::asprintf("%.1f%%", cwinRate));
                disadvantagesTableWidget->setItem(rowIndex, 1, item);

                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(QString::asprintf("%.1f%%", cpickRate));
                disadvantagesTableWidget->setItem(rowIndex, 2, item);
                disadvantagesTableWidget->setRowHeight(rowIndex, 32);
                disadvantagesHeaders.append(QString::asprintf("%02d", rowIndex + 1));
              }
              else
              {
                std::call_once(
                    advantagesFlag,
                    [&] {
                      alignmentTabWidget->addTab(advantagesTableWidget, "优势");
                    });
                auto rowIndex = advantagesTableWidget->rowCount();
                advantagesTableWidget->insertRow(rowIndex);

                auto item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setIcon(this->leagueClientUxAPI->readChampionIcon(championId));
                item->setText(this->leagueClientUxAPI->readChampionName(championId));
                advantagesTableWidget->setItem(rowIndex, 0, item);

                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(QString::asprintf("%.1f%%", cwinRate));
                advantagesTableWidget->setItem(rowIndex, 1, item);

                item = new QTableWidgetItem();
                item->setTextAlignment(Qt::AlignCenter);
                item->setText(QString::asprintf("%.1f%%", cpickRate));
                advantagesTableWidget->setItem(rowIndex, 2, item);
                advantagesTableWidget->setRowHeight(rowIndex, 32);
                advantagesHeaders.append(QString::asprintf("%02d", rowIndex + 1));
              }
            }
            if (!advantagesTableWidget->rowCount() && !disadvantagesTableWidget->rowCount())
            {
              auto label = new QLabel(positionTabWidget);
              label->setStyleSheet(
                  "color: #FAFAFA;"
                  "font: 16px \"HarmonyOS Sans SC\";");
              label->setGeometry(0, 50, width, 120);
              label->setAlignment(Qt::AlignCenter);
              label->setText("无数据");
            }
            else
            {
              advantagesTableWidget->setVerticalHeaderLabels(advantagesHeaders);
              disadvantagesTableWidget->setVerticalHeaderLabels(disadvantagesHeaders);
            }

            if (!advantagesTableWidget->rowCount())
              advantagesTableWidget->deleteLater();

            if (!disadvantagesTableWidget->rowCount())
              disadvantagesTableWidget->deleteLater();

            auto textLabel = new QLabel(positionTabWidget);
            textLabel->setObjectName("PerkTextLabel");
            textLabel->setGeometry(0, 230, 50, 24);
            textLabel->setText("符文：");

            auto perkListWidget = new QListWidget(positionTabWidget);
            perkListWidget->setObjectName("PerkListWidget");
            perkListWidget->setGeometry(0, 256, width, 216); // 216
            perkListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            perkListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
            auto firstPerkDetail = championLane[key].toObject()["perkdetail"].toObject();

            for (const auto& firstKey : firstPerkDetail.keys())
            {
              auto secondPerkDetail = firstPerkDetail[firstKey].toObject();
              for (const auto& secondKey : secondPerkDetail.keys())
              {
                auto object = secondPerkDetail[secondKey].toObject();
                auto perkIds = object["perk"].toString().split('&');
                auto pwinRate = object["winrate"].toDouble() / 100.0;

                auto item = new QListWidgetItem();
                auto frame = new QFrame();
                auto label = new QLabel(frame);

                item->setSizeHint(QSize(0, 54));

                label->setScaledContents(true);
                label->setGeometry(4, 7, 40, 40);
                label->setStyleSheet(
                    "background: #424242;"
                    "border-radius: 20px;"
                    "border: 2px solid #B39DDB;");
                label->setPixmap(this->leagueClientUxAPI->readRuneIcon(perkIds[0].toInt()));

                auto dropShadowEffect = new QGraphicsDropShadowEffect(this);
                dropShadowEffect->setBlurRadius(2);
                dropShadowEffect->setOffset(0, 0);
                dropShadowEffect->setColor(QColor(0, 0, 0));
                label->setGraphicsEffect(dropShadowEffect);

                for (auto index = 1; index < 6; index++)
                {
                  auto perkId = perkIds[index].toInt();
                  auto pixmap = this->leagueClientUxAPI->readRuneIcon(perkId);

                  if (pixmap.isNull() || perkId < 8000)
                  {
                    delete item;
                    frame->deleteLater();
                    item = nullptr;
                    break;
                  }

                  label = new QLabel(frame);
                  label->setScaledContents(true);
                  label->setGeometry(50 + 28 * (index - 1), 4, 24, 24);
                  label->setPixmap(pixmap);

                  dropShadowEffect = new QGraphicsDropShadowEffect(this);
                  dropShadowEffect->setBlurRadius(2);
                  dropShadowEffect->setOffset(0, 0);
                  dropShadowEffect->setColor(QColor(0, 0, 0));
                  label->setGraphicsEffect(dropShadowEffect);
                }

                if (item)
                {
                  for (auto index = 6; index < perkIds.count(); index++)
                  {
                    auto perkId = perkIds[index].toInt();
                    auto pixmap = this->leagueClientUxAPI->readRuneIcon(perkId);

                    label = new QLabel(frame);
                    label->setStyleSheet(
                        "background: #263238;"
                        "border-radius: 10px;");
                    label->setScaledContents(true);
                    label->setGeometry(50 + 24 * (index - 6), 30, 20, 20);
                    label->setPixmap(pixmap);

                    dropShadowEffect = new QGraphicsDropShadowEffect(this);
                    dropShadowEffect->setBlurRadius(2);
                    dropShadowEffect->setOffset(0, 0);
                    dropShadowEffect->setColor(QColor(0, 0, 0));
                    label->setGraphicsEffect(dropShadowEffect);
                  }

                  label = new QLabel(frame);
                  label->setScaledContents(true);
                  label->setStyleSheet(
                      "color: #424242;"
                      "font: 12px \"HarmonyOS Sans SC\";");
                  label->setGeometry(122, 30, 80, 20);
                  label->setText(QString::asprintf("胜率：%.1f%%", pwinRate));

                  auto button = new QPushButton(frame);
                  button->setObjectName("ApplyPerkPushButton");
                  button->setGeometry(200, 11, 74, 32);
                  button->setText("应用");
                  button->setProperty("UserData", QVariant::fromValue(perkIds));

                  connect(
                      button,
                      &QPushButton::clicked,
                      this,
                      [this] {
                        auto* button = qobject_cast<QPushButton*>(QObject::sender());
                        auto originList = button->property("UserData").toStringList();
                        QList<QVariant> runeIdList;
                        std::transform(
                            originList.begin(),
                            originList.end(),
                            std::back_inserter(runeIdList),
                            [](const QString& rune) {
                              return QVariant(rune);
                            });
                        this->leagueClientUxAPI->modifyCurrentRunePage(runeIdList);
                      });

                  dropShadowEffect = new QGraphicsDropShadowEffect(this);
                  dropShadowEffect->setBlurRadius(2);
                  dropShadowEffect->setOffset(0, 0);
                  dropShadowEffect->setColor(QColor(0, 0, 0));
                  button->setGraphicsEffect(dropShadowEffect);

                  perkListWidget->addItem(item);
                  perkListWidget->setItemWidget(item, frame);
                }
              }
            }
            this->ui->ChampionDetailsTabWidget->addTab(
                positionTabWidget,
                this->leagueClientUxAPI->readPositionName(key.toUtf8()));
          }
        }
        this->loadingLabel->hide();
        this->loadingMovie->stop();
        this->ui->AssistantFrame->show();
      });
}

AssistantWidget::~AssistantWidget()
{
  delete this->ui;
}

void AssistantWidget::display(bool enable)
{
  if (!enable)
  {
    this->hide();
  }
  this->isEnable = enable;
}
