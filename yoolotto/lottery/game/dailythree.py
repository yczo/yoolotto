import re
import json
import decimal
import itertools

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.lottery.models import LotteryGameComponent
from yoolotto.lottery.models import LotteryDraw
from yoolotto.util.serialize import dumps
from yoolotto.util import currency

class DailyThreeGame(LotteryGame):
    NAME = "DailyThree"
    
    STRAIGHT = "STRAIGHT"
    STRAIGHT_AND_BOX = "STRAIGHT_AND_BOX"
    BOX = "BOX"
    BOX_ONLY = "BOX_ONLY"


    ENUM = {
        1: "SUM",
        3: "ANY",
        9: "COMBO",
        2: "EXT",
        4: "EXTANY",
    }
    
    BALLS = 3
    
    SUMITUP = {
        0: {"0.50": 250, "1.00": 500, "2.00": 1000, "3.00": 1500, "4.00": 2000, "5.00": 2500},
        1: {"0.50": 83, "1.00": 166, "2.00": 333, "3.00": 500, "4.00": 666, "5.00": 833},
        2: {"0.50": 41, "1.00": 83, "2.00": 166, "3.00": 250, "4.00": 333, "5.00": 416},
        3: {"0.50": 25, "1.00": 50, "2.00": 100, "3.00": 150, "4.00": 200, "5.00": 250},
        4: {"0.50": 16, "1.00": 33, "2.00": 66, "3.00": 100, "4.00": 133, "5.00": 166},
        5: {"0.50": 11, "1.00": 23, "2.00": 47, "3.00": 71, "4.00": 95, "5.00": 119},
        6: {"0.50": 8, "1.00": 17, "2.00": 35, "3.00": 53, "4.00": 71, "5.00": 89},
        7: {"0.50": 6, "1.00": 13, "2.00": 27, "3.00": 41, "4.00": 55, "5.00": 69},
        8: {"0.50": 5, "1.00": 11, "2.00": 22, "3.00": 33, "4.00": 44, "5.00": 55},
        9: {"0.50": 4, "1.00": 9, "2.00": 18, "3.00": 27, "4.00": 36, "5.00": 45},
        10: {"0.50": 3, "1.00": 7, "2.00": 15, "3.00": 23, "4.00": 31, "5.00": 39},
        11: {"0.50": 3, "1.00": 7, "2.00": 14, "3.00": 21, "4.00": 28, "5.00": 36},
        12: {"0.50": 3, "1.00": 6, "2.00": 13, "3.00": 20, "4.00": 27, "5.00": 34},
        13: {"0.50": 3, "1.00": 6, "2.00": 13, "3.00": 20, "4.00": 26, "5.00": 33},
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
                    if idx_result == [1,1,2]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [0,2,2]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [0,1,2]:
                        total = cls.STRAIGHT_AND_BOX
                    elif idx_result == [2,2,2]:
                        total = cls.STRAIGHT_AND_BOX
		    elif idx_result == [2,1,2]:
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
        if "drawTime" not in record or record["drawTime"] not in [3,4,5]:
            raise LotteryPlayInvalidException("Unknown Draw Time")
        
        if record["drawTime"] == 4:
            return LotteryGameComponent.objects.get(identifier="DailyThreeMidday")
        else:
            return LotteryGameComponent.objects.get(identifier="DailyThreeEvening")
        
    @classmethod
    def get_game_meta(cls, component):
        if component.identifier == "DailyThreeMidDay":
            return {"drawTime": 4}
        
        if component.identifier == "DailyThreeEvening":
            return {"drawTime": 3}
        
        return {"drawTime": 4}
    
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
GameManager.register(DailyThreeGame)
