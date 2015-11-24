class GeoManager(object):
    HANDLERS = {}
    
    @classmethod
    def get(cls, name):
        if name not in cls.HANDLERS:
            raise RuntimeError("Unknown Handler: %s" % name)
        
        return cls.HANDLERS[name]
    
    @classmethod
    def register(cls, handler):
        if handler.NAME not in cls.HANDLERS:
            cls.HANDLERS[handler.NAME] = handler
            
from yoolotto.coupon.geo.seven_eleven import SevenElevenHandler
from yoolotto.coupon.geo.dickeys import DickeysHandler

GeoManager.register(SevenElevenHandler)
GeoManager.register(DickeysHandler)