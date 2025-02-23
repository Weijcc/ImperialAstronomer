import sys, os, json, shutil, requests

from PySide6.QtGui import QPixmap, QPainter, QPainterPath
from PySide6.QtCore import Qt, QRect, QSize, QTimer
from PySide6.QtWidgets import QApplication

version = '15.4.1'
dragon_path = 'C:/Users/leopa/Desktop/15.4.1/' + version

ver_json = requests.get('https://ddragon.leagueoflegends.com/api/versions.json').json()[0:10]

def crop_circle(path: str, size: QSize, radius: bool):
    pixmap = QPixmap(path)

    tmp_pixmap = pixmap.scaled(
        size.width(), size.height(),
        Qt.AspectRatioMode.KeepAspectRatio, Qt.TransformationMode.SmoothTransformation)

    res_pixmap = QPixmap(size)
    res_pixmap.fill(Qt.GlobalColor.transparent)

    painter = QPainter(res_pixmap)
    painter.setRenderHint(QPainter.RenderHint.Antialiasing, True)
    painter.setRenderHint(QPainter.RenderHint.SmoothPixmapTransform, True)

    painter_path = QPainterPath()
    rect = QRect(0, 0, size.width(), size.height())

    if radius:
        painter_path.addRoundedRect(rect, size.width() / 2, size.height() / 2)
    else:
        painter_path.addRect(rect)
    painter.setClipPath(painter_path)
    painter.drawPixmap(0, 0, tmp_pixmap)
    return res_pixmap


def handler_runes_icon():
    output_dir = './ResourceFiles/PerkIconFiles'

    stat_mods_dict = {'StatModsAdaptiveForceIcon': '5008',
                      'StatModsAttackSpeedIcon': '5005',
                      'StatModsCDRScalingIcon': '5007',
                      'StatModsHealthPlusIcon': '5011',
                      'StatModsHealthScalingIcon': '5001',
                      'StatModsMovementSpeedIcon': '5010',
                      'StatModsTenacityIcon': '5013'}

    os.makedirs(output_dir)

    with open(dragon_path + '/data/zh_CN/runesReforged.json', 'r', encoding='utf-8') as file:
        json_data = json.load(file)

    for data in json_data:
        for slot in data['slots']:
            for rune in slot['runes']:
                id = rune['id']
                icon = rune['icon']
                pixmap = crop_circle(dragon_path + '/../img/' + icon, QSize(32, 32), False)
                pixmap.save(output_dir + '/' + str(id) + '.png')
                print(dragon_path + '/../img/' + icon, output_dir + '/' + str(id) + '.png')

    stat_mods_icons = os.listdir(dragon_path + '/../img/perk-images/StatMods')
    for stat_icon in stat_mods_icons:
        id = stat_mods_dict.get(stat_icon.replace('.png', ''), '')
        if len(id) != 0:
            pixmap = crop_circle(dragon_path + '/../img/perk-images/StatMods/' + stat_icon, QSize(32, 32), False)
            pixmap.save(output_dir + '/' + id + '.png')
            print(dragon_path + '/../img/perk-images/StatMods/' + stat_icon, output_dir + '/' + id + '.png')


def handler_profile_icon():
    output_dir = './ResourceFiles/ProfileIconFiles'
    os.makedirs(output_dir)

    profile_icons = os.listdir(dragon_path + '/img/profileicon')
    for profile_icon in profile_icons:
        pixmap = crop_circle(dragon_path + '/img/profileicon' + '/' + profile_icon, QSize(64, 64), True)
        pixmap.save(output_dir + '/' + profile_icon)
        print(dragon_path + '/img/profileicon' + '/' + profile_icon, output_dir + '/' + profile_icon)


def handler_item_icon():
    output_dir = './ResourceFiles/ItemIconFiles'
    os.makedirs(output_dir)

    pixmap = crop_circle(dragon_path + '/img/spell/Summoner_UltBookPlaceholder.png', QSize(64, 64), False)
    pixmap.save(output_dir + '/0.png')
    item_icons = os.listdir(dragon_path + '/img/item')
    for item_icon in item_icons:
        pixmap = crop_circle(dragon_path + '/img/item' + '/' + item_icon, QSize(64, 64), False)
        pixmap.save(output_dir + '/' + item_icon)
        print(dragon_path + '/img/item' + '/' + item_icon, output_dir + '/' + item_icon)

def handler_champion_icon():
    output_dir = './ResourceFiles/ChampionIconFiles'
    os.makedirs(output_dir)

    with open(dragon_path + '/data/zh_CN/champion.json', 'r', encoding='utf-8') as file:
        json_data = json.load(file)

    champion_icons = os.listdir(dragon_path + '/img/champion')

    data = json_data['data']
    champion_dict = dict()
    for item in data:
        id = str(data[item]['id'])
        key = str(data[item]['key'])
        champion_dict[id.lower()] = key

    for champion_icon in champion_icons:
        key = champion_icon.replace('.png', '').lower()
        pixmap = crop_circle(dragon_path + '/img/champion/' + champion_icon, QSize(64, 64), False)
        pixmap.save(output_dir + '/' + champion_dict.get(key) + '.png')
        print(dragon_path + '/img/champion/' + champion_icon, output_dir + '/' + champion_dict.get(key) + '.png')


