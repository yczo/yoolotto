from django.views.generic import View
from django.shortcuts import render_to_response

from yoolotto.rest.decorators import rest

class GamesStub(View):
    def get(self, request):
        return render_to_response("comingsoon.html")
    
    @rest
    def post(self, request):
        return False