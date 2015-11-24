import re
import json

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps

class LottoCaliforniaGame(LotteryGame):
    NAME = "POWERBALL"
    JACKPOT = "J"
    
    FORMATS = ["POWERBALL"]
        
    @classmethod
    def decode(cls, raw, format="POWERBALL"):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if len(result) != 6:
            raise ValueError("Unexpected Decode Value: %s" % result)
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format="POWERBALL"):
        values = json.loads(encoded)
        
        if len(values) != 6:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values)
    
    @classmethod
    def validate_numbers(cls, numbers):
        if len(numbers) != 6:
            raise LotteryResultsInvalidException("Expected 6 Numbers, Received %s" % len(numbers))
        
        if len(set(numbers)) != 6:
            raise LotteryResultsInvalidException("Duplicate Number %s" % numbers)
        
        for number in numbers:
            if number not in xrange(1, 55):
                raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
                
        return True
    
    @classmethod
    def earnings(cls, play):
        return cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play))
    
    @classmethod
    def _earnings(cls, results, play):        
        multiplier = play["multiplier"]
        
        white = set(results[0:5]).intersection(set(play["numbers"][0:5]))
        powerball = results[5] == play["numbers"][5]
        
        if not white and not powerball:
            return 0
        
        if len(white) == 0 and powerball:
            return 165 if multiplier else 55
            
        if len(white) == 1 and powerball:
            return 333 if multiplier else 111
        
        if len(white) == 2 and powerball:
            return 1212 if multiplier else 706
        
        if len(white) == 3:
            if powerball:
                return 24490 if multiplier else 12245
            else:
                return 720 if multiplier else 360
            
        if len(white) == 4:
            if powerball:
                return 2595904 if multiplier else 648976
            else:
                return 38176 if multiplier else 19088
            
        if len(white) == 5:
            if powerball:
                return cls.JACKPOT
            else:
                return 10307266 if multiplier else 5153633
        
        return 0
                
        raise RuntimeError("Unknown Win State?")
            
    @classmethod
    def validate_play(cls, data):
        if "numbers" not in data or "multiplier" not in data:
            raise LotteryPlayInvalidException("Invalid Format %s" % data)
        
        if data["multiplier"] not in (True, False):
            raise LotteryPlayInvalidException("Invalid Multiplier %s" % data)
        
        try:
            cls.validate_numbers(data["numbers"])
        except LotteryResultsInvalidException as e:
            raise LotteryPlayInvalidException(str(e))
        
        return True
    
    @classmethod
    def preprocess_ticket(cls, record):
        processed = []
        
        for i, play in enumerate(record["lines"]):
            _play = {}
            
            for field in ["numbers", "multiplier"]:
                if field in play:
                    _play[field] = play[field]

            processed.append(_play)
            
        record["lines"] = processed
        
        return record
    
from yoolotto.lottery.game.manager import GameManager
GameManager.register(LottoCaliforniaGame)