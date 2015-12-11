import csv
import json

from django.contrib.auth import hashers
from django.db import IntegrityError
from django.db.models import Count, Sum
from django.http import HttpResponse, HttpResponseNotFound
from django.views.generic import View

from yoolotto.rest import exceptions
from yoolotto.rest.decorators import rest, Authenticate
from yoolotto.coin.models import CoinTransaction, CoinTicketGenericTransaction, EmailCoins, DeviceCoins
from yoolotto.user.models import Device as DeviceModel, YooLottoUser, UserPreference, UserToken, UserDetails, UserClientLogin, PasswordReset, UserCoinsDetails
from yoolotto.user.forms import UserUpdateForm, PreferenceUpdateForm
from yoolotto.lottery.models import LotteryTicketClient
from django.db.models import F
import random
import datetime
from yoolotto.coupon.models import CouponVendor as CouponVendorModel
from yoolotto.second_chance.models import FiberCoins,ResetCoins,DeviceLoginDetails

# class to register user
class RegisterUser(View):
    @rest
    @Authenticate()
    def post(self,request):
        user_data = json.loads(request.body)
              
        try:
            user = YooLottoUser(name=user_data['user_name'],email=user_data['email'],password=hashers.make_password(user_data['password']))
            user.save()
            
            user_details = UserDetails(user=user,phone=user_data['phone'],address=user_data['address'])
            user_details.save()
            
            return {"email":user.email,"user_name":user.name,"phone":user_details.phone,"address":user_details.address,"id":user.id}
        except IntegrityError:
            raise exceptions.WebServiceException("Email already exist.")

#class to logout user in app
class LogoutUser(View):
    @rest
    @Authenticate()
    def post(self, request):
        print 111111111111111111111
        print "skawhijekkfkes",request.yoo['device']
        logout_user = UserClientLogin.objects.filter(device=request.yoo['device'])
        print "logout_user",logout_user
        for user in logout_user:
            user.client_login = None
            user.save()
            print user.client_login
	    device_record, created = DeviceCoins.objects.get_or_create(device_id=request.yoo['device'])
	
        return {"coins":device_record.get_coins()}

#class to logout user in app
class Services(View):
    @rest
    @Authenticate()
    def get(self, request):
        yoo_prize_code =  random.randint(0,1)
	yoo_game_code =  random.randint(0,1)
        return {"yoo_prize":yoo_prize_code,"yoo_game":yoo_game_code}


# class to send mail to reset password, when user forgot password    
class ForgotPassword(View):
    @rest
    @Authenticate()
    def post(self, request):
        
        from django.core.mail import EmailMessage
        
        data = json.loads(request.body)
        email = data["email"]
        try:
            YooLottoUser.objects.get(email=email)
            reset_code =  random.randint(1,100000000)
            PasswordResetRecord = PasswordReset.objects.filter(email=email)
            if PasswordResetRecord:
                PasswordResetRecord.update(code=reset_code, reset = False)
            else:
                PasswordReset(email=email, code=reset_code, reset = False).save()
            
            email = EmailMessage(' Reset Password Code YooLotto', 'reset_code = '+str(reset_code)+'', to=[email])
            email.send()
            
            return {"email":1}
        except:
            return {"email":0, "message": "email is not registered."}
        
class VerifyPasswordCode(View):
    @rest
    @Authenticate()
    def post(self, request):
        data = json.loads(request.body)
        email = data["email"]
        code = data["code"]
        
        try:
            PasswordResetRecord = PasswordReset.objects.get(email=email,code=code)
            return {"verified":True}
        except:
            return {"verified":False}
        
class ResetPassword(View):
    @rest
    @Authenticate()
    def post(self, request):
        try:
            data = json.loads(request.body)
            email = data["email"]
            password = data["password"]
            code = data["code"]
            try:
                PasswordResetRecord = PasswordReset.objects.get(email=email,code=code)
                YooLottoUser.objects.filter(email=email).update(password=hashers.make_password(password))
                return {"reset": True}
            except:
                raise exceptions.WebServiceException("Please enter valid verification code")
        except:
            return {"reset": False}
        
        
