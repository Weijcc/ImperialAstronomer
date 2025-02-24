#include <QDir>
#include <QDebug>
#include <QProcess>
#include "LeagueAPI.h"

LeagueAPI::LeagueAPI()
{
  this->eventLoop = new QEventLoop(this);
  this->threadPool = new QThreadPool(this);
}

LeagueAPI::~LeagueAPI()
{
  delete this->requestLCU;
  delete this->requestSGP;
  delete this->requestLOL;
  if (this->execFlag.load())
  {
    this->execFlag.store(false);
    this->threadPool->waitForDone();
  }
}

bool LeagueAPI::initialize()
{
  QProcess process;
  process.setProgram("powershell.exe");
  process.setArguments(
      {"-Command",
       R"(Get-CimInstance -Query "SELECT * from Win32_Process WHERE name LIKE 'LeagueClientUx.exe'" | Select-Object -ExpandProperty CommandLine)"});
  process.start();
  process.waitForFinished();

  auto standardOutput = process.readAllStandardOutput();
  if (standardOutput.isEmpty() || (standardOutput.indexOf("--app-port=") == -1)
      || (standardOutput.indexOf("--install-directory") == -1)
      || (standardOutput.indexOf("--remoting-auth-token") == -1) || (standardOutput.indexOf("--rso_platform_id") == -1))
  {
    return false;
  }

  QRegularExpression regularExpression;
  regularExpression.setPattern("--app-port=([0-9]*)");
  this->PORT = regularExpression.match(standardOutput).captured(0).replace("--app-port=", "").toUtf8();
  regularExpression.setPattern("--remoting-auth-token=([\\w-]*)");
  this->AUTH = regularExpression.match(standardOutput).captured(0).replace("--remoting-auth-token=", "").toUtf8();
  regularExpression.setPattern("--rso_platform_id=([\\w-]*)");
  this->REGION = regularExpression.match(standardOutput).captured(0).replace("--rso_platform_id=", "").toUtf8();
  regularExpression.setPattern(
      "--install-directory=([a-z]:(\\[\\w\u4E00-\u9FA5..\\s]*)*[.\\w\\\\u4E00-"
      "\u9FA5\\s]*.*\\\\)");
  this->PATH = regularExpression.match(standardOutput).captured(0).replace("--install-directory=", "").toUtf8();

  if (this->PORT.isEmpty() || this->AUTH.isEmpty() || this->REGION.isEmpty() || this->PATH.isEmpty())
  {
    return false;
  }

  this->SERVER = "https://127.0.0.1:" + this->PORT;
  this->SGP = "https://" + this->REGION.toLower() + "-sgp.lol.qq.com:21019";

  this->requestLCU = new httplib::Client(this->SERVER.toStdString());
  this->requestLCU->enable_server_certificate_verification(false);
  this->requestLCU->set_basic_auth("riot", this->AUTH.toStdString());
  this->requestLCU->set_default_headers(this->defaultHeaders);
  this->requestLCU->set_connection_timeout(1);
  this->requestLCU->set_write_timeout(1);
  this->requestLCU->set_read_timeout(1);
  this->requestLCU->set_keep_alive(true);

  this->requestLOL = new httplib::Client("https://lol.qq.com");
  this->requestLOL->enable_server_certificate_verification(false);
  this->requestLOL->set_default_headers(this->defaultHeaders);
  this->requestLOL->set_connection_timeout(1);
  this->requestLOL->set_write_timeout(1);
  this->requestLOL->set_read_timeout(1);
  this->requestLOL->set_keep_alive(true);

  this->requestSGP = new httplib::Client(this->regionDictSGP[this->REGION].toStdString());
  this->requestSGP->enable_server_certificate_verification(false);
  this->requestSGP->set_default_headers(this->defaultHeaders);
  this->requestSGP->set_connection_timeout(1);
  this->requestSGP->set_write_timeout(1);
  this->requestSGP->set_read_timeout(1);
  this->requestSGP->set_keep_alive(true);

  this->queueDict[0] = "训练模式";
  auto queues = QJsonDocument::fromJson(QResource(":/ResourceFiles/DataFiles/queue.json").uncompressedData()).array();
  for (const auto& queue : queues)
  {
    auto object = queue.toObject();
    auto queueId = object["id"].toInt();
    auto queueName = object["name"].toString().toUtf8();
    this->queueDict[queueId] = queueName.isEmpty() ? QByteArray("未知模式") : queueName;
  }

  auto champions =
      QJsonDocument::fromJson(QResource(":/ResourceFiles/DataFiles/champion.json").uncompressedData())["data"]
          .toObject();
  for (const auto& champion : champions)
  {
    auto object = champion.toObject();
    auto champId = object["key"].toString().toInt();
    auto champName = object["name"].toString().toUtf8();
    this->championDict[champId] = champName;
  }
  emit this->readyAccessChampionDict(this->championDict);

  auto latest = QJsonDocument::fromJson(QResource(":/ResourceFiles/DataFiles/versions.json").uncompressedData())
                    .array()
                    .at(0)
                    .toString();
  latest = latest.mid(0, latest.lastIndexOf('.'));

  auto runeList = QJsonDocument::fromJson(
                      QResource(":/ResourceFiles/DataFiles/runesReforged" + latest + ".json").uncompressedData())
                      .array();
  for (const auto& rune : runeList)
  {
    auto object = rune.toObject();
    auto masterId = object["id"].toInt();
    auto subList = object["slots"].toArray();

    for (const auto& sub : subList)
    {
      auto prekList = sub.toObject()["runes"].toArray();
      for (const auto& perk : prekList)
      {
        auto subId = perk.toObject()["id"].toInt();
        this->runeIdDict[subId] = masterId;
      }
    }
  }

  auto dataFileList = QDir(":/ResourceFiles/DataFiles").entryList();
  for (const auto& dataFile : dataFileList)
  {
    if (dataFile.indexOf("item") == 0)
    {
      auto version = dataFile.mid(0 + QString("item").size()).replace(".json", "").toUtf8();
      this->itemDict[version] =
          QJsonDocument::fromJson(QResource(":/ResourceFiles/DataFiles/" + dataFile).uncompressedData())["data"]
              .toObject();
    }
    else if (dataFile.indexOf("runesReforged") == 0)
    {
      auto version = dataFile.mid(0 + QString("runesReforged").size()).replace(".json", "").toUtf8();
      this->runeDict[version] =
          QJsonDocument::fromJson(QResource(":/ResourceFiles/DataFiles/" + dataFile).uncompressedData()).array();
    }
  }

  auto skins = QJsonDocument::fromJson(QResource(":/ResourceFiles/DataFiles/skins.json").uncompressedData()).array();
  for (const auto& skin : skins)
  {
    auto object = skin.toObject();
    auto id = object["id"].toString().toInt();
    auto name = object["name"].toString().toUtf8();
    auto file = object["file"].toString().toUtf8();
    this->skinsDict[id] = std::make_pair(name, ":/ResourceFiles/SkinImageFiles/" + file);
  }
  emit this->readyAccessSkinsDict(this->skinsDict);

  // qDebug().noquote() << "\n\nInit OK" << PORT << AUTH << SGP;
  return true;
}

