import os
import datetime
from xml.etree import ElementTree
import ast
from django.conf import settings
from ast import literal_eval
from yoolotto.lottery.game.manager import GameManager
from yoolotto.lottery.models import LotteryCountryDivision, LotteryGame, LotteryGameComponent, LotteryDraw
from yoolotto.lottery.tasks import notify_draw_result_all, notify_frenzy_all

class LotteryTicketImporter(object):
    
    """
    lotteryGameCode: {
        componentName: componentFormat
    }
    
    """
    GAMES = {
        "Powerball": {
            "game": "Powerball",
            "components": {
                "Powerball": ["Powerball", "Powerball"]
            }
        },
        "MegaMillions": {
            "game": "Mega Millions",
            "components": {
                "MEGA Millions": ["MegaMillions", "MegaMillions"],
                "Megaplier": ["Megaplier", "Megaplier"]
            }
        },
        "Lotto": {
            "game": "Lotto Texas",
            "components": {
                "Lotto": ["Lotto", "LottoTexas"]
            }
        },
        "DailyFour": {
            "game": "Daily Four",
            "components": {
                "Daily 4 Day": ["DailyFour", "DailyFourDay"],
                "Daily 4 Night": ["DailyFour", "DailyFourNight"],
                "Daily 4 Morning": ["DailyFour", "DailyFourMorning"],
                "Daily 4 Evening": ["DailyFour", "DailyFourEvening"]
            }
        },
        "PickThree": {
            "game": "Pick Three",
            "components": {
                "Pick 3 Day": ["PickThree", "PickThreeDay"],
                "Pick 3 Night": ["PickThree", "PickThreeNight"],
                "Pick 3 Morning":  ["PickThree", "PickThreeMorning"],
                "Pick 3 Evening": ["PickThree", "PickThreeEvening"]
            }
        },
        "CashFive": {
            "game": "Cash Five",
            "components": {
                "Cash 5": ["CashFive", "CashFive"]
            }
        },
        "TwoStep": {
            "game": "Two Step",
            "components": {
                "Two Step": ["TwoStep", "TwoStep"]
            }
        },
        "AllOrNothing": {
            "game": "All or Nothing",
            "components": {
                "All or Nothing Day": ["AllOrNothing", "AllOrNothingDay"],
                "All or Nothing Night": ["AllOrNothing", "AllOrNothingNight"],
                "All or Nothing Morning":  ["AllOrNothing", "AllOrNothingMorning"],
                "All or Nothing Evening": ["AllOrNothing", "AllOrNothingEvening"]
            }
        },
    }
    
    def __init__(self, provider):
        self._provider = provider
        
    def _filter_game(self, name):
        for k, v in self.GAMES.iteritems():
            for _name, format in v["components"].iteritems():
                if _name == name:
                    return {
                        "game_name": v["game"], # Verbose Game Name
                        "comp_name": _name,     # Verbose Component Name (e.g. game_name in data)
                        "game_format": k,       # Game Code
                        "comp_format": format[0],   # Component Name
                        "comp_identifier": format[1]
                    }
            
        return None
        
    def run(self):
        # Debug / Logging of Imports
        if not os.path.exists(settings.IMPORT_LOG_PATH):
            os.mkdir(settings.IMPORT_LOG_PATH)
            
        path = os.path.join(settings.IMPORT_LOG_PATH, datetime.datetime.now().strftime("%Y-%m-%dT%H-%M-%S"))
        if not os.path.exists(path):
            os.mkdir(path)
            
        file_dump = os.path.join(path, "import.xml")
        file_log = os.path.join(path, "import.log")
                
        data = self._provider()
        
        fp_dump = open(file_dump, "w")
        fp_log = open(file_log, "w")
        
        fp_dump.write(data)
        fp_dump.close()
        
        # Start Import
        xml = ElementTree.fromstring(data)
        
        for state in xml.findall("StateProv"):
            if state.get("stateprov_name") != "Texas":
                continue
                        
            division, _ = LotteryCountryDivision.objects.get_or_create(
                name=state.get("stateprov_name"), remote_id=state.get("stateprov_id"), \
                remote_country=state.get("country"))
            division.save()
                            
            for _game in state.findall("game"):
                _meta = self._filter_game(_game.get("game_name"))
                if not _meta:
                    continue
                
                fp_log.write("Found Game %s\n" % _meta)
                
                # game, _ = LotteryGame.objects.get_or_create(name=_meta["game_name"], code=_meta["game_format"])
                                
                # component, _ = LotteryGameComponent.objects.get_or_create(remote_id=_game.get("game_id"), name=_meta["comp_name"])
                game, _ = LotteryGame.objects.get_or_create(
                            name=_meta["game_name"],
                            code=_meta["game_format"]
                            ) 
                component, _ = LotteryGameComponent.objects.get_or_create(
                                remote_id=_game.get("game_id"),
                                name=_meta["comp_name"],
                                format=_meta["comp_format"]
                                )
                component.parent = game
                component.division.add(division)
                component.format = _meta["comp_format"]
                component.identifier = _meta["comp_identifier"]
                component.save()
                
                handler = GameManager.get(_meta["game_format"])
                fp_log.write("\tUsing Handler %s\n" % handler)
                
                # Load Jackpot
                jackpot = _game.find("jackpot")
                jackpot_date = None
                jackpot_prize = None
                                
                if jackpot is not None:
                    jackpot_date = datetime.datetime.strptime(jackpot.get("date"), "%m/%d/%Y").date()
                    jackpot_prize = int(jackpot.text)
                    fp_log.write("\t[jackpot] Found Jackpot %s\n" % jackpot_prize)
                    fp_log.write("\t[jackpot] Jackpot Date %s\n" % jackpot_date)
                else:
                    fp_log.write("\t[jackpot] No Jackpot\n")
                
                # Load Last Draw
                last_draw_date = _game.find("lastdraw_date")
                last_draw_numbers = _game.find("lastdraw_numbers")
                
                fp_log.write("\n\t[last] last_draw_date %s\n" % last_draw_date.text if last_draw_date is not None else None )
                fp_log.write("\t[last] last_draw_numbers %s\n" % last_draw_numbers.text if last_draw_numbers is not None else None)
                
                if last_draw_date is not None and last_draw_numbers is not None:
                    last_draw_date = datetime.datetime.strptime(last_draw_date.text, "%m/%d/%Y").date()
                    fp_log.write("\t[last] last_draw_date %s\n" % last_draw_date)
                    
                    if _meta["game_name"] in ["All or Nothing"] and not jackpot:
                        jackpot_prize = 250000
                        jackpot_date = last_draw_date
                    
                    _notification = False
                    
                    # Find Draw Object
                    try:
                        draw, created = \
                        LotteryDraw.objects.get_or_create(component=component,
                                                         date=last_draw_date,
                                                         division_id=1)
                    except Exception as e:
                        print e
                    if not draw.result:
                        _notification = True
                        
                    draw.result = handler.decode(last_draw_numbers.text, format=_meta["comp_format"])
                    if _meta["game_name"] == "Powerball":
                        if len(literal_eval(draw.result)) != 7:
                            raise ValueError("Unexpected Result Value: %s" % draw.result)
                        else:
                            draw.powerplay = literal_eval(draw.result)[-1]
                    
                    fp_log.write("\t[last] draw id %s, created: %s \n" % (draw.pk, created))
                    
                    # If the draw object was created OR the draw became official, 
                    # fire a notification for Draw Results
                    if created or not draw.official:
                        _notification = True
                    
                    draw.official = True
                    
                    if jackpot_prize is not None and jackpot_date == last_draw_date:
                        draw.jackpot = jackpot_prize
                    
                    draw.save()
                    print "Draw ID %s of state %s" % (draw.id, draw.division.remote_id)
                    fp_log.write("\t[last] draw id %s SAVED\n" % draw.pk)
                    fp_log.write("\t[last] %s\n" % draw.representation())
                    fp_log.write("\t[last] Firing Notification: %s\n" % _notification)
                    
                    if _meta["game_name"] == "Powerball" and len(ast.literal_eval(draw.result)) == 7:
                        # For last_draw_date

                        powerplay = ast.literal_eval(draw.result)[6:7]
                        fp_log.write("\t[last] Found Powerplay %s \n" % powerplay)
                        # first create a fake powerplay game component
                        fp_log.write("\t[last] Creating Fake Powerplay Component with value %s \n" % powerplay)
                        powerplay_component, _ = LotteryGameComponent.objects.get_or_create(
                                        remote_id=_game.get("game_id"),
                                        name="Powerplay"
                                        )
                        powerplay_component.parent = game
                        powerplay_component.division.add(division)
                        powerplay_component.format = "Powerplay"
                        powerplay_component.identifier = "Powerplay"
                        powerplay_component.save()
                        fp_log.write("\t[last] Fake Powerplay Object Saved %s \n" % powerplay_component.id)
                        # creating draw object for Powerplay
                        fp_log.write("\t[last] Creating Powerplay Draw \n")
                        powerplay_draw, powerplay_created = LotteryDraw.objects.get_or_create(
                                                component=powerplay_component,
                                                date=last_draw_date,
                                                division_id=1,
                                                result=powerplay
                                                )
                        # If the draw object was created OR the draw became official, 
                        # fire a notification for Draw Results
                        if powerplay_created or not powerplay_draw.official:
                            _notification = True

                        powerplay_draw.official = True
                        powerplay_draw.save()
                        fp_log.write("\t[last] Powerplay Object Saved %s \n" % powerplay_draw.id)
                        # Finally Storing the draw result without powerplay
                        draw.result = str(ast.literal_eval(draw.result)[0:6])
                        draw.save()

                    if _notification:
                        notify_draw_result_all.apply_async(countdown=0, 
                            kwargs={"draw_id": draw.pk})
                
                # Load Current Draw
                next_draw_date = _game.find("nextdraw_date")
                
                fp_log.write("\n\t[next] Processing NEXT Draw\n")
                fp_log.write("\t[next] next_draw_date %s\n" % next_draw_date.text if next_draw_date is not None else None)
                
                if next_draw_date is not None:
                    next_draw_date = datetime.datetime.strptime(next_draw_date.text, "%m/%d/%Y").date()
                    fp_log.write("\t[next] next_draw_date %s\n" % next_draw_date)
                    
                    _notification = False
                    
                    # Find Draw Object
                    draw, created = LotteryDraw.objects.get_or_create(component=component,
                                                                     date=next_draw_date,
                                                                     division_id=1)
                    
                    # If the draw object was created OR the draw became official, 
                    # fire a notification for Draw Results
                    if created or not draw.official:
                        _notification = True
                                   
                    draw.official = True
                    
                    fp_log.write("\t[next] draw id %s, created: %s \n" % (draw.pk, created))
                    
                    if jackpot_date == next_draw_date:
                        draw.jackpot = jackpot_prize
                        fp_log.write("\t[next] draw prize %s\n" % jackpot_prize)
                    
#                    fp_log.write("\t[last] Firing Notification: %s\n" % (draw.jackpot and _notification,))
#                    if draw.jackpot and _notification:
#                        notify_frenzy_all.apply_async(countdown=300, 
#                            kwargs={"draw_id": draw.pk})
                    
                    fp_log.write("\t[next] %s" % draw.representation())
                    draw.save()
                    
                fp_log.write("\n\n-----------------------------------------\n\n")
                
        # Close Logger
        fp_log.close()