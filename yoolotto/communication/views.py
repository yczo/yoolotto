import socket
import json

from django.views.generic import View

from yoolotto.communication.apn import APNSender
from yoolotto.communication.gcm_sender import GCMSender
from yoolotto.rest.decorators import rest
from yoolotto.rest.exceptions import WebServiceAuthenticationFailed

class Notification(View):
    @rest
    def post(self, request):
        # Retrieve LMT A Record + Artur's 360 Nueces IP
        AUTHORIZED = [socket.gethostbyname("rio.leanmeantech.com"), "64.30.186.209", "127.0.0.1"]
        TOKEN = "uiiXC1vbTd45QR7O3Q8FlGHclXb9iAcZfY0GM0UVYaMnvd5F6ARMVFMeXND9rZQ4"
        
        if request.GET.get("token", None) != TOKEN:            
            raise WebServiceAuthenticationFailed()
        
        if request.META["REMOTE_ADDR"] not in AUTHORIZED:
            raise WebServiceAuthenticationFailed()
        
        protocol = request.GET.get("type", None)
        if protocol not in ["apns", "gcm"]:
            raise RuntimeError("specify a GET var named 'type' that is either apns or gcm")
        
        device_token = request.GET.get("device_token", None)
        if not device_token:
            raise RuntimeError("specify a GET var named 'device_token' that contains the push notification token")
        
        data = json.loads(request.body)
                
        if protocol == "apns":
            if "text" not in data:
                raise RuntimeError("expecting text in post body")
            
            apn = APNSender(device_token, text=data["text"], 
                custom=data.get("custom", {}), badge=data.get("badge", 0))
            apn.send()
            
            return data
        
        elif protocol == "gcm":
            gcm = GCMSender(to=[device_token], data=data)
            response = gcm.send()
            
            return gcm.send()
        
        raise RuntimeError()