import datetime
import math
import json
from django.db.models import Q
from django.views.generic import View
from django.shortcuts import render_to_response
from yoolotto.rest import exceptions
from yoolotto.rest.decorators import rest, Authenticate
from django.core.mail import send_mail
from yoolotto.coupon.geo.manager import GeoManager
from yoolotto.user.models import Device, UserCoinsDetails, UserClientLogin, UserCoinsDetails,UserToken,YooLottoUser
from yoolotto.second_chance.sendemail import common_send_email
from yoolotto.lottery.models import LotteryTicket
from yoolotto.settings_local import COUPON_MAIL_CC, COUPON_REDEEM_DURATION
from yoolotto.second_chance.models import AdIssue
#from yoolotto.settings import AFTER_LOGON_OX, ox, email, password, domain, realm, consumer_key, consumer_secret
from yoolotto.second_chance.models import FAQ as FaqModel,FantasyFAQ
from yoolotto.second_chance.models import Advertisor as AdvertisorModel, AdInventory as InventoryModel
from django.http.response import HttpResponse
from django.conf import settings
from django.db.models import F
from yoolotto.coin.models import EmailCoins, DeviceCoins
from yoolotto.second_chance.models import FiberCoins, DwollaUserInfo, DwollaTransactionInfo,EventInfo,DwollaFAQ
from yoolotto.communication.apn import APNSender
from yoolotto.communication.gcm_sender import GCMSender


def yoo_games_webpage(request):
    return render_to_response('demo.html')


def login_openx():
    import ox3apiclient

    ox = ox3apiclient.Client(
        email=email,
        password=password,
        domain=domain,
        realm=realm,
        consumer_key=consumer_key,
        consumer_secret=consumer_secret,
        api_path='/ox/4.0'
        )

    AFTER_LOGON_OX=ox.logon(email, password)

    return AFTER_LOGON_OX

class InAppFuelCallback(View):
	@rest
	
	def get(self,request,userID,currency,game_id,sig):
		return {"result":0}

# class to fetch the coupons of a second chance vendor
class AdCoupons(View):
    @rest
    @Authenticate()
    def get(self, request,filter="active"):
        cv_id = request.GET.get('cv_id', None)
        if filter == "active":
            adunits =  InventoryModel.objects.filter(inventory__gt = 0,account_id=cv_id,status='Active')
	    try:
            	return {'results':map(lambda x: x.representation(),adunits),"adunit_id":"537154909"}
	    except:
		return {'results':[]}
        else:
            raise exceptions.WebServiceException("Invalid Coupon Filter")

class AdColony(View):
    @rest
    @Authenticate()
    def get(self,request):
	if request.META["HTTP_YOO_DEVICE_TYPE"] == 'ANDROID':
		zone = "vz337c35351159410f8d"
	else:
        	zone = "vz3ee9d6022cef430bbb"
        return {"send_ticket":zone,"check_ticket":zone,"yoo_games":zone}

# Sample second chance game for android ( In place of In App Fuel of Iphone )        
class AndroidGame(View):
    @rest
    @Authenticate()
    def post(self, request):
        number = settings.GAME_NUMBER
        data = json.loads(request.body)
	if number == data['number']:
            return {'matched':True}
        else:
            return {'matched':False}

# class to provide range of numbers for android game 
class AndroidNumbers(View):
    @rest
    @Authenticate()
    def get(self,request):
        return {"Max":settings.ANDROID_GAME_MAX_NUMBER,"Min":settings.ANDROID_GAME_MIN_NUMBER}

# class to send fb, twitter message 
class SocialMessage(View):
    @rest
    @Authenticate()
    def get(self, request):
        return {"message":settings.SOCIAL_MESSAGE}

# class to generate unique url for sca
class SCA_url(View):
    @rest
    @Authenticate()
    def post(self, request):
        url="http://cqa.yoolotto.com/yoogames"
	return{"url":url}


