import re
import json
from yoolotto.lottery.models import LotteryDraw
from yoolotto.lottery.models import LotteryGameComponent 
from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps

class FantasyPowerBallGame(PowerBallGame):
    NAME = "FantasyPowerball"
    JACKPOT = "J"
    
    FORMATS = ["FantasyPowerball"]
    
#    DAYS = []
    FIVE_OF_FIVE = "FIVE_OF_FIVE"

    FOUR_OF_FIVE = "FOUR_OF_FIVE"
    FOUR_OF_FIVE_POWERBALL = "FOUR_OF_FIVE_POWERBALL"

    THREE_OF_FIVE = "THREE_OF_FIVE"
    THREE_OF_FIVE_POWERBALL = "THREE_OF_FIVE_POWERBALL"

    TWO_OF_FIVE_POWERBALL = "TWO_OF_FIVE_POWERBALL"
    TWO_OF_FIVE = "TWO_OF_FIVE"
    
    ONE_OF_FIVE_POWERBALL = "ONE_OF_FIVE_POWERBALL"
    ONE_OF_FIVE = "ONE_OF_FIVE"
    POWERBALL_ONLY = "POWERBALL_ONLY"
    
    @classmethod
    def decode(cls, raw, format="FantasyPowerball"):

        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        result_without_powerplay = result[0:6]

        if format == "Powerball":
            if len(result_without_powerplay) != 6:
                raise ValueError("Unexpected Decode Value: %s for %s" % (result, format))
        else:
            raise NotImplementedError()
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format="FantasyPowerball"):

        values = json.loads(encoded)
        
        if len(values) != 6:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values[:-1]) + " Powerball: " + values[-1]
    
    @classmethod
    def earnings(cls, play):
        try:
            return cls._earnings(json.loads(play.ticket.draw.result),json.loads(play.play))
        except:
            import ast
            return cls._earnings(json.loads(play.ticket.draw.result),ast.literal_eval(play.play))
    
    
    @classmethod
    def _earnings(cls, results, powerplay, play):
        print play
        
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
GameManager.register(FantasyPowerBallGame)
