import re
import json
import decimal
import itertools

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.lottery.game.pickthree import PickThreeGame
from yoolotto.lottery.models import LotteryGameComponent
from yoolotto.util.serialize import dumps
from yoolotto.util import currency

class DailyFourGame(PickThreeGame):
    NAME = "DailyFour"
    
    ENUM = {
        1: "SUM",
        2: "STR",
        3: "BOX",
        4: "STRBOX",
        5: "FRONT",
        6: "MID",
        7: "BACK",
        9: "COMBO"
    }
    
    BALLS = 4
    
    SUMITUP = {
        0: {"0.50": 2500, "1.00": 5000, "2.00": 10000, "3.00": 15000, "4.00": 20000, "5.00": 25000},
        1: {"0.50": 625, "1.00": 1250, "2.00": 2500, "3.00": 3750, "4.00": 5000, "5.00": 6250},
        2: {"0.50": 250, "1.00": 500, "2.00": 1000, "3.00": 1500, "4.00": 2000, "5.00": 2500},
        3: {"0.50": 125, "1.00": 250, "2.00": 500, "3.00": 750, "4.00": 1000, "5.00": 1250},
        4: {"0.50": 71, "1.00": 142, "2.00": 285, "3.00": 428, "4.00": 571, "5.00": 714},
        5: {"0.50": 44, "1.00": 89, "2.00": 178, "3.00": 267, "4.00": 357, "5.00": 446},
        6: {"0.50": 29, "1.00": 59, "2.00": 119, "3.00": 178, "4.00": 238, "5.00": 297},
        7: {"0.50": 20, "1.00": 41, "2.00": 83, "3.00": 125, "4.00": 166, "5.00": 208},
        8: {"0.50": 15, "1.00": 30, "2.00": 60, "3.00": 90, "4.00": 121, "5.00": 151},
        9: {"0.50": 11, "1.00": 22, "2.00": 45, "3.00": 68, "4.00": 90, "5.00": 113},
        10: {"0.50": 8, "1.00": 17, "2.00": 35, "3.00": 53, "4.00": 70, "5.00": 88},
        11: {"0.50": 7, "1.00": 14, "2.00": 28, "3.00": 43, "4.00": 57, "5.00": 71},
        12: {"0.50": 6, "1.00": 12, "2.00": 24, "3.00": 36, "4.00": 48, "5.00": 60},
        13: {"0.50": 5, "1.00": 10, "2.00": 20, "3.00": 31, "4.00": 41, "5.00": 52},
        14: {"0.50": 4, "1.00": 9, "2.00": 18, "3.00": 27, "4.00": 37, "5.00": 46},
        15: {"0.50": 4, "1.00": 8, "2.00": 16, "3.00": 25, "4.00": 33, "5.00": 42},
        16: {"0.50": 3, "1.00": 7, "2.00": 15, "3.00": 23, "4.00": 31, "5.00": 39},
        17: {"0.50": 3, "1.00": 7, "2.00": 15, "3.00": 22, "4.00": 30, "5.00": 37},
        18: {"0.50": 3, "1.00": 7, "2.00": 14, "3.00": 22, "4.00": 29, "5.00": 37},
    }
        
    @classmethod
    def _earnings(cls, results, play, _type="ALL"):
        total = 0
        mod = decimal.Decimal(play["baseWager"]) / decimal.Decimal("0.50")
        if _type in ["ALL", "SUM"]:   
            if "sum" in play and play["sum"] is not None:
                if sum(results) == sum([x for x in play["numbers"] if x is not None]):
                    _index = sum(results)
                    _index = 36 - _index if _index > 18 else _index
                    total += cls.SUMITUP[_index][play["sumWager"]]
        
        if _type in ["ALL", "BASE"]:
            if play["lineType"] == "STR":
                if results == play["numbers"]:
                    base = 2500
                    total += base * mod
                    
            elif play["lineType"] in ["FRONT", "MID", "BACK"]:            
                if play["lineType"] == "FRONT" and results[0:2] == play["numbers"][0:2] \
                    or play["lineType"] == "MID" and results[1:3] == play["numbers"][1:3] \
                    or play["lineType"] == "BACK" and results[2:4] == play["numbers"][2:4]:
                    total += 25 * mod
                    
            else:
                _numbers = set(play["numbers"])
                ways = len(set(itertools.permutations(play["numbers"])))
                if ways == 1:
                    ways = 6  
                if play["lineType"] == "BOX":
                    base = 2400 / ways
                    
                    if cls._match(play["numbers"], results):
                        total += base * mod
                                
                elif play["lineType"] == "STRBOX":
                    base = 2400 / ways
                    
                    # BOX
                    if cls._match(play["numbers"], results):
                        total += base * mod
                        
                    # STR
                    if results == play["numbers"]:
                        total += 2500 * mod
                
                elif play["lineType"] == "COMBO":                
                    for permutation in set(itertools.permutations(play["numbers"])):
                        if list(permutation) == results:
                            total += 2500 * mod
                           
                        if ways == 6 and mod == 6:
                            total = 2500
                        if ways == 6 and mod == 12:
                            total = 5000 
                else:
                    raise NotImplementedError()
            
        return total
    
    @classmethod
    def get_component(cls, record):
        print 'component'
        if record["drawTime"] == 6:
            return LotteryGameComponent.objects.get(identifier="DailyFourDay")

        if "drawTime" not in record or record["drawTime"] not in [0, 1, 2, 3]:
            raise LotteryPlayInvalidException("Unknown Draw Time")
        
        if record["drawTime"] == 0:
            return LotteryGameComponent.objects.get(identifier="DailyFourDay")
        elif record["drawTime"] == 2:
            return LotteryGameComponent.objects.get(identifier="DailyFourMorning")
        elif record["drawTime"] == 3:
            return LotteryGameComponent.objects.get(identifier="DailyFourEvening")
        else:
            return LotteryGameComponent.objects.get(identifier="DailyFourNight")
        
    @classmethod
    def get_game_meta(cls, component):
        print 'meta'
        if component.identifier == "DailyFourDay":
            return {"drawTime": 0}
        
        if component.identifier == "DailyFourMorning":
            return {"drawTime": 2}
        
        if component.identifier == "DailyFourEvening":
            return {"drawTime": 3}
        
        return {"drawTime": 1}

    @classmethod
    def get_next_components(cls,component):
        day_component = LotteryGameComponent.objects.get(identifier="DailyFourDay")
        morning_component = LotteryGameComponent.objects.get(identifier="DailyFourMorning")
        evening_component = LotteryGameComponent.objects.get(identifier="DailyFourEvening")
        night_component = LotteryGameComponent.objects.get(identifier="DailyFourNight")

        order = [morning_component,day_component,evening_component,night_component]
        index = order.index(component)
        draws_list = [order[item] for item in range(len(order)) if item >= index]
        return draws_list

    @classmethod
    def get_previous_components(cls,component):
        day_component = LotteryGameComponent.objects.get(identifier="DailyFourDay")
        morning_component = LotteryGameComponent.objects.get(identifier="DailyFourMorning")
        evening_component = LotteryGameComponent.objects.get(identifier="DailyFourEvening")
        night_component = LotteryGameComponent.objects.get(identifier="DailyFourNight")

        order = [morning_component,day_component,evening_component,night_component]
        index = order.index(component)
        draws_list = [order[item] for item in range(len(order)) if item <= index]
        return draws_list

    @classmethod
    def get_all_components(cls):
        day_component = LotteryGameComponent.objects.get(identifier="DailyFourDay")
        morning_component = LotteryGameComponent.objects.get(identifier="DailyFourMorning")
        evening_component = LotteryGameComponent.objects.get(identifier="DailyFourEvening")
        night_component = LotteryGameComponent.objects.get(identifier="DailyFourNight")

        draws_list = [morning_component,day_component,evening_component,night_component]
        return draws_list
    
from yoolotto.lottery.game.manager import GameManager
GameManager.register(DailyFourGame)