'''class SCA_url(View):
    @rest
    @Authenticate()
    def post(self, request):
        import urllib2
        data = json.loads(request.body)
        print "data",data
        user = request.yoo['user']
        device = request.yoo['device']
        if request.META["HTTP_YOO_APP_VERSION"] < "5.1":
            try:
                coins = DeviceCoins.objects.get_or_create(device_id=client_login_record.device).coins
            except:
                coins = DeviceCoins.objects.filter(device_id= device)[0].coins
            if user.email is not None and user.email != '':
                email_record, created = EmailCoins.objects.get_or_create(email=user.email, defaults={'coins': 0})
                coins =  coins+email_record.coins 
        else:
            coins = EmailCoins.objects.filter(email=user.email)[0].coins
        print user.id
        print str(user.id)
        print coins
        if data['add_coins']:
            if data['gameType'] == 'scratch':
                url = "https://twittaboom.herokuapp.com/dplayurl/yoolotto4?username=yoolotto&password=eE6xzvjG&user="+str(user.id)+"&domain=pro&coins="+str(coins)+""

            elif data['gameType'] == 'slot':
                url = "https://twittaboom.herokuapp.com/dplayurl/yoolotto3?username=yoolotto&password=eE6xzvjG&user="+str(user.id)+"&domain=pro&coins="+str(coins)+""
                #url = " https://twittaboom.herokuapp.com/dplayurl/slotcloud?username=demo&password=secret&user="+str(client_login_record.id)+"&domain=qa&coins="+str(coins)+""
        else:
            url = "https://twittaboom.herokuapp.com/dplayurl/yoolottoaer?username=yoolotto&password=yoolotto739"

        req = urllib2.Request(url)
        res = urllib2.urlopen(req)
        dynamic_url = res.read()

        return {"url":dynamic_url}'''

# class to show dynamic ads for every screen
class ScreenAds(View):
    @rest
    @Authenticate()
    def get(self,request):              
	import random
        adunits_list = []
        sections_list = []
        result = []
	try:
           '''try:'''
           adunits =  AFTER_LOGON_OX.get('http://ox-ui.yoolotto.com/ox/4.0/adunit?limit=0')['objects']
	   '''except:
               AFTER_LOGON_OX = login_openx()
               adunits =  AFTER_LOGON_OX.get('http://ox-ui.yoolotto.com/ox/4.0/adunit?limit=0')['objects']'''

           sections =  AFTER_LOGON_OX.get('http://ox-ui.yoolotto.com/ox/4.0/sitesection?limit=0')['objects']

           for section in sections :
            if section['site_id'] == settings.OPENX_SITE_ID:
                sections_list.append({"section_id":section['id'],"site_id":section['site_id'],"name":section["name"]})

           for adunit in adunits:
            if adunit['site_id'] == settings.OPENX_SITE_ID:

                if adunit['type_full'] == 'adunit.mobile':
                    if adunit['primary_size'] == '320x50':
	                if adunit['tag_type'] == 'json':
                            adunit_type = 'adunit.bannerad'
                        elif adunit['tag_type'] == 'html':
                            adunit_type = 'adunit.text'
                    elif adunit['primary_size'] == '320x480':
                        adunit_type = 'adunit.image'
                else:
                    adunit_type = adunit['type_full']
		
                adunits_list.append({"adunit_id":adunit['id'],"section_id":adunit['sitesection_id'],"type":adunit_type})

           for index in sections_list:
            info = [{"adunit":item['adunit_id'],"screen":index["name"],"type":item["type"]} for item in adunits_list if item['section_id']==index['section_id']]
            if info:
                result.append(info[random.randint(0,len(info)-1)])

	   valid_screens = ["Yoo Games","Jackpot Result","ScanScreen"]
           valid_data = []
           for index in result:
               if index['screen'] in valid_screens:
                   valid_data.append(index)
           results =  {"ads":valid_data,"domain":"ox-d.yoolotto.com","video_ad_url":"http://ox-d.yoolotto.com/v/1.0/av?auid="}
	   return {"results":results}
	except:
	   return result
        
