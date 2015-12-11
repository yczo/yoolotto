import re
import json

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps

class CashFiveGame(LotteryGame):
    NAME = "CashFive"
    JACKPOT = "J"
            
    @classmethod
    def decode(cls, raw, format=None):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if len(result) != 5:
            raise ValueError("Unexpected Decode Value: %s" % result)
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format=None):
        values = json.loads(encoded)
        
        if len(values) != 5:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values)
    
    @classmethod
    def validate_numbers(cls, numbers):
        if -1 in numbers:
            valid_numbers = [number for number in numbers if number != -1]
            
            if len(valid_numbers) != len(set(valid_numbers)):
                raise LotteryResultsInvalidException("Duplicate Number %s" % valid_numbers)

            for number in valid_numbers:
                if number not in xrange(1, 38):
                    raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
        else:
            if len(numbers) != 5:
                raise LotteryResultsInvalidException("Expected 5 Numbers, Received %s" % len(numbers))
            
            if len(set(numbers)) != 5:
                raise LotteryResultsInvalidException("Duplicate Number %s" % numbers)
        
            for number in numbers:
                if number not in xrange(1, 38):
                    raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
                
        return True
    
    @classmethod
    def earnings(cls, play):
	try:
        	return cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play))
	except:
		import ast
		return cls._earnings(json.loads(play.ticket.draw.result), ast.literal_eval(play.play))
    
    @classmethod
    def _earnings(cls, results, play):                
        
        matched = set(results).intersection(set(play["numbers"]))
        
        if len(matched) < 2:
            return 0
        
        if len(matched) == 2:
            return 2
        
        if len(matched) == 3:
            return 11
        
        if len(matched) == 4:
            return 123
        
        if len(matched) == 5:
            return 20000
                        
    @classmethod
    def validate_play(cls, data):
        if "numbers" not in data:
            raise LotteryPlayInvalidException("Invalid Format %s" % data)
        
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
            
            for field in ["numbers"]:
                if field in play:
                    _play[field] = play[field]

            processed.append(_play)
            
        record["lines"] = processed
        
        return record
    
from yoolotto.lottery.game.manager import GameManager
GameManager.register(CashFiveGame)
