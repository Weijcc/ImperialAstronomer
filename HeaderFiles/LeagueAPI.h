#pragma once

#include <qthread.h>
#include <QRect>
#include <QUuid>
#include <QTimer>
#include <QMutex>
#include <QPixmap>
#include <QObject>
#include <QThread>
#include <QUrlQuery>
#include <QRunnable>
#include <QResource>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonValue>
#include <QThreadPool>
#include <QJsonObject>
#include <QJsonDocument>

#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>

class LeagueAPI : public QObject
{
  class Runnable : public QRunnable
  {
    std::function<void()> handler = nullptr;

  public:
    template <typename Fx, typename... Fa>
    Runnable(Fx&& execute, Fa&&... args)
    {
      this->handler = [function = execute, params = std::make_tuple(std::forward<Fa>(args)...)] {
        std::apply(function, params);
      };
    }

    Runnable() = default;
    ~Runnable() = default;

  protected:
    void run() override
    {
      if (this->handler)
      {
        this->handler();
      }
      QThread::currentThread()->exit();
    }
  };

  Q_OBJECT

  std::atomic<qint32> waitTime = 500;
  std::atomic<bool> execFlag = false;

  QEventLoop* eventLoop = nullptr;
  QThreadPool* threadPool = nullptr;
  httplib::Client* requestLCU = nullptr;
  httplib::Client* requestLOL = nullptr;
  httplib::Client* requestSGP = nullptr;
  QByteArray SERVER, PORT, AUTH, REGION, PATH, SGP;

  QMutex monitorSummonerMutex;
  QByteArray monitorSummonerUuid;
  QByteArray monitorSummonerName;

  QMutex monitorAccessTokenMutex;
  QByteArray accessToken;

  std::atomic<qint32> autoBanChampionId = -1;
  std::atomic<qint32> autoPickChampionId = -1;
  std::atomic<bool> matchmakingAutoAccept = false;

  qint32 selectChampionId = -1;

  std::string chatMeBody;
  std::string summonerBody;
  std::string rankedStatsBody;
  std::string accessTokenBody;
  std::string matchHistoryBody;
  std::string localFriendsBody;
  std::string localSummonerBody;
  std::string championMasteryBody;
  std::string localRankedStatsBody;

  LeagueAPI();
  ~LeagueAPI();
  LeagueAPI(LeagueAPI&) = delete;
  LeagueAPI(LeagueAPI&&) = delete;
  LeagueAPI(const LeagueAPI&) = delete;
  const LeagueAPI& operator=(const LeagueAPI&) = delete;

  bool initialize();
  void monitorPlatform();
  void monitorRankData();
  void monitorSummoner();
  void monitorAccessToken();
  void monitorLocalFriends();
  void monitorRankFightData();
  void monitorLocalSummoner();
  void monitorChampionSelect();
  void monitorChampionMastery();
  void monitorMatchmakingAutoAccept();

  QRect readWindowGeometry();
  QByteArray readAccessToken();
  void updateAccessToken(const QByteArray& token);

  const QList<std::function<void()>> runnableHandlerList = {
      std::bind(&LeagueAPI::monitorPlatform, this),
      std::bind(&LeagueAPI::monitorRankData, this),
      std::bind(&LeagueAPI::monitorSummoner, this),
      std::bind(&LeagueAPI::monitorAccessToken, this),
      std::bind(&LeagueAPI::monitorLocalFriends, this),
      std::bind(&LeagueAPI::monitorRankFightData, this),
      std::bind(&LeagueAPI::monitorLocalSummoner, this),
      std::bind(&LeagueAPI::monitorChampionSelect, this),
      std::bind(&LeagueAPI::monitorChampionMastery, this),
      std::bind(&LeagueAPI::monitorMatchmakingAutoAccept, this)};

  const httplib::Headers headersLCU = {{"Accept", "*/*"}, {"Connection", "keep-alive"}, {"Cache-Control", "no-cache"}};
  const httplib::Headers headersLOL = {
      {"Accept", "*/*"},
      {"Connection", "keep-alive"},
      {"Cache-Control", "no-cache"},
      {"Accept-Encoding", "gzip, deflate"}};
  const httplib::Headers headersSGP = {
      {"Accept", "*/*"},
      {"Connection", "keep-alive"},
      {"Cache-Control", "no-cache"},
      {"Accept-Encoding", "gzip, deflate"}};

