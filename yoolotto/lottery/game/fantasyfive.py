import re
import json
from yoolotto.lottery.game.base import LotteryGame
from yoolotto.lottery.game.base import LotteryPlayInvalidException
from yoolotto.lottery.game.base import LotteryResultsInvalidException
from yoolotto.util.serialize import dumps


class FantasyFiveGame(LotteryGame):
    NAME = "FantasyFive"
    JACKPOT = "J"
    
    FORMATS = ["FantasyFive"]

    FIVE_OF_FIVE = "FIVE_OF_FIVE"
    FOUR_OF_FIVE = "FOUR_OF_FIVE"
    THREE_OF_FIVE = "THREE_OF_FIVE"
    TWO_OF_FIVE = "TWO_OF_FIVE"
    ONE_OF_FIVE = "ONE_OF_FIVE"


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
        if not numbers:
            raise LotteryResultsInvalidException("Please enter numbers")
            
        if len(numbers) != 5:
            raise LotteryResultsInvalidException("Expected 5 Numbers, Received %s" % len(numbers))
        
        if len(set(numbers)) != 5:
            raise LotteryResultsInvalidException("Duplicate Number %s" % numbers)
        
        for number in numbers:
            if number not in xrange(1, 40):
                raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
                
        return True
    
    @classmethod
    def earnings(cls, play):
        return cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play))
    
    @classmethod
    def _earnings(cls, results, play):                
        
        matched = set(results).intersection(set(play["numbers"]))
        
        if len(matched) < 2:
            return 0
        
        if len(matched) == 2:
            return 1
        
        if len(matched) == 3:
            return cls.THREE_OF_FIVE
        
        if len(matched) == 4:
            return cls.FOUR_OF_FIVE
        
        if len(matched) == 5:
            return cls.JACKPOT
                        
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
GameManager.register(FantasyFiveGame)
