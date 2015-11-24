import re
import json
import decimal
import itertools
from django.shortcuts import get_object_or_404

from yoolotto.lottery.game.base import LotteryGame
from yoolotto.lottery.game.base import LotteryPlayInvalidException
from yoolotto.lottery.game.base import LotteryResultsInvalidException
from yoolotto.lottery.game.pickthree import PickThreeGame
from yoolotto.lottery.models import LotteryGameComponent
from yoolotto.lottery.models import LotteryDraw
from yoolotto.util.serialize import dumps
from yoolotto.util import currency

class CADailyFourGame(LotteryGame):
    NAME = "DailyFor"
    JACKPOT = "J"
    FORMATS = ["DailyFor"] 

    STRAIGHT = "STRAIGHT"
    BOX = "BOX"
    STRAIGHT_AND_BOX = "STRAIGHT_AND_BOX"
    BOX_ONLY = "BOX_ONLY"
    ENUM = {
        #1: "SUM",
        2: "EXT",
        3: "ANY",
        4: "EXTANY",
        #5: "FRONT",
        #6: "MID",
        #7: "BACK",
        #9: "COMBO"
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
    def decode(cls, raw, format=None):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if len(result) != cls.BALLS:
            raise ValueError("Unexpected Decode Value: %s" % result)
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format=None):
        values = json.loads(encoded)
        
        if len(values) != cls.BALLS:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values)

    @classmethod
    def validate_numbers(cls, numbers):
        if len(numbers) != cls.BALLS:
            raise LotteryResultsInvalidException(
                "Expected %s Numbers, Received %s" % (cls.BALLS, len(numbers)))
        
        if -1 in numbers :
            validate_numbers = [number for number in numbers if number != -1]
            for number in validate_numbers:
                if number not in xrange(0, 10) and number is not None:
                    raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)

        else:
            for number in numbers:
                if number not in xrange(0, 10) and number is not None:
                    raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
            
        return True

    @classmethod
    def _match(cls, candidate, target):
        """
        Match in any order
        Very naive implementation
        
        """
        for x in xrange(10):
            if len(filter(lambda y: y == x, candidate)) != len(filter(lambda y: y == x, target)):
                return False
            
        return True

    # @classmethod
    # def earnings(cls, play):
    #     return [
    #         cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play)),
    #         cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play), _type="BASE"),
    #         # cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play), _type="SUM")
    #     ]

    
    @classmethod
    def _actual_earnings(cls,play, data_list):
        draw = LotteryDraw.objects.get(id=play.ticket.draw.id)
        if 'BOX' in data_list:
            return [draw.box, draw.box]
        elif 'STRAIGHT' in data_list:
            return [draw.straight, draw.straight]
        elif 'STRAIGHT_AND_BOX' in data_list:
            return [draw.staright_and_box, draw.staright_and_box]
        elif 'BOX_ONLY' in data_list:
            return [draw.box_only, draw.box_only]

    @classmethod
    def earnings(cls, play):
        return cls._actual_earnings(play,[
            cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play)),
            cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play), _type="BASE")
            #cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play), _type="SUM")
        ])
    
    @classmethod
    def _earnings(cls, results, play, _type="ALL"):
        
        total = 0
        mod = decimal.Decimal(play["baseWager"]) / decimal.Decimal("0.50")
        
        if _type in ["ALL", "BASE"]:
            if play["lineType"] == "EXT":
                if results == play["numbers"]:
                    total = cls.STRAIGHT
                    
            elif play["lineType"] == "ANY":
                multiplier = 1 if len(set(play["numbers"])) == 3 else 2
                if cls._match(play["numbers"], results):
                    total = cls.BOX
                    
            elif play["lineType"] == "EXTANY":
                if results == play["numbers"]:
                    index_dict = dict((value, idx) for idx,value in enumerate(play['numbers']))
                    idx_result = [index_dict[x] for x in results]
                    if idx_result == [2,2,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [3,3,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [3,1,3,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [0,3,3,3]:
              		total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [1,1,3,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [2,3,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [3,2,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [1,1,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [2,1,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [3,1,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [0,3,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [0,1,3,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [0,2,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [0,1,2,3]:
                        total = cls.STRAIGHT_AND_BOX
                   

                else:
                    if set(play['numbers']) == set(results):
                        total = cls.BOX_ONLY
                    else:
                        return 0
                            
            else:
                raise NotImplementedError()
        print total
        return total
    
    @classmethod
    def validate_play(cls, data):
        elements = ["numbers", "lineType", "baseWager", "sum", "sumWager"]
        
        for element in elements:
            if element not in data:
                raise LotteryPlayInvalidException("Invalid Format %s" % data)
        
        if data["lineType"] not in cls.ENUM.values():
            raise LotteryPlayInvalidException("Invalid Variant %s [%s]" % (data["lineType"], cls.ENUM.values()))
        
        try:
            decimal.Decimal(data["baseWager"])
        except (TypeError, decimal.InvalidOperation):
            raise LotteryPlayInvalidException("Invalid Base Wager %s" % data)
        
        if data["sum"] is not None:
            if data["sum"] not in xrange(0, (cls.BALLS * 10) + 1):
                raise LotteryPlayInvalidException("Invalid SUM IT UP %s" % data)
            
            try:
                decimal.Decimal(data["sumWager"])
            except (TypeError, decimal.InvalidOperation):
                raise LotteryPlayInvalidException("Invalid Sum Wager %s" % data)
        
        try:
            cls.validate_numbers(data["numbers"])
        except LotteryResultsInvalidException as e:
            raise LotteryPlayInvalidException(str(e))
        
        return True

    @classmethod
    def get_component(cls, record):
        if "drawTime" not in record or record["drawTime"] != 6:
            raise LotteryPlayInvalidException("Unknown Draw Time")
        
        if record["drawTime"] == 6:
            return LotteryGameComponent.objects.get(identifier="DailyFour")
    @classmethod
    def get_game_meta(cls, component):
        if component.identifier == "DailyFour":
            return {"drawTime": 6}
        return {"drawTime": 6}

    @classmethod
    def preprocess_ticket(cls, record):                
        for i, play in enumerate(record["lines"]):
            if "lineType" in play:
                record["lines"][i]["lineType"] = cls.ENUM.get(play["lineType"], play["lineType"])
                
        processed = []
        last = None
        
        for i, play in enumerate(record["lines"]):            
            if play["lineType"] == "SUM" and not last:
                continue
            
            if play["lineType"] != "SUM":
                _play = {
                    "baseWager": play["amount"],
                    "lineType": play["lineType"],
                    "totalWager": currency.format(play["amount"]),
                    "baseWager": None,
                    "numbers": play["numbers"],
                    "sum": None,
                    "sumWager": None
                }
                
                if _play["lineType"] == "COMBO":
                    _play["baseWager"] = currency.format(float(play["amount"]) / len(set(itertools.permutations(play["numbers"]))))
                elif _play["lineType"] in ["STRBOX", "EXTANY"]:
                    _play["baseWager"] = currency.format(float(play["amount"]) / 2)
                else:
                    _play["baseWager"] = currency.format(play["amount"])
                
                last = _play
                processed.append(_play)
            else:
                _play["sum"] = play["numbers"][0]
                _play["sumWager"] = currency.format(play["amount"])
                
                last = None
                        
        record["lines"] = processed        
        return record
    
    @classmethod
    def postprocess_play(cls, record):
        enum = None
        for k, v in cls.ENUM.iteritems():
             if v == record["lineType"]:
                 record["lineType"] = k
                 break
        
        return record
    
from yoolotto.lottery.game.manager import GameManager
GameManager.register(CADailyFourGame)
