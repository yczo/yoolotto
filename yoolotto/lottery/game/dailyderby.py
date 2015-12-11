import re
import json
import decimal
import itertools
import collections
from django.shortcuts import get_object_or_404
from yoolotto.lottery.game.base import LotteryGame
from yoolotto.lottery.game.base import LotteryPlayInvalidException
from yoolotto.lottery.game.base import LotteryResultsInvalidException
from yoolotto.lottery.models import LotteryGameComponent
from yoolotto.lottery.models import LotteryDraw
from yoolotto.util.serialize import dumps
from yoolotto.util import currency


class DailyDerbyGame(LotteryGame):
    NAME = "DailyDerby"
    JACKPOT = "J"
    FORMATS = ["DailyDerby"] 
    HORSES = 3

    TRIFECTA = "TRIFECTA"
    EXACTA = "EXACTA"
    EXACTA_WITH_RACETIME = "EXACTA_WITH_RACETIME"
    WIN_WITH_RACETIME = "WIN_WITH_RACETIME"
    WIN = "WIN"
    RACETIME_ONLY = "RACETIME_ONLY"

    
    @classmethod
    def decode(cls, raw, format=None):
        pattern = re.compile(r"\d+")
        #result = map(int, re.findall(pattern, raw))
        
        result_from_xml = map(int, re.findall(pattern, raw))
        temp_race_time = result_from_xml[-3:]
        racetime = ':'.join(str(x) for x in temp_race_time)
        result = result_from_xml[0:-3][1::2]
        
        if len(result) != 3:
            raise ValueError("Unexpected Decode Value: %s" % result)
        
        return dumps(result)

    @classmethod
    def decode_race_time(cls, raw, format=None):
        
        pattern = re.compile(r"\d+")
        #result = map(int, re.findall(pattern, raw))
        
        result_from_xml = map(int, re.findall(pattern, raw))
        temp_race_time = result_from_xml[-3:]
        for idx,num in enumerate(temp_race_time):
            if idx==2 and num < 10:
                temp_race_time[idx]='0'+str(num)
        racetime = ':'.join(str(x) for x in temp_race_time)
        result = result_from_xml[0:-3][1::2]
        racetime = racetime.replace(':','.').replace('.',':',1)
        if len(temp_race_time) != 3:
            raise ValueError("Unexpected Race Value: %s" % result)
        
        return dumps(racetime)

    
    @classmethod
    def representation(cls, encoded, format=None):
        values = json.loads(encoded)
        
        if len(values) != cls.HORSES:
            raise ValueError("Unexpected Representation Value: %s" % result)
        
        return "-".join(values)
    
    @classmethod
    def validate_numbers(cls, numbers):
        
        if len(numbers) == cls.HORSES:
            duplicates = [x for x, y in collections.Counter(numbers).items() if y > 1]
            if len(duplicates) >= 1:
                raise LotteryResultsInvalidException("Duplicate Horse Number %s" % numbers)


        if len(numbers) != cls.HORSES:
            raise LotteryResultsInvalidException(
                "Expected %s Horse Numbers, Received %s" % (cls.HORSES, len(numbers)))
        
        for number in numbers:
            if number not in xrange(1, 13) and number is not None:
                raise LotteryResultsInvalidException("Invalid Horse Number %s" % number)
            
        return True

    
    @classmethod
    def earnings(cls, play):
        
        draw = play.ticket.draw
        
        try:
            component = LotteryGameComponent.objects.get(parent__code=cls.NAME, format="DailyDerby")
            draw = LotteryDraw.objects.get(date=draw.date, official=True, component=component)
            race_time = draw.race_time
            
        except LotteryDraw.DoesNotExist:
            if cls.NAME == "DailyDerby":
                #No Powerplay
                # set it to 1
                race_time = None
                return cls._earnings(json.loads(play.ticket.draw.result), race_time, json.loads(play.play))
            else:
                return None

        if not draw.result:
            return None
        
        if not race_time:
            _race_time = None
        else:
            _race_time = json.loads(race_time)
        return cls._earnings(json.loads(play.ticket.draw.result), _race_time, json.loads(play.play))
    
    @classmethod
    def _earnings(cls, results, race_time, play):        
        
        white = results[0:3]
        raceTime = play["raceTime"]
        checking_index_match = [list(white), play['numbers']]
        index_result = [i for i, j in enumerate(zip(*checking_index_match)) if all(j[0]==k for k in j[1:])]
        if race_time != None:
            import re
            pat = re.compile(race_time)
            if pat.match(raceTime):
            #if race_time == raceTime:
                raceTimematch = True
            else:
                raceTimematch = False

            if not white and not raceTimematch == False:
                return 0
            
            if len(index_result) == 0 and raceTimematch == False:
                return 0

            if len(index_result) == 0 and raceTimematch == True:
                return cls.RACETIME_ONLY
                
            if len(index_result) == 1 and index_result == [0]:
                if raceTimematch:
                    return cls.WIN_WITH_RACETIME
                else:
                    return cls.WIN
            
            if len(index_result) == 2 and index_result == [0, 1]:
                if raceTimematch:
                    return cls.EXACTA_WITH_RACETIME
                else:
                    return cls.EXACTA
            
            if len(index_result) == 3 and index_result == [0, 1, 2]:
                if raceTimematch:
                    return cls.JACKPOT        
                else:
                    return cls.TRIFECTA

            if raceTimematch:
                return cls.RACETIME_ONLY
            
            return 0
                    
            raise RuntimeError("Unknown Win State?")

    @classmethod
    def validate_play(cls, data):
        
        if "numbers" not in data:
            raise LotteryPlayInvalidException("Invalid Horse Number")
	if "raceTime" not in data:
	    raise LotteryPlayInvalidException("Race time to more than 1:40:00")

        
        if data["raceTime"]:
            import re
            import datetime
            start_range = datetime.time(0,1,40,00)
            end_range = datetime.time(0,1,49,99)
            split_racetime = data["raceTime"].replace(':','.').replace('.',':',1)
            split_racetime = re.split('[: .]',split_racetime)
            actual_racetime = \
            datetime.time(0,int(split_racetime[0]), int(split_racetime[1]), int(split_racetime[2]))
            if not actual_racetime > start_range:
                raise LotteryPlayInvalidException("Invalid race time entered,\
                please enter race time from 1:40:00 to 1:49:99 %s" % data)
            if not actual_racetime <= end_range:
                raise LotteryPlayInvalidException("Invalid race time entered,\
                please enter race time from 1:40:00 to 1:49:99 %s" % data)
        try:
            cls.validate_numbers(data["numbers"])
        except LotteryResultsInvalidException as e:
            raise LotteryPlayInvalidException(str(e))
        
        return True

 
    @classmethod
    def get_component(cls, record):
        
        if "drawTime" not in record or record["drawTime"] not in [6]:
            raise LotteryPlayInvalidException("Unknown Draw Time")
        
        if record["drawTime"] == 6:
            return LotteryGameComponent.objects.get(identifier="Daily Derby")
        
    @classmethod
    def get_game_meta(cls, component):
        if component.identifier == "DailyDerby":
            return {"drawTime": 6}
        
        return {"drawTime": 6}

    @classmethod
    def preprocess_ticket(cls, record):
        processed = []

        for i, play in enumerate(record["lines"]):
            _play = {}
            
            for field in ["numbers", "raceTime"]:
                if field in play:
                    _play[field] = play[field]

            processed.append(_play)
            
        record["lines"] = processed
        
        return record
     
from yoolotto.lottery.game.manager import GameManager
GameManager.register(DailyDerbyGame)
