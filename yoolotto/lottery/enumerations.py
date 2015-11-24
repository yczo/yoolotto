"""
Mapping support between iOS client and Backend

//Game types, recall AllOrNothing is out of scope..
[[GameType alloc] initWithName:@"MEGA MILLIONS" apiType:0 enumType:GameTypeMegaMillions]];
[[GameType alloc] initWithName:@"POWERBALL" apiType:1 enumType:GameTypePowerball]];
[[GameType alloc] initWithName:@"LOTTO TEXAS" apiType:2 enumType:GameTypeLottoTexas]];
[[GameType alloc] initWithName:@"PICK 3" apiType:3 enumType:GameTypePick3]];
[[GameType alloc] initWithName:@"DAILY 4" apiType:4 enumType:GameTypeDaily4]];
[[GameType alloc] initWithName:@"CA$H FIVE" apiType:5 enumType:GameTypeCashFive]];
[[GameType alloc] initWithName:@"Two Step" apiType:6 enumType:GameTypeTwoStep]];

//Draw times
[[DrawTime alloc] initWithName:@"Day" apiType:0 enumType:DrawTimeDay];
[[DrawTime alloc] initWithName:@"Night" apiType:1 enumType:DrawTimeNight];
[[DrawTime alloc] initWithName:@"Morning" apiType:2 enumType:DrawTimeMorning];
[[DrawTime alloc] initWithName:@"Evening" apiType:3 enumType:DrawTimeEvening];

//Line types
[LineType alloc] initWithName:@"NORMAL" apiType:0 enumType:LineTypeNormal];
[LineType alloc] initWithName:@"SUM IT UP" apiType:1 enumType:LineTypeSumItUp];
[LineType alloc] initWithName:@"STR" apiType:2 enumType:LineTypeStraight];
[LineType alloc] initWithName:@"BOX" apiType:3 enumType:LineTypeBox];
[LineType alloc] initWithName:@"STRBOX" apiType:4 enumType:LineTypeStraightBox];
[LineType alloc] initWithName:@"FRONT" apiType:5 enumType:LineTypeFront];
[LineType alloc] initWithName:@"MID" apiType:6 enumType:LineTypeMiddle];
[LineType alloc] initWithName:@"BACK" apiType:7 enumType:LineTypeBack];
[LineType alloc] initWithName:@"ANY" apiType:8 enumType:LineTypeAny];
[LineType alloc] initWithName:@"COMBO" apiType:9 enumType:LineTypeCombo];
[LineType alloc] initWithName:@"EXT" apiType:10 enumType:LineTypeExtra];
[LineType alloc] initWithName:@"EXTANY" apiType:11 enumType:LineTypeExtraAny];

// Russell
[self additem:[[LineType alloc] initWithName:@"EXT" apiType:10 enumType:LineTypeExact]];
[self additem:[[LineType alloc] initWithName:@"EXTANY" apiType:11 enumType:LineTypeExactAny]];

"""

from yoolotto.lottery.models import LotteryGame

"""
Enumerations for Games, Objects, Handlers, etc...

Enumeration values (e.g. 0, 1, 2, etc) are client-specific codes for a "Game"
GAMES maps this enumeration to an LotteryGame (via code). 

LotteryGame (DB Object) is mapped to an LotteryGame (Game Handler) via 
model.code == handler.NAME

"""
GAMES = {
    0: LotteryGame.objects.get(code="MegaMillions"),
    1: LotteryGame.objects.get(code="Powerball"),
    2: LotteryGame.objects.get(code="Lotto"),
    3: LotteryGame.objects.get(code="PickThree"),
    4: LotteryGame.objects.get(code="DailyFour"),
    5: LotteryGame.objects.get(code="CashFive"),
    6: LotteryGame.objects.get(code="TwoStep"),
    7: LotteryGame.objects.get(code="AllOrNothing"),
    8: LotteryGame.objects.get(code="SuperLottoPlus"),
    9: LotteryGame.objects.get(code="FantasyFive"),
    10: LotteryGame.objects.get(code="DailyFor"),
    11: LotteryGame.objects.get(code="PowerballCA"),
    12: LotteryGame.objects.get(code="DailyThree"),
    13: LotteryGame.objects.get(code="MegaMillionsCA"),
    14: LotteryGame.objects.get(code="DailyDerby"),
    #15: LotteryGame.objects.get(code = "FantasyMegamillions"),
   # 16: LotteryGame.objects.get(code = "FantasyPowerball")
}


class EnumerationManager(object):
    @staticmethod
    def game(id):
        #print "id",id
        #print "GAMES",GAMES
        if id in GAMES:
            #print "GAMES",GAMES
            return GAMES[id]
        else:
            raise ValueError("Enumeration %s Does Not Exist" % id)
        
    @staticmethod
    def game_reverse(id):
        for k, v in GAMES.iteritems():
            if v.pk == id:
                return k
        
        raise ValueError("Enumeration %s Does Not Exist" % id)
