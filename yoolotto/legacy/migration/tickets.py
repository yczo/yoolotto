import json
import uuid
import datetime

from yoolotto.legacy.models import Tickets as LegacyTicket, Users as LegacyUser
from yoolotto.lottery.models import LotteryDraw, LotteryGame, LotteryTicket, LotteryTicketPlay
from yoolotto.user.models import YooLottoUser
from yoolotto.util.serialize import dumps

class MigrateTickets(object):
    def __init__(self, source, target):
        if not isinstance(source, LegacyUser):
            raise RuntimeError()
        
        if not isinstance(target, YooLottoUser):
            raise RuntimeError()
    
        self._source = source
        self._target = target
    
    def run(self):        
        raise RuntimeError("Deprecated")
        
        for ticket in LegacyTicket.objects.filter(uid=self._source.pk):
            if ticket.type == "megamillions":
                game = LotteryGame.objects.get(code="MegaMillions")
            elif ticket.type == "powerball":
                game = LotteryGame.objects.get(code="Powerball")
            else:
                continue
            
            # Get Plays
            try:
                plays = json.loads(ticket.rows)
            except:
                continue
            
            # Get Component/ Handler
            handler = game.handler
            component = game.components.all()[0]
            date = datetime.datetime.fromtimestamp(ticket.dtime).date()
            
            submission = uuid.uuid4().hex[:16]
                        
            # Convert Tickets
            validated = []
            for play in plays:
                if game.code == "Powerball":
                    _play = {
                        u'numbers': play["numbers"] + [play["power"]], 
                        u'multiplier': play["pp"]
                    }
                else:
                    _play = {
                        u'numbers': play["numbers"] + [play["mega"]], 
                        u'multiplier': play["mp"]
                    }
                
                try:
                    handler.validate_play(_play)
                except Exception as e:
                    continue
                
                validated.append(_play)
                
            if not validated:
                continue
                            
            # Retrieve Draw 
            draw, created = LotteryDraw.objects.get_or_create(component=component, date=date)
                        
            # Retrieve Ticket
            ticket, created = LotteryTicket.objects.get_or_create(user=self._target, draw=draw)
            
            for _play in validated:
                serialized = dumps(_play)
                
                # Retrieve Play
                play, created = LotteryTicketPlay.objects.get_or_create(ticket=ticket, 
                    play=serialized)
                
                if not created:
                    continue
                
                # Calculate Winnings (if applicable)
                play.submission = submission
                play.update_winnings(save=True)
                
            # Mark Checked
            ticket.checked = True
            ticket.update(full=True)