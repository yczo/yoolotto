import os
import urllib2

from django.conf import settings

class LotteryTicketDataProviderFeedException(Exception): pass

class LotteryTicketDataProvider(object):
    """
    This class is responsible for retrieving the data feed
    
    """
    PROVIDER_URL = "http://www.lotterynumbersxml.com/lotterydata/yoolotto.com/jkdladfasdf/lottery.xml"
    
    @staticmethod
    def sample():
        path = os.path.join(settings.SITE_ROOT, "lottery/data/sample.xml")
        fp = open(path)
        result = fp.read()
        fp.close()
        
        return result
    
    @staticmethod
    def remote():
        req = urllib2.Request(LotteryTicketDataProvider.PROVIDER_URL)
        p = urllib2.urlopen(req, timeout=15)
        
        if p:
            return p.read()


class LotteryWinningCADataProvider(object):
    """
    This class is responsible for retrieving the winning data
    
    """
    PROVIDER_URL = "http://www.elottonumbers.com/feed/payout/yoolotto.com/kdadljasdf/payout-CA.xml"
    
    @staticmethod
    def sample():
        path = os.path.join(settings.SITE_ROOT, "lottery/data/sample-ca.xml")
        fp = open(path)
        result = fp.read()
        fp.close()
        
        return result
    
    @staticmethod
    def remote():
        req = urllib2.Request(LotteryWinningCADataProvider.PROVIDER_URL)
        p = urllib2.urlopen(req, timeout=15)
        
        if p:
            return p.read()


class LotteryWinningTXDataProvider(object):
    """
    This class is responsible for retrieving the winning data
    
    """
    PROVIDER_URL = "http://www.elottonumbers.com/feed/payout/yoolotto.com/kdadljasdf/payout-TX.xml"
    
    @staticmethod
    def sample():
        path = os.path.join(settings.SITE_ROOT, "lottery/data/sample-tx.xml")
        fp = open(path)
        result = fp.read()
        fp.close()
        
        return result
    
    @staticmethod
    def remote():
        req = urllib2.Request(LotteryWinningTXDataProvider.PROVIDER_URL)
        p = urllib2.urlopen(req, timeout=15)
        
        if p:
            return p.read()