def handler_spell_icons():
    output_dir = './ResourceFiles/SpellIconFiles'
    os.makedirs(output_dir)

    spell_dict = {'Summoner_UltBookPlaceholder': '54',
                  'Summoner_UltBookSmitePlaceholder': '55',
                  'SummonerBarrier': '21',
                  'SummonerBoost': '1',
                  'SummonerCherryFlash': '2202',
                  'SummonerCherryHold': '2201',
                  'SummonerDot': '14',
                  'SummonerExhaust': '3',
                  'SummonerFlash': '4',
                  'SummonerHaste': '6',
                  'SummonerHeal': '7',
                  'SummonerMana': '13',
                  'SummonerPoroRecall': '30',
                  'SummonerPoroThrow': '31',
                  'SummonerSmite': '11',
                  'SummonerSnowball': '32',
                  'SummonerSnowURFSnowball_Mark': '39',
                  'SummonerTeleport': '12'}

    spell_icons = os.listdir(dragon_path + '/img/spell')
    for spell_icon in spell_icons:
        pixmap = crop_circle(dragon_path + '/img/spell' + '/' + spell_icon, QSize(32, 32), False)
        id = spell_dict.get(spell_icon.replace('.png', ''))
        if id != None:
            pixmap.save(output_dir + '/' + id + '.png')
            print(dragon_path + '/img/item' + '/' + spell_icon, output_dir + '/' + id + '.png')

def handler_data_file():
    os.makedirs('./ResourceFiles/DataFiles')

    shutil.copy(dragon_path + '/data/zh_CN/champion.json', './ResourceFiles/DataFiles/champion.json')
    print(dragon_path + '/data/zh_CN/champion.json', './ResourceFiles/DataFiles/champion.json')

    for ver in ver_json:
        idx = str(ver).rfind('.')
        item_json = requests.get('https://ddragon.leagueoflegends.com/cdn/' + ver + '/data/zh_CN/item.json').json()
        rune_json = requests.get('https://ddragon.leagueoflegends.com/cdn/' + ver + '/data/zh_CN/runesReforged.json').json()
        print('https://ddragon.leagueoflegends.com/cdn/' + ver + '/data/zh_CN/item.json', './ResourceFiles/DataFiles/item' + ver[0:idx] + '.json')
        print('https://ddragon.leagueoflegends.com/cdn/' + ver + '/data/zh_CN/runesReforged.json', './ResourceFiles/DataFiles/runesReforged' + ver[0:idx] + '.json')

        with open('./ResourceFiles/DataFiles/item' + ver[0:idx] + '.json', "w", encoding="utf-8") as file:
            file.write(json.dumps(item_json, indent=4, ensure_ascii=False))

        with open('./ResourceFiles/DataFiles/runesReforged' + ver[0:idx] + '.json', "w", encoding="utf-8") as file:
            file.write(json.dumps(rune_json, indent=4, ensure_ascii=False))

    result_list = list()
    champion_list = os.listdir(dragon_path + '/data/zh_CN/champion')
    for champion in champion_list:
        print(dragon_path + '/data/zh_CN/champion/' + champion)
        with open(dragon_path + '/data/zh_CN/champion/' + champion, 'r', encoding='utf-8') as file:
            data = json.load(file)['data']
            for key in data:
                name = data[key]['name']
                skins = data[key]['skins']

                for idx in range(len(skins)):
                    if skins[idx]['name'] == 'default':
                        skins[idx]['name'] = name

                    num = skins[idx]['num']
                    file = key + '_' + str(num) + '.png'
                    skins[idx]['file'] = file
                result_list += skins

    with open('./ResourceFiles/DataFiles/skins.json', "w", encoding="utf-8") as file:
        file.write(json.dumps(result_list, indent=4, ensure_ascii=False))

def handler_skin_img():
    os.makedirs('./ResourceFiles/SkinImageFiles')
    centereds = os.listdir(dragon_path + '/../img/champion/centered')

    for centered in centereds:
        pixmap = crop_circle(dragon_path + '/../img/champion/centered/' + centered, QSize(256, 144), False)
        new_file = centered.replace('.jpg', '.png')
        pixmap.save('./ResourceFiles/SkinImageFiles/' + new_file)
        print(new_file, 'saved')

def start():
    handler_runes_icon()
    handler_profile_icon()
    handler_item_icon()
    handler_champion_icon()
    handler_spell_icons()
    handler_data_file()
    handler_skin_img()
    print('All Finished')
    QApplication.exit(0)

if __name__ == "__main__":
    app = QApplication(sys.argv)

    if os.path.exists('./ResourceFiles'):
        shutil.rmtree('./ResourceFiles')

    print('wait app exec')
    QTimer.singleShot(500, start)

    app.exec()
