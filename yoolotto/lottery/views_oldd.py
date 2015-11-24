from django.views.generic import View
from yoolotto.rest import exceptions
from yoolotto.rest.decorators import rest, Authenticate
from django.shortcuts import render
from yoolotto.lottery.enumerations import EnumerationManager
from yoolotto.lottery.models import LotteryGame, LotteryDraw, LotteryTicket,Fantasy_Game_Info,Fantasy_Help_Info
from yoolotto.coupon.models import CouponVendor as CouponVendorModel,\
    CouponIssue
from yoolotto.coupon.models import Coupon as CouponModel
from yoolotto.user.models import UserClientLogin
from yoolotto.second_chance.models import Advertisor as AdvertisorModel, AdInventory as InventoryModel
from django.db.models import Max

class Winner(View):
    @rest
    @Authenticate()
    def get(self, request):
        highest_winning_amount = LotteryTicket.objects.all().aggregate(Max('winnings'))
        amount = int(highest_winning_amount["winnings__max"])
        import locale
        try:
            locale.setlocale(locale.LC_ALL, 'en_US.UTF-8') #use locale.format for commafication
        except locale.Error:
            locale.setlocale(locale.LC_ALL, '') #set to default locale (works on windows)
        winning_amount = locale.format('%d', amount , True)
        return {"winning_amount":winning_amount}

class Game(View):
    @rest
    @Authenticate()
    def get(self, request, id=None):
        
	user = request.yoo["user"]
	if request.yoo["new_version"]:
		ocr_games = ['Powerball','Megamillions']        
        	if id:
            		return LotteryGame.objects.get(pk=id).representation(user)
        
        	active = request.GET.get("active", "active")
        	division = request.GET.get("division", None)
        
        	#games = LotteryGame.objects.all()
		if division == 'TX':
			games = LotteryGame.objects.all()
		else:
			games = LotteryGame.objects.filter(code__in=ocr_games)
        
        	if active:
            		if active == "active":
                		games = games.filter(active=True)
            		elif active == "inactive":
                		games = games.filter(active=False)

		if  division == 'TX':
			games = games.filter(components__division__remote_id=division)                
		return map(lambda x: x.representation(user), games)
	else:
	
        
        	if id:
            		return LotteryGame.objects.get(pk=id).representation(user)
        
        	active = request.GET.get("active", "active")
        	division = request.GET.get("division", None)
        
        	games = LotteryGame.objects.all()
        
        	if active:
            		if active == "active":
                		games = games.filter(active=True)
	            	elif active == "inactive":
                		games = games.filter(active=False)
                
        	if division:
           		games = games.filter(components__division__remote_id=division)
        
	        return map(lambda x: x.representation(user), games)
        
        
    
    
class GameCheck(View):
    @rest
    @Authenticate()
    def post(self, request, _id):
        user = request.yoo["user"]
        division=request.GET.get("division")
	client_login_record = UserClientLogin.objects.filter(device=request.yoo['device'])
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
        
        draw = LotteryDraw.objects.get(pk=_id)        
	try:
        	ticket = draw.tickets.get(user=user,division__remote_id=division)
	except:
		ticket = draw.tickets.filter(user=user,division__remote_id=division)[0]
    
        allocated = None
        if ticket.draw.result:
            for submission in ticket.submissions.all():
                submission.checked = True
                submission.save()

            allocated = ticket.update(request.yoo["device"],client_login_record[0].client_login,full=True)
            
        ticket = LotteryTicket.objects.get(pk=ticket.pk)
                
        _result = ticket.representation()
        _result["coins"] = ticket.winnings_coins
        print "reeeeeeeeeeeeeeeeeee",_result["coins"]
        if _id and user:
            import datetime
            now =datetime.datetime.now()
            try:
                issue = CouponIssue.objects.filter(user=user).latest('added_at')
                if (now - issue.redeemed).days > 1:
                    _result['to_show_redeem_button'] = 1
                else:

                    _result['to_show_redeem_button'] = 0 
            except Exception as e:
                _result['to_show_redeem_button'] = 1
            
            no_winning_coupon = map(lambda x: x.representation(), \
            CouponModel.objects.filter(valid_to__gte=now,redeem_limit__gte=1).order_by('sequence'))
            try:
                _result['coupon'] = no_winning_coupon[0]
            except Exception as e:
                _result['coupon'] = None
            
            Inventories = InventoryModel.objects.filter(inventory__gt = 0,status='Active')
            Vendors = [inventory.account for inventory in Inventories ]
            if Vendors:
                Vendors = [Vendors[0]]
                second_chance_vendor = map(lambda x: x.representation(),Vendors)
                _result['second_chance_vendor'] = second_chance_vendor[0]
            '''else:
                _result['second_chance_vendor'] = []'''

        return _result

    def put(self, *args, **kwargs):
        return self.post(*args, **kwargs)


class UserProfile(View):
    @rest
    @Authenticate()
    def post(self, request):
        user = request.yoo["user"]
        device = request.yoo["device"]
        data = json.loads(request.body)
        #user = data["user"]
        #device = data["device"]
        #user = YooLottoUser.objects.get(id=user)
        
        client_login_info = UserClientLogin.objects.filter(email = data['email'])
        if client_login_info.device == device:
            user_info = YooLottoUser.objects.get(email = data['email'])
            name = user_info.name
            emailcoins = EmailCoins.objects.get(email=user_info.email).coins
            devicecoins = DeviceCoins.objects.get(device_id = device).coins
            fiber_coins = FiberCoins.objects.filter()
            total_coins = emailcoins + devicecoins
        gametypes = LotteryTicketAvailable.objects.filter(ticket__user_id = user_info.id).values("gameType").distinct()
        gamelist = []
        for x in gametypes:
            gamelist.append(x["gameType"])
        gamelist = [x if x not in [u'11', u'1'] else 1 for x in gamelist]
        gamelist = [x if x not in [u'13', u'0'] else 0 for x in gamelist]
        powerball_vars = [u'11', u'1']
        megamillion_vars = [u'13', u'0']
        powerball_count = LotteryTicketAvailable.objects.filter(ticket__user_id = user, gameType__in = powerball_vars).count()
        megamillion_count = LotteryTicketAvailable.objects.filter(ticket__user_id = user, gameType__in = megamillion_vars).count()
        gamesnotplayed = list(set([0,1]) - set(gamelist))
        data1 = {"name": user.name, "coins": total_coins, "powerball_count": powerball_count, "megamillion_count": megamillion_count, "games_not_played": gamesnotplayed}
        return data1


class FantasyHelpInfo(View):
    @rest
    @Authenticate()
    def get(self, request):
        data = {}
        helpinfo = Fantasy_Help_Info.objects.all()
        for quesans in helpinfo:
            data[quesans.question] = quesans.answer
        return data

class FantasyGameInfo(View):
    @rest
    @Authenticate()
    def get(self, request):
        data = {}
        gameinformation = Fantasy_Game_Info.objects.all()
        for info in gameinformation:
            data["gameinfo"] = info.gameinfo
        return data