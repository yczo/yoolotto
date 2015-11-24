class LotteryGameFormatManager(object):
    GAMES = {}
    
    @classmethod
    def get(cls, name):
        if name not in cls.GAMES:
            raise RuntimeError("Unknown Formatter: %s" % name)
        
        return cls.GAMES[name]
    
    @classmethod
    def register(cls, formatter):
        if formatter.NAME not in cls.GAMES:
            cls.GAMES[formatter.NAME] = formatter
            
            
from yoolotto.lottery.format.generic import GenericFormatter
from yoolotto.lottery.format.powerball import PowerballFormatter
from yoolotto.lottery.format.megamillions import MegaMillionsFormatter

LotteryGameFormatManager.register(GenericFormatter)
LotteryGameFormatManager.register(PowerballFormatter)
LotteryGameFormatManager.register(MegaMillionsFormatter)