# class to add coins to device or email on invitation or message        
class InviteFriends(View):
    @rest
    @Authenticate()
    def post(self, request):
        data = json.loads(request.body)
        coins_record, created = EmailCoins.objects.get_or_create(email=data['email'],defaults={'coins': 0})
        user = YooLottoUser.objects.get(email = data['email'])
        
        if data['share']:
            coins_record.coins = coins_record.coins + float(data['share'])*2
            coins_record.share = int(coins_record.share) + int(data['share'])
            amount = float((data['share'])*2)/100
            coins_record.dollar_amount = coins_record.dollar_amount + amount
            coins_record.save()
            share_coins = float(data['share'])*2

        coins = share_coins
        email_coins = EmailCoins.objects.filter(email=data['email'])[0]
        total_coins = email_coins.coins
        red_alert = data['red_alert']
        ticket_id = data['ticket_id']
        from yoolotto.lottery.models import LotteryTicket
        try:
            ticket_info = LotteryTicket.objects.filter(id = ticket_id)[0]
            ticket_info.red_alert = red_alert
            ticket_info.save()
            red_alertt = ticket_info.red_alert
        except:
            red_alertt = 0
        
        return {"total_coins":total_coins,"coins":coins,"red_alert":red_alertt}

# class to add coins to device or email, when user plays IPhone game (In App Fuel)        
class AddCoins(View):# used for fiber
    @rest
    @Authenticate()
    def post(self, request):
        data = json.loads(request.body)
        try:
            coins_record = EmailCoins.objects.filter(email = request.META['HTTP_YOO_EMAIL_ID'])[0]
        except:
            coins_record,created = EmailCoins.objects.get_or_create(email = request.META['HTTP_YOO_EMAIL_ID'] )
        try:
           coins_data = FiberCoins.objects.filter(email = coins_record.email)[0]
        except: 
            coins_data ,created = FiberCoins.objects.get_or_create(email = coins_record.email)
        coins_data.offer_wall =int(coins_data.offer_wall) + int(data['offer_wall'])
        coins_data.rewarded_video = int(coins_data.rewarded_video) + int(data['rewarded_video'])
        coins_data.save()

        coins_record.coins = float(coins_record.coins) + float(data['offer_wall']) + float(data['rewarded_video'])
        temp = float(data['offer_wall']) + float(data['rewarded_video'])
        amount = float(temp)/100
        coins_record.dollar_amount = coins_record.dollar_amount + amount
        coins_record.save()
	    
        total_coins = coins_record.coins
        
        red_alert = data['red_alert']
        ticket_id = data['ticket_id']
        from yoolotto.lottery.models import LotteryTicket
        try:
            ticket_info = LotteryTicket.objects.filter(id = ticket_id)[0]
            ticket_info.red_alert = red_alert
            ticket_info.save()
            red_alertt = ticket_info.red_alert
        except:
            red_alertt = 0
        
        return {"total_coins":total_coins,"red_alert":red_alertt}

class CoinsReduction(View):
    @rest
    @Authenticate()
    def get(self, request):
        #data = json.loads(request.body)
        user = request.yoo['user']
        email_coins_record = EmailCoins.objects.get(email=user.email)
        if email_coins_record.get_coins() >= 2:
            email_coins_record.coins = F('coins') - 2
            amount = float(2)/100
            email_coins_record.dollar_amount = email_coins_record.dollar_amount - amount
            email_coins_record.save()
        return {"success":True}

