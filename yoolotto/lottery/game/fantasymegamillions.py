import re
import json

from yoolotto.lottery.models import LotteryDraw, LotteryGameComponent
from yoolotto.util.serialize import dumps


class FantasyMegaMillionsGame():
    NAME = "FantasyMegaMillions"
    JACKPOT = 1000000
    five_of_five = 5000
    four_of_five_megaball = 500
    four_of_five = 10
    
    FORMATS = ["FantasyMegaMillions"]

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
    def decode(cls, raw, format="FantasyMegaMillions"):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if format == "FantasyMegaMillions":
            if len(result) != 6:
                raise ValueError("Unexpected Decode Value: %s for %s" % (result, format))
        else:
            raise NotImplementedError()
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format="FantasyMegaMillions"):
        values = json.loads(encoded)
        if format == "FantasyMegaMillions":
            if len(values) != 6:
                raise ValueError("Unexpected Representation Value: %s" % result)
            return "-".join(values[:-1]) + " Megaball: " + values[-1]
        else:
            raise NotImplementedError()
    @classmethod
    def earnings(cls, play):
    	try:
    		return cls._earnings(json.loads(play.ticket.draw.result), _megaplier, json.loads(play.play))
    	except:
    		import ast
    		return cls._earnings(json.loads(play.ticket.draw.result), _megaplier, ast.literal_eval(play.play))
    
    @classmethod
    def _earnings(cls, results, megaplier, play):        
        white = set(results[0:5]).intersection(set(play["numbers"][0:5]))
        megaball = results[5] == play["numbers"][5]

        if not white and not megaball:
            return 0
        
        if len(white) == 0 and megaball:
            #return 1 * megaply
            return cls.MEGABALL_ONLY
            
        if len(white) == 1 and megaball:
            #return 2 * megaply
            return cls.ONE_OF_FIVE_MEGABALL

        if len(white) == 2 and megaball:
            #return 5 * megaply
            return cls.TWO_OF_FIVE_MEGABALL
        
        if len(white) == 3:
            if megaball:
                #return 50 * megaply
                return cls.THREE_OF_FIVE_MEGABALL
            else:
                #return 5 * megaply
                return cls.THREE_OF_FIVE

        if len(white) == 4:
            if megaball:
                #return 5000 * megaply
                return cls.FOUR_OF_FIVE_MEGABALL
            else:
                #return 500 * megaply
                return cls.FOUR_OF_FIVE
            
        if len(white) == 5:
            if megaball:
                return cls.JACKPOT
            else:
                #return 1000000 * megaply
                return cls.FIVE_OF_FIVE
        
        return 0
                
        raise RuntimeError("Unknown Win State?")
            
    @classmethod
    def preprocess_ticket(cls, record):
        
        processed = []
        #validate_ball_number_from_dates(record['drawingDate'], record[''])
        try:
            drawingDate = record['drawingDate']
        except:
            drawingDate = record['drawdate']

        
        for i, play in enumerate(record["lines"]):
            _play = {}
            
            for field in ["numbers"]:
                if field in play:
                    cls.validate_ball_number_from_dates(play['numbers'], drawingDate)
                    _play[field] = play[field]

            processed.append(_play)
            
        record["lines"] = processed
        
        return record
    
from yoolotto.lottery.game.manager import GameManager
GameManager.register(FantasyMegaMillionsGame)