void LeagueAPI::monitorRankData()
{
  using namespace std::string_literals;
  QUrlQuery requestQuery;
  requestQuery.addQueryItem("iChartId", "319518");
  requestQuery.addQueryItem("iSubChartId", "319518");
  requestQuery.addQueryItem("sIdeToken", "F7h8FE");
  requestQuery.addQueryItem("championid", "666");
  requestQuery.addQueryItem("lane", "all");
  requestQuery.addQueryItem("ijob", "all");
  requestQuery.addQueryItem("gamequeueconfigid", "420");
  requestQuery.addQueryItem("tier", "311");

  const httplib::Headers headersRankData = {
      {"Accept"s, "*/*"s},
      {"Connection"s, "keep-alive"s},
      {"Cache-Control"s, "no-cache"s},
      {"Accept-Encoding"s, "gzip, deflate"s}};

  httplib::Client requestRankData("https://comm.ams.game.qq.com"s);

  auto createTime = QDateTime::currentSecsSinceEpoch();

  while (this->execFlag.load())
  {
    while (true)
    {
      auto response = requestRankData.Post(
          "/ide/"s,
          headersRankData,
          requestQuery.toString(QUrl::FullyEncoded).toStdString(),
          "application/x-www-form-urlencoded"s);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        emit this->updateProgressRate("Loading rank data...");
        auto document = QJsonDocument::fromJson(response->body.c_str());

        if (document["jData"]["sMsg"].toString() == "ok")
        {
          emit this->readyAccessMonitorRankData(document);
          emit this->updateProgressRate("Loading rank data finished!");
          break;
        }
      }
      QThread::msleep(this->waitTime.load());
    }

    while (QDateTime::currentSecsSinceEpoch() - createTime < 14400 && this->execFlag.load())
    {
      QThread::msleep(this->waitTime.load());
    }
    createTime = QDateTime::currentSecsSinceEpoch();
  }
}

void LeagueAPI::monitorPlatform()
{
  using namespace std::string_literals;

  auto handler = [](const QString& name) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (!::Process32First(snapshot, &entry))
    {
      ::CloseHandle(snapshot);
      return false;
    }

    do
    {
      if (!_tcsicmp(entry.szExeFile, name.toStdWString().c_str()))
      {
        ::CloseHandle(snapshot);
        return true;
      }
    } while (::Process32Next(snapshot, &entry));
    ::CloseHandle(snapshot);
    return false;
  };

  while (this->execFlag.load())
  {
    if (handler("LeagueClientUx.exe"))
    {
      auto response = this->requestLCU->Get("/lol-summoner/v1/status"s);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        auto document = QJsonDocument::fromJson(response->body.c_str());
        if (!document["ready"].toBool())
        {
          emit this->summonerError();
          break;
        }
      }
      else
      {
        emit this->summonerError();
        break;
      }
    }
    else
    {
      emit this->summonerError();
      break;
    }
    QThread::msleep(this->waitTime.load());
  }
}

