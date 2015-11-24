from apns import APNs, Payload, PayloadTooLargeError
from django.conf import settings

class APNSender(object):
    def __init__(self, to, text, badge=0, custom={}):
        self._to = to
        self._text = text
        self._badge = badge
        self._custom = custom
        
    def send(self):
        apn = APNs(use_sandbox=settings.NOTIFICATIONS_SANDBOX, 
            cert_file=settings.NOTIFICATIONS_APNS_CERT_PATH, 
            key_file=settings.NOTIFICATIONS_APNS_KEY_PATH)
        
        payload = Payload(alert=self._text, badge=self._badge, custom=self._custom)
        apn.gateway_server.send_notification(self._to, payload)