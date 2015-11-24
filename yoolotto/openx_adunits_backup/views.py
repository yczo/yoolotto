#from yoolotto.rest.decorators import rest, Authenticate
from django.views.generic import View
from django.http.response import HttpResponse
from yoolotto.openx_adunits.config import AD_UNIT_CONFIG
import json
from yoolotto.user.models import Device
from threading import Thread
from yoolotto.communication.apn import APNSender
from yoolotto.communication.gcm_sender import GCMSender
from collections import OrderedDict
from yoolotto.openx_adunits.models import adVisit
class adUnit(View):
    
    def fetch_from_remote(self,url):
        try:
            import urllib2
            req = urllib2.Request(url)
            can_open= urllib2.urlopen(req)
            #print can_open.info()
            #print can_open.getcode()
            data=can_open.read()
            #print data
            return data if can_open else None
        except urllib2.URLError:#server cann't connect to openx
            return None
    
    def get_and_process_ad_text(self,for_platform):
        data=self.fetch_from_remote(AD_UNIT_CONFIG[for_platform]['notification']['supported_format']['json'])
        if data:
            data=json.loads(data)
            creative=data['ads']['ad'][0]['creative'][0]
            if AD_UNIT_CONFIG[for_platform]['notification']['consume_impression']:
                self.fetch_from_remote(creative['tracking']['impression'])
                #print "consume impression : %s"%creative['tracking']['impression']
                print "consume impression for <%s> "%for_platform
            if AD_UNIT_CONFIG[for_platform]['notification']['consume_click']:
                self.fetch_from_remote(creative['tracking']['click'])
                #print "consume click : %s"%creative['tracking']['click']    
                print "consume click for <%s> "%for_platform
            return [creative['media'],creative['tracking']['click'],creative['tracking']['impression']]
            
    def send_notification_text(self,for_platform,for_token,for_text=None):
        import uuid
        push_data=[for_text,'',''] if AD_UNIT_CONFIG[for_platform]['notification']['allow_custom_msg'] and for_text else self.get_and_process_ad_text(for_platform)
        custom={
                'code':'ad',
                'unique':str(uuid.uuid4()),
                "ad_text":push_data[0]
                
                }
        
        if not AD_UNIT_CONFIG[for_platform]['notification']['consume_click']:
            custom["click_url"]=push_data[1]
        
        if not AD_UNIT_CONFIG[for_platform]['notification']['consume_impression']:
            custom["impression_url"]=push_data[2]
            
            
        if for_platform=='iphone':
            apn = APNSender(for_token,
                            text=for_text,
                            custom=custom)                                    
            #apn.send()
        elif for_platform=='android':
            gcm = GCMSender(to=[for_token], data=custom)
            #gcm.send()
                 
