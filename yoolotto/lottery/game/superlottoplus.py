import re
import json

from yoolotto.lottery.models import LotteryDraw, LotteryGameComponent
from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps


class SuperLottoPlusGame(LotteryGame):
    NAME = "SuperLottoPlus"
    JACKPOT = "J"
    
    FORMATS = ["SuperLottoPlus"]
        
    FIVE_OF_FIVE = "FIVE_OF_FIVE"
    FOUR_OF_FIVE = "FOUR_OF_FIVE"
    FOUR_OF_FIVE_MEGABALL = "FOUR_OF_FIVE_MEGABALL"
    THREE_OF_FIVE = "THREE_OF_FIVE"
    THREE_OF_FIVE_MEGABALL = "THREE_OF_FIVE_MEGABALL"
    TWO_OF_FIVE_MEGABALL = "TWO_OF_FIVE_MEGABALL"
    TWO_OF_FIVE = "TWO_OF_FIVE"
    ONE_OF_FIVE_MEGABALL = "ONE_OF_FIVE_MEGABALL"
    ONE_OF_FIVE = "ONE_OF_FIVE"
    MEGABALL_ONLY = "MEGABALL_ONLY"

    @classmethod
    def decode(cls, raw, format="SuperLottoPlus"):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if format == "SuperLottoPlus":
            if len(result) != 6:
                raise ValueError("Unexpected Decode Value: %s for %s" % (result, format))
        else:
            raise NotImplementedError()
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format="SuperLottoPlus"):
        values = json.loads(encoded)
        
        if format == "SuperLottoPlus":
            if len(values) != 6:
                raise ValueError("Unexpected Representation Value: %s" % result)
            return "-".join(values[:-1]) + " Megaball: " + values[-1]
        else:
            raise NotImplementedError()
    
    @classmethod
    def validate_numbers(cls, numbers):
        if not numbers:
            raise LotteryResultsInvalidException("Please enter numbers")
        if len(numbers) != 6:
            raise LotteryResultsInvalidException("Invalid Numbers %s" % numbers)
        
        white = numbers[0:5]
        megaball = numbers[5]
        
        if len(set(white)) != 5:
            raise LotteryResultsInvalidException("Duplicate Number %s" % white)
        
        for number in white:
            if number not in xrange(1, 48):
                raise LotteryResultsInvalidException("Invalid White Ball Number %s" % number)
                
        if megaball not in xrange(1, 28):
            raise LotteryResultsInvalidException("Invalid megaball Number %s" % megaball)
                
        return True
    
    @classmethod
    def earnings(cls, play):
        draw = play.ticket.draw
        
        try:
            component = LotteryGameComponent.objects.get(parent__code=cls.NAME, format="SuperLottoPlus")
            megaplier = LotteryDraw.objects.get(date=draw.date, official=True, component=component)
        except LotteryDraw.DoesNotExist:
            return None
        
        if not megaplier.result:
            return None
        
        _megaplier = json.loads(megaplier.result)[0]
        
        return cls._earnings(json.loads(play.ticket.draw.result), _megaplier, json.loads(play.play))
    
    @classmethod
    def _earnings(cls, results, megaplier, play):        
        white = set(results[0:5]).intersection(set(play["numbers"][0:5]))
        megaball = results[5] == play["numbers"][5]
        
        megaply = 1
        if megaplier and play["multiplier"]:
            megaply = megaplier
        
        if not white and not megaball:
            return 0
        
        if len(white) == 0 and megaball:
            return cls.MEGABALL_ONLY

        if len(white) == 1 and megaball:
            return cls.ONE_OF_FIVE_MEGABALL

        if len(white) == 2 and megaball:
            return cls.TWO_OF_FIVE_MEGABALL

        if len(white) == 3:
            if megaball:
                return cls.THREE_OF_FIVE_MEGABALL
            else:
                return cls.THREE_OF_FIVE
            
        if len(white) == 4:
            if megaball:
                return cls.FOUR_OF_FIVE_MEGABALL
            else:
                return cls.FOUR_OF_FIVE
            
        if len(white) == 5:
            if megaball:
                return cls.JACKPOT
            else:
                return cls.FIVE_OF_FIVE
        
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
GameManager.register(SuperLottoPlusGame)