  const QMap<QByteArray, QByteArray> regionDict = {
      {"NJ100", "联盟一区"},
      {"GZ100", "联盟二区"},
      {"CQ100", "联盟三区"},
      {"TJ100", "联盟四区"},
      {"TJ101", "联盟五区"},
      {"HN10", "黑色玫瑰"},
      {"HN1", "艾欧尼亚"},
      {"BGP2", "峡谷之巅"}};

  const QMap<QByteArray, QByteArray> regionDictSGP = {
      {"NJ100", "https://nj100-sgp.lol.qq.com:21019"},
      {"GZ100", "https://gz100-sgp.lol.qq.com:21019"},
      {"CQ100", "https://cq100-sgp.lol.qq.com:21019"},
      {"TJ100", "https://tj100-sgp.lol.qq.com:21019"},
      {"TJ101", "https://tj101-sgp.lol.qq.com:21019"},
      {"HN10", "https://hn10-k8s-sgp.lol.qq.com:21019"},
      {"HN1", "https://hn1-k8s-sgp.lol.qq.com:21019"},
      {"BGP2", "https://bgp2-k8s-sgp.lol.qq.com:21019"}};

  const QMap<QByteArray, QByteArray> availabilityDict =
      {{"offline", "离线"}, {"away", "离开"}, {"dnd", "游戏中"}, {"chat", "在线"}, {"mobile", "在线分组"}};

  const QMap<QByteArray, QByteArray> positionDict = {
      {"TOP", "上单"},
      {"JUNGLE", "打野"},
      {"MID", "中单"},
      {"ADC", "下路"},
      {"SUPPORT", "辅助"},
      {"bottom", "下路"},
      {"jungle", "打野"},
      {"mid", "中单"},
      {"support", "辅助"},
      {"top", "上路"}};

  const QMap<QByteArray, QByteArray> rankedShortDict = {
      {"IRON", "黑铁"},
      {"BRONZE", "黄铜"},
      {"SILVER", "白银"},
      {"GOLD", "黄金"},
      {"PLATINUM", "白金"},
      {"EMERALD", "翡翠"},
      {"DIAMOND", "钻石"},
      {"MASTER", "大师"},
      {"GRANDMASTER", "宗师"},
      {"CHALLENGER", "王者"}};

  const QList<std::pair<QByteArray, std::pair<QByteArray, QByteArray>>> rankedDivisionList = {
      {"无段位", {"UNRANKED", ""}},       {"坚韧黑铁Ⅳ", {"IRON", "IV"}},     {"坚韧黑铁Ⅲ", {"IRON", "III"}},
      {"坚韧黑铁Ⅱ", {"IRON", "II"}},      {"坚韧黑铁Ⅰ", {"IRON", "I"}},      {"英勇黄铜Ⅳ", {"BRONZE", "IV"}},
      {"英勇黄铜Ⅲ", {"BRONZE", "III"}},   {"英勇黄铜Ⅱ", {"BRONZE", "II"}},   {"英勇黄铜Ⅰ", {"BRONZE", "I"}},
      {"不屈白银Ⅳ", {"SILVER", "IV"}},    {"不屈白银Ⅲ", {"SILVER", "III"}},  {"不屈白银Ⅱ", {"SILVER", "II"}},
      {"不屈白银Ⅰ", {"SILVER", "I"}},     {"荣耀黄金Ⅳ", {"GOLD", "IV"}},     {"荣耀黄金Ⅲ", {"GOLD", "III"}},
      {"荣耀黄金Ⅱ", {"GOLD", "II"}},      {"荣耀黄金Ⅰ", {"GOLD", "I"}},      {"华贵铂金Ⅳ", {"PLATINUM", "IV"}},
      {"华贵铂金Ⅲ", {"PLATINUM", "III"}}, {"华贵铂金Ⅱ", {"PLATINUM", "II"}}, {"华贵铂金Ⅰ", {"PLATINUM", "I"}},
      {"流光翡翠Ⅳ", {"EMERALD", "IV"}},   {"流光翡翠Ⅲ", {"EMERALD", "III"}}, {"流光翡翠Ⅱ", {"EMERALD", "II"}},
      {"流光翡翠Ⅰ", {"EMERALD", "I"}},    {"璀璨钻石Ⅳ", {"DIAMOND", "IV"}},  {"璀璨钻石Ⅲ", {"DIAMOND", "III"}},
      {"璀璨钻石Ⅱ", {"DIAMOND", "II"}},   {"璀璨钻石Ⅰ", {"DIAMOND", "I"}},   {"超凡大师", {"MASTER", ""}},
      {"傲世宗师", {"GRANDMASTER", ""}},  {"最强王者", {"CHALLENGER", ""}}};

