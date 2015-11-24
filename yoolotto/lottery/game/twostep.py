import re
import json

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps

class TwoStepGame(LotteryGame):
    NAME = "TwoStep"
    JACKPOT = "J"

    FOUR_OF_FOUR_BONUS = "FOUR_OF_FOUR_BONUS"
    FOUR_OF_FOUR = "FOUR_OF_FOUR"
    THREE_OF_FOUR_BONUS = "THREE_OF_FOUR_BONUS"
    THREE_OF_FOUR = "THREE_OF_FOUR"
    TWO_OF_FOUR_BONUS = "TWO_OF_FOUR_BONUS"
    ONE_OF_FOUR_BONUS = "ONE_OF_FOUR_BONUS"
    ZERO_OF_BONUS = "ZERO_OF_BONUS"


            
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
        if len(numbers) != 5:
            raise LotteryResultsInvalidException("Expected 5 Numbers, Received %s" % len(numbers))
        
        if len(set(numbers[:4])) != 4:
            raise LotteryResultsInvalidException("Duplicate Number %s" % numbers[:4])
        
	for number in numbers[0:4]:
            if number not in xrange(1, 36):
                raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)
	if numbers[4] not in xrange(1, 36):
	    raise LotteryResultsInvalidException("Invalid Bonus Ball Number %s" % numbers[4])
        
                
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
        
        white = set(results[0:4]).intersection(set(play["numbers"][0:4]))
        bonus = results[4] == play["numbers"][4]
        
        if not white and not bonus:
            return 0
        
        if len(white) == 0 and bonus:
            return cls.ZERO_OF_BONUS
        
        if len(white) == 1 and bonus:
            return cls.ONE_OF_FOUR_BONUS
        
        if len(white) == 2 and bonus:
            return cls.TWO_OF_FOUR_BONUS
        
        if len(white) == 3:
            return cls.THREE_OF_FOUR_BONUS if bonus else cls.THREE_OF_FOUR
        
        if len(white) == 4:
            return cls.JACKPOT if bonus else cls.FOUR_OF_FOUR
        
        return 0
                
    @classmethod
    def validate_play(cls, data):
        if "numbers" not in data:
            raise LotteryPlayInvalidException("Invalid Format %s" % data)
        
        try:
            cls.validate_numbers(data["numbers"])
        except LotteryResultsInvalidException as e:
            raise LotteryPlayInvalidException(str(e))
        
        return True
    
from yoolotto.lottery.game.manager import GameManager
GameManager.register(TwoStepGame)
