"""
Basic Validation of PowerballGame Validation + Earnings Logic

"""
from django.test import TestCase
from yoolotto.lottery.game.base import LotteryResultsInvalidException, LotteryPlayInvalidException
from yoolotto.lottery.game.manager import GameManager

class PowerballGameTest(TestCase):
    HANDLER = GameManager.get("Powerball")
    
    def test_powerball_game_validate(self):
        valid = [
            [1, 2, 3, 4, 5, 6],
            [7, 8, 9, 10, 11, 12],
            [13, 14, 15, 16, 17, 18],
            [19, 20, 21, 22, 23, 24],
            [25, 26, 27, 28, 29, 30],
            [31, 32, 33, 34, 35, 35],
            [36, 37, 38, 39, 40, 17],
            [41, 42, 43, 44, 45, 31],
            [46, 47, 48, 49, 50, 21],
            [51, 52, 53, 54, 55, 15],
            [56, 55, 54, 53, 52, 7]
        ]
        
        for numbers in valid:
            self.assertTrue(self.HANDLER.validate_numbers(numbers))
        
        invalid = [
            [1, 1, 2, 3, 4, 5],
            [1, 100, 2, 3, 4, 5],
            [1, 6, 2, 3, 4, 37],
            [1, 57, 2, 3, 4, 5]
        ]
        
        for numbers in invalid:
            with self.assertRaises(LotteryResultsInvalidException):
                self.HANDLER.validate_numbers(numbers)
                
    def test_powerball_play_validate(self):
        valid = [
             {'numbers': [1, 2, 3, 4, 5, 6], 'multiplier': True},
             {'numbers': [7, 8, 9, 10, 11, 12], 'multiplier': True},
             {'numbers': [13, 14, 15, 16, 17, 18], 'multiplier': False},
             {'numbers': [19, 20, 21, 22, 23, 24], 'multiplier': True},
             {'numbers': [25, 26, 27, 28, 29, 30], 'multiplier': True},
             {'numbers': [31, 32, 33, 34, 35, 35], 'multiplier': False},
             {'numbers': [36, 37, 38, 39, 40, 17], 'multiplier': True},
             {'numbers': [41, 42, 43, 44, 45, 31], 'multiplier': True},
             {'numbers': [46, 47, 48, 49, 50, 21], 'multiplier': False},
             {'numbers': [51, 52, 53, 54, 55, 15], 'multiplier': True},
             {'numbers': [56, 55, 54, 53, 52, 7], 'multiplier': True}
        ]
        
        for play in valid:
            self.assertTrue(self.HANDLER.validate_play(play))
        
        invalid = [
             {'numbers': [1, 57, 3, 4, 5, 6], 'multiplier': True},
             {'numbers': [7, 8, 9, 10, 10, 12], 'multiplier': True},
             {'numbers': [-1, 14, 15, 16, 17, 18], 'multiplier': False},
             {'numbers': [19, 20, 21, 22, 23, 37], 'multiplier': True},
             {'numbers': [31, 32, 33, 34, 35, 35], 'multiplier': "True"}
        ]
        
        for data in invalid:
            with self.assertRaises(LotteryPlayInvalidException):
                self.HANDLER.validate_play(data)
                
    def test_powerball_earnings(self):
        results = [10, 20, 30, 40, 50, 10]
        
        expected = [
            # 0
            [0, {"numbers": [1, 2, 3, 4, 5, 5], "multiplier": True}],
            [4, {"numbers": [1, 2, 3, 4, 5, 10], "multiplier": False}],
            
            # 1
            [12, {"numbers": [1, 2, 3, 4, 5, 10], "multiplier": True}],
            [4, {"numbers": [10, 2, 3, 4, 5, 10], "multiplier": False}],
            
            # 2
            [12, {"numbers": [10, 2, 3, 4, 5, 10], "multiplier": True}],
            [7, {"numbers": [10, 20, 3, 4, 5, 10], "multiplier": False}],
            [14, {"numbers": [10, 20, 3, 4, 5, 10], "multiplier": True}],
            
            # 3
            [7, {"numbers": [10, 20, 30, 4, 5, 5], "multiplier": False}],
            [14, {"numbers": [10, 20, 30, 4, 5, 5], "multiplier": True}],
            [100, {"numbers": [10, 20, 30, 4, 5, 10], "multiplier": False}],
            [200, {"numbers": [10, 20, 30, 4, 5, 10], "multiplier": True}],
            
            # 4
            [100, {"numbers": [10, 20, 30, 40, 5, 5], "multiplier": False}],
            [200, {"numbers": [10, 20, 30, 40, 5, 5], "multiplier": True}],
            [10000, {"numbers": [10, 20, 30, 40, 5, 10], "multiplier": False}],
            [40000, {"numbers": [10, 20, 30, 40, 5, 10], "multiplier": True}],
            
            # 5
            [1e6, {"numbers": [10, 20, 30, 40, 50, 5], "multiplier": False}],
            [2e6, {"numbers": [10, 20, 30, 40, 50, 5], "multiplier": True}],
            
            # Jackpot
            [self.HANDLER.JACKPOT, {"numbers": [10, 20, 30, 40, 50, 10], "multiplier": True}],
            [self.HANDLER.JACKPOT, {"numbers": [10, 20, 30, 40, 50, 10], "multiplier": True}],
        ]
        
        for play in expected:
            self.assertEqual(play[0], self.HANDLER._earnings(results, play[1]))
            
