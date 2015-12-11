import re
import json
import decimal
import itertools

from yoolotto.lottery.game.base import LotteryGame, LotteryPlayInvalidException, LotteryResultsInvalidException
from yoolotto.lottery.models import LotteryGameComponent
from yoolotto.util.serialize import dumps
from yoolotto.util import currency

class AllOrNothingGame(LotteryGame):
    NAME = "AllOrNothing"
    BALLS = 12    
    
    
    TWELVE_OF_TWELVE = "TWELVE_OF_TWELVE"
    ELEVEN_OF_TWELVE = "ELEVEN_OF_TWELVE"
    TEN_OF_TWELVE = "TEN_OF_TWELVE"
    NINE_OF_TWELVE = "NINE_OF_TWELVE"
    EIGHT_OF_TWELVE = "EIGHT_OF_TWELVE"
    FOUR_OF_TWELVE = "FOUR_OF_TWELVE"
    THREE_OF_TWELVE = "THREE_OF_TWELVE"
    TWO_OF_TWELVE = "TWO_OF_TWELVE"
    ONE_OF_TWELVE = "ONE_OF_TWELVE"
    ZERO_OF_TWELVE = "ZERO_OF_TWELVE"

    @classmethod
    def decode(cls, raw, format=None):
        pattern = re.compile(r"\d+")
        result = map(int, re.findall(pattern, raw))
        
        if len(result) != 12:
            raise ValueError("Unexpected Decode Value: %s" % result)
        
        return dumps(result)
    
    @classmethod
    def representation(cls, encoded, format=None):
        values = json.loads(encoded)
        
        if len(values) != 12:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values)
    
    @classmethod
    def validate_numbers(cls, numbers):
        if -1 not in numbers:
            if len(numbers) != 12:
                raise LotteryResultsInvalidException("Expected 6 Numbers, Received %s" % len(numbers))
            
            if len(set(numbers)) != 12:
                raise LotteryResultsInvalidException("Duplicate Number %s" % numbers)
            
            for number in numbers:
                if number not in xrange(1, 25):
                    raise LotteryResultsInvalidException("Invalid Ball Number %s" % number)

        else:

            valid_numbers = [number for number in numbers if number != -1]

            if(len(valid_numbers)) != len(set(valid_numbers)):
                raise LotteryResultsInvalidException("Duplicate Number %s" % valid_numbers)

            for number in valid_numbers:
                if number not in xrange(1, 25):
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
        matched = len(set(results).intersection(set(play["numbers"])))
        
        if matched in [0, 12]:
            return cls.TWELVE_OF_TWELVE
        elif matched in [1, 11]:
            return cls.ELEVEN_OF_TWELVE
        elif matched in [2, 10]:
            return cls.TEN_OF_TWELVE
        elif matched in [3, 9]:
            return cls.NINE_OF_TWELVE
        elif matched in [4, 8]:
            return cls.EIGHT_OF_TWELVE
        else:
            return 0
                                
        raise RuntimeError("Unknown Win State?")
    
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
    def get_component(cls, record):
        if record["drawTime"] == 6:
            return LotteryGameComponent.objects.get(identifier="AllOrNothingDay")

        if "drawTime" not in record or record["drawTime"] not in [0, 1, 2, 3]:
            raise LotteryPlayInvalidException("Unknown Draw Time")
        
        if record["drawTime"] == 0:
            return LotteryGameComponent.objects.get(identifier="AllOrNothingDay")
        elif record["drawTime"] == 2:
            return LotteryGameComponent.objects.get(identifier="AllOrNothingMorning")
        elif record["drawTime"] == 3:
            return LotteryGameComponent.objects.get(identifier="AllOrNothingEvening")
        else:
            return LotteryGameComponent.objects.get(identifier="AllOrNothingNight")
        
    @classmethod
    def get_game_meta(cls, component):
        if component.identifier == "AllOrNothingDay":
            return {"drawTime": 0}
        
        if component.identifier == "AllOrNothingMorning":
            return {"drawTime": 2}
        
        if component.identifier == "AllOrNothingEvening":
            return {"drawTime": 3}
        
        return {"drawTime": 1}

    @classmethod
    def get_next_components(cls,component):
        day_component = LotteryGameComponent.objects.get(identifier="AllOrNothingDay")
        morning_component = LotteryGameComponent.objects.get(identifier="AllOrNothingMorning")
        evening_component = LotteryGameComponent.objects.get(identifier="AllOrNothingEvening")
        night_component = LotteryGameComponent.objects.get(identifier="AllOrNothingNight")

        order = [morning_component,day_component,evening_component,night_component]
        index = order.index(component)
        draws_list = [order[item] for item in range(len(order)) if item >= index]
        return draws_list

    @classmethod
    def get_previous_components(cls,component):
        day_component = LotteryGameComponent.objects.get(identifier="AllOrNothingDay")
        morning_component = LotteryGameComponent.objects.get(identifier="AllOrNothingMorning")
        evening_component = LotteryGameComponent.objects.get(identifier="AllOrNothingEvening")
        night_component = LotteryGameComponent.objects.get(identifier="AllOrNothingNight")

        order = [morning_component,day_component,evening_component,night_component]
        index = order.index(component)
        draws_list = [order[item] for item in range(len(order)) if item <= index]
        return draws_list

    @classmethod
    def get_all_components(cls):
        day_component = LotteryGameComponent.objects.get(identifier="AllOrNothingDay")
        morning_component = LotteryGameComponent.objects.get(identifier="AllOrNothingMorning")
        evening_component = LotteryGameComponent.objects.get(identifier="AllOrNothingEvening")
        night_component = LotteryGameComponent.objects.get(identifier="AllOrNothingNight")

        draws_list = [morning_component,day_component,evening_component,night_component]
        return draws_list
    
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
GameManager.register(AllOrNothingGame)
