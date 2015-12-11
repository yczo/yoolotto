class GameManager(object):
    GAMES = {}
        
    @classmethod
    def get(cls, name):
        if name not in cls.GAMES:
            raise RuntimeError("Unknown Game: %s" % name)
        
        return cls.GAMES[name]
    
    @classmethod
    def register(cls, game):
        if game.NAME not in cls.GAMES:
            cls.GAMES[game.NAME] = game
        
from yoolotto.lottery.game import powerball
from yoolotto.lottery.game import powerballca
from yoolotto.lottery.game import megamillionsca
from yoolotto.lottery.game import megamillions
from yoolotto.lottery.game import lottotexas
from yoolotto.lottery.game import dailyfour
from yoolotto.lottery.game import pickthree
from yoolotto.lottery.game import twostep
from yoolotto.lottery.game import cashfive
from yoolotto.lottery.game import allornothing
from yoolotto.lottery.game import superlottoplus
from yoolotto.lottery.game import dailythree
from yoolotto.lottery.game import dailyfor
from yoolotto.lottery.game import fantasyfive
from yoolotto.lottery.game import dailyderby
#from yoolotto.lottery.game import fantasymegamillions
#from yoolotto.lottery.game import fantasypowerball