class LoginUser(View):
    @rest
    @Authenticate()
    def post(self,request):
        user_data = json.loads(request.body)
    
        email = user_data['email']
        password = user_data['password']
        user = None
        try:
            user = YooLottoUser.objects.get(email=email)
        except:
            raise exceptions.WebServiceException("This email Id is not registered with the Yoolotto")
        if user:
            if hashers.check_password(password, user.password):
                try:
                    user_details = UserDetails.objects.get(user=user)
                    phone = user_details.phone
                    address = user_details.address
                except:
                    phone = None
                    address = None
                try:
                    print "device id in login",request.yoo['device']
                    ClientLoginRecord = UserClientLogin.objects.filter(device=request.yoo['device'])[0]
                    ClientLoginRecord.client_login = email
                    ClientLoginRecord.save()    
                except:
                    ClientLoginRecord, created = UserClientLogin.objects.get_or_create(device=request.yoo['device'],client_login = email)               
                print "clientttttttttttt",ClientLoginRecord
                try:
                    UserTicketsRecord, created = LotteryTicketClient.objects.get_or_create(device=request.yoo['device'],defaults={"email":ClientLoginRecord.client_login})
                except:
                    pass
                yoo_user = request.yoo["user"]
                empty_string = ""
                try:
                    reset_coins_record = ResetCoins.objects.get_or_create(device = request.yoo['device'], defaults = {"reset_datetime" : datetime.datetime.now(), "added_at" : datetime.datetime.now()})
                except:
                    reset_coins_record = ResetCoins.objects.filter(device = request.yoo['device'])[0]   
                try:
                    login_details = DeviceLoginDetails.objects.get_or_create(device = request.yoo["device"], email = user.email, user = request.yoo["user"])
                except:
                    login_details = DeviceLoginDetails.objects.filter(device = request.yoo["device"], email = user.email, user = request.yoo["user"])[0]
                return {"email":user.email,"user_name":user.name or empty_string,"phone":phone or empty_string,"address":address or empty_string,"id":user.id}
            else:
                raise exceptions.WebServiceException("Invalid Password")
            
                
        
    def _modify_email(self, user, email, password):
        # Search Others
        other = YooLottoUser.objects.filter(email=email)
        
        # Exclude current user
        if user:
            other = other.exclude(pk=user.pk)
        
        if not other:
            user.email = email
            
            if password:
                user.password = hashers.make_password(password)
            
            user.save()
            
            return
        
        other = other[0]
        if not other.password:
            self._merge(user, email, other)
            return
                                
        if not password:
            raise exceptions.WebServiceAuthorizationFailed("AUTH_REQUIRED")
        
        if hashers.check_password(password, other.password):
            self._merge(user, email, other)            
            return
        
        raise exceptions.WebServiceAuthorizationFailed("AUTH_INVALID")
        
    def _merge(self, user, email, other):
        from yoolotto.legacy.migration.tickets import MigrateTickets
        from yoolotto.legacy.models import Tickets as LegacyTicket, Users as LegacyUser
        
        # Import Data
	try:
        	for legacy in LegacyUser.objects.filter(email=email):
            		MigrateTickets(legacy, user).run()
	except:
		pass
                
        # Users
        devices = user.devices.all()
        
        # Magic Here...
        for device in devices:
            device.user = other
            device.save()
            
        other.save()

