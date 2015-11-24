import json
from django.http import HttpResponse

class LotteryPlayInvalidException(Exception):
    def representation(self):
        if not isinstance(self.args[0], list):
            detail = [self.args[0]]
        else:
           # detail = self.args[0]
      	    try: 
	    	detail = [self.args[0][0].split(':')[1]]
	    except Exception as e:
		pass
	  
        
	result = {
            "sucesss": False,
            "type": self.__class__.__name__,
            "error": {
                "detail": detail
            }
        }
        
        return HttpResponse(content=json.dumps(result), status=500, content_type="application/json")
    
class LotteryResultsInvalidException(Exception): pass


class LotteryGame(object):
    JACKPOT = "J"
    
    @classmethod
    def get_component(cls, record):
        """
        Determine the models.LotteryGameComponent to use for the current record,
        return None to let the submitter determine (defaults to first component)
        
        """
        return None

    @classmethod
    def get_component1(cls):
        """
        Determine the models.LotteryGameComponent to use for the current record,
        return None to let the submitter determine (defaults to first component)
        
        """
        return None
    
    @classmethod
    def get_game_meta(cls, component):
        """
        Return meta data (for game representation) based on the component
        
        """
        return {}
    
    @classmethod
    def preprocess_ticket(cls, record):
        return record
    
    @classmethod
    def postprocess_play(cls, record):
        return record
    
#    @classmethod
#    def get_play_meta(cls, component):
#        """
#        Return meta data (for play / line representation) based on the component
#        
#        """
#        return {}
