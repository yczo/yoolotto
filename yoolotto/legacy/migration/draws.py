import json
import datetime

from yoolotto.legacy.models import Drawings as LegacyDrawings, Drawings2 as LegacyDrawings2

from yoolotto.lottery.game.manager import GameManager
from yoolotto.lottery.models import LotteryGameComponent, LotteryDraw
from yoolotto.util.serialize import dumps

class MigrateDraws(object):
    def __init__(self):
        # Retrieve Components
        self._powerball = LotteryGameComponent.objects.get(identifier="Powerball")
        self._megamillions = LotteryGameComponent.objects.get(identifier="MegaMillions")
        self._megaplier = LotteryGameComponent.objects.get(identifier="Megaplier")
    
    def run(self):
        start = datetime.date(2010, 1, 1)
                        
        # Create Merged Representation
        merged = {}
        for _cls in LegacyDrawings, LegacyDrawings2:
            records = _cls.objects.all()
            for record in records:
                date = datetime.datetime.fromtimestamp(record.dtime).date()
                
                if date < start:
                    continue
                
                if date not in merged:
                    merged[date] = []
                    
                merged[date].append(record)
        
        # Sort
        keys = merged.keys()
        keys.sort()
        
        for key in keys:
            for record in merged[key]:
                _cls = None
                if record.type == "powerball":
                    self._handle_powerball(key, record)
                elif record.type == "megamillions":
                    self._handle_megamillions(key, record)
                else:
                    raise RuntimeError()
    
    def _handle_powerball(self, date, record):
        existing = LotteryDraw.objects.filter(component=self._powerball, 
            date=date)
        
        result = dumps(json.loads(record.winnings))
        
        # Check Data Conflicts
        if existing:
            existing = existing[0]
            if existing.result and existing.result != result:
                raise RuntimeError("Data Mismatch -%s- -%s-" % (existing.result, result))
        
        # Check Data
        handler = GameManager.get(self._powerball.parent.name)
        _result = handler.decode(record.winnings)
        handler.validate_numbers(json.loads(record.winnings))
        
        # Create
        try:
            draw = LotteryDraw.objects.get(component=self._powerball, date=date)
        except LotteryDraw.DoesNotExist:
            draw, created = LotteryDraw.objects.get_or_create(
                component=self._powerball, date=date)
        finally:
            draw.result = _result
            draw.official = True
            draw.save()
            
    def _handle_megamillions(self, date, record):
        winnings = json.loads(record.winnings)
        
        _base = winnings[:-1]
        _mega = winnings[-1:]
        
        base = dumps(_base)
        mega = dumps(_mega)
        
        # Check Existing
        existing = LotteryDraw.objects.filter(component=self._megamillions, 
            date=date)
        if existing:
            existing = existing[0]
            if existing.result and existing.result != base:
                raise RuntimeError("Data Mismatch -%s- -%s-" % (existing.result, base))
        
        # Check Existing (Megaplier)
        existing = LotteryDraw.objects.filter(component=self._megaplier, 
            date=date)
        if existing:
            existing = existing[0]
            if existing.result and existing.result != mega:
                raise RuntimeError("Data Mismatch -%s- -%s-" % (existing.result, mega))
            
        # Check Data
        handler = GameManager.get(self._megamillions.parent.code)
        handler.validate_numbers(_base)
        result_base = handler.decode(base)
        result_mega = handler.decode(mega, "Megaplier")
        
        
        # Create
        try:
            draw = LotteryDraw.objects.get(component=self._megamillions, date=date)
        except LotteryDraw.DoesNotExist:
            draw, created = LotteryDraw.objects.get_or_create(
                component=self._megamillions, date=date)
        finally:
            draw.result = result_base
            draw.official = True
            draw.save()
        
        # Create Megaplier
        try:
            draw = LotteryDraw.objects.get(component=self._megaplier, date=date)
        except LotteryDraw.DoesNotExist:
            draw, created = LotteryDraw.objects.get_or_create(
                component=self._megaplier, date=date)
        finally:
            draw.result = result_mega
            draw.official = True
            draw.save()