class User(View):
    @rest
    @Authenticate()
    def get(self, request):
        """
        Retrieves the details of the currently logged in user, will create a 
        new user if the device is not identified
        """
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
        print request.yoo["device"]
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
        print "request",request.yoo["new_version"]
        if request.yoo["new_version"]:
            try:
                coins_record1, created = DeviceCoins.objects.get_or_create(device_id=request.yoo['device'], defaults={'coins': 1})
                if created == True:
                    reset_coins_record = ResetCoins(device = request.yoo['device'], reset_datetime = datetime.datetime.now(), added_at = datetime.datetime.now())
                    reset_coins_record.save()
            except:
                pass
            user = request.yoo["user"]
            device=request.yoo['device']
            wallet = user.get_wallet()
             
            plays = sum(map(lambda x: x.plays.count(), user.tickets.all()))
            from django.db.models import Sum
            try:
                preferences = user.preferences
            except UserPreference.DoesNotExist:
                try:
                    preferences = UserPreference(user=user)
                    preferences.save()
                except IntegrityError:
                    preferences = user.preferences
    	    try:                
            	ClientLoginRecord, created = UserClientLogin.objects.get_or_create(device=request.yoo['device'], defaults={'client_login':None})
    	    except:
    		    ClientLoginRecord = UserClientLogin.objects.filter(device=request.yoo['device'])[0]
    	    try:
            	device_coins_record, created = DeviceCoins.objects.get_or_create(device_id=request.yoo['device'], defaults={'coins': 0})
    	    except:
    		    device_coins_record = DeviceCoins.objects.filter(device_id = request.yoo['device'])[0]
            if ClientLoginRecord.client_login:
                try:
                    coins_record, created = EmailCoins.objects.get_or_create(email=ClientLoginRecord.client_login, defaults={'coins': 0})
                except:
                    coins_record = EmailCoins.objects.filter(email=ClientLoginRecord.client_login)

                coins = coins_record.coins + device_coins_record.get_coins()
                print "innnnnnnnnnnnnnnnnnnnnn loginnnnnnnnnnnnnnnnnn in userrrrrrrrr"
                tickets = LotteryTicketClient.objects.filter(email=ClientLoginRecord.client_login).count()
            else:
                coins = device_coins_record.coins
            tickets = LotteryTicketClient.objects.filter(device=request.yoo['device']).count()
            #reset_coins_record = ResetCoins.objects.get_or_create(user_id = user.id,coins = coins)
            yoo_prize_code =  0
            yoo_game_code =  1
            print "coinsssss",coins
            if request.yoo["new_version"]:
                #coins = wallet.coins
                print "coinnnnnnnnnnnnnnnnn",coins
                return {
                "email": user.email,
                "password": True if user.password else False,
                "tokens": dict([[token.name, token.token] for token in user.tokens.all()]),
                "coins": coins,
                "tickets": tickets,
                "uncheckedTickets": sum(map(lambda x: x.plays.count(), user.tickets.filter(submissions__checked=False))),
                "plays": plays,
                "referral": user.referral,
                "preferences": {
                    "jackpot_drawing": preferences.jackpot_drawing,
                    "jackpot_frenzy": preferences.jackpot_frenzy,
                    "newsletter": preferences.newsletter
                },
    	    "freeStuff":yoo_prize_code,
    	    "yoo_game":yoo_game_code,
             }
            else:
    	 
             # Short-Circuit for Authentication Errors caused by invalid Device IDs
                if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
                    raise exceptions.WebServiceAuthorizationFailed()
                 
                user = request.yoo["user"]
                print "in else userrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr"
                wallet = user.get_wallet()
                
                tickets = user.tickets.filter(plays__isnull=False).count()
                plays = sum(map(lambda x: x.plays.count(), user.tickets.all()))
                
                try:
                    preferences = user.preferences
                except UserPreference.DoesNotExist:
                    try:
                        preferences = UserPreference(user=user)
                        preferences.save()
                    except IntegrityError:
                        preferences = user.preferences

                return {
                    "email": user.email,
                    "password": True if user.password else False,
                    "tokens": dict([[token.name, token.token] for token in user.tokens.all()]),
                    "coins": wallet.coins,
                    "tickets": tickets,
                    "uncheckedTickets": sum(map(lambda x: x.plays.count(), user.tickets.filter(submissions__checked=False))),
                    "plays": plays,
                    "referral": user.referral,
                    "preferences": {
                        "jackpot_drawing": preferences.jackpot_drawing,
                        "jackpot_frenzy": preferences.jackpot_frenzy,
                        "newsletter": preferences.newsletter
                    }
                 }
        
    @rest
    @Authenticate(create=True)
    def post(self, request):
        """
        Updates the details of the currently logged in user
        """
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
	

	if request.yoo["new_version"]:
         if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
         user = request.yoo["user"]
         device = request.yoo["device"]
        
         try:
            data = json.loads(request.body)
         except:
            data = request.POST
            
         form = UserUpdateForm(data)
         if not form.is_valid():
            raise exceptions.WebServiceFormValidationFailed(form)

         email = form.cleaned_data["email"]
         password = form.cleaned_data["password"]
	        
         # Routing Logic        
         if email and password:
            self._modify_email(user, email, password)
         elif email:
            self._modify_email(user, email, password=None)
         elif password:
            self._modify_password(user, password)
        
         existing = dict([[x.name, x] for x in user.tokens.all()])
        
         for key in ["fbToken", "twitterToken", "contestEmail", "requestedEmail"]:
            if key not in data:
                continue
            
            if not form.cleaned_data[key]:
                if key in existing:
                    existing[key].delete()
            else:
                if key in existing:
                    existing[key].token = form.cleaned_data[key]
                else:
                    existing[key] = UserToken(user=user, name=key, token=form.cleaned_data[key])
            
                existing[key].save()
                
         if "pushToken" in form.cleaned_data and form.cleaned_data["pushToken"]:
            request.yoo["device"].device_token = form.cleaned_data["pushToken"]
            request.yoo["device"].save()
            
         if "referral" in form.cleaned_data and form.cleaned_data["referral"]:
            user.referral = form.cleaned_data["referral"]
            user.save()

         if email:
            try:
                coins_record, created = EmailCoins.objects.get_or_create(email=email,defaults={'coins': 0})
            except:
                coins_record = EmailCoins.objects.filter(email=email)[0]
            coins_record.save()

        
         if user.password and user.email or user.tokens.filter(name="requestedEmail").exists():
            try:
                CoinTicketGenericTransaction.objects.get(user=user, type="register")
            except CoinTicketGenericTransaction.DoesNotExist:
                transaction = CoinTransaction(wallet=user.get_wallet())
                transaction.save()
                
                _coins = CoinTicketGenericTransaction(user=user, type="register", transaction=transaction)
                _coins.save()
                                
                transaction.amount = _coins.amount
                transaction.save()
		try:
			device_record = DeviceCoins.objects.get(device_id=request.yoo['device'])
		except:
			device_record = DeviceCoins.objects.filter(device_id=request.yoo['device'])[0]
                device_record.coins = F('coins') + int(_coins.amount)
                device_record.save()
         try:
        	ClientLoginRecord, created = UserClientLogin.objects.get_or_create(device=request.yoo['device'], defaults={})
	 except:
		ClientLoginRecord  = UserClientLogin.objects.filter(device=request.yoo['device'])[0]
	 print data
	 if data.has_key('fbToken'):
            if data['fbToken'] != '':
               ClientLoginRecord.client_login = data['user_email']
            else:
               ClientLoginRecord.client_login = email
	 elif data.has_key('twitterToken'):
	    if data['twitterToken'] != '':
	       ClientLoginRecord.client_login = data['user_email']
	    else:
	       ClientLoginRecord.client_login = email
	 else:
	    ClientLoginRecord.client_login = email
	 ClientLoginRecord.save()
        # ClientLoginRecord.save()

         try:
            if created:
                token_info = UserToken.objects.filter(user_id = user.id)
                if token_info.name == "fbToken":
                    coins_record.coins = F('coins')+0.5
                    coins_record.save()
         except:
            pass


         
         return self.get(request)
        else:
	  
         # Short-Circuit for Authentication Errors caused by invalid Device IDs
         if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
        
         user = request.yoo["user"]
         device = request.yoo["device"]
        
         try:
            data = json.loads(request.body)
         except:
            data = request.POST
        
         form = UserUpdateForm(data)
         if not form.is_valid():
            raise exceptions.WebServiceFormValidationFailed(form)

         email = form.cleaned_data["email"]
         password = form.cleaned_data["password"]
                
         # Routing Logic        
         if email and password:
            self._modify_email(user, email, password)
         elif email:
            self._modify_email(user, email, password=None)
         elif password:
            self._modify_password(user, password)
        
         existing = dict([[x.name, x] for x in user.tokens.all()])
        
         for key in ["fbToken", "twitterToken", "contestEmail", "requestedEmail"]:
            if key not in data:
                continue
            
            if not form.cleaned_data[key]:
                if key in existing:
                    existing[key].delete()
            else:
                if key in existing:
                    existing[key].token = form.cleaned_data[key]
                else:
                    existing[key] = UserToken(user=user, name=key, token=form.cleaned_data[key])
            
                existing[key].save()
                
         if "pushToken" in form.cleaned_data and form.cleaned_data["pushToken"]:
            request.yoo["device"].device_token = form.cleaned_data["pushToken"]
            request.yoo["device"].save()
            
         if "referral" in form.cleaned_data and form.cleaned_data["referral"]:
            user.referral = form.cleaned_data["referral"]
            user.save()
        
         if user.tokens.filter(name="requestedEmail").exists():
            try:
                CoinTicketGenericTransaction.objects.get(user=user, type="register")
            except CoinTicketGenericTransaction.DoesNotExist:
                transaction = CoinTransaction(wallet=user.get_wallet())
                transaction.save()
                
                _coins = CoinTicketGenericTransaction(user=user, type="register", transaction=transaction)
                _coins.save()
                                
                transaction.amount = _coins.amount
                transaction.save()
        
         return self.get(request)
    
    def _modify_password(self, user, password):
        user.password = hashers.make_password(password)
        user.save()                
    
    def _modify_email(self, user, email, password):
        # Search Others
        other = YooLottoUser.objects.filter(email=email)
        
        # Exclude current user
        if user:
            other = other.exclude(pk=user.pk)
        
        if not other:
            user.email = email
            
            if password:
                user.password = hashers.make_password(password)
            
            user.save()
            
            return
        
        other = other[0]
        if not other.password:
            self._merge(user, email, other)
            return
                                
        if not password:
            raise exceptions.WebServiceAuthorizationFailed("AUTH_REQUIRED")
        
        if hashers.check_password(password, other.password):
            self._merge(user, email, other)            
            return
        
        raise exceptions.WebServiceAuthorizationFailed("AUTH_INVALID")
        
    def _merge(self, user, email, other):
        from yoolotto.legacy.migration.tickets import MigrateTickets
        from yoolotto.legacy.models import Tickets as LegacyTicket, Users as LegacyUser
        
        # Import Data
	try:
        	for legacy in LegacyUser.objects.filter(email=email):
            		MigrateTickets(legacy, user).run()
        except:
		pass
        # Users
        devices = user.devices.all()
        
        # Magic Here...
        for device in devices:
            device.user = other
            device.save()
            
        other.save()
        
class Preference(View):
    @rest
    @Authenticate()
    def get(self, request):
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
        
        user = request.yoo["user"]
        
        try:
            preferences = user.preferences
        except UserPreference.DoesNotExist:
            preferences = UserPreference(user=user)
            preferences.save()
        
        return {
            "jackpot_drawing": preferences.jackpot_drawing,
            "jackpot_frenzy": preferences.jackpot_frenzy,
            "newsletter": preferences.newsletter
        }
    
    @rest
    @Authenticate(create=False)
    def post(self, request):
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
        
        user = request.yoo["user"]
        
        try:
            data = json.loads(request.body)
        except:
            data = {}
        
        form = PreferenceUpdateForm(data)
        if not form.is_valid():
            raise exceptions.WebServiceFormValidationFailed(form)
        
        try:
            preferences = user.preferences
        except UserPreference.DoesNotExist:
            preferences = UserPreference(user=user)
            preferences.save()
            
#        for k, v in form.cleaned_data.iteritems():
#            print k, v, type(v)
                    
        preferences.jackpot_drawing = form.cleaned_data["jackpot_drawing"]
        preferences.jackpot_frenzy = form.cleaned_data["jackpot_frenzy"]
        preferences.newsletter = form.cleaned_data["newsletter"]
        
        preferences.save()
        
        return self.get(request)
    
    