class AdRedeem(View):
    @rest
    @Authenticate()
    def post(self, request):
    #valid = False
        data = json.loads(request.body)
        #return_data = {}    
        client_login_record = UserClientLogin.objects.filter(device = request.yoo['device'])[0]
        device_coins_record = DeviceCoins.objects.filter(device_id=request.yoo['device'])[0]
        device_coins = device_coins_record.coins

        if device_coins_record.get_coins() >= 20:
            device_coins_record.coins = F('coins') - 20
            device_coins_record.save()

        else:
            if client_login_record.client_login:
                email_coins_record = EmailCoins.objects.get(email=client_login_record.client_login)
                if email_coins_record.get_coins() >= 20:
                    email_coins_record.coins = F('coins') - 20
                    email_coins_record.save()

        return {"success":True}

        '''if inventory > 0:
            if device_coins >= int(ad_data_source["coins"]):
                valid = True
            else:
                if client_login_record.client_login:
                    email_coins_record = EmailCoins.objects.filter(email=client_login_record.client_login)[0]
                    if email_coins_record.coins >= int(ad_data_source["coins"]):
                        valid = True

            if valid:
                if data["winner"]:
                    ad_data_source["inventory"] = inventory - 1
                    jsonString = json.dumps(ad_data_source)
                    info = {"source": jsonString}
                    AFTER_LOGON_OX.put('http://ox-ui.yoolotto.com/ox/4.0/ad/'+str(InventoryRecord.ad_id)+'',info) # update single lineitem
                    updated_inventory = inventory - 1
                else:
                    updated_inventory = inventory
                try:
                    InventoryRecord.ad_image = ad_data_source['ad_image']
                except:
                    InventoryRecord.ad_image = None
                try:
                    InventoryRecord.vendor = ad_data_source['vendor']
                except:
                    InventoryRecord.vendor = None
                try:
                    InventoryRecord.vendor_image = ad_data_source['vendor_image']
                except:
                    InventoryRecord.vendor_image = None
                try:
                    InventoryRecord.coins = ad_data_source['coins']
                except:
                    InventoryRecord.coins = 0
                try:
                    InventoryRecord.InventoryRecord.timer = ad_data_source['timer']
                except:
                    InventoryRecord.timer = False
                try:
                    InventoryRecord.ad_type = ad_data_source['type']
                except:
                    InventoryRecord.ad_type = None
                try:
                    InventoryRecord.video_url = ad_data_source['videourl']
                except:
                    InventoryRecord.video_url = None
                InventoryRecord.inventory = updated_inventory
                
                InventoryRecord.save()

                if InventoryRecord.type == 'physical':
                    AdIssueRecord = AdIssue(ad=InventoryRecord,address=data['address'],email=data['email'],device=request.yoo['device'],phone=data['phone'],won=data['winner'])
                else:
                    AdIssueRecord = AdIssue(ad=InventoryRecord,email=data['email'],device=request.yoo['device'],won=data['winner'])
                AdIssueRecord.save()

                if data['winner']:
                    subject = "Second Chance Ad"
                    context = {}
                    text_template_path = "second_chance_email.txt"
                    html_template_path = "second_chance_email.html"
                    context_data = {'second_chance_obj': InventoryRecord}
                    recipients = [data['email'], 'subodh.deoli@spa-systems.com', 'kapil.soni@spa-systems.com']
		    common_send_email(subject, text_template_path, html_template_path,context_data, recipients)
		    
                    return_data['email'] = 1
		    return_data['message'] = 'Email is sent'
                    return_data['screen'] = settings.SECOND_CHANCE_WINNING_MESSAGE 
                else:
                    return_data['email'] = 0
		    #return_data['message'] = 'Better luck next time'
		    return_data['screen'] = settings.SECOND_CHANCE_LOOSING_MESSAGE

                
                if device_coins_record.get_coins() >= 20:
                    device_coins_record.coins = F('coins') - 20
                    device_coins_record.save()

                    
                else:
                    if client_login_record.client_login:
                        email_coins_record = EmailCoins.objects.get(email=client_login_record.client_login)
                        if email_coins_record.get_coins() >= 20:
                            email_coins_record.coins = F('coins') - 20
                            email_coins_record.save()

            else:
                return_data['email'] = 0
                return_data['message'] = 'You do not have enough coins'
                return_data['screen'] = settings.SECOND_CHANCE_LOOSING_MESSAGE
                
                
        else:
            
	    return_data['email'] = 0
            return_data['message'] = 'No Inventory left for this ad in OpenX'
            return_data['screen'] = settings.SECOND_CHANCE_LOOSING_MESSAGE

	return return_data'''

#class to show the list of advertisors for second chance
class Advertisors(View):
    @rest
    @Authenticate()
    def get(self, request):
	user = request.yoo["user"]
        
        Inventories = InventoryModel.objects.filter(inventory__gt = 0,status='Active')
        Vendors = set([inventory.account for inventory in Inventories ])
        
        return map(lambda x: x.representation(),Vendors)

#class to show the list of frequently asked questions and answers
class FrequentQuestions(View):
    @rest
    def get(self, request):
        Faqs = FaqModel.objects.all()
        return map(lambda x: x.representation(), Faqs)
        