  QMap<qint32, qint32> runeIdDict;
  QMap<qint32, QByteArray> queueDict;
  QMap<qint32, QByteArray> championDict;

  QMap<QByteArray, QJsonArray> runeDict;
  QMap<QByteArray, QJsonObject> itemDict;
  QMap<qint32, std::pair<QByteArray, QByteArray>> skinsDict;

public:
  void exec();
  void exit();

  void modifyWaitTime(qint32 time);

  void modifyMonitorLocalSummoner();
  void modifyMatchmakingAutoAccept(bool isAccept);
  void modifyChatMeInfo(const QJsonDocument& body);
  void modifyAutoBanChampion(qint32 banChampionId);
  void modifyAutoPickChampion(qint32 pickChampionId);
  void modifyLobbyMessage(const QByteArray& message);
  void modifyMonitorSummoner(const QByteArray& details);
  void modifySpectateSummoner(const QByteArray& details);
  void modifyLocalSummonerProfile(const QJsonDocument& body);
  void modifyCurrentRunePage(const QList<QVariant>& runeIdList);
  void modifySummonerRankedList(const QList<qint64>& summonerIds, const QList<QByteArray>& queueTypes);

  void deleteFriend(const QByteArray& id);
  void friendRequest(const QByteArray& name);

  static LeagueAPI* instance();

signals:
  void initializeFail();
  void summonerError();
  void errorString(const QString& text);
  void banPickFinished();
  void autoBanPickFinished();
  void updateProgressRate(const QByteArray& text);
  void pickChampionChange(const QJsonDocument& document);
  void readyAccessWindowGeometry(bool changed, const QRect& rect = {});

  void readyAccessChampionDict(const QMap<qint32, QByteArray>& championDict);
  void readyAccessSkinsDict(const QMap<qint32, std::pair<QByteArray, QByteArray>>& skinsDict);

  void readyAccessMonitorRankData(const QJsonDocument& document);
  void readyAccessMonitorSummoner(const QJsonDocument& document);
  void readyAccessSummonerRankedList(const QJsonDocument& document);
  void readyAccessMonitorRankedStats(const QJsonDocument& document);
  void readyAccessMonitorMatchHistory(const QJsonDocument& document);
  void readyAccessMonitorLocalFriends(const QJsonDocument& document);
  void readyAccessMonitorLocalSummoner(const QJsonDocument& document);
  void readyAccessMonitorRankFightData(const QJsonDocument& document);
  void readyAccessMonitorChampionMastery(const QJsonDocument& document);
  void readyAccessMonitorLocalRankedStats(const QJsonDocument& document);

public:
  static QPixmap readRuneIcon(qint32 runeId);
  static QPixmap readItemIcon(qint32 itemId);
  static QPixmap readSpellIcon(qint32 spellId);
  static QPixmap readChampionIcon(qint32 champId);
  static QPixmap readProfileIcon(qint32 profileIconId);
  static QPixmap readOtherIcon(const QByteArray& name);
  static QPixmap readRankedIcon(const QByteArray& name);

  QByteArray readLocalRegion();
  QByteArray readGameInstallPath();
  QByteArray readQueueName(qint32 queueId);
  QByteArray readChampionName(qint32 champId);
  QByteArray readPositionName(const QByteArray& pos);
  QByteArray readAvailability(const QByteArray& state);
  QByteArray readRankedShortName(const QByteArray& tier);
  std::pair<QByteArray, QByteArray> readRankedPair(const QByteArray& name);
  QList<std::pair<QByteArray, std::pair<QByteArray, QByteArray>>> readRankedDivisionList();
  QByteArray readRankedName(const QByteArray& tier, const QByteArray& division);
  QByteArray readItemData(qint32 itemId, const QByteArray& version);
  QByteArray readRuneData(qint32 runeId, const QByteArray& version);
};