void LeagueAPI::monitorSummoner()
{
  using namespace std::string_literals;

  while (this->execFlag.load())
  {
    this->monitorSummonerMutex.lock();
    auto name = this->monitorSummonerName;
    auto puuid = this->monitorSummonerUuid;
    this->monitorSummonerMutex.unlock();

    if (!puuid.isEmpty())
    {
      emit this->updateProgressRate("Loading monitor summoner...");
      auto response = this->requestLCU->Get("/lol-summoner/v2/summoners/puuid/"s + puuid.toStdString());
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
          && this->summonerBody != response->body)
      {
        this->summonerBody = response->body;
        auto document = QJsonDocument::fromJson(response->body.c_str());
        emit this->readyAccessMonitorSummoner(document);
      }

      response = this->requestLCU->Get("/lol-ranked/v1/ranked-stats/"s + puuid.toStdString());
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
          && this->rankedStatsBody != response->body)
      {
        this->rankedStatsBody = response->body;
        auto document = QJsonDocument::fromJson(response->body.c_str());
        emit this->readyAccessMonitorRankedStats(document);
      }

      QByteArray token;
      do
      {
        token = this->readAccessToken();
      } while (token.isEmpty());

      this->requestSGP->set_bearer_token_auth(token.toStdString());
      response = this->requestSGP->Get(
          "/match-history-query/v1/products/lol/player/"s + puuid.toStdString() + "/SUMMARY"s,
          httplib::Params{{"startIndex"s, "0"s}, {"count"s, "50"s}},
          this->defaultHeaders);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
          && this->matchHistoryBody != response->body)
      {
        this->matchHistoryBody = response->body;
        auto document = QJsonDocument::fromJson(response->body.c_str());
        emit this->readyAccessMonitorMatchHistory(document);
      }
      emit this->updateProgressRate("Loading monitor summoner finished!");
    }
    else if (!name.isEmpty())
    {
      auto response = this->requestLCU->Get(
          "/lol-summoner/v1/summoners"s, httplib::Params{{"name"s, name.toStdString()}}, this->defaultHeaders);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        auto document = QJsonDocument::fromJson(response->body.c_str());

        this->monitorSummonerMutex.lock();
        this->monitorSummonerName.clear();
        this->monitorSummonerUuid = document["puuid"].toString().toUtf8();
        this->monitorSummonerMutex.unlock();
      }
    }
    QThread::msleep(this->waitTime.load());
  }
}

void LeagueAPI::monitorAccessToken()
{
  using namespace std::string_literals;
  while (this->execFlag.load())
  {
    emit this->updateProgressRate("Loading access token...");
    auto response = this->requestLCU->Get("/entitlements/v1/token"s);
    if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
        && this->accessTokenBody != response->body)
    {
      this->accessTokenBody = response->body;
      auto document = QJsonDocument::fromJson(response->body.c_str());
      this->updateAccessToken(document["accessToken"].toString().toUtf8());
      emit this->updateProgressRate("Loading access token finished!");
    }
    QThread::msleep(this->waitTime.load());
  }
}

void LeagueAPI::monitorLocalFriends()
{
  using namespace std::string_literals;
  while (this->execFlag.load())
  {
    emit this->updateProgressRate("Loading local friends...");
    auto response = this->requestLCU->Get("/lol-chat/v1/friends"s);
    if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
        && this->localFriendsBody != response->body)
    {
      this->localFriendsBody = response->body;
      auto document = QJsonDocument::fromJson(response->body.c_str());
      auto friendsList = document.array().toVariantList();
      std::sort(friendsList.begin(), friendsList.end(), [](const QVariant& first, const QVariant& second) {
        auto firstAvailability = first.toJsonObject()["availability"].toString();
        auto secondAvailability = second.toJsonObject()["availability"].toString();
        if (firstAvailability != secondAvailability && firstAvailability != "offline" && firstAvailability != "away"
            && firstAvailability != "dnd" && firstAvailability != "chat" && firstAvailability != "mobile")
          return true;
        return false;
      });
      std::sort(friendsList.begin(), friendsList.end(), [](const QVariant& first, const QVariant& second) {
        auto firstAvailability = first.toJsonObject()["availability"].toString();
        auto secondAvailability = second.toJsonObject()["availability"].toString();
        if (firstAvailability != secondAvailability && firstAvailability == "mobile")
          return true;
        return false;
      });
      std::sort(friendsList.begin(), friendsList.end(), [](const QVariant& first, const QVariant& second) {
        auto firstAvailability = first.toJsonObject()["availability"].toString();
        auto secondAvailability = second.toJsonObject()["availability"].toString();
        if (firstAvailability != secondAvailability && firstAvailability == "away")
          return true;
        return false;
      });
      std::sort(friendsList.begin(), friendsList.end(), [&](QVariant& first, QVariant& second) {
        auto firstAvailability = first.toJsonObject()["availability"].toString();
        auto secondAvailability = second.toJsonObject()["availability"].toString();
        if (firstAvailability != secondAvailability && firstAvailability == "dnd")
        {
          auto puuid = first.toJsonObject()["puuid"].toString().toStdString();

          this->requestSGP->set_bearer_token_auth(this->readAccessToken().toStdString());
          response = this->requestSGP->Get(
              "/gsm/v1/ledge/spectator/region/" + this->REGION.toLower().toStdString() + "/puuid/" + puuid);

          if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
          {
            auto playerCredentials = QJsonDocument::fromJson(response->body.c_str())["playerCredentials"].toObject();
            auto object = first.toJsonObject();
            object.insert("playerCredentials", playerCredentials);
            first = QVariant::fromValue(object);
          }
          return true;
        }
        return false;
      });
      std::sort(friendsList.begin(), friendsList.end(), [](const QVariant& first, const QVariant& second) {
        auto firstAvailability = first.toJsonObject()["availability"].toString();
        auto secondAvailability = second.toJsonObject()["availability"].toString();
        if (firstAvailability != secondAvailability && firstAvailability == "chat")
          return true;
        return false;
      });

      emit this->readyAccessMonitorLocalFriends(QJsonDocument(QJsonArray::fromVariantList(friendsList)));
      emit this->updateProgressRate("Loading local friends finished!");
    }
    QThread::msleep(this->waitTime.load());
  }
}

