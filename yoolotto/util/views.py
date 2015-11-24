import os
import json

from django.conf import settings
from django.views.generic import View

from yoolotto.rest.decorators import rest

class BuildVersion(View):
    @rest
    def get(self, request):
        path = os.path.join(settings.SITE_CONTAINER, "deploy", "version.json")
        
        try:
            fp = open(path, "r")
            data = fp.read()
            fp.close()
            return json.loads(data)
        except:
            return {}