import os
import csv
import datetime
import pprint
import json

from django.conf import settings
from yoolotto.lottery.game.manager import GameManager
from yoolotto.lottery.models import LotteryCountryDivision, LotteryGame, LotteryGameComponent, LotteryDraw

class LotteryTicketHistoryImporter(object):
    FILTER = ["101", "113", "141", "TX1", "TX2", "TX3", "TXB", "TXC", "TXD", "TXA", "TXJ", "TXK", "TXL", "TXM"]
    
    GAMES = {
        "101": {
            "game": "Powerball",
            "components": {
                "Powerball": ["Powerball", "Powerball"]
            }
        },
        "113": {
            "game": "Mega Millions",
            "components": {
                "MEGA Millions": ["MegaMillions", "MegaMillions"]
            }
        },
        "141": {
            "game": "Mega Millions",
            "components": {
                "Megaplier": ["Megaplier", "Megaplier"]
            }
        },
        "TX1": {
            "game": "Lotto Texas",
            "components": {
                "Lotto": ["Lotto", "LottoTexas"]
            }
        },
        "TXB": {
            "game": "Daily Four",
            "components": {
                "Daily 4 Day": ["DailyFour", "DailyFourDay"],
            }
        },
        "TXD": {
            "game": "Daily Four",
            "components": {
                "Daily 4 Night": ["DailyFour", "DailyFourNight"]
            }
        },
        "TXL": {
            "game": "Daily Four",
            "components": {
                "Daily 4 Morning": ["DailyFour", "DailyFourMorning"]
            }
        },
        "TXM": {
            "game": "Daily Four",
            "components": {
                "Daily 4 Evening": ["DailyFour", "DailyFourEvening"]
            }
        },
        "TXC": {
            "game": "Pick Three",
            "components": {
                "Pick 3 Day": ["PickThree", "PickThreeDay"],
            }
        },
        "TXA": {
            "game": "Pick Three",
            "components": {
                "Pick 3 Night": ["PickThree", "PickThreeNight"]
            }
        },
        "TXJ": {
            "game": "Pick Three",
            "components": {
                "Pick 3 Morning": ["PickThree", "PickThreeMorning"]
            }
        },
        "TXK": {
            "game": "Pick Three",
            "components": {
                "Pick 3 Evening": ["PickThree", "PickThreeEvening"]
            }
        },
        "TX2": {
            "game": "Cash Five",
            "components": {
                "Cash 5": ["CashFive", "CashFive"]
            }
        },
        "TX3": {
            "game": "Two Step",
            "components": {
                "Two Step": ["TwoStep", "TwoStep"]
            }
        },
    }
    
    def __init__(self):
        pass
    
    def run(self):
        files = []
        
        for item in os.listdir(os.path.join(settings.SITE_ROOT, "lottery/data/history")):
            _path = os.path.join(settings.SITE_ROOT, "lottery/data/history", item)
            files.append(_path)
            
        data = []
        
        handlers = {}
        components = {}
            
        for file in files:
            fp = open(file, "r")
            reader = csv.reader(fp)
            
            for row in reader:
                if row[0] not in self.GAMES.keys():
                    continue
                
                _meta = self.GAMES[row[0]]["components"].values()[0]

                if _meta[0] not in handlers:
                    handlers[_meta[0]] = GameManager.get(_meta[0])
                
                if _meta[1] not in components:
                    components[_meta[1]] = LotteryGameComponent.objects.get(identifier=_meta[1])
                
                component = components[_meta[1]]
                handler = handlers[_meta[0]]
                   
                result = map(int, row[2:])
                parsed = handler.decode("-".join(map(str, result)), _meta[0])
                
                if _meta[0] != "Megaplier":
                    handler.validate_numbers(result)          
                   
                _data = {
                    "format": _meta[0],
                    "component": component,
                    "handler": handler,
                    "date": datetime.datetime.strptime(row[1], "%Y%m%d"),
                    "raw": result,
                    "result": parsed,
                }
                   
                data.append(_data)
                
            fp.close()
        
        # Validate no Conflicts
        for row in data:
            try:
                draw = LotteryDraw.objects.get(component=row["component"], date=row["date"])
            except LotteryDraw.DoesNotExist:
                continue
            
            if draw and draw.result != None and draw.result != row["result"]:
                if set(json.loads(draw.result)) != set(json.loads(row["result"])):
                    pprint.pprint(row)
                    pprint.pprint(draw.__dict__)
                    raise RuntimeError("Data Mismatch!")
                else:
                    pass
            elif draw.result != None and draw.result == row["result"]:
                pass
        
        # Insert Data
        for row in data:
            try:
                draw = LotteryDraw.objects.get(component=row["component"], date=row["date"])
                
                if draw.result and draw.official:
                    continue
            except LotteryDraw.DoesNotExist:
                draw = LotteryDraw(component=row["component"], date=row["date"])
                
            draw.official = True
            draw.result = row["result"]
            draw.save()
            
            
            
        
       # pprint.pprint(components)
       # pprint.pprint(handlers)