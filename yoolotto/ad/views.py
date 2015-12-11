from django.views.generic import View
from django.template import RequestContext, loader

from yoolotto.rest.decorators import rest

class AdvertisementRetriever(View):
    @rest
    def get(self, request):
        
        # Retrieve Sizing Data
        width = request.META.get("HTTP_DEVICE_WIDTH", None)
        height = request.META.get("HTTP_DEVICE_HEIGHT", None)
        
        if not width:
            width = request.GET.get("width", 320)
        
        if not height:
            height = request.GET.get("height", 480)
        
        width = int(width)
        height = int(height)
            
        if width >= 640:
            params = {
                "div_id": "azk45918",
                "size_id": "824",
                "comment": "/* load placement for account: YooLotto1, site: YooLotto, size: 640x100 - 640 x 100*/",
                "width": 640,
                "height": 100
            }
        else:        
            params = {
                "div_id": "azk20791",
                "size_id": "23",
                "comment": "/* load placement for account: YooLotto1, site: YooLotto, size: 320x50 - Mobile: 320x50*/",
                "width": 320,
                "height": 50
            }
            
        scale = float(width) / params["width"]
        
        
        template = loader.get_template('wrapper.html')
        context = RequestContext(request, params)
        
        html = template.render(context)
        
        return {
            "html": html,
            "width": int(params["width"] * scale),
            "height": int(params["height"] * scale)
        }