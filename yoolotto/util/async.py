import logging
import traceback

from functools import wraps

from django.conf import settings
from yoolotto.rest.error import format_error

logger = logging.getLogger("yoolotto.uncaught")

def report_errors(fn):
    @wraps(fn)
    def inner(*args, **kwargs):
        try:
            return fn(*args, **kwargs)
        except Exception as e:
            _err = ""
            
            try:
                _err = format_error(None, e)
            except:
                _err = str(e)
            
            try:    
                logger.error(_err)
            except:
                _err += "\n !COULD NOT LOG EXCEPTION LOCALLY!"
            
            if settings.ERROR_NOTIFICATION:
                try:
                    from yoolotto.communication.email import EmailSender
                    email = EmailSender(settings.ERROR_EMAIL, "YL Exception: " + str(e), 
                        text=_err)
                    email.send()
                except:
                    logger.error("\n\nERROR HANDLING EXCEPTION")
                    logger.error(traceback.format_exc() + "\n\n")
            raise
        
    return inner