void LeagueAPI::monitorRankFightData()
{
  using namespace std::string_literals;
  httplib::Params requestParams = {
      {"dtstatdate"s, QDateTime::currentDateTime().addDays(-2).toString("yyyyMMdd").toStdString()},
      {"callback"s, "getRankFightCallback"s},
      {"ts"s, QString::number(QDateTime::currentMSecsSinceEpoch() / 600000).toStdString()}};

  const httplib::Headers headersRankFightData = {
      {"Accept"s, "*/*"s},
      {"Host"s, "lol.sw.game.qq.com"s},
      {"Connection"s, "keep-alive"s},
      {"Cache-Control"s, "no-cache"s},
      {"Referer"s, "https://101.qq.com/"s},
      {"Accept-Encoding"s, "gzip, deflate"s}};

  httplib::Client requestRankFightData("https://lol.sw.game.qq.com"s);

  auto createTime = QDateTime::currentSecsSinceEpoch();

  while (this->execFlag.load())
  {
    emit this->updateProgressRate("Loading rank fight data...");

    while (true)
    {
      auto response = requestRankFightData.Get(
          "/lol/lwdcommact/a20211015billboard/a20211015api/fight"s, requestParams, headersRankFightData);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        QByteArray result(response->body);
        result.replace("getRankFightCallback(", "");
        result.replace(")", "");

        auto document = QJsonDocument::fromJson(result);

        if (document["code"].toInt() == 0)
        {
          emit this->readyAccessMonitorRankFightData(document);
          emit this->updateProgressRate("Loading rank fight data finished!");
          break;
        }
      }
      QThread::msleep(this->waitTime.load());
    }

    while (QDateTime::currentSecsSinceEpoch() - createTime < 14400 && this->execFlag.load())
    {
      QThread::msleep(this->waitTime.load());
    }
    createTime = QDateTime::currentSecsSinceEpoch();
  }
}

void LeagueAPI::monitorLocalSummoner()
{
  using namespace std::string_literals;
  while (this->execFlag.load())
  {
    emit this->updateProgressRate("Loading local summoner...");
    QJsonDocument document;
    auto response = this->requestLCU->Get("/lol-summoner/v1/current-summoner"s);
    if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
        && this->localSummonerBody != response->body)
    {
      this->localSummonerBody = response->body;
      document = QJsonDocument::fromJson(response->body.c_str());
      emit this->readyAccessMonitorLocalSummoner(document);
      emit this->updateProgressRate("Loading local summoner finished!");
    }

    auto puuid = document["puuid"].toString().toUtf8();

    emit this->updateProgressRate("Loading local summoner ranked stats...");
    response = this->requestLCU->Get("/lol-ranked/v1/ranked-stats/"s + puuid.toStdString());
    if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
        && this->localRankedStatsBody != response->body)
    {
      this->localRankedStatsBody = response->body;
      document = QJsonDocument::fromJson(response->body.c_str());
      emit this->readyAccessMonitorLocalRankedStats(document);
      emit this->updateProgressRate("Loading local summoner ranked stats finished!");
    }

    this->monitorSummonerMutex.lock();
    if (this->monitorSummonerUuid.isEmpty())
    {
      this->monitorSummonerUuid = puuid;
    }
    this->monitorSummonerMutex.unlock();
    QThread::msleep(this->waitTime.load());
  }
}

