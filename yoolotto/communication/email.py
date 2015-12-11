import pystmark

from django.conf import settings

class EmailSender(object):
    def __init__(self, to, subject, text=None, html=None):
        self._to = to
        self._subject = subject
        self._text = text
        self._html = html
        self._sender = None
        
    def send(self):
        pm = pystmark.Message(sender=self._sender if self._sender else settings.POSTMARK_SENDER, 
            to=self._to, subject=self._subject, text=self._text, html=self._html)
        
        
        res = pystmark.send(pm, api_key=settings.POSTMARK_API_KEY)
        res.raise_for_status()