class Device(View):
    @rest
    @Authenticate()
    def get(self, request):
        """
        
        
        """
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
        
        device = request.yoo["device"]
        
        return {
            "type": device.device_type,
            "token": True if device.device_token else False
        }
        
    @rest
    @Authenticate(create=False)
    def post(self, request):
        # Short-Circuit for Authentication Errors caused by invalid Device IDs
        if not request.yoo["user"] and request.yoo["auth"].get("__internal__reject__", False):
            raise exceptions.WebServiceAuthorizationFailed()
        
        device = request.yoo["device"]
        
        try:
            data = json.loads(request.body)
        except:
            data = {}
        
        token = data.get("token", None)
        
        if not token:
            raise exceptions.WebServiceException("No Token Provided")
        
        existing = DeviceModel.objects.filter(device_token=token).exclude(pk=device.pk)
        for _existing in existing:
            _existing.device_token = None
            _existing.save()
        
        device.device_token = token
        device.save()
        
        return self.get(request)
    
class ReferralStats(View):
    @rest
    def get(self, request):
        if request.GET.get("token", None) != "j5nwvYbuIQflKSkUpetYh8OADJFXljvn":
            return HttpResponseNotFound()
        
        result = [["code", "count"],]
        data = YooLottoUser.objects.filter(referral__isnull=False).values('referral').annotate(Count('referral')).order_by("referral__count")
        map(lambda x: result.append([x["referral"], str(x["referral__count"])]), data)
        
        return HttpResponse(content="\n".join(map(lambda x: ", ".join(x), result)), content_type="text/csv")
        