void LeagueAPI::monitorChampionSelect()
{
  using namespace std::string_literals;

  while (this->execFlag.load())
  {
    auto response = this->requestLCU->Get("/lol-champ-select/v1/session"s);
    if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
    {
      auto document = QJsonDocument::fromJson(response->body.c_str());
      auto localPlayerCellId = document["localPlayerCellId"].toInt();
      auto actions = document["actions"].toArray();
      auto myTeam = document["myTeam"].toArray();

      QJsonObject actionJson;
      QJsonObject resultJson;

      for (const auto& actionList : actions)
      {
        for (const auto& action : actionList.toArray())
        {
          if (localPlayerCellId == action.toObject()["actorCellId"].toInt())
          {
            actionJson = action.toObject();
          }
        }
      }

      for (const auto& member : myTeam)
      {
        auto object = member.toObject();
        if (localPlayerCellId == object["cellId"].toInt())
        {
          resultJson.insert("championId", object["championId"].toInt());
        }
      }

      if (!actionJson.isEmpty())
      {
        resultJson.insert("action", actionJson);
      }

      document = QJsonDocument(resultJson);

      auto championId = 0;
      auto champId = document["championId"].toInt();
      auto action = document["action"].toObject();

      if (!action.isEmpty())
      {
        auto id = action["id"].toInt();
        auto type = action["type"].toString();
        auto completed = action["completed"].toBool();
        championId = action["championId"].toInt();
        if (!completed && !championId)
        {
          QJsonObject body;
          if (this->autoBanChampionId.load() && type == "ban")
          {
            body.insert("championId", this->autoBanChampionId.load());
            body.insert("type", "ban");
            body.insert("completed", true);
          }
          else if (this->autoPickChampionId.load() && type == "pick")
          {
            body.insert("championId", this->autoPickChampionId.load());
            body.insert("type", "pick");
            body.insert("completed", false);
          }

          if (!body.isEmpty())
          {
            response = this->requestLCU->Patch(
                "/lol-champ-select/v1/session/actions/"s + std::to_string(id),
                QJsonDocument(body).toJson().toStdString(),
                "application/json"s);
            if (response && response->status == httplib::StatusCode::NoContent_204)
            {
              emit this->autoBanPickFinished();
            }
          }
        }
        else if (completed && type == "pick")
        {
          emit this->banPickFinished();
        }
      }

      if (champId && (champId == championId || action.isEmpty()))
      {
        if (this->selectChampionId != champId)
        {
          this->selectChampionId = champId;
          response = this->requestLOL->Get(
              "/act/lbp/common/guides/champDetail/champDetail_"s + std::to_string(champId) + ".js"s);
          if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
          {
            QJsonObject result;
            QByteArray body(response->body);
            body.replace("var CHAMPION_DETAIL_" + QByteArray::number(champId) + "=", "");
            body.replace("\n", "");
            body.replace(R"(\")", R"(")");
            body.replace(R"("{)", "{");
            body.replace(R"(}")", "}");
            body.replace(R"("[)", "[");
            body.replace(R"(]")", "]");
            auto end = body.lastIndexOf(";/*");
            body.replace(end, body.size() - end, "");
            document = QJsonDocument::fromJson(body);

            auto championLane = document["list"]["championLane"].toObject();
            auto championFight = document["list"]["championFight"].toObject();

            result.insert("championLane", championLane);
            result.insert("championFight", championFight);
            emit this->readyAccessWindowGeometry(true, this->readWindowGeometry());
            emit this->pickChampionChange(QJsonDocument(result));
          }
        }
        if (this->selectChampionId != -1)
        {
          emit this->readyAccessWindowGeometry(false, this->readWindowGeometry());
        }
      }
    }
    else
    {
      if (this->selectChampionId != -1)
      {
        this->selectChampionId = -1;
        emit this->readyAccessWindowGeometry(false);
      }
    }
    QThread::msleep(this->waitTime.load());
  }
}

void LeagueAPI::monitorChampionMastery()
{
  using namespace std::string_literals;
  while (this->execFlag.load())
  {
    this->monitorSummonerMutex.lock();
    auto puuid = this->monitorSummonerUuid;
    this->monitorSummonerMutex.unlock();
    if (!puuid.isEmpty())
    {
      emit this->updateProgressRate("Loading local champion mastery...");
      auto response = this->requestLCU->Get("/lol-champion-mastery/v1/"s + puuid.toStdString() + "/champion-mastery"s);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
          && this->championMasteryBody != response->body)
      {
        this->championMasteryBody = response->body;
        auto document = QJsonDocument::fromJson(response->body.c_str());
        emit this->readyAccessMonitorChampionMastery(document);
        emit this->updateProgressRate("Loading local champion mastery finished!");
      }
    }
    QThread::msleep(this->waitTime.load());
  }
}

void LeagueAPI::monitorMatchmakingAutoAccept()
{
  using namespace std::string_literals;
  while (this->execFlag.load())
  {
    auto response = this->requestLCU->Get("/lol-matchmaking/v1/ready-check"s);
    if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty()
        && this->matchmakingAutoAccept.load())
    {
      auto document = QJsonDocument::fromJson(response->body.c_str());
      if (document["state"].toString() == "InProgress")
      {
        this->requestLCU->Post("/lol-matchmaking/v1/ready-check/accept"s);
      }
    }
    QThread::msleep(this->waitTime.load());
  }
}

QByteArray LeagueAPI::readAccessToken()
{
  QMutexLocker locker(&this->monitorAccessTokenMutex);
  return this->accessToken;
}

void LeagueAPI::updateAccessToken(const QByteArray& token)
{
  QMutexLocker locker(&this->monitorAccessTokenMutex);
  this->accessToken = token;
}

QRect LeagueAPI::readWindowGeometry()
{
  RECT clientWinRect;
  auto clientWinHandle = ::FindWindow(L"RCLIENT", L"League of Legends");
  ::GetWindowRect(clientWinHandle, &clientWinRect);
  return {
      clientWinRect.left,
      clientWinRect.top,
      clientWinRect.right - clientWinRect.left,
      clientWinRect.bottom - clientWinRect.top};
}

void LeagueAPI::exec()
{
  if (!this->initialize())
  {
    emit this->initializeFail();
    QThread::currentThread()->exit();
    return;
  }

  this->execFlag.store(true);
  for (const auto& runnableHandler : this->runnableHandlerList)
  {
    auto runnable = new Runnable(runnableHandler);
    this->threadPool->start(runnable);
  }

  this->eventLoop->exec();
  QThread::currentThread()->exit();
}

void LeagueAPI::exit()
{
  if (this->execFlag.load())
  {
    this->waitTime.store(0);
    this->execFlag.store(false);
    this->threadPool->waitForDone();
    this->eventLoop->quit();
  }
}