class TimerAd(View):
    @rest
    @Authenticate()
    def post(self, request):
        user = request.yoo["user"]
        data = json.loads(request.body)
        _id = int(data['id'])
        
        #issue = AdIssue.objects.get(pk=_id)
        
        try:
            #ad_stores = AdStoresModel.objects.filter(ad=issue.ad)
            result = []
            
            _result1 = {
                "name": "Dominos",
                "code": "Dominos",
                "address": "Sector q",
                "address_2": "Sector 18",
                "city": "Noida",
                "state": "Uttar Pradesh",
                "postal_code": "248001",
                "phone": "9999998899",
                "latitude": "28.646951",
                "longitude": "77.310984",
                "distance": "280"
            }
            
            result.append(_result1)
            
            _result2 = {
                "name": "KFC",
                "code": "KFC",
                "address": "Block A",
                "address_2": "Sector 1",
                "city": "Gurgaon",
                "state": "Haryana",
                "postal_code": "248332",
                "phone": "8989898898",
                "latitude": "28.644897",
                "longitude": "77.335583",
                "distance": "780"
            }
            
            result.append(_result2)     

            return result
        except:
            raise exceptions.WebServiceException("This is not a timer coupon")
            
            
class TimeCal(View):
    @rest
    @Authenticate()
    def get(self,request):
        #client_login_details = UserClientLogin.objects.filter(device = request.yoo['device'])[0]
        email_coins_info = EmailCoins.objects.get(email = request.META['HTTP_YOO_EMAIL_ID'])
        now = datetime.datetime.now()
        now = datetime.datetime.now()
        reset_date = email_coins_info.reset_date + datetime.timedelta(days=30)
        time_left = (reset_date - now).total_seconds()
        return {"time_left":time_left}


class Dwolla(View):
    @rest
    @Authenticate()
    def post(self,request):
        data = json.loads(request.body)
        #from yoolotto.user.models import *
        user_details = YooLottoUser.objects.get(email = data['email'])
        if data['isDwollaUser'] == 1:
            dwolla_info,created = DwollaUserInfo.objects.get_or_create(user = user_details,dwolla_email=data['dwolla_email'])
            dwolla_info.dwolla_amount = data['dwolla_amount']
            dwolla_info.total_amount = float(dwolla_info.total_amount) + float(data['dwolla_amount'])
            #dwolla_info.phone = data['phone']
            dwolla_info.email = data['email']
            dwolla_info.name = data['name']
            dwolla_info.dwolla_pin = data['dwolla_pin']
	    dwolla_info.isDwollaUser = 1
            dwolla_info.save()
            if data['source'] == "coins":
                coins_info = EmailCoins.objects.filter(email= data['email'])[0]
                coins_info.dollar_amount = float(coins_info.dollar_amount) - float(data['dwolla_amount'])
                coins_reduced = round((float(data['dwolla_amount'])* 100),2)
                coins_info.coins = float(coins_info.coins) - float(coins_reduced)
                #amount = float(coins_reduced)/100
                #coins_info.dollar_amount= coins_info.dollar_amount -amount
                coins_info.save()
                tranc_info = DwollaTransactionInfo(dwolla_detail = dwolla_info,source = "coins",source_info = coins_reduced,amount_requested = data['dwolla_amount'],transaction_id = data['transaction_id'])
            if data['source'] == "ticket":
                if data['ticket_id'] != None or data['ticket_id'] != '':
                    ticket_info = LotteryTicket.objects.get(id = data['ticket_id'])
                    ticket_info.winnings = float(ticket_info.winnings) - float(data['dwolla_amount'])
                    coins_info.dollar_amount = coins_info.dollar_amount - float(data['dwolla_amount'])
                    ticket_info.save()
                    tranc_info = DwollaTransactionInfo(dwolla_detail = dwolla_info,source = "ticket",source_info = data['ticket_id'],amount_requested = data['dwolla_amount'],transaction_id = data['transaction_id'])
            tranc_info.save()    
        else:
	    try:
                if data['phone'] != '':
                    dwolla_info,created = DwollaUserInfo.objects.get_or_create(user = user_details,phone =data['phone'],email = data['email'],dwolla_amount = data['dwolla_amount'])
                else:
                   dwolla_info,created = DwollaUserInfo.objects.get_or_create(user = user_details,email = data['email'],dwolla_email=data['dwolla_email'],dwolla_amount = data['dwolla_amount']) 
                   #from django.core.mail import send_mail
                   #send_mail('dwolla email test', 'We will get back to u', 'kanika.chugh@spa-systems.com',[data['dwolla_email']],fail_silently=False)
                dwolla_info.isDwollaUser = 0
		dwolla_info.total_amount = float(dwolla_info.total_amount) + float(data['dwolla_amount'])
                dwolla_info.save()
		coins_info = EmailCoins.objects.filter(email= data['email'])[0]
                coins_info.dollar_amount = float(coins_info.dollar_amount) - float(data['dwolla_amount'])
                coins_reduced = round((float(data['dwolla_amount'])* 100),2)
                coins_info.coins = float(coins_info.coins) - float(coins_reduced)
                #coins_info.dollar_amount = coins_info.dollar_amount - float(data['dwolla_amount'])
                coins_info.save()
		#except:
		    #pass
            except:
		pass
        return {"success":True} 