#     @rest
#     @Authenticate()
    def get(self, request):
        supported_platform=['android','iphone']
        supported_ad_types=['text','video','image','notification']
        
        default_platform="android"
        default_ad_type='video'
        
        map_to_http_format={
                            'xml':'xml',
                            'json':'json',
                            'jsonp':'json'
                            }
        
        platform=request.GET.get('platform',default_platform)
        platform=platform if platform else default_platform
        if platform not in supported_platform:
            return HttpResponse("Invalid Platform type '%s', supported platforms are %s ."%(platform,supported_platform))
        
        ad_type=request.GET.get('ad_type',default_ad_type)
        ad_type=ad_type if ad_type else default_ad_type #ensure if supplied without value
        if ad_type not in supported_ad_types:
            return HttpResponse("Invalid AdUnit type '%s', supported types are %s ."%(ad_type,supported_ad_types)) 
        
        _format=request.GET.get('format',AD_UNIT_CONFIG[platform][ad_type]['default_format'])
        _format=_format if _format else AD_UNIT_CONFIG[platform][ad_type]['default_format']#ensure if supplied without value
        if _format not in AD_UNIT_CONFIG[platform][ad_type]['available_format']:
            return HttpResponse("Unsupported response format '%s' for '%s' type , supported formats are %s ."%(_format,ad_type,AD_UNIT_CONFIG[platform][ad_type]['available_format']))
        
        data=None
        if ad_type=='video':
            if platform=='android':
                data=self.fetch_from_remote(AD_UNIT_CONFIG[platform][ad_type]['supported_format'][_format])
                data=data if data else '<error>connection to openx failed, please try again.</error>'
                return HttpResponse(data,mimetype='application/%s'%map_to_http_format[_format])
            elif platform=='iphone':
                data={
                      'platform':platform,
                      #'ad_type':ad_type,
                      'ad_unit_id':AD_UNIT_CONFIG[platform][ad_type]['ad_unit_id'],
                      'domain':AD_UNIT_CONFIG[platform][ad_type]['domain'],
                      'url':AD_UNIT_CONFIG[platform][ad_type]['ad_call_url'],
                      }
                return HttpResponse(json.dumps(OrderedDict(data), indent=4),mimetype='application/%s'%map_to_http_format[_format])

        elif ad_type=='image':
            
            data={'platform':platform,
                  'ad_unit_id':AD_UNIT_CONFIG[platform][ad_type]['ad_unit_id'],
                  'domain':AD_UNIT_CONFIG[platform][ad_type]['domain'],
                  'ad_unit_real_id':AD_UNIT_CONFIG[platform][ad_type]['real_ad_unit_id']
                  }
            
            
            return HttpResponse(json.dumps(OrderedDict(data), indent=4),mimetype='application/%s'%map_to_http_format[_format])
        elif ad_type=='text':
            data={}
            
            device_id=request.GET.get('device_id',None)
            device_id=device_id if device_id else None
            if not device_id:
                return HttpResponse("you must pass 'device_id' for 'text' ad_type .")
            
            
            dev=adVisit.objects.get_or_create(device_id=device_id)[0]
            dev.increamentVisit()
            if_show=dev.count%AD_UNIT_CONFIG[platform]['text']['show_after']
            if if_show==0:
                push_data=self.get_and_process_ad_text(platform)
                data={'platform':platform,
                      'message':push_data[0],
                      'ad_unit_id':AD_UNIT_CONFIG[platform][ad_type]['ad_unit_id'],
                      'domain':AD_UNIT_CONFIG[platform][ad_type]['domain']
                      #'ad_unit_real_id':AD_UNIT_CONFIG[platform][ad_type]['real_ad_unit_id']
                      }
                
                if not AD_UNIT_CONFIG[platform]['text']['consume_click'] and if_show==0:
                    data["click_url"]=push_data[1]
                
                #if not AD_UNIT_CONFIG[platform]['text']['consume_impression']:
                #    data["impression_url"]=push_data[2]
                    
                
            else:
                data={'platform':platform,'ad_unit_id':AD_UNIT_CONFIG[platform][ad_type]['ad_unit_id'],
                      'domain':AD_UNIT_CONFIG[platform][ad_type]['domain'],'visit':if_show}
            return HttpResponse(json.dumps(OrderedDict(data), indent=4),mimetype='application/%s'%map_to_http_format[_format])
        elif ad_type=='notification':
            #data=None
            device_id=request.GET.get('device_id',None)
            device_id=device_id if device_id else None
            
            for_text=request.GET.get('msg',None)
            #for_text=for_text if for_text else None
            
            if not device_id:
                return HttpResponse("you must pass 'device_id' for 'notification' ad_type .")
            
            if for_text and not AD_UNIT_CONFIG[platform][ad_type]['allow_custom_msg']:
                return HttpResponse("sending custom message for '%s' is disabled."%platform)
            
            if for_text=='':
                return HttpResponse("msg can not be empty.")
            
            
            if Device.objects.filter(device_id=device_id).exists():
                if Device.objects.get(device_id=device_id).device_type!={'android':'ANDROID','iphone':'IPHONE'}[platform]:
                    return HttpResponse("Invalid token for '%s'."%platform)
                if Device.objects.get(device_id=device_id).device_token:
                    Thread(target=self.send_notification_text,args=[platform,Device.objects.get(device_id=device_id).device_token,for_text]).start()
                    data={'success':True}
                else:
                    data={'success':False,'reason':'token not exist for this device.'}
            else:
                data={'success':False,'reason':" device id '%s' not exist."%device_id}
                
            #device_token=Device.objects.get(device_id=device_id).device_token  and Device.objects.get(device_id=device_id).device_token else 'NOT FOUND'
            return HttpResponse(json.dumps(OrderedDict(data),indent=4),mimetype='application/%s'%map_to_http_format[_format])










# 
# 
# 
#  
# import urllib2
# req = urllib2.Request('http://ox-d.yoolotto.com/ma/1.0/ri?ts=1fHJpZD05ZmY4NmQyYy0zZDZjLTRlZjctYTJkNy0yYTRkNTc0NzNlZjl8cnQ9MTQwODI1MDIyNnxhdWlkPTUzNzE1NzU4NnxhdW09RE1JRC5NT0JJTEV8c2lkPTUzNzEwNzY3M3xwdWI9NTM3MDg0MzMwfHBjPVVTRHxyYWlkPTA4OGUzZDQ1LWI3MmMtNDc2MC1iNDk4LTBmM2E0MmVkNDI3OHxhaWQ9NTM3MzE5MTc2fHQ9MTB8YXM9MzIweDUwfGxpZD01MzcyMjA1NTN8b2lkPTUzNzE1MzY0NnxwPTEwfHByPTEwfGFkdj01MzcwODQzMzF8YWM9VVNEfHBtPVBSSUNJTkcuQ1BNfGJtPUJVWUlORy5OT05HVUFSQU5URUVEfHVyPWJJVDJKUkw5OXE')
# can_open= urllib2.urlopen(req)
# print can_open.info();print 
# print can_open.getcode();print 
# print can_open.url;print 
# data=can_open.read()
#print data


# import urllib2;
# print urllib2.build_opener(urllib2.HTTPHandler(debuglevel=1)).open(urllib2.Request('http://ox-d.yoolotto.com/ma/1.0/ri?ts=1fHJpZD05ZmY4NmQyYy0zZDZjLTRlZjctYTJkNy0yYTRkNTc0NzNlZjl8cnQ9MTQwODI1MDIyNnxhdWlkPTUzNzE1NzU4NnxhdW09RE1JRC5NT0JJTEV8c2lkPTUzNzEwNzY3M3xwdWI9NTM3MDg0MzMwfHBjPVVTRHxyYWlkPTA4OGUzZDQ1LWI3MmMtNDc2MC1iNDk4LTBmM2E0MmVkNDI3OHxhaWQ9NTM3MzE5MTc2fHQ9MTB8YXM9MzIweDUwfGxpZD01MzcyMjA1NTN8b2lkPTUzNzE1MzY0NnxwPTEwfHByPTEwfGFkdj01MzcwODQzMzF8YWM9VVNEfHBtPVBSSUNJTkcuQ1BNfGJtPUJVWUlORy5OT05HVUFSQU5URUVEfHVyPWJJVDJKUkw5OXE'))