void LeagueAPI::modifyWaitTime(qint32 time)
{
  this->waitTime.store(time);
}

void LeagueAPI::modifyMonitorLocalSummoner()
{
  using namespace std::string_literals;

  auto handler = [this] {
    int retry = 10;
    QJsonDocument document;

    while (retry--)
    {
      auto response = this->requestLCU->Get("/lol-summoner/v1/current-summoner"s);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        document = QJsonDocument::fromJson(response->body.c_str());
        this->monitorSummonerMutex.lock();
        this->monitorSummonerUuid = document["puuid"].toString().toUtf8();
        this->monitorSummonerMutex.unlock();
        break;
      }
    }
  };
  this->threadPool->start(new Runnable(handler));
}

void LeagueAPI::modifyMatchmakingAutoAccept(bool isAccept)
{
  this->matchmakingAutoAccept.store(isAccept);
}

void LeagueAPI::modifyChatMeInfo(const QJsonDocument& body)
{
  using namespace std::string_literals;

  auto handler = [this](const QJsonDocument& data) {
    int retry = 10;
    while (retry--)
    {
      auto response = this->requestLCU->Put("/lol-chat/v1/me"s, data.toJson().toStdString(), "application/json"s);
      if (response && response->status == httplib::StatusCode::OK_200)
      {
        break;
      }
    }
  };
  this->threadPool->start(new Runnable(handler, body));
}

void LeagueAPI::modifyAutoBanChampion(qint32 banChampionId)
{
  this->autoBanChampionId.store(banChampionId);
}

void LeagueAPI::modifyAutoPickChampion(qint32 pickChampionId)
{
  this->autoPickChampionId.store(pickChampionId);
}

void LeagueAPI::modifyLobbyMessage(const QByteArray& message)
{
  using namespace std::string_literals;

  auto handler = [this](const QByteArray& text) {
    int retry = 10;

    while (retry--)
    {
      auto response = this->requestLCU->Get("/lol-chat/v1/conversations"s);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        QString id;
        auto document = QJsonDocument::fromJson(response->body.c_str());
        auto conversations = document.array();
        for (const auto& conversation : conversations)
        {
          if (conversation.toObject()["type"].toString() == "championSelect")
          {
            id = conversation.toObject()["id"].toString();
            break;
          }
        }
        QJsonObject body;
        body.insert("type", "chat");
        body.insert("body", QString(text));

        retry = 10;
        while (retry--)
        {
          response = this->requestLCU->Post(
              "/lol-chat/v1/conversations/"s + id.toStdString() + "/messages"s,
              QJsonDocument(body).toJson().toStdString(),
              "application/json"s);
          if (response && response->status == httplib::StatusCode::OK_200)
          {
            break;
          }
        }
        break;
      }
    }
  };
  this->threadPool->start(new Runnable(handler, message));
}

void LeagueAPI::modifyMonitorSummoner(const QByteArray& details)
{
  auto handler = [this, summoner = details] {
    QMutexLocker locker(&this->monitorSummonerMutex);
    auto puuid = QUuid(summoner);

    this->summonerBody.clear();
    this->rankedStatsBody.clear();
    this->matchHistoryBody.clear();

    if (puuid.isNull())
    {
      this->monitorSummonerUuid.clear();
      this->monitorSummonerName = summoner;
    }
    else
    {
      this->monitorSummonerUuid = puuid.toString(QUuid::WithoutBraces).toUtf8();
    }
  };
  this->threadPool->start(new Runnable(handler));
}

void LeagueAPI::modifySpectateSummoner(const QByteArray& details)
{
  using namespace std::string_literals;

  auto handler = [this](const QByteArray& summoner) {
    QJsonObject body;
    QByteArray fullName;

    auto puuid = QUuid(summoner);
    if (puuid.isNull())
    {
      int retry = 10;
      fullName = summoner;
      while (retry--)
      {
        auto response = this->requestLCU->Get(
            "/lol-summoner/v1/summoners"s, httplib::Params{{"name"s, fullName.toStdString()}}, this->defaultHeaders);
        if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
        {
          auto document = QJsonDocument::fromJson(response->body.c_str());
          puuid = document["puuid"].toVariant().toUuid();

          body.insert("allowObserveMode", "ALL");
          body.insert("dropInSpectateGameId", QString(fullName));
          body.insert("gameQueueType", "");
          body.insert("puuid", puuid.toString(QUuid::WithoutBraces));
          break;
        }
      }
    }
    else
    {
      int retry = 10;
      while (retry--)
      {
        auto response = this->requestLCU->Get(
            "/lol-summoner/v2/summoners/puuid/"s + puuid.toString(QUuid::WithoutBraces).toStdString());
        if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
        {
          auto document = QJsonDocument::fromJson(response->body.c_str());
          auto gameName = document["gameName"].toString();
          auto tagLine = document["tagLine"].toString();
          fullName = (gameName + "#" + tagLine).toUtf8();

          body.insert("allowObserveMode", "ALL");
          body.insert("dropInSpectateGameId", QString(fullName));
          body.insert("gameQueueType", "");
          body.insert("puuid", puuid.toString(QUuid::WithoutBraces));
          break;
        }
      }
    }

    auto response = this->requestLCU->Post(
        "/lol-spectator/v1/spectate/launch"s, QJsonDocument(body).toJson().toStdString(), "application/json"s);

    if (response && response->status == httplib::StatusCode::BadRequest_400)
    {
      emit this->errorString("该玩家不在游戏中");
    }
    else if (response && response->status == httplib::StatusCode::NoContent_204)
    {
      RECT clientWinRect;
      auto clientWinHandle = ::FindWindow(L"RCLIENT", L"League of Legends");
      ::SetForegroundWindow(clientWinHandle);
    }
  };
  this->threadPool->start(new Runnable(handler, details));
}