class FantasyFrequentQuestions(View):
    @rest
    def get(self, request):
        faqs = FantasyFAQ.objects.all()
        return map(lambda x: x.representation(), faqs)

class EventTracking(View):
    #@rest
    def post(self,request):
	data = json.loads(request.body)
	referring_user = data['first_referring_identity']
	event_occured = data['event']
	event_details = EventInfo(referring_user = referring_user,event_occured = event_occured)
	event_details.save()
	user_details = YooLottoUser.objects.get(email = referring_user)
	user_device_details = Device.objects.filter(user = user_details.id)
	for device in user_device_details:
	    text = "You have been awarded for referral"
	    try:
                if device.device_type == "IPHONE":
                    apn = APNSender(device.device_token, text=text,custom={"code": "referral_tracking"})
                    apn.send()
                elif device.device_type == "ANDROID":
                    gcm = GCMSender(to=[device.device_token], data={"text": text,"code": "referral_tracking"})
                    gcm.send()
            except:
                pass
        return HttpResponse('result')


class CoinsAdditionReferral(View):
    @rest
    def post(self,request):
	data = json.loads(request.body)
	referring_user = data['referring_user']
	#referred_user = data['referred_user']
	#event_occured = data['event_occured']
	referring_user_coins = data['referring_user_coins']
	referred_user_coins = data['referred_user_coins']
	try:
	    event_details = EventInfo.objects.get(referring_user = referring_user,referred_user= data['referred_user'])
	except:
	    try:
		event_details,created = EventInfo.objects.get_or_create(referring_user = referring_user,referred_user = '')
	    except:
		event_details = EventInfo.objects.filter(referring_user = referring_user,referred_user = '')[0]
	        event_details.referred_user = data['referred_user']
	event_details.referred_user_coins = event_details.referred_user_coins + float(data['referred_user_coins'])
	event_details.referring_user_coins = event_details.referring_user_coins + float(data['referred_user_coins'])
	event_details.save()
	try:
	    referring_coins = EmailCoins.objects.get(email = referring_user)
            referring_coins.coins = referring_coins.coins + float(data['referring_user_coins'])
            amount = float(data['referring_user_coins'])/100
            referring_coins.dollar_amount = referring_coins.dollar_amount + amount
            referring_coins.save()
            if data['referred_user']:
                r_user = data['referred_user']
                referred_coins = EmailCoins.objects.get(email = r_user)
                referred_coins.coins = referred_coins.coins + float(data['referred_user_coins'])
                amount = float(data['referred_user_coins'])/100
                referred_coins.dollar_amount = referred_coins.dollar_amount + amount
                referred_coins.save()
	except:
	    pass
	return {"success":True}


class DwollaFrequentQuestions(View):
    @rest
    def get(self, request):
        faqs = DwollaFAQ.objects.all()
        return map(lambda x: x.representation(), faqs)
