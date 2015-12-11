import re
import json
import requests

from django.views.generic import View
from django.shortcuts import render_to_response
from django.http import HttpResponse

class PromoLanding(View):
    def get(self, request, mode=None):
#        email = request.COOKIES.get("574a50850e524620a891fb8470f69b51", None)
#        if email:
#            return self._handle_email(email, request.META.get('HTTP_X_FORWARDED_FOR', request.META.get("REMOTE_ADDR")))
        
        if mode == "played":
            return render_to_response("played.html")
        elif mode == "notawin":
            return render_to_response("nowin.html")
        elif mode == "win":
            return render_to_response("win.html")
 
        return render_to_response("landing.html")
    
    def post(self, request):
        if "email" not in request.POST or not request.POST["email"]:
            return render_to_response("landing.html")
        
        return self._handle_email(request.POST["email"], None)
    
    def _handle_email(self, email, ip):        
        try:
            # Make External Request
            payload = {"email": email, "ip": ip}
            r = requests.post("https://api.yoolotto.com/v1/may2012promo/index.html", data=payload)
            
            body = r.text
            
            if len(re.findall("already played", body)):
                return HttpResponse(json.dumps({"url": "/yl/promo/played"}), content_type="application/json")
            
            if len(re.findall("YL-game-notawin", body)) == 2:
                return HttpResponse(json.dumps({"url": "/yl/promo/notawin"}), content_type="application/json")
            
            if len(re.findall("YL-game-WINNER", body)) == 2:
                return HttpResponse(json.dumps({"url": "/yl/promo/win"}), content_type="application/json")
            
        except:
            return HttpResponse(json.dumps({"url": "/yl/promo/notawin"}), content_type="application/json")
        
        return HttpResponse(json.dumps({"url": "/yl/promo/notawin"}), content_type="application/json")