void LeagueAPI::modifyLocalSummonerProfile(const QJsonDocument& body)
{
  using namespace std::string_literals;

  auto handler = [this, data = body] {
    int retry = 10;
    while (retry--)
    {
      auto response = this->requestLCU->Post(
          "/lol-summoner/v1/current-summoner/summoner-profile"s, data.toJson().toStdString(), "application/json"s);
      if (response && response->status == httplib::StatusCode::OK_200)
      {
        break;
      }
      QThread::msleep(this->waitTime.load());
    }
  };
  this->threadPool->start(new Runnable(handler));
}

void LeagueAPI::modifyCurrentRunePage(const QList<QVariant>& runeIdList)
{
  using namespace std::string_literals;

  auto handler = [this](const QList<QVariant>& runeVariantList) {
    if (runeVariantList.size() != 9)
    {
      return;
    }

    int retry = 10;
    while (retry--)
    {
      auto response = this->requestLCU->Get("/lol-perks/v1/pages"s);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        auto document = QJsonDocument::fromJson(response->body.c_str());
        auto runePageList = document.array();
        auto currentPageId = -1;
        if (!runePageList.isEmpty())
        {
          for (const auto& runePage : runePageList)
          {
            auto object = runePage.toObject();

            if (object["current"].toBool())
            {
              currentPageId = object["id"].toInt();
              break;
            }
          }

          if (currentPageId != -1)
          {
            this->requestLCU->Delete("/lol-perks/v1/pages/"s + std::to_string(currentPageId));
          }
        }

        QJsonObject body;
        body.insert("autoModifiedSelections", QJsonArray());
        body.insert("current", true);
        body.insert("name", "司天监-" + QDateTime::currentDateTime().toString("hh:mm:ss"));
        body.insert("order", 0);
        body.insert("primaryStyleId", this->runeIdDict[runeVariantList.at(0).toInt()]);
        body.insert("subStyleId", this->runeIdDict[runeVariantList.at(4).toInt()]);
        body.insert("selectedPerkIds", QJsonArray::fromVariantList(runeVariantList));

        retry = 10;
        while (retry--)
        {
          response = this->requestLCU->Post(
              "/lol-perks/v1/pages"s, QJsonDocument(body).toJson().toStdString(), "application/json"s);
          if (response && response->status == httplib::StatusCode::OK_200)
          {
            this->modifyLobbyMessage("符文已应用，祝您游戏愉快！");
            break;
          }
        }
        break;
      }
    }
  };
  this->threadPool->start(new Runnable(handler, runeIdList));
}

void LeagueAPI::modifySummonerRankedList(const QList<qint64>& summonerIds, const QList<QByteArray>& queueTypes)
{
  using namespace std::string_literals;

  auto handler = [this](const QList<qint64>& ids, const QList<QByteArray>& types) {
    std::string summonerIds;
    summonerIds.append("["s);
    for (const auto& summonerId : ids)
    {
      summonerIds.append(std::to_string(summonerId) + ","s);
    }
    summonerIds.pop_back();
    summonerIds.append("]"s);

    std::string queueTypes;
    queueTypes.append("["s);
    for (const auto& queueType : types)
    {
      auto type = queueType;
      type.insert(0, R"(")");
      type.append(R"(")");
      queueTypes.append(type.toStdString() + ","s);
    }
    queueTypes.pop_back();
    queueTypes.append("]"s);

    int retry = 10;
    while (retry--)
    {
      auto response = this->requestLCU->Get(
          "/lol-ranked/v2/tiers"s,
          httplib::Params{{"summonerIds"s, summonerIds}, {"queueTypes"s, queueTypes}},
          this->defaultHeaders);
      if (response && response->status == httplib::StatusCode::OK_200 && !response->body.empty())
      {
        auto document = QJsonDocument::fromJson(response->body.c_str());
        emit this->readyAccessSummonerRankedList(document);
        break;
      }
    }
  };
  this->threadPool->start(new Runnable(handler, summonerIds, queueTypes));
}

void LeagueAPI::deleteFriend(const QByteArray& id)
{
  using namespace std::string_literals;

  auto handler = [this, _id = id] {
    int retry = 10;
    while (retry--)
    {
      auto response = this->requestLCU->Delete("/lol-chat/v1/friends/"s + _id.toStdString());
      if (response && response->status == httplib::StatusCode::OK_200)
      {
        break;
      }
    }
  };
  this->threadPool->start(new Runnable(handler));
}

