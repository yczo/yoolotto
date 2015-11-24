import re
import json

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps

class LottoTexasGame(LotteryGame):
    NAME = "Lotto"
    JACKPOT = "J"
    
    FORMATS = ["Lotto"]

    SIX_OF_SIX = "SIX_OF_SIX"
    FIVE_OF_SIX = "FIVE_OF_SIX"
    FOUR_OF_SIX = "FOUR_OF_SIX"
    THREE_OF_SIX = "THREE_OF_SIX"
    TWO_OF_SIX = "TWO_OF_SIX"
    FIVE_OF_SIX_EXTRA = "FIVE_OF_SIX_EXTRA"
    FOUR_OF_SIX_EXTRA = "FOUR_OF_SIX_EXTRA"
    THREE_OF_SIX_EXTRA = "THREE_OF_SIX_EXTRA"
    TWO_OF_SIX_EXTRA = "TWO_OF_SIX_EXTRA"

        
    @classmethod
    def decode(cls, raw, format="Lotto"):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if len(result) != 6:
            raise ValueError("Unexpected Decode Value: %s" % result)
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format="Lotto"):
        values = json.loads(encoded)
        
        if len(values) != 6:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values)
    
    @classmethod
    def validate_numbers(cls, numbers):
	if -1 in numbers:
            valid_numbers = [number for number in numbers if number != -1]
            if len(valid_numbers) != len(set(valid_numbers)):
                raise LotteryResultsInvalidException("Duplicate Number %s" % numbers)

            for number in valid_numbers:
                if number not in xrange(1, 55):
                    raise LotteryPlayInvalidException("Invalid White Ball Number %s" % number)

        else:
            if len(numbers) != 6:
                raise LotteryResultsInvalidException("Expected 6 Numbers, Received %s" % len(numbers))
            
            if len(set(numbers)) != 6:
                raise LotteryResultsInvalidException("Duplicate Number %s" % numbers)
            
            for number in numbers:
                if number not in xrange(1, 55):
                    raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
                
        return True

        '''if len(numbers) != 6:
            raise LotteryResultsInvalidException("Expected 6 Numbers, Received %s" % len(numbers))
        
        if len(set(numbers)) != 6:
            raise LotteryResultsInvalidException("Duplicate Number %s" % numbers)
        
        for number in numbers:
            if number not in xrange(1, 55):
                raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
                
        return True'''
    
    @classmethod
    def earnings(cls, play):
	try:
        	return cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play))
	except:
            import ast
            return cls._earnings(json.loads(play.ticket.draw.result), ast.literal_eval(play.play))
    
    @classmethod
    def _earnings(cls, results, play):        
        multiplier = play["multiplier"]
        
        matched = set(results).intersection(set(play["numbers"]))
        
        if len(matched) < 2:
            return 0
        
        if len(matched) == 2:
            return cls.TWO_OF_SIX_EXTRA if multiplier else cls.TWO_OF_SIX
        
        if len(matched) == 3:
            return cls.THREE_OF_SIX_EXTRA if multiplier else cls.THREE_OF_SIX
        
        if len(matched) == 4:
            return cls.FOUR_OF_SIX_EXTRA if multiplier else cls.FOUR_OF_SIX
        
        if len(matched) == 5:
            return cls.FIVE_OF_SIX_EXTRA if multiplier else cls.FIVE_OF_SIX
        
        if len(matched) == 6:
            return cls.JACKPOT
                        
        raise RuntimeError("Unknown Win State?")
            
    @classmethod
    def validate_play(cls, data):
        if "numbers" not in data or "multiplier" not in data:
            raise LotteryPlayInvalidException("Invalid Format %s" % data)

        if data["multiplier"] == 'True':
            data["multiplier"] = True
        elif data["multiplier"] == 'False':
            data["multiplier"] = False
        
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
GameManager.register(LottoTexasGame)
