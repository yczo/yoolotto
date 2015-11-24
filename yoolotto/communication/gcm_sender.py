from django.conf import settings

class GCMSender(object):
    def __init__(self, to, data):
        if not hasattr(to, "__iter__"):
            self._to = [to,]
        
        self._to = to
        self._data = data
        
    def send(self):
        from gcm import GCM
        gcm = GCM(settings.NOTIFICATIONS_GCM_KEY)
        try:
            response = gcm.json_request(registration_ids=self._to, data=self._data)
        except Exception as e:
            
            raise
        return response