void LeagueAPI::friendRequest(const QByteArray& name)
{
  using namespace std::string_literals;

  auto handler = [this](const QByteArray& name) {
    auto pos = name.indexOf("#");
    auto gameName = QString(name.mid(0, pos));
    auto tagLine = QString(name.mid(pos + 1));

    QJsonObject body;
    body.insert("gameName", gameName);
    body.insert("tagLine", tagLine);

    int retry = 10;
    while (retry--)
    {
      auto response = this->requestLCU->Post(
          "/lol-chat/v2/friend-requests"s, QJsonDocument(body).toJson().toStdString(), "application/json"s);
      if (response && response->status == httplib::StatusCode::OK_200)
      {
        break;
      }
    }
  };
  this->threadPool->start(new Runnable(handler, name));
}

LeagueAPI* LeagueAPI::instance()
{
  static LeagueAPI _instance;
  return (&_instance);
}

QPixmap LeagueAPI::readRuneIcon(qint32 runeId)
{
  return {":/ResourceFiles/RuneIconFiles/" + QByteArray::number(runeId) + ".png"};
}

QPixmap LeagueAPI::readItemIcon(qint32 itemId)
{
  return {":/ResourceFiles/ItemIconFiles/" + QByteArray::number(itemId) + ".png"};
}

QPixmap LeagueAPI::readSpellIcon(qint32 spellId)
{
  return {":/ResourceFiles/SpellIconFiles/" + QByteArray::number(spellId) + ".png"};
}

QPixmap LeagueAPI::readChampionIcon(qint32 champId)
{
  return {":/ResourceFiles/ChampionIconFiles/" + QByteArray::number(champId) + ".png"};
}

QPixmap LeagueAPI::readProfileIcon(qint32 profileIconId)
{
  return {":/ResourceFiles/ProfileIconFiles/" + QByteArray::number(profileIconId) + ".png"};
}

QPixmap LeagueAPI::readOtherIcon(const QByteArray& name)
{
  return {":/ResourceFiles/OtherIconFiles/" + name + ".png"};
}

QPixmap LeagueAPI::readRankedIcon(const QByteArray& name)
{
  return {":/ResourceFiles/RankedIconFiles/" + name + ".png"};
}

QByteArray LeagueAPI::readLocalRegion()
{
  return this->regionDict[this->REGION];
}

QByteArray LeagueAPI::readGameInstallPath()
{
  return this->PATH;
}

QByteArray LeagueAPI::readQueueName(qint32 queueId)
{
  return this->queueDict[queueId];
}

QByteArray LeagueAPI::readChampionName(qint32 champId)
{
  return this->championDict[champId];
}

QByteArray LeagueAPI::readAvailability(const QByteArray& state)
{
  return this->availabilityDict[state];
}

QByteArray LeagueAPI::readPositionName(const QByteArray& pos)
{
  return this->positionDict[pos];
}

QByteArray LeagueAPI::readRankedShortName(const QByteArray& tier)
{
  return this->rankedShortDict[tier];
}

std::pair<QByteArray, QByteArray> LeagueAPI::readRankedPair(const QByteArray& name)
{
  for (const auto& rankedDivision : this->rankedDivisionList)
  {
    if (rankedDivision.first == name)
    {
      return rankedDivision.second;
    }
  }
  return {};
}

QList<std::pair<QByteArray, std::pair<QByteArray, QByteArray>>> LeagueAPI::readRankedDivisionList()
{
  return this->rankedDivisionList;
}

QByteArray LeagueAPI::readRankedName(const QByteArray& tier, const QByteArray& division)
{
  for (const auto& rankedDivision : this->rankedDivisionList)
  {
    auto ranked = rankedDivision.second;
    if (division.isEmpty() && ranked.first == tier)
    {
      return rankedDivision.first;
    }
    if (ranked.first == tier && ranked.second == division)
    {
      return rankedDivision.first;
    }
  }

  return {};
}

QByteArray LeagueAPI::readItemData(qint32 itemId, const QByteArray& version)
{
  auto data = this->itemDict[version];
  auto name = data[QString::number(itemId)].toObject()["name"].toString().toUtf8() + "\n";
  auto description = data[QString::number(itemId)].toObject()["description"].toString().toUtf8();

  description.replace("<br>", "\n");
  while (description.indexOf("<") != -1)
  {
    auto start = description.indexOf("<");
    auto end = description.indexOf(">") + 1;
    description = description.remove(start, end - start);
  }

  return {name + description};
}

QByteArray LeagueAPI::readRuneData(qint32 runeId, const QByteArray& version)
{
  for (const auto& rune : this->runeDict[version])
  {
    auto slotList = rune.toObject()["slots"].toArray();
    for (const auto& slot : slotList)
    {
      auto runes = slot.toObject()["runes"].toArray();
      for (const auto result : runes)
      {
        auto object = result.toObject();
        if (object["id"].toInt() == runeId)
        {
          auto name = object["name"].toString().toUtf8() + "\n";
          auto longDesc = object["longDesc"].toString().toUtf8();
          longDesc.replace("<br>", "\n");
          while (longDesc.indexOf("<") != -1)
          {
            auto start = longDesc.indexOf("<");
            auto end = longDesc.indexOf(">") + 1;
            longDesc = longDesc.remove(start, end - start);
          }
          while (longDesc.indexOf("@") != -1)
          {
            auto start = longDesc.indexOf("@");
            auto end = longDesc.indexOf("@", start + 1) + 1;
            longDesc = longDesc.remove(start, end - start);
          }
          return {name + longDesc};
        }
      }
    }
  }
  return {};
}
