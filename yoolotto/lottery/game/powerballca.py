import re
import json

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps

class CAPowerballGame(LotteryGame):
    NAME = "PowerballCA"
    JACKPOT = "J"
    
    FORMATS = ["PowerballCA"]

    FIVE_OF_FIVE = "FIVE_OF_FIVE"
    FIVE_OF_FIVE_WITH_POWERPLAY = "FIVE_OF_FIVE_WITH_POWERPLAY"

    FOUR_OF_FIVE = "FOUR_OF_FIVE"
    FOUR_OF_FIVE_POWERBALL = "FOUR_OF_FIVE_POWERBALL"
    FOUR_OF_FIVE_WITH_POWERPLAY = "FOUR_OF_FIVE_WITH_POWERPLAY"
    FOUR_OF_FIVE_POWERBALL_WITH_POWERPLAY = "FOUR_OF_FIVE_POWERBALL_WITH_POWERPLAY"

    THREE_OF_FIVE = "THREE_OF_FIVE"
    THREE_OF_FIVE_WITH_POWERPLAY = "THREE_OF_FIVE_WITH_POWERPLAY"
    THREE_OF_FIVE_POWERBALL = "THREE_OF_FIVE_POWERBALL"
    THREE_OF_FIVE_POWERBALL_WITH_POWERPLAY = "THREE_OF_FIVE_POWERBALL_WITH_POWERPLAY"

    
    TWO_OF_FIVE = "TWO_OF_FIVE"
    TWO_OF_FIVE_POWERBALL = "TWO_OF_FIVE_POWERBALL"
    TWO_OF_FIVE_POWERBALL_WITH_POWERPLAY = "TWO_OF_FIVE_POWERBALL_WITH_POWERPLAY"

    
    ONE_OF_FIVE = "ONE_OF_FIVE"
    ONE_OF_FIVE_POWERBALL = "ONE_OF_FIVE_POWERBALL"
    ONE_OF_FIVE_POWERBALL_WITH_POWERPLAY = "ONE_OF_FIVE_POWERBALL_WITH_POWERPLAY"

    POWERBALL_ONLY = "POWERBALL_ONLY"
    POWERBALL_ONLY_WITH_POWERPLAY = "POWERBALL_ONLY_WITH_POWERPLAY"
        
    @classmethod
    def decode(cls, raw, format="Powerballca"):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        result_without_powerplay = result[0:6]
        powerplay = result[6:7]

        if format == "Powerballca":
            if len(result_without_powerplay) != 6:
                raise ValueError("Unexpected Decode Value: %s for %s" % (result, format))
        elif format == "Powerplay":
            if len(powerplay) != 1:
                raise ValueError("Unexpected Decode Value: %s for %s" % (powerplay, format))
        else:
            raise NotImplementedError()
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format="Powerball"):
        values = json.loads(encoded)
        
        if len(values) != 6:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values[:-1]) + " Powerball: " + values[-1]
    
    @classmethod
    def validate_numbers(cls, numbers):
        print "here is exception"
        if not numbers:
            raise LotteryResultsInvalidException("No number detected in white ball")

        if len(numbers) < 5:
            raise LotteryResultsInvalidException("Please enter all white ball numbers")

        if len(numbers) == 5:
            raise LotteryResultsInvalidException("Please enter Powerball number")
        
        white = numbers[0:5]
        powerball = numbers[5]
        
	if -1 in white:
            valid_whites = [number for number in white if number != -1]
            if len(valid_whites) != len(set(valid_whites)):
                raise LotteryResultsInvalidException("Duplicate Number %s" % white)

            for number in valid_whites:
                if number not in xrange(1, 70): 
                    raise LotteryResultsInvalidException("Invalid White Ball Number %s" % number)

	else:

        	if len(set(white)) != 5:
            		raise LotteryResultsInvalidException("Duplicate Number %s" % white)
        
        	for number in white:
            		if number not in xrange(1, 70):
                		raise LotteryResultsInvalidException("Invalid White Ball Number %s" % number)
        
        if powerball != -1:
        	if powerball not in xrange(1, 27):
            		raise LotteryResultsInvalidException("Invalid Powerball Number %s" % powerball)    
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
        multiplier = play["multiplier"]
        
        white = set(results[0:5]).intersection(set(play["numbers"][0:5]))
        powerball = results[5] == play["numbers"][5]

        if not white and not powerball:
            return 0
        
        if len(white) == 0 and powerball:
            return cls.POWERBALL_ONLY
            
        if len(white) == 1 and powerball:
            return cls.ONE_OF_FIVE_POWERBALL
        
        if len(white) == 2 and powerball:
            return cls.TWO_OF_FIVE_POWERBALL
        
        if len(white) == 3:
            if powerball:
                return cls.THREE_OF_FIVE_POWERBALL        
            else:
                return cls.THREE_OF_FIVE
            
        if len(white) == 4:
            if powerball:
                return cls.FOUR_OF_FIVE_POWERBALL
            else:
                return cls.FOUR_OF_FIVE
            
        if len(white) == 5:
            if powerball:
                return cls.JACKPOT
            else:
                return cls.FIVE_OF_FIVE
        
        return 0
                
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
GameManager.register(CAPowerballGame)
