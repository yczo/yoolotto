import json
import ast

class TicketRepresentation(object):
    def __init__(self, ticket):
        self.ticket = ticket
        self.plays = self.ticket.plays.all()
        
class TicketRepresentationShortList(TicketRepresentation):
    def __call__(self, *args, **kwargs):
        import datetime
        from yoolotto.lottery.enumerations import EnumerationManager
        from yoolotto.lottery.models import LotteryGameComponent
        draw = self.ticket.draw
        plays = self.ticket.plays.exclude(play=None)
        handler = draw.component.parent.handler
        lg_obj = LotteryGameComponent.objects.filter(parent=draw.component.parent.pk)[0]
        state = self.ticket.division.remote_id   
        
        _result = {
            "game": {
                "id": draw.component.parent.pk,
                "name": draw.component.parent.name,
                "gameType": EnumerationManager.game_reverse(draw.component.parent.pk),
                "state":state,
            },
            "current_date":datetime.date.today().strftime("%Y-%m-%d"),
            #"plays": self.ticket.plays.count(),
            "plays": [],
            "draw": draw.representation(ticket=self.ticket),
            "ticket_id": self.ticket.pk,
            "winnings": str(self.ticket.winnings) if self.ticket.winnings is not None else None,
            "coins": self.ticket.coin_representation(),
            "checked": self.ticket.all_checked,
            "representation": self.__class__.__name__
        }
	pending = False
	try:
		if not self.ticket.ticket_submissions.all()[0].available and not self.ticket.ticket_submissions.all()[0].rejected:
			pending = True
	except:
		pending = True
        
	if not self.ticket.all_checked and not self.ticket.draw.result:
            _result["gameState"] = 0
    	elif not self.ticket.all_checked and self.ticket.draw.result :
    	    _result["gameState"] = 1
        elif not self.ticket.ticket_submissions.all()[0].available and not self.ticket.ticket_submissions.all()[0].rejected:
            _result["gameState"] = 4
    	elif self.ticket.ticket_submissions.all()[0].rejected:
    	    _result["gameState"] = 3
        else:
            _result["gameState"] = 2
        
        _result["game"].update(handler.get_game_meta(draw.component))

        for play in plays:
	    ticket_image = self.ticket.ticket_submissions.all()
            if ticket_image :
                image = str(ticket_image[0].image_first)
            else:
                image = None
            try:
		play_data = json.loads(play.play)
		raw = handler.postprocess_play(play_data)
	    except:
		play_data = json.loads(json.dumps(play.play))
		raw = handler.postprocess_play(play_data)
	    try:
		raw_data = json.loads(raw)
	    except:
		raw_data = eval(str(raw))

	    print 1111111111111111111111111111111111111111111111111111111111111111111
	    print  raw_data
	    print 22222222222222222222222222222222222222222222222222222
            _play = {
                "play_id": play.pk,
                "winnings": str(play.winnings) if play.winnings is not None else None,
                "play": raw_data,
                "checked":play.submission.checked,
                "submission": play.submission_old,
		"image": image,
            }

	    
            '''try:

	    	play_raw = json.loads(_play['play'])
            	_play['play'] = play_raw
	    except:

		_play['play'] = handler.postprocess_play(play_data)'''
            
            _play["baseWinnings"] = str(play.winnings_base) if play.winnings_base else _play["winnings"]
            _play["sumWinnings"] = str(play.winnings_sum) if play.winnings_sum else None
            
            _result["plays"].append(_play)
        
        return _result
        
class TicketRepresentationGeneric(TicketRepresentation):
    def __call__(self, *args, **kwargs):
        import datetime
        from yoolotto.lottery.enumerations import EnumerationManager
        from yoolotto.lottery.models import LotteryGameComponent
        
        ticket = self.ticket
        plays = ticket.plays.exclude(play=None)
        draw = ticket.draw
        handler = draw.component.parent.handler
        lg_obj = LotteryGameComponent.objects.filter(parent=draw.component.parent.pk)[0]
        state = self.ticket.division.remote_id

        _result = {
            "game": {
                "id": draw.component.parent.pk,
                "name": draw.component.parent.name,
                "gameType": EnumerationManager.game_reverse(draw.component.parent.pk),
                "state":state,
            },                
            "draw": draw.representation(ticket=self.ticket),
            "ticket_id": ticket.pk,
            "winnings": str(ticket.winnings) if ticket.winnings is not None else None,
            "current_date":datetime.date.today().strftime("%Y-%m-%d"),
            "plays": [],
            "coupon": ticket.coupon_representation(),
            "coins": ticket.coin_representation(),
            "checked": ticket.all_checked,
            "representation": self.__class__.__name__
        }
	pending = False
        try:    
                if not self.ticket.ticket_submissions.all()[0].available and not self.ticket.ticket_submissions.all()[0].rejected:
                        pending = True
        except:
                pending = True

        
	if not self.ticket.ticket_submissions.all()[0].available and not self.ticket.ticket_submissions.all()[0].rejected:
            _result["gameState"] = 4
	elif not self.ticket.all_checked and self.ticket.draw.result:
	    _result["gameState"] = 1
        elif not self.ticket.all_checked and not self.ticket.draw.result:
            _result["gameState"] = 0
	elif self.ticket.ticket_submissions.all()[0].rejected:
	    _result["gameState"] = 3
        else:
            _result["gameState"] = 2
        
        _result["game"].update(handler.get_game_meta(draw.component))
        
        for play in plays:
	    ticket_image = self.ticket.ticket_submissions.all()
            if ticket_image :
                image = str(ticket_image[0].image_first)
            else:
                image = None
            try:
                play_data = json.loads(play.play)
                raw = handler.postprocess_play(play_data)
            except:
                play_data = json.loads(json.dumps(play.play))
                raw = handler.postprocess_play(play_data)
            try:
                raw_data = json.loads(raw)
            except:
                raw_data = eval(str(raw))

            _play = {
                "play_id": play.pk,
                "winnings": str(play.winnings) if play.winnings is not None else None,
                "play": raw_data, 
                "checked":play.submission.checked,
                "submission": play.submission_old,
		"image": image,
            }
	    '''try:
	    	play_raw = json.loads(_play['play'])
            	_play['play'] = play_raw
	    except:
		 
                _play['play'] = handler.postprocess_play(play_data)'''

            
            _play["baseWinnings"] = str(play.winnings_base) if play.winnings_base else _play["winnings"]
            _play["sumWinnings"] = str(play.winnings_sum) if play.winnings_sum else None
            
            _result["plays"].append(_play)
        
        return _result
