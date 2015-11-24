import re
import json

from yoolotto.lottery.models import LotteryDraw, LotteryGameComponent
from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.util.serialize import dumps


class CAMegaMillionsGame(LotteryGame):
    NAME = "MegaMillionsCA"
    JACKPOT = "J"
    
    FORMATS = ["MegaMillionsCA"]

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
        
    '''@classmethod
    def decode(cls, raw, format="MegaMillionsCA"):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if format == "MegaMillionsCA":
            if len(result) != 6:
                raise ValueError("Unexpected Decode Value: %s for %s" % (result, format))
        else:
            raise NotImplementedError()
        
        return dumps(result)'''

    @classmethod
    def decode(cls, raw, format="MegaMillionsCA"):

        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        result_without_megaplay = result[0:6]
        megaplier = result[6:7]

        if format == "MegaMillionsCA":
            if len(result_without_megaplay) != 6:
                raise ValueError("Unexpected Decode Value: %s for %s" % (result, format))
        elif format == "Megaplier":
            if len(megaplier) != 1:
                raise ValueError("Unexpected Decode Value: %s for %s" % (megaplier, format))
        else:
            raise NotImplementedError()
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format="MegaMillionsCA"):
        values = json.loads(encoded)
        if format == "MegaMillionsCA":
            if len(values) != 6:
                raise ValueError("Unexpected Representation Value: %s" % result)
            return "-".join(values[:-1]) + " Megaball: " + values[-1]
        elif format == "Megaplier":
            return values[0]
        else:
            raise NotImplementedError()
    
    @classmethod
    def validate_ball_number_from_dates(cls, numbers, drawingDate):
        
        import datetime
        #previous_date = datetime.date(2013,10,18)
        next_date = datetime.date(2013,10,22)
        from datetime import datetime
        drawing_date = datetime.strptime(drawingDate , '%Y-%m-%d').date()
        
        if len(numbers) != 6:
            raise LotteryPlayInvalidException("Invalid Numbers %s" % numbers)
        
        white = numbers[0:5]
        megaball = numbers[5]
        
        if drawing_date < next_date:
            megaball_number_range = xrange(1, 47)
            white_ball_number_range = xrange(1, 57)

        elif drawing_date >= next_date:
            megaball_number_range = xrange(1, 16)
            white_ball_number_range = xrange(1, 76)

	if -1 in white:
            valid_whites = [number for number in white if number != -1]
            if len(valid_whites) != len(set(valid_whites)):
                raise LotteryPlayInvalidException("Duplicate Number %s" % white)

            for number in valid_whites:
                if number not in white_ball_number_range:
                    raise LotteryPlayInvalidException("Invalid White Ball Number %s" % number)
	else:        
        	if len(set(white)) != 5:
            		raise LotteryPlayInvalidException("Duplicate Number %s" % white)

       	        for number in white:
            		if number not in white_ball_number_range:
                		raise LotteryPlayInvalidException("Invalid White Ball Number %s" % number)

	if megaball != -1:
        	if megaball not in megaball_number_range:
            		raise LotteryPlayInvalidException("Invalid megaball Number %s" % megaball)
            
        return True

    @classmethod
    def validate_numbers(cls, numbers):
        if not numbers:
            raise LotteryResultsInvalidException("No number detected in white ball")
        
        if len(numbers)<5:
            raise LotteryResultsInvalidException("Please enter all white ball numbers")
        
        if len(numbers)==5:
            raise LotteryResultsInvalidException("Please enter Megaball number")
        
        white = numbers[0:5]
        powerball = numbers[5]
        

        if len(set(white)) != 5:
            raise LotteryResultsInvalidException("Duplicate Number %s" % white)
        
        for number in white:
            if number not in xrange(1, 76):
                raise LotteryResultsInvalidException("Please enter valid white ball number")
        
        
        if powerball not in xrange(1, 16):
            raise LotteryResultsInvalidException("Please enter valid Megaball number")    
        return True    

    
    @classmethod
    def earnings(cls, play):
        try:
            return cls._earnings(json.loads(play.ticket.draw.result), json.loads(play.play))
        except:
            import ast
            #print "in except"
            return cls._earnings(json.loads(play.ticket.draw.result), ast.literal_eval(play.play))

    
    @classmethod
    def _earnings(cls, results, play):    


        white = set(results[0:5]).intersection(set(play["numbers"][0:5]))
        megaball = results[5] == play["numbers"][5]
        megaply = 1
        if play["multiplier"]:
            megaply = megaplier
            
        if not white and not megaball:
            return 0
        
        if len(white) == 0 and megaball:
            #print cls.MEGABALL_ONLY
            return cls.MEGABALL_ONLY

        if len(white) == 1 and megaball:
            #print cls.ONE_OF_FIVE_MEGABALL
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
                print 2222222222222222222222
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
        
        if data["multiplier"] == 'True':
            data["multiplier"] = True
        elif data["multiplier"] == 'False':
            data["multiplier"] = False
        if data["multiplier"] not in (True, False):
            raise LotteryPlayInvalidException("Invalid Multiplier %s" % data)
        
        # try:
        #     cls.validate_numbers(data["numbers"])
        # except LotteryResultsInvalidException as e:
        #     raise LotteryPlayInvalidException(str(e))
        
        return True
    
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
            
            for field in ["numbers", "multiplier"]:
                if field in play:
                    cls.validate_ball_number_from_dates(play['numbers'], drawingDate)
                    _play[field] = play[field]

            processed.append(_play)
            
        record["lines"] = processed
        
        return record
    
from yoolotto.lottery.game.manager import GameManager
GameManager.register(CAMegaMillionsGame)