class DailyFourGameTest(TestCase):
    HANDLER = GameManager.get("DailyFour")
    
    def test_dailyfour_game_validate(self):
        valid = [
            [1, 2, 3, 4],
            [0, 1, 2, 3],
            [9, 8, 7, 6],
            [5, 6, 0, 9],
            [0, 0, 0, 0],
            [9, 9, 9, 9]
        ]
        
        for numbers in valid:
            self.assertTrue(self.HANDLER.validate_numbers(numbers))
        
        invalid = [
            [1, 1, 2, 10],
            [0, 0, 0, 10],
            [5, 5, 5, 25]
        ]
        
        for numbers in invalid:
            with self.assertRaises(LotteryResultsInvalidException):
                self.HANDLER.validate_numbers(numbers)
        
    def test_dailyfour_play_validate(self):
        valid = [
             {"baseWager":"0.50","lineType":"BOX","numbers":[7,5,2,4],"sum":18,"sumWager":"1.00"},
             {"baseWager":"1.00","lineType":"STRBOX","numbers":[6,2,7,1],"sum":16,"sumWager":"2.00"},
             {"baseWager":"0.50","lineType":"FRONT","numbers":[9,2,0,0],"sum":11,"sumWager":"3.00"},
             {"baseWager":"0.50","lineType":"MID","numbers":[0,8,7,0],"sum":15,"sumWager":"4.00"},
             {"baseWager":"0.50","lineType":"BACK","numbers":[0,0,1,2],"sum":None,"sumWager":None},
             {"baseWager":"0.50","lineType":"MID","numbers":[0,0,7,0],"sum":None,"sumWager":None},
             {"baseWager":"0.50","lineType":"STR","numbers":[0,0,0,0],"sum":0,"sumWager":"5.00"},
             {"baseWager":"0.50","lineType":"STR","numbers":[9,9,9,9],"sum":36,"sumWager":"5.00"}
        ]
        
        for play in valid:
            self.assertTrue(self.HANDLER.validate_play(play))
        
        invalid = [
             {"baseWager":None,"lineType":"BOX","numbers":[7,5,2,4],"sum":18,"sumWager":"1.00"},
             {"baseWager":"1.00","lineType":"DERP","numbers":[6,2,7,1],"sum":16,"sumWager":"2.00"},
             {"baseWager":"0.50","lineType":"FRONT","numbers":[9,2,0,0],"sum":11,"sumWager":"None"},
        ]
        
        for data in invalid:
            with self.assertRaises(LotteryPlayInvalidException):
                self.HANDLER.validate_play(data)
                
    def test_dailyfour_earnings(self):
        results = [0, 0, 0, 0]
        
        expected = [
            # STR
            [7500, {"baseWager":"0.50","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"1.00"}],
            [10000, {"baseWager":"1.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"1.00"}],
            [50000, {"baseWager":"5.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"5.00"}],
            [10000, {"baseWager":"2.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            [25000, {"baseWager":"5.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            
            [0, {"baseWager":"0.50","lineType":"STR","numbers":[9, 9, 9, 9],"sum":36,"sumWager":"1.00"}],
            [0, {"baseWager":"1.00","lineType":"STR","numbers":[9, 9, 9, 9],"sum":36,"sumWager":"1.00"}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[9, 9, 9, 9],"sum":36, "sumWager":"5.00"}],
            
            [0, {"baseWager":"0.50","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19,"sumWager":"1.00"}],
            [0, {"baseWager":"1.00","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19,"sumWager":"1.00"}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19, "sumWager":"5.00"}],
            
            # FRONT
            [25, {"baseWager":"0.50","lineType":"FRONT","numbers":[0, 0, None, None],"sum":None,"sumWager": None}],
            [250, {"baseWager":"5.00","lineType":"FRONT","numbers":[0, 0, None, None],"sum":None,"sumWager": None}],
            [5025, {"baseWager":"0.50","lineType":"FRONT","numbers":[0, 0, None, None],"sum":0,"sumWager": "1.00"}],
            [15250, {"baseWager":"5.00","lineType":"FRONT","numbers":[0, 0, None, None],"sum":0,"sumWager": "3.00"}],
            
            # MID
            [25, {"baseWager":"0.50","lineType":"MID","numbers":[None, 0, 0, None],"sum":None,"sumWager": None}],
            [250, {"baseWager":"5.00","lineType":"MID","numbers":[None, 0, 0, None],"sum":None,"sumWager": None}],
            [5025, {"baseWager":"0.50","lineType":"MID","numbers":[None, 0, 0, None],"sum":0,"sumWager": "1.00"}],
            [15250, {"baseWager":"5.00","lineType":"MID","numbers":[None, 0, 0, None],"sum":0,"sumWager": "3.00"}],
            
            # BACK
            [25, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 0, 0],"sum":None,"sumWager": None}],
            [250, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 0, 0],"sum":None,"sumWager": None}],
            [5025, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 0, 0],"sum":0,"sumWager": "1.00"}],
            [15250, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 0, 0],"sum":0,"sumWager": "3.00"}],
            
            # STRBOX
            # This particular play is nonsense 
            [7400, {"baseWager":"0.50","lineType":"STRBOX","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"0.50"}],
            [0, {"baseWager":"0.50","lineType":"STRBOX","numbers":[0, 0, 0, 1],"sum":1,"sumWager":"0.50"}],
            
            
            # COMBO
            # This particular play is nonsense 
            [2500, {"baseWager":"0.50","lineType":"COMBO","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            [0, {"baseWager":"0.50","lineType":"COMBO","numbers":[0, 0, 0, 1],"sum":1,"sumWager":"0.50"}],
            
        ]
        
        for play in expected:
            try:
                self.assertEqual(play[0], self.HANDLER._earnings(results, play[1]))
            except:
                print play[1]
                raise
            
        
        # More Results
        results = [9, 9, 9, 9]
        
        expected = [
            # STR
            [0, {"baseWager":"0.50","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"1.00"}],
            [0, {"baseWager":"1.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"1.00"}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"5.00"}],
            [0, {"baseWager":"2.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            
            [7500, {"baseWager":"0.50","lineType":"STR","numbers":[9, 9, 9, 9],"sum":36,"sumWager":"1.00"}],
            [10000, {"baseWager":"1.00","lineType":"STR","numbers":[9, 9, 9, 9],"sum":36,"sumWager":"1.00"}],
            [50000, {"baseWager":"5.00","lineType":"STR","numbers":[9, 9, 9, 9],"sum":36, "sumWager":"5.00"}],
            
            [0, {"baseWager":"0.50","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19,"sumWager":"1.00"}],
            [0, {"baseWager":"1.00","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19,"sumWager":"1.00"}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19, "sumWager":"5.00"}],
            
            # FRONT
            [0, {"baseWager":"0.50","lineType":"FRONT","numbers":[0, 0, None, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"5.00","lineType":"FRONT","numbers":[0, 0, None, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"0.50","lineType":"FRONT","numbers":[0, 0, None, None],"sum":0,"sumWager": "1.00"}],
            [0, {"baseWager":"5.00","lineType":"FRONT","numbers":[0, 0, None, None],"sum":0,"sumWager": "3.00"}],
            
            [25, {"baseWager":"0.50","lineType":"FRONT","numbers":[9, 9, None, None],"sum":None,"sumWager": None}],
            [250, {"baseWager":"5.00","lineType":"FRONT","numbers":[9, 9, None, None],"sum":None,"sumWager": None}],
            [25, {"baseWager":"0.50","lineType":"FRONT","numbers":[9, 9, None, None],"sum":18,"sumWager": "1.00"}],
            [250, {"baseWager":"5.00","lineType":"FRONT","numbers":[9, 9, None, None],"sum":18,"sumWager": "3.00"}],
            
            # MID
            [0, {"baseWager":"0.50","lineType":"MID","numbers":[None, 0, 0, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"5.00","lineType":"MID","numbers":[None, 0, 0, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"0.50","lineType":"MID","numbers":[None, 0, 0, None],"sum":0,"sumWager": "1.00"}],
            [0, {"baseWager":"5.00","lineType":"MID","numbers":[None, 0, 0, None],"sum":0,"sumWager": "3.00"}],
            
            # BACK
            [0, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 0, 0],"sum":None,"sumWager": None}],
            [0, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 0, 0],"sum":None,"sumWager": None}],
            [0, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 0, 0],"sum":0,"sumWager": "1.00"}],
            [0, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 0, 0],"sum":0,"sumWager": "3.00"}],
            
            # STRBOX
            # This particular play is nonsense 
            [0, {"baseWager":"0.50","lineType":"STRBOX","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"0.50"}],
            [7400, {"baseWager":"0.50","lineType":"STRBOX","numbers":[9, 9, 9, 9],"sum":36,"sumWager":"0.50"}],
            [0, {"baseWager":"0.50","lineType":"STRBOX","numbers":[0, 0, 0, 1],"sum":1,"sumWager":"0.50"}],
            
            
            # COMBO
            # This particular play is nonsense 
            [2500, {"baseWager":"0.50","lineType":"COMBO","numbers":[9, 9, 9, 9],"sum":None,"sumWager":None}],
            [0, {"baseWager":"0.50","lineType":"COMBO","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            [0, {"baseWager":"0.50","lineType":"COMBO","numbers":[0, 0, 0, 1],"sum":1,"sumWager":"0.50"}],

        ]
        
        for play in expected:
            try:
                self.assertEqual(play[0], self.HANDLER._earnings(results, play[1]))
            except:
                print play[1]
                raise
            
        # Some Interesting Numbers
        results = [4, 5, 8, 2]
        
        expected = [
            # STR
            [0, {"baseWager":"0.50","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"1.00"}],
            [0, {"baseWager":"1.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"1.00"}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"5.00"}],
            [0, {"baseWager":"2.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            
            
            [2503, {"baseWager":"0.50","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19,"sumWager":"0.50"}],
            [5007, {"baseWager":"1.00","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19,"sumWager":"1.00"}],
            [25037, {"baseWager":"5.00","lineType":"STR","numbers":[4, 5, 8, 2],"sum":19, "sumWager":"5.00"}],
            
            [0, {"baseWager":"0.50","lineType":"STR","numbers":[4, 5, 8, 3],"sum":19,"sumWager":"1.00"}],
            [0, {"baseWager":"1.00","lineType":"STR","numbers":[4, 5, 8, 3],"sum":19,"sumWager":"1.00"}],
            [0, {"baseWager":"5.00","lineType":"STR","numbers":[4, 5, 8, 3],"sum":19, "sumWager":"5.00"}],
            
            # FRONT
            [0, {"baseWager":"0.50","lineType":"FRONT","numbers":[0, 0, None, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"5.00","lineType":"FRONT","numbers":[0, 0, None, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"0.50","lineType":"FRONT","numbers":[0, 0, None, None],"sum":0,"sumWager": "1.00"}],
            [0, {"baseWager":"5.00","lineType":"FRONT","numbers":[0, 0, None, None],"sum":0,"sumWager": "3.00"}],
            
            [25, {"baseWager":"0.50","lineType":"FRONT","numbers":[4, 5, None, None],"sum":None,"sumWager": None}],
            [250, {"baseWager":"5.00","lineType":"FRONT","numbers":[4, 5, None, None],"sum":None,"sumWager": None}],
            [25, {"baseWager":"0.50","lineType":"FRONT","numbers":[4, 5, None, None],"sum":9,"sumWager": "1.00"}],
            [250, {"baseWager":"5.00","lineType":"FRONT","numbers":[4, 5, None, None],"sum":9,"sumWager": "3.00"}],
            
            # MID
            [0, {"baseWager":"0.50","lineType":"MID","numbers":[None, 0, 0, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"5.00","lineType":"MID","numbers":[None, 0, 0, None],"sum":None,"sumWager": None}],
            [0, {"baseWager":"0.50","lineType":"MID","numbers":[None, 0, 0, None],"sum":0,"sumWager": "1.00"}],
            [0, {"baseWager":"5.00","lineType":"MID","numbers":[None, 0, 0, None],"sum":0,"sumWager": "3.00"}],
            
            [25, {"baseWager":"0.50","lineType":"MID","numbers":[None, 5, 8, None],"sum":None,"sumWager": None}],
            [250, {"baseWager":"5.00","lineType":"MID","numbers":[None, 5, 8, None],"sum":None,"sumWager": None}],
            [25, {"baseWager":"0.50","lineType":"MID","numbers":[None, 5, 8, None],"sum":13,"sumWager": "1.00"}],
            [250, {"baseWager":"5.00","lineType":"MID","numbers":[None, 5, 8, None],"sum":13,"sumWager": "3.00"}],
            
            # BACK
            [0, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 0, 0],"sum":None,"sumWager": None}],
            [0, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 0, 0],"sum":None,"sumWager": None}],
            [0, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 0, 0],"sum":0,"sumWager": "1.00"}],
            [0, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 0, 0],"sum":0,"sumWager": "3.00"}],
            
            [25, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 8, 2],"sum":None,"sumWager": None}],
            [250, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 8, 2],"sum":None,"sumWager": None}],
            [25, {"baseWager":"0.50","lineType":"BACK","numbers":[None, None, 8, 2],"sum":10,"sumWager": "1.00"}],
            [250, {"baseWager":"5.00","lineType":"BACK","numbers":[None, None, 8, 2],"sum":10,"sumWager": "3.00"}],
            
            # STRBOX
            # This particular play is nonsense 
            [0, {"baseWager":"0.50","lineType":"STRBOX","numbers":[0, 0, 0, 0],"sum":0,"sumWager":"0.50"}],
            [0, {"baseWager":"0.50","lineType":"STRBOX","numbers":[9, 9, 9, 9],"sum":36,"sumWager":"0.50"}],
            [0, {"baseWager":"0.50","lineType":"STRBOX","numbers":[0, 0, 0, 1],"sum":1,"sumWager":"0.50"}],
            
            [5200, {"baseWager":"1.00","lineType":"STRBOX","numbers":[4, 5, 8, 2],"sum":None,"sumWager":None}],
            [5237, {"baseWager":"1.00","lineType":"STRBOX","numbers":[4, 5, 8, 2],"sum":19,"sumWager":"5.00"}],
            [100, {"baseWager":"0.50","lineType":"STRBOX","numbers":[2, 8, 5, 4],"sum":None,"sumWager":None}],
            [100, {"baseWager":"0.50","lineType":"STRBOX","numbers":[4, 8, 2, 5],"sum":None,"sumWager":None}],
            [100, {"baseWager":"0.50","lineType":"STRBOX","numbers":[5, 4, 8, 2],"sum":None,"sumWager":None}],            
            
            # COMBO
            [2500, {"baseWager":"0.50","lineType":"COMBO","numbers":[4, 5, 8, 2],"sum":None,"sumWager":None}],
            [2500, {"baseWager":"0.50","lineType":"COMBO","numbers":[5, 8, 2, 4],"sum":None,"sumWager":None}],
            
            # This would be a $120 ticket
            [25000, {"baseWager":"5.00","lineType":"COMBO","numbers":[4, 5, 8, 2],"sum":None,"sumWager":None}],
            [25000, {"baseWager":"5.00","lineType":"COMBO","numbers":[5, 8, 2, 4],"sum":None,"sumWager":None}],
            
            [0, {"baseWager":"0.50","lineType":"COMBO","numbers":[0, 0, 0, 0],"sum":None,"sumWager":None}],
            [0, {"baseWager":"0.50","lineType":"COMBO","numbers":[0, 0, 0, 1],"sum":1,"sumWager":"0.50"}],
        ]
        
        for play in expected:
            try:
                self.assertEqual(play[0], self.HANDLER._earnings(results, play[1]))
            except:
                print play[